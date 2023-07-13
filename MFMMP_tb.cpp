#include "ap_int.h"
#include <cstdlib>
#include <iostream>

#include "hls_stream.h"
#include "ap_axi_sdata.h"

#define MA_SZ 16  // matrix size
#define NO_SZ 32  // number size
#define ST_SZ 512 // stream size

typedef ap_axiu<512, 0, 0, 0> pkt;

void MFMMP(hls::stream<pkt> &din, hls::stream<pkt> &dout);

#pragma HLS interface axis port = din
#pragma HLS interface axis port = dout

int main() {
  
    ap_uint<512> din;
    ap_uint<512> dout;
    ap_uint<32> expected_output = 0;

    ap_uint<NO_SZ> A [MA_SZ];
    ap_uint<NO_SZ> B [MA_SZ];
    //ap_uint<NO_SZ> C [MA_SZ];

    int pass = 1;

    for (int i = 0; i < 1; i++) {

        std::cout << "TEST " << i << "\n";

        // randomize and calculate sum of products
        for (int j = 0; j < MA_SZ; j++){
            A[j] = rand() % 128;
            B[j] = rand() % 128;
            expected_output = expected_output + A[j] * B[j];
        }
        
        MFMMP(din, dout);

        ap_uint<ST_SZ> din_tmp;

        // read in data for matrix A
        std::cout << "A: " << "\n";
        for (int j = 0; j < MA_SZ; j++){
            int k = j % (ST_SZ / NO_SZ / 2);
            din_tmp(k * 32 + 31, k * 32) = A[j];
            std::cout << "\t" << A[j];
            if(k == 7){
                din.write(din_tmp)
                std::cout << "\n";
            }
        }
        // read in data for matrix B
        std::cout << "B: " << "\n";
        for (int j = 0; j < MA_SZ; j++){
            int k = j % (ST_SZ / NO_SZ / 2);
            din_tmp(k * 32 + 31, k * 32) = B[j];
            std::cout << "\t" << B[j];
            if(k == 7){
                din.write(din_tmp)
                std::cout << "\n";
            }
        }

        ap_uint<NO_SZ> dout_tmp;
        dout.read(dout_tmp);
        std::cout << "OUT: " << "\n\t" << dout_tmp << "\n";
        std::cout << "EXP: " << "\n\t" << expected_output << "\n";

        if (dout != expected_output)
            pass = 0;
    }

    if (pass)
        std::cout << "ALL RESULTS MATCH" << std::endl;

    return 0;
}
