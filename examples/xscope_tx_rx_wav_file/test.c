#include <platform.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xscope.h>
#include <xcore/assert.h>
#include <xcore/hwtimer.h>
#include "xscope_io_device.h"
#include "wav_utils.h"

#include <assert.h>

#define appconfINPUT_FILENAME    "testin.wav"
#define appconfOUTPUT_FILENAME   "testout.wav"

// #define appconfMAX_CHANNELS 1
#define appconfOUT_NUM_OF_CHANNELS 2
#define appconfFRAME_ADVANCE 240
#define appconfFRAME_ELEMENT_SIZE sizeof(int32_t)
#define appconfDATA_FRAME_SIZE_BYTES   (appconfFRAME_ADVANCE * appconfFRAME_ELEMENT_SIZE)

//input buffer and output buffer are interleaved as wav file
void process_frame(uint8_t * input_buf, uint8_t * output_buf)
{
    unsigned i;
    for(i=0;i<appconfDATA_FRAME_SIZE_BYTES;i++){
        output_buf[i] = input_buf[i];   //add audio processing here
    }
}



void deinterleaveFrame(const uint8_t* interleavedFrame, uint16_t* deinterleavedFrames, unsigned frameSize, short numChannels, short sampleWidth)
{
    int i, j;

    // Calculate the number of bytes per sample
    int bytesPerSample = sampleWidth / 8;


    // Deinterleave the samples
    for (i = 0; i < frameSize; i++) {
        for (j = 0; j < numChannels; j++) {
            const uint8_t* samplePtr = interleavedFrame + (i * numChannels + j) * bytesPerSample;

            // Convert the interleaved sample bytes to a uint8_t value
            switch (sampleWidth) {
                case 16:
                    *((uint16_t*) deinterleavedFrames + (j * appconfFRAME_ADVANCE) + i )= *((uint16_t*)samplePtr);
                    break;
                case 24:
                    *((uint16_t*) deinterleavedFrames + (j * appconfFRAME_ADVANCE) + i )=
                        (uint16_t)(samplePtr[0] | (samplePtr[1] << 8) | (samplePtr[2] << 16));
                    break;
                case 32:
                    // *((uint32_t*) deinterleavedFrames + (j * appconfFRAME_ADVANCE) + i  )= *((uint32_t*)samplePtr);
                    deinterleavedFrames[j * appconfFRAME_ADVANCE + i]  = *((uint32_t*)samplePtr);
                    if(((i * numChannels + j) ) == 5 )
                    {
                        printf("data 5 is 0x%x\n",*((uint32_t*)samplePtr));
                        // deinterleavedFrames[0]=1;
                        // deinterleavedFrames[1]=2;
                        // deinterleavedFrames[5]=3;
                        printf("deinterleavedFrames[1] is %d\n",deinterleavedFrames[1]);
                    }
                    break;
                default:
                    // Unsupported sample width
                    printf("Unsupported sample width: %d\n", sampleWidth);
                    break;
            }
        }
    }
}


