#include "ap_int.h"

#define MFMMP_SIZE 16
#define NUM_SIZE 32

void MFMMP(ap_uint<1024> &din, ap_uint<NUM_SIZE> &dout) {

#pragma HLS pipeline
#pragma HLS interface axis port = din
#pragma HLS interface axis port = dout
#pragma HLS interface ap_ctrl_none port = return // removes start and finish signals - starts when there is data

    ap_uint<32> dout_tmp = 0;

    ap_uint<NUM_SIZE> C [MFMMP_SIZE];
    for (int i = 0; i < MFMMP_SIZE; i++) {
#pragma HLS unroll
        C[i] = ap_uint<32>(din(i * 32 + 31, i * 32)) * ap_uint<32>(din(512 + i * 32 + 31, 512 + i * 32)); // C = A * B
        dout_tmp = dout_tmp + C[i];
    }
    
    dout = dout_tmp;
}
