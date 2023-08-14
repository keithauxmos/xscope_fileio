xscope_tx_rx_wav_file
===============================
This example is for showing the way to read a wav file from xscope and send out the processing result as another wav file through xscope.


- Compiling the example
To compile this example, at the example root path, i.e. examples/throughput_c_tx_rx_file , enter "xmake".
If compilation is successful, a file "fileio_test.xe" is generated.

- Running the example
1. Inside the file "run_example.py", there is a value "adapter_id" that needs to be replaced with the xTAG's Adpater ID that currently be used.
To check the Adapter ID, enter "xrun -l", 

2. To run the example, at example root path, i.e. examples/throughput_c_tx_rx_file , enter "python run_example.py".
Note that Python 3.7 or above is required.

The input file name "testin.wav" is defined in the code and be read when the script is run.
After finishing running the script, the output file "testout.wav" is stored at the same path. It should be an identical file as "testin.wav"

- Audio processing function
A function "process_frame" is available for adding audio processing codes. Currently its content is a dummy flow that just simply copy data at the input_buf to output_buf