void audio_proc_test(void){
    xscope_file_t infile = xscope_open_file(appconfINPUT_FILENAME, "rb");
    xscope_file_t outfile = xscope_open_file(appconfOUTPUT_FILENAME, "wb");

    unsigned input_header_size;
    wav_header input_header_struct, output_header_struct;
    unsigned frame_count;
    unsigned block_count;
    unsigned bytes_per_block;
    unsigned samples_per_block;
    size_t bytes_read = 0;
    unsigned y=0;

    uint8_t * p_in_buf;
    uint16_t * p_audio_in_channels;     //audio buffer of one block with samples arranged as channels
    uint8_t * p_out_buf;
    // buffer for Deinterleave the frame
    // uint16_t** deinterleavedFrames;

    // uint8_t in_buf[appconfDATA_FRAME_SIZE_BYTES] = {0};
    // uint8_t out_buf[appconfDATA_FRAME_SIZE_BYTES] = {0};



    if(get_wav_header_details(&infile, &input_header_struct, &input_header_size) != 0){
        printf("Error: error in get_wav_header_details()\n");
        _Exit(1);
    }

    printf("Input wav file bit depth : %d\n", input_header_struct.bit_depth);
    printf("Input wav file number of channels : %d\n", input_header_struct.num_channels);

    // // Ensure 32bit wav file
    // if(input_header_struct.bit_depth != 32)
    // {
    //     printf("Error: unsupported wav bit depth (%d) for %s file. Only 32 supported\n", input_header_struct.bit_depth, appconfINPUT_FILENAME);
    //     _Exit(1);
    // }
    // // Ensure input wav file contains correct number of channels 
    // if(input_header_struct.num_channels != appconfMAX_CHANNELS){
    //     printf("Error: wav num channels(%d) does not match (%u)\n", input_header_struct.num_channels, appconfMAX_CHANNELS);
    //     _Exit(1);
    // }
    
    // Calculate number of frames in the wav file
    frame_count = wav_get_num_frames(&input_header_struct);
    block_count = frame_count / appconfFRAME_ADVANCE; 
    bytes_per_block = wav_get_num_bytes_per_frame(&input_header_struct) * appconfFRAME_ADVANCE;

    p_in_buf = malloc(bytes_per_block);

    switch (input_header_struct.bit_depth) {
        case 16:
            p_audio_in_channels = (int16_t * )malloc(appconfFRAME_ADVANCE * sizeof(int16_t) * input_header_struct.num_channels);
            break;
        case 24:
        case 32:
            p_audio_in_channels = (int16_t * )malloc(appconfFRAME_ADVANCE * sizeof(int32_t) * input_header_struct.num_channels);
            break;
        default:
            // Unsupported sample width
            printf("Unsupported sample width: %d\n", input_header_struct.bit_depth);
            break;
    }




    // Create output wav file
    wav_form_header(&output_header_struct,
        input_header_struct.audio_format,
        appconfOUT_NUM_OF_CHANNELS,
        input_header_struct.sample_rate,
        input_header_struct.bit_depth,
        block_count*appconfFRAME_ADVANCE);

    
    printf("Write wav header to output file\n");
    xscope_fwrite(&outfile, (uint8_t*)(&output_header_struct), WAV_HEADER_BYTES);


    printf("Start running pipeline test, bytes_per_block%d\n",bytes_per_block);

    for(unsigned b=0; b</*block_count*/1; b++) {
        memset(p_in_buf, 0x00, bytes_per_block);
        long input_location =  wav_get_frame_start(&input_header_struct, b * appconfFRAME_ADVANCE, input_header_size);
        xscope_fseek(&infile, input_location, SEEK_SET);
        bytes_read = xscope_fread(&infile, p_in_buf, bytes_per_block);
        memset(p_in_buf + bytes_read, 0x00, appconfDATA_FRAME_SIZE_BYTES - bytes_read);   //fill in buffer with zeros if data read is smaller than one frame data size.

        deinterleaveFrame(p_in_buf, p_audio_in_channels, appconfFRAME_ADVANCE,input_header_struct.num_channels, input_header_struct.bit_depth );

        printf("p_in_buf: 0x");
        for(unsigned i=0;i<30;i++){
            printf("%x ",p_in_buf[i]);
        }

        printf("\n p_audio_in_channels: 0x");
        for(unsigned y=0;y<30;y++){
            printf("%x ",p_audio_in_channels[y]);
        }
        printf("\n");


        // process_frame(p_in_buf, p_out_buf);

        // xscope_fwrite(&outfile, p_out_buf, appconfDATA_FRAME_SIZE_BYTES);
    }

    printf("file processing finished!\n");
}

void main_tile0(chanend_t xscope_chan)
{
    xscope_io_init(xscope_chan);
    audio_proc_test();
    xscope_close_all_files();
}
