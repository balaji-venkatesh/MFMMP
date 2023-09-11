# A Multi FPGA Matrix Acceleration Platform

## Progress

I've only gotten as far as a single core implementation. It tends to run through the CNN test in a few seconds (when it actually runs, that is).

## Items

- `MFMMP.cpp` 
    - main multiplier core
    - originally written using 32-bit integers, now using floats
    - interacts with other cores using 512-bit AXI Streams for input and output
        - the data structure used interally is an array of 16 floats
- `MFMMP_tb.cpp` 
    - testbench using integers
- `MFMMP_cnntb.cpp` 
    - testbench using floats from a CNN
    - includes `util/`
- `MFMMP.tcl`
    - HLS simulation setup
- `MFMMP_HLS/` 
    - HLS project used to solve into Verilog IP 
- `MFMMP_proj/` 
    - also an HLS project used to solve into Verilog IP
- `MFMMP_RTL/` 
    - block design using MFMMP IP and EasyDMA 
- `easydma-modifications/example_bd.tcl` 
    - example block design modified with full names of IPs for Vitis 2023.1
    - was used to create `MFMMP_RTL``
- `software/cnn_DMA.cpp`
    - CNN tester that uses MFMMP core
    - communicates with core through EasyDMA

## Known Issues

- In the latest set of changes, the output stream from the core simply echoes the input stream, rather than providing any actually output data. I have not been able to find the error, but it may be due to a configuration error or other issue with `software/cnn_DMA.cpp` that I wasn't able to debug in time.

- The CNN tester and MFMMP core are currently configured to send single 1x1 arrays for testing simplicity. For performance, expand the array to include the marker `//Weighted Sum: MARKER` in `MFMMP_cnntb.cpp`, and expand `MATRIX_SIZE` to 192x64x1.