#include "ap_int.h"
void MFMMP(
    ap_uint<32> &din0,
    ap_uint<32> &din1,
    ap_uint<32> &dout
) {
    #pragma HLS interface axis port=din0
    #pragma HLS interface axis port=din1
    #pragma HLS interface axis port=dout
    #pragma HLS interface ap_ctrl_none port=return
    ap_uint<32> dout_tmp;
    for (int i = 0; i < 4; i++) {
        #pragma HLS unroll
        dout_tmp(i*8+7,i*8) = ap_uint<8>(din0(i*8+7,i*8)) * ap_uint<8>(din1(i*8+7,i*8));
    }
    dout = dout_tmp;
}
