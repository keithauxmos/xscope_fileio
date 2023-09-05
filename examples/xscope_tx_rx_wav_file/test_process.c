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
    void* stage01_buffer;

    uint8_t temp_byte_buf[2];   //temp buffer for receiving 16-bit samples

    uint8_t first_block=1;
    uint8_t bit_depth, num_chan;

    uint16_t samples_per_block;


    bit_depth=chan_in_byte(input_c);
    num_chan=chan_in_byte(input_c);

    samples_per_block = appconfFRAME_ADVANCE *num_chan;

    switch (bit_depth){
        case 16:
            stage01_buffer = malloc(samples_per_block * sizeof(uint16_t));
            break;
        case 24:
        case 32:
            stage01_buffer = malloc(samples_per_block * sizeof(uint32_t));
            break;
    }


    while(1) {        
        switch (bit_depth){
            case 16:
                for(i=0;i<samples_per_block;i++){
                    temp_byte_buf[0]=chan_in_byte(input_c);
                    temp_byte_buf[1]=chan_in_byte(input_c);
                    *(uint16_t *)(stage01_buffer+i*sizeof(uint16_t))=temp_byte_buf[0]<<8 | temp_byte_buf[1];
                }
                break;
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    *(uint32_t *)(stage01_buffer+i*sizeof(uint32_t))=chan_in_word(input_c);
                }
                break;
        }

        //Tile 0, core 1
        //Add your processing here with buffer of stage01_buffer


        if(first_block){
            chan_out_byte(output_c, bit_depth);
            chan_out_byte(output_c, num_chan);
            first_block=0;
        }
        switch (bit_depth){
            case 16:
                for(i=0;i<samples_per_block;i++){
                    chan_out_byte(output_c, (*((uint16_t *)(stage01_buffer+i*sizeof(uint16_t))))>>8 & 0xFF);
                    chan_out_byte(output_c, (*((uint16_t *)(stage01_buffer+i*sizeof(uint16_t)))) & 0xFF);
                }
                break;
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    chan_out_word(output_c,*(uint32_t *)(stage01_buffer+i*sizeof(uint32_t)));
                }
                break;
        }
        
    }
}

void process_stage_02(chanend_t input_c, chanend_t output_c)
{
    uint32_t i;
    void* stage02_buffer;

    uint8_t temp_byte_buf[2];   //temp buffer for receiving 16-bit samples

    uint8_t first_block=1;
    uint8_t bit_depth, num_chan;

    uint16_t samples_per_block;


    bit_depth=chan_in_byte(input_c);
    num_chan=chan_in_byte(input_c);

    samples_per_block = appconfFRAME_ADVANCE *num_chan;

    switch (bit_depth){
        case 16:
            stage02_buffer = malloc(samples_per_block * sizeof(uint16_t));
            break;
        case 24:
        case 32:
            stage02_buffer = malloc(samples_per_block * sizeof(uint32_t));
            break;
    }


    while(1) {        
        switch (bit_depth){
            case 16:
                for(i=0;i<samples_per_block;i++){
                    temp_byte_buf[0]=chan_in_byte(input_c);
                    temp_byte_buf[1]=chan_in_byte(input_c);
                    *(uint16_t *)(stage02_buffer+i*sizeof(uint16_t))=temp_byte_buf[0]<<8 | temp_byte_buf[1];
                }
                break;
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    *(uint32_t *)(stage02_buffer+i*sizeof(uint32_t))=chan_in_word(input_c);
                }
                break;
        }

        //Tile 0, core 2
        //Add your processing here with buffer of stage02_buffer


        if(first_block){
            chan_out_byte(output_c, bit_depth);
            chan_out_byte(output_c, num_chan);
            first_block=0;
        }
        switch (bit_depth){
            case 16:
                for(i=0;i<samples_per_block;i++){
                    chan_out_byte(output_c, (*((uint16_t *)(stage02_buffer+i*sizeof(uint16_t))))>>8 & 0xFF);
                    chan_out_byte(output_c, (*((uint16_t *)(stage02_buffer+i*sizeof(uint16_t)))) & 0xFF);
                }
                break;
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    chan_out_word(output_c,*(uint32_t *)(stage02_buffer+i*sizeof(uint32_t)));
                }
                break;
        }
        
    }
}

