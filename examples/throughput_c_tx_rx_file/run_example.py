#We assume that the Xscope FileIO Python library has been installed via pip beforehand and is available to import. Please see readme for instuctions.
import numpy as np
import xscope_fileio
import os


firmware_xe = os.path.dirname(os.path.realpath(__file__)) + "/fileio_test.xe"
adapter_id = "NA7A9UPT" #enter your xtag id here. Use xrun -l to find out what it is


xscope_fileio.run_on_target(adapter_id, firmware_xe, use_xsim=False)


print("script finished")