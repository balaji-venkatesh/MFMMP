#include "ap_int.h"

void MFMMP(ap_uint<512> &din, ap_uint<32> &dout) {

#pragma HLS pipeline
#pragma HLS interface axis port = din
#pragma HLS interface axis port = dout
#pragma HLS interface ap_ctrl_none port = return // removes start and finish signals - starts when there is data

    // multiplier
    ap_uint<256> C;
    for (int i = 0; i < 8; i++) {
#pragma HLS unroll
        C(i * 32 + 31, i * 32) = ap_uint<32>(din(i * 32 + 31, i * 32)) * ap_uint<32>(din(256 + i * 32 + 31, 256 + i * 32)); // C = A * B
    }

    // adder tree
    ap_uint<32> dout_tmp;
    dout_tmp = ((C(31, 0) + C(63, 32)) + (C(95, 64) + C(127, 96))) + ((C(159, 128) + C(191, 160)) + (C(223, 192) + C(255, 224)));

    dout = dout_tmp;
}
