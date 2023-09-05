// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef TEST_PROC_H
#define TEST_PROC_H

#include <stdint.h>

#include "wav_utils.h"


extern wav_header input_header_struct, output_header_struct;
extern uint8_t read_input_header_rdy;

void get_input_wav_details();


#define appconfOUT_NUM_OF_CHANNELS 2
#define appconfFRAME_ADVANCE 240


#endif // TEST_PROC_H
