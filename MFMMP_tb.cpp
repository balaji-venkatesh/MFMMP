/* #include <cstdlib>
#include <iostream>

#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"

#define MA_SZ 128  // matrix size
#define NO_SZ 32  // number size
#define ST_SZ 512 // stream size

typedef hls::axis<ap_int<ST_SZ>, 0, 0, 0> pkt;

void MFMMP(hls::stream<pkt> &to_MFMMP, hls::stream<pkt> &from_MFMMP);

#pragma HLS interface axis port = to_MFMMP
#pragma HLS interface axis port = from_MFMMP

int main() {
  
    pkt to_MFMMP_tmp, from_MFMMP_tmp;
    hls::stream<pkt> to_MFMMP, from_MFMMP;
    ap_uint<32> expected_output;

    ap_uint<NO_SZ> A [MA_SZ];
    ap_uint<NO_SZ> B [MA_SZ];

    int pass = 1;

    for (int i = 0; i < 100; i++) {

        std::cout << "============================== TEST " << i << " ==============================\n";

        // randomize and calculate sum of products
        expected_output = 0;
        for (int j = 0; j < MA_SZ; j++){
            A[j] = rand() % 128;
            B[j] = rand() % 128;
            expected_output = expected_output + A[j] * B[j];
        }

        // read in data for matrix A
        std::cout << "A:";
        for (int j = 0; j < MA_SZ; j++){
            int k = j % (ST_SZ / NO_SZ / 2);
            to_MFMMP_tmp.data(k * 32 + 31, k * 32) = A[j];
            std::cout << "\t" << A[j];
            if(k == 7){
                to_MFMMP.write(to_MFMMP_tmp);
                std::cout << "\n";
            }
        }
        // read in data for matrix B
        std::cout << "B:";
        for (int j = 0; j < MA_SZ; j++){
            int k = j % (ST_SZ / NO_SZ / 2);
            to_MFMMP_tmp.data(k * 32 + 31, k * 32) = B[j];
            std::cout << "\t" << B[j];
            if(k == 7){
                to_MFMMP.write(to_MFMMP_tmp);
                std::cout << "\n";
            }
        }

        MFMMP(to_MFMMP, from_MFMMP);

        from_MFMMP.read(from_MFMMP_tmp);
        std::cout << "OUT:" << "\t" << from_MFMMP_tmp.data << "\n";
        std::cout << "EXP:" << "\t" << expected_output << "\n";

        if (from_MFMMP_tmp.data != expected_output)
            pass = 0;
    }

    std::cout << "==============================  DONE  ==============================\n";

    if (pass)
        std::cout << "ALL RESULTS MATCH\n\n";
    else
        std::cout << "SEE ERRORS ABOVE\n\n";

    return 0;
}
 */