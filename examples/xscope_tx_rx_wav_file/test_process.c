#include <platform.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xscope.h>
#include <xcore/assert.h>
#include <xcore/hwtimer.h>
#include <xcore/channel.h>
#include "xscope_io_device.h"
#include "wav_utils.h"

#include "test_process.h"

#include <assert.h>

#define SAMPLES_PER_BLOCK   (appconfFRAME_ADVANCE * input_header_struct.num_channels)

//audioprocess_frame function
void process_stage_01(chanend_t input_c, chanend_t output_c)
{
    uint32_t i;
    uint32_t* stage01_buffer_32;

    uint8_t first_block=1;
    uint8_t bit_depth, num_chan;

    uint16_t samples_per_block;


    bit_depth=chan_in_byte(input_c);
    num_chan=chan_in_byte(input_c);

    samples_per_block = appconfFRAME_ADVANCE *num_chan;

    printf("process_stage_01, %d bit %d-ch\n",bit_depth,num_chan);

    switch (bit_depth){
        case 24:
        case 32:
            stage01_buffer_32 = (uint32_t*)malloc(samples_per_block * sizeof(uint32_t));
            break;
    }
    printf("got malloc process_stage_01, add:%d\n",stage01_buffer_32);


    while(1) {        
        switch (bit_depth){
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    stage01_buffer_32[i]=chan_in_word(input_c);
                }
                break;
        }

        //Add your processing here with buffer of stage01_buffer_32


        if(first_block){
            chan_out_byte(output_c, bit_depth);
            chan_out_byte(output_c, num_chan);
            first_block=0;
        }
        switch (bit_depth){
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    chan_out_word(output_c,stage01_buffer_32[i]);
                    // printf("got ch %lu\n",stage01_buffer_32[i]);
                }
                break;
        }
        
    }
}

void process_stage_11(chanend_t input_c, chanend_t output_c)
{
    uint32_t i;
    uint32_t* stage11_buffer_32;

    uint8_t first_block=1;
    uint8_t bit_depth, num_chan;

    uint16_t samples_per_block;


    bit_depth=chan_in_byte(input_c);
    num_chan=chan_in_byte(input_c);

    samples_per_block = appconfFRAME_ADVANCE *num_chan;

    printf("process_stage_11, %d bit %d-ch\n",bit_depth,num_chan);

    switch (bit_depth){
        case 24:
        case 32:
            stage11_buffer_32 = (uint32_t*)malloc(samples_per_block * sizeof(uint32_t));
            break;
    }
    printf("got malloc process_stage_11, add:%d\n",stage11_buffer_32);


    while(1) {        
        switch (bit_depth){
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    stage11_buffer_32[i]=chan_in_word(input_c);
                }
                break;
        }

        //Add your processing here with buffer of stage1_buffer_32


        // bit_depth=chan_out_byte(output_c, bit_depth);
        // num_chan=chan_out_byte(output_c, num_chan);
        switch (bit_depth){
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    chan_out_word(output_c,stage11_buffer_32[i]);
                }
                break;
        }
        
    }
}

