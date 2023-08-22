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

#define appconfINPUT_FILENAME    "testin_ch6_32bit.wav"
#define appconfOUTPUT_FILENAME   "testout.wav"

#define appconfOUT_NUM_OF_CHANNELS 2
#define appconfFRAME_ADVANCE 240
#define appconfFRAME_ELEMENT_SIZE sizeof(int32_t)
#define appconfDATA_FRAME_SIZE_BYTES   (appconfFRAME_ADVANCE * appconfOUT_NUM_OF_CHANNELS * appconfFRAME_ELEMENT_SIZE)

//input buffer and output buffer are interleaved as wav file
void process_frame(void* input_buf, void* output_buf, short num_ch_out, short sampleWidth)
{
    unsigned i, j;
    size_t offset;

    for(i=0;i<num_ch_out;i++){
        for(j=0;j<appconfFRAME_ADVANCE;j++){
            //add audio processing here

            switch(sampleWidth){
                case 16:
                    offset =  (i * appconfFRAME_ADVANCE + j) * (sampleWidth/8);
                    *((uint16_t *)(output_buf + offset))=*((uint16_t *)(input_buf + offset));
                    break;
                case 24:
                    offset =  (i * appconfFRAME_ADVANCE + j) * (32/8);
                    *((uint32_t *)(output_buf + offset))=*((uint32_t *)(input_buf + offset));
                    break;
                case 32:
                    offset =  (i * appconfFRAME_ADVANCE + j) * (sampleWidth/8);
                    *((uint32_t *)(output_buf + offset))=*((uint32_t *)(input_buf + offset));
                    break;
            }
        }
    }

}

//interleaveFrame for sending back to xscope output
void interleaveFrame(uint8_t* interleavedFrame, void* deinterleavedFrames, unsigned frameSize, short numChannels, short sampleWidth)
{
    int i, j;

    // Calculate the number of bytes per sample
    int bytesPerSample = sampleWidth / 8;
     
    uint16_t temp_interleaved_sample_16;
    uint32_t temp_interleaved_sample_32;


    for (i = 0; i < frameSize; i++) {
        for (j = 0; j < numChannels; j++) {
            uint8_t* samplePtr = interleavedFrame + (i * numChannels + j) * bytesPerSample;

            // Convert the processed audio buffer back to interleaved samples for sending back to xscope
            switch (sampleWidth) {
                case 16:
                    temp_interleaved_sample_16 = *((uint16_t*)(deinterleavedFrames + (j * appconfFRAME_ADVANCE + i)*bytesPerSample));
                    *samplePtr = temp_interleaved_sample_16 & 0xFF;
                    *(samplePtr+1) = (temp_interleaved_sample_16 >> 8) & 0xFF;
                    break;
                case 24:
                    temp_interleaved_sample_32 = *((uint32_t*)(deinterleavedFrames + (j * appconfFRAME_ADVANCE + i)*(32/8)));
                    *samplePtr = temp_interleaved_sample_32 & 0xFF;
                    *(samplePtr+1) = (temp_interleaved_sample_32 >> 8) & 0xFF;
                    *(samplePtr+2) = (temp_interleaved_sample_32 >> 16) & 0xFF;
                    break;
                case 32:
                    temp_interleaved_sample_32 = *((uint32_t*)(deinterleavedFrames + (j * appconfFRAME_ADVANCE + i)*bytesPerSample));
                    *samplePtr = temp_interleaved_sample_32 & 0xFF;
                    *(samplePtr+1) = (temp_interleaved_sample_32 >> 8) & 0xFF;
                    *(samplePtr+2) = (temp_interleaved_sample_32 >> 16) & 0xFF;
                    *(samplePtr+3) = (temp_interleaved_sample_32 >> 24) & 0xFF;
                    break;
            }
        }
    }
}



