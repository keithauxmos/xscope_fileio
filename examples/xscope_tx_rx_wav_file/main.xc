#include <platform.h>
#include <xs1.h>
#include <xscope.h>
#include "test_process.h"

extern "C" {
void main_tile0(chanend, chanend, chanend);
}

// void main_tile0(chanend xscope_chan, chanend samples_out_c);

int main (void)
{
  chan xscope_chan;
  chan c01, c02;
  chan c11, c12;
  chan c0_out_to_xtag;
  
  par
  {
    xscope_host_data(xscope_chan);
    on tile[0]: {
      par{
        main_tile0(xscope_chan,c01,c0_out_to_xtag);
        process_stage_01(c01,c11);
      }
    }
    on tile[1]: {
      par{
        process_stage_11(c11,c0_out_to_xtag);
      }
    }
  }
  return 0;
}