void process_stage_11(chanend_t input_c, chanend_t output_c)
{
    uint32_t i;
    void* stage11_buffer;

    uint8_t temp_byte_buf[2];   //temp buffer for receiving 16-bit samples

    uint8_t first_block=1;
    uint8_t bit_depth, num_chan;

    uint16_t samples_per_block;


    bit_depth=chan_in_byte(input_c);
    num_chan=chan_in_byte(input_c);

    samples_per_block = appconfFRAME_ADVANCE *num_chan;

    switch (bit_depth){
        case 16:
            stage11_buffer = malloc(samples_per_block * sizeof(uint16_t));
            break;
        case 24:
        case 32:
            stage11_buffer = malloc(samples_per_block * sizeof(uint32_t));
            break;
    }


    while(1) {        
        switch (bit_depth){
            case 16:
                for(i=0;i<samples_per_block;i++){
                    temp_byte_buf[0]=chan_in_byte(input_c);
                    temp_byte_buf[1]=chan_in_byte(input_c);
                    *(uint16_t *)(stage11_buffer+i*sizeof(uint16_t))=temp_byte_buf[0]<<8 | temp_byte_buf[1];
                }
                break;
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    *(uint32_t *)(stage11_buffer+i*sizeof(uint32_t))=chan_in_word(input_c);
                }
                break;
        }

        //Tile 1, core 1
        //Add your processing here with buffer of stage11_buffer


        if(first_block){
            chan_out_byte(output_c, bit_depth);
            chan_out_byte(output_c, num_chan);
            first_block=0;
        }
        switch (bit_depth){
            case 16:
                for(i=0;i<samples_per_block;i++){
                    chan_out_byte(output_c, (*((uint16_t *)(stage11_buffer+i*sizeof(uint16_t))))>>8 & 0xFF);
                    chan_out_byte(output_c, (*((uint16_t *)(stage11_buffer+i*sizeof(uint16_t)))) & 0xFF);
                }
                break;
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    chan_out_word(output_c,*(uint32_t *)(stage11_buffer+i*sizeof(uint32_t)));
                }
                break;
        }
        
    }
}

void process_stage_12(chanend_t input_c, chanend_t output_c)
{
    uint32_t i;
    uint32_t* stage12_buffer;

    uint8_t temp_byte_buf[2];   //temp buffer for receiving 16-bit samples

    uint8_t bit_depth, num_chan;

    uint16_t samples_per_block;


    bit_depth=chan_in_byte(input_c);
    num_chan=chan_in_byte(input_c);

    samples_per_block = appconfFRAME_ADVANCE *num_chan;


    switch (bit_depth){
        case 16:
            stage12_buffer = malloc(samples_per_block * sizeof(uint16_t));
            break;
        case 24:
        case 32:
            stage12_buffer = malloc(samples_per_block * sizeof(uint32_t));
            break;
    }


    while(1) {        
        switch (bit_depth){
            case 16:
                for(i=0;i<samples_per_block;i++){
                    temp_byte_buf[0]=chan_in_byte(input_c);
                    temp_byte_buf[1]=chan_in_byte(input_c);
                    *(uint16_t *)(stage12_buffer+i*sizeof(uint16_t))=temp_byte_buf[0]<<8 | temp_byte_buf[1];
                }
                break;
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    *(uint32_t *)(stage12_buffer+i*sizeof(uint32_t))=chan_in_word(input_c);
                }
                break;
        }

        //Tile 1, core 2
        //Add your processing here with buffer of stage12_buffer


        switch (bit_depth){
            case 16:
                for(i=0;i<samples_per_block;i++){
                    chan_out_byte(output_c, (*((uint16_t *)(stage12_buffer+i*sizeof(uint16_t))))>>8 & 0xFF);
                    chan_out_byte(output_c, (*((uint16_t *)(stage12_buffer+i*sizeof(uint16_t)))) & 0xFF);
                }
                break;
            case 24:
            case 32:
                for(i=0;i<samples_per_block;i++){
                    chan_out_word(output_c,*(uint32_t *)(stage12_buffer+i*sizeof(uint32_t)));
                }
                break;
        }
        
    }
}