void deinterleaveFrame(const uint8_t* interleavedFrame, void* deinterleavedFrames, unsigned frameSize, short numChannels, short sampleWidth)
{
    int i, j;

    // Calculate the number of bytes per sample
    int bytesPerSample = sampleWidth / 8;


    // Deinterleave the samples
    for (i = 0; i < frameSize; i++) {
        for (j = 0; j < numChannels; j++) {
            const uint8_t* samplePtr = interleavedFrame + (i * numChannels + j) * bytesPerSample;

            switch (sampleWidth) {
                case 16:
                    *((uint16_t*) deinterleavedFrames + (j * appconfFRAME_ADVANCE) + i )= *((uint16_t*)samplePtr);
                    break;
                case 24:
                    *(((uint32_t*) deinterleavedFrames) + (j * appconfFRAME_ADVANCE) + i )=
                        (uint32_t)(samplePtr[0] | (samplePtr[1] << 8) | (samplePtr[2] << 16));
                    break;
                case 32:
                    *(((uint32_t*)deinterleavedFrames) + (j * appconfFRAME_ADVANCE) + i )  = *((uint32_t*)samplePtr);
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
    size_t bytes_read = 0;

    uint8_t * p_in_buf;
    uint16_t * p_audio_in_channels_16;     //audio buffer of one block with samples arranged as channels, for sample width of 16bit
    uint32_t * p_audio_in_channels_32;     //audio buffer of one block with samples arranged as channels, for sample width of 24 or 32bit
    uint16_t * p_audio_out_channels_16;     //audio buffer of one block of processed output samples, for sample width of 16bit
    uint32_t * p_audio_out_channels_32;     //audio buffer of one block of processed output samples, for sample width of 24 or 32bit
    uint8_t * p_out_buf;
    size_t out_buf_size;



    if(get_wav_header_details(&infile, &input_header_struct, &input_header_size) != 0){
        printf("Error: error in get_wav_header_details()\n");
        _Exit(1);
    }

    printf("Input wav file bit depth : %d\n", input_header_struct.bit_depth);
    printf("Input wav file number of channels : %d\n", input_header_struct.num_channels);
    
    // Calculate number of frames in the wav file
    frame_count = wav_get_num_frames(&input_header_struct);
    block_count = frame_count / appconfFRAME_ADVANCE; 
    bytes_per_block = wav_get_num_bytes_per_frame(&input_header_struct) * appconfFRAME_ADVANCE;

    p_in_buf = malloc(bytes_per_block);

    out_buf_size = appconfFRAME_ADVANCE*appconfOUT_NUM_OF_CHANNELS*(input_header_struct.bit_depth/8);
    p_out_buf = malloc(out_buf_size);

    switch (input_header_struct.bit_depth) {
        case 16:
            p_audio_in_channels_16 = (uint16_t * )malloc(appconfFRAME_ADVANCE * sizeof(uint16_t) * input_header_struct.num_channels);
            p_audio_out_channels_16 = (uint16_t * )malloc(appconfFRAME_ADVANCE * sizeof(uint16_t) * appconfOUT_NUM_OF_CHANNELS);
            break;
        case 24:
        case 32:
            p_audio_in_channels_32 = (uint32_t * )malloc(appconfFRAME_ADVANCE * sizeof(uint32_t) * input_header_struct.num_channels);
            p_audio_out_channels_32 = (uint32_t * )malloc(appconfFRAME_ADVANCE * sizeof(uint32_t) * appconfOUT_NUM_OF_CHANNELS);
            break;
        default:
            // Unsupported sample width
            printf("Unsupported sample width: %d!!\n", input_header_struct.bit_depth);
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

    for(unsigned b=0; b<block_count; b++) {
        memset(p_in_buf, 0x00, bytes_per_block);
        long input_location =  wav_get_frame_start(&input_header_struct, b * appconfFRAME_ADVANCE, input_header_size);
        xscope_fseek(&infile, input_location, SEEK_SET);
        bytes_read = xscope_fread(&infile, p_in_buf, bytes_per_block);
        memset(p_in_buf + bytes_read, 0x00, bytes_per_block - bytes_read);   //fill in buffer with zeros if data read is smaller than one frame data size.

        switch (input_header_struct.bit_depth) {
            case 16:
                deinterleaveFrame(p_in_buf, p_audio_in_channels_16, appconfFRAME_ADVANCE,input_header_struct.num_channels, input_header_struct.bit_depth );
                process_frame(p_audio_in_channels_16, p_audio_out_channels_16, appconfOUT_NUM_OF_CHANNELS, input_header_struct.bit_depth);
                interleaveFrame(p_out_buf, p_audio_out_channels_16, appconfFRAME_ADVANCE, appconfOUT_NUM_OF_CHANNELS, input_header_struct.bit_depth );
                break;
            case 24:
            case 32:
                deinterleaveFrame(p_in_buf, p_audio_in_channels_32, appconfFRAME_ADVANCE,input_header_struct.num_channels, input_header_struct.bit_depth );
                process_frame(p_audio_in_channels_32, p_audio_out_channels_32, appconfOUT_NUM_OF_CHANNELS, input_header_struct.bit_depth);
                interleaveFrame(p_out_buf, p_audio_out_channels_32, appconfFRAME_ADVANCE, appconfOUT_NUM_OF_CHANNELS, input_header_struct.bit_depth );
                break;
        }

        xscope_fwrite(&outfile, p_out_buf, out_buf_size);
    }

    printf("file processing finished!\n");
}

void main_tile0(chanend_t xscope_chan)
{
    xscope_io_init(xscope_chan);
    audio_proc_test();
    xscope_close_all_files();
}
