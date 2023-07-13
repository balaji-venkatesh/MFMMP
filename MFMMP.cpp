#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"

#define MA_SZ 16  // matrix size
#define NO_SZ 32  // number size
#define ST_SZ 512 // stream size

#define DWIDTH 32
#define type ap_int<DWIDTH>
typedef hls::axis<type, 0, 0, 0> pkt;

void MFMMP(hls::stream<pkt> &din, hls::stream<pkt> &dout){

    //#pragma HLS pipeline
    #pragma HLS interface axis port = din
    #pragma HLS interface axis port = dout
    #pragma HLS interface ap_ctrl_none port = return // removes start and finish signals - starts when there is data

    pkt din_tmp;
    pkt dout_tmp;

    ap_uint<NO_SZ> A [MA_SZ];
    ap_uint<NO_SZ> B [MA_SZ];
    ap_uint<NO_SZ> C [MA_SZ];

    // read in data for matrix A
    for (int i = 0; i < MA_SZ; i++) {
        #pragma HLS unroll
        int j = i % (ST_SZ / NO_SZ / 2);
        if(j == 0)
            din_tmp = din.read();
        A[i] = din_tmp(j * NO_SZ + NO_SZ - 1, j * NO_SZ);
    }

    // read in data for matrix B
    for (int i = 0; i < MA_SZ; i++) {
        #pragma HLS unroll
        int j = i % (ST_SZ / NO_SZ / 2);
        if(j == 0)
            din.read(din_tmp);
        B[i] = din_tmp(j * NO_SZ + NO_SZ - 1, j * NO_SZ);
    }

    // other dout_tmps
    pkt dout_tmp_1;

    // multiply matrices and sum full
    dout_tmp = 0;
    for (int i = 0; i < MA_SZ/2; i++) {
        #pragma HLS unroll
        C[i] = A[i] + B[i]; // C = A * B
        dout_tmp = dout_tmp + C[i];
    }
    dout_tmp1 = 0;
    for (int i = MA_SZ/2; i < MA_SZ; i++) {
        #pragma HLS unroll
        C[i] = A[i] + B[i]; // C = A * B
        dout_tmp1 = dout_tmp_1 + C[i];
    }
    dout.write(dout_tmp);
    dout.write(dout_tmp1);


    // multiply matrices and sum to half
    dout_tmp = 0;
    for (int i = 0; i < MA_SZ; i++) {
        #pragma HLS unroll
        C[i] = A[i] + B[i]; // C = A * B
        dout_tmp = dout_tmp + C[i];
    }

    // multiply matrices and sum quarter
    dout_tmp = 0;
    for (int i = 0; i < MA_SZ; i++) {
        #pragma HLS unroll
        C[i] = A[i] + B[i]; // C = A * B
        dout_tmp = dout_tmp + C[i];
    }
    
    dout.write(dout_tmp);

}
