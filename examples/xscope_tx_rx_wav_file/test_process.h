// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#ifndef TEST_PROC_H
#define TEST_PROC_H

#include <stdint.h>

#include "wav_utils.h"


extern wav_header input_header_struct, output_header_struct;
extern uint8_t read_input_header_rdy;

void get_input_wav_details();

void process_stage_01(chanend_t input_c, chanend_t output_c);
void process_stage_11(chanend_t input_c, chanend_t output_c);


#endif // TEST_PROC_H
