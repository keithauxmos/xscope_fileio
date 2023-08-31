xscope_tx_rx_wav_file
===============================
This example is for showing the way to read a wav file from xscope and send out the processing result as another wav file through xscope.


-- Compiling the example
To compile this example, at the example root path, i.e. examples/throughput_c_tx_rx_file , enter "xmake".
If compilation is successful, a file "fileio_test.xe" is generated.

-- Running the example
1. Inside the file "run_example.py", there is a value "adapter_id" that needs to be replaced with the xTAG's Adpater ID that currently be used.
To check the Adapter ID, enter "xrun -l", 

2. To run the example, at example root path, i.e. examples/throughput_c_tx_rx_file , enter "python run_example.py".
Note that Python 3.7 or above is required.

The input file name "testin_ch6_32bit.wav" is defined in the code as "appconfINPUT_FILENAME" and be read when the script is run.
It is an 6-ch wav file with 32-bit bit depth. 
After finishing running the script, the output file "testout.wav" is stored at the same path.
Another two test wav file with 16-bit and 24-bit depth are also available.

-- Audio processing function
Samples in a multi-channel wav file is interleaved. The function "deinterleaveFrame()" de-interleave the samples and the result is written to the buffer "p_audio_in_channels_16(for 16-bit  wav) or p_audio_in_channels_32(for 24-bit or 32-bit wav). For example, p_audio_in_channels_32[0] to p_audio_in_channels_32[239] are the samples for ch1, while p_audio_in_channels_32[240] to p_audio_in_channels_32[479] are the samples for ch2.

The function "process_frame()" is then called as a function space for adding audio processing. Current it only copies the first two channels of p_audio_in_channels_16/32 to p_audio_out_channels_16/32.

After audio processing function, the output p_audio_out_channels_16/32 is then used as the input for the function "interleaveFrame()" so that process samples are interleaved and can be used as the data of the output wav file.


