#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define MATRIX_SIZE 1  // matrix size (in number of floats)
#define STREAM_SIZE 16 // stream size (in number of floats)

struct innerpkt {
    float data[STREAM_SIZE];
};

typedef hls::axis<struct innerpkt, 0, 0, 0> pkt;

void MFMMP(hls::stream<pkt> &din, hls::stream<pkt> &dout) {

// #pragma HLS pipeline
#pragma HLS interface axis port = din
#pragma HLS interface axis port = dout
#pragma HLS interface ap_ctrl_none port = return // removes start and finish signals - starts when there is data

    pkt din_tmp;
    pkt dout_tmp;

    float A[MATRIX_SIZE];
    float B[MATRIX_SIZE];
    float C[MATRIX_SIZE];

    int stream_index = 0;

    // read in data for matrix A
    for (int matrix_index = 0; matrix_index < MATRIX_SIZE; matrix_index++) {
#pragma HLS unroll
        if (stream_index % STREAM_SIZE == 0)
            din.read(din_tmp);
        A[matrix_index] = din_tmp.data.data[stream_index];
        stream_index++;
    }

    // read in data for matrix B
    for (int matrix_index = 0; matrix_index < MATRIX_SIZE; matrix_index++) {
#pragma HLS unroll
        if (stream_index % STREAM_SIZE == 0)
            din.read(din_tmp);
        B[matrix_index] = din_tmp.data.data[stream_index];
        stream_index++;
    }

    // multiply matrices and sum full
    dout_tmp.data.data[0] = 0;
    for (int i = 0; i < MATRIX_SIZE; i++) {
#pragma HLS unroll
        C[i] = A[i] * B[i]; // C = A * B
        dout_tmp.data.data[0] = dout_tmp.data.data[0] + C[i];
    }

    dout.write(dout_tmp);

}
