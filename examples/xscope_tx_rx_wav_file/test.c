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

#define appconfMAX_CHANNELS 1
#define appconfFRAME_ADVANCE 240
#define appconfFRAME_ELEMENT_SIZE sizeof(int32_t)
#define appconfDATA_FRAME_SIZE_BYTES   (appconfFRAME_ADVANCE * appconfFRAME_ELEMENT_SIZE)

void process_frame(uint8_t * input_buf, uint8_t * output_buf)
{
    unsigned i;
    for(i=0;i<appconfDATA_FRAME_SIZE_BYTES;i++){
        output_buf[i] = input_buf[i];   //add audio processing here
    }
}

void audio_proc_test(void){
    xscope_file_t infile = xscope_open_file(appconfINPUT_FILENAME, "rb");
    xscope_file_t outfile = xscope_open_file(appconfOUTPUT_FILENAME, "wb");

    unsigned input_header_size;
    wav_header input_header_struct, output_header_struct;
    unsigned frame_count;
    unsigned block_count;
    size_t bytes_read = 0;

    uint8_t in_buf[appconfDATA_FRAME_SIZE_BYTES] = {0};
    uint8_t out_buf[appconfDATA_FRAME_SIZE_BYTES] = {0};



    if(get_wav_header_details(&infile, &input_header_struct, &input_header_size) != 0){
        printf("Error: error in get_wav_header_details()\n");
        _Exit(1);
    }

    // Ensure 32bit wav file
    if(input_header_struct.bit_depth != 32)
    {
        printf("Error: unsupported wav bit depth (%d) for %s file. Only 32 supported\n", input_header_struct.bit_depth, appconfINPUT_FILENAME);
        _Exit(1);
    }
    // Ensure input wav file contains correct number of channels 
    if(input_header_struct.num_channels != appconfMAX_CHANNELS){
        printf("Error: wav num channels(%d) does not match (%u)\n", input_header_struct.num_channels, appconfMAX_CHANNELS);
        _Exit(1);
    }
    
    // Calculate number of frames in the wav file
    frame_count = wav_get_num_frames(&input_header_struct);
    block_count = frame_count / appconfFRAME_ADVANCE; 

    // Create output wav file
    wav_form_header(&output_header_struct,
        input_header_struct.audio_format,
        appconfMAX_CHANNELS,
        input_header_struct.sample_rate,
        input_header_struct.bit_depth,
        block_count*appconfFRAME_ADVANCE);

    
    printf("Write wav header to output file\n");
    xscope_fwrite(&outfile, (uint8_t*)(&output_header_struct), WAV_HEADER_BYTES);


    printf("Start running pipeline test\n");

    for(unsigned b=0; b<block_count; b++) {
        memset(in_buf, 0x00, appconfDATA_FRAME_SIZE_BYTES);
        long input_location =  wav_get_frame_start(&input_header_struct, b * appconfFRAME_ADVANCE, input_header_size);
        xscope_fseek(&infile, input_location, SEEK_SET);
        bytes_read = xscope_fread(&infile, in_buf, appconfDATA_FRAME_SIZE_BYTES);
        memset(in_buf + bytes_read, 0x00, appconfDATA_FRAME_SIZE_BYTES - bytes_read);   //fill in buffer with zeros if data read is smaller than one frame data size.

        process_frame(in_buf, out_buf);

        xscope_fwrite(&outfile, out_buf, appconfDATA_FRAME_SIZE_BYTES);
    }

    printf("file processing finished!\n");
}

void main_tile0(chanend_t xscope_chan)
{
    xscope_io_init(xscope_chan);
    audio_proc_test();
    xscope_close_all_files();
}
