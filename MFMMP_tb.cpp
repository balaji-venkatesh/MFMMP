#include "ap_int.h"
#include <cstdlib>
#include <iostream>

#define MFMMP_SIZE 8

void MFMMP(ap_uint<512> &din, ap_uint<32> &dout);

int main() {
    
    ap_uint<512> din;
    ap_uint<32> dout;
    ap_uint<32> expected_output;

    int pass = 1;

    for (int i = 0; i < 100; i++) {

        for (int j = 0; j < 16; j++) 
            din(j * 32 + 31, j * 32) = rand() % 100;

        expected_output = din(31, 0)    * din(287, 256) 
                        + din(63, 32)   * din(319, 288) 
                        + din(95, 64)   * din(351, 320) 
                        + din(127, 96)  * din(383, 352) 
                        + din(159, 128) * din(415, 384) 
                        + din(191, 160) * din(447, 416) 
                        + din(223, 192) * din(479, 448) 
                        + din(255, 224) * din(511, 480);

        MFMMP(din, dout);

        if (dout != expected_output) {
            std::cout << "Test failed at iteration " << i << "\n"
                      << "A: (" << din(31, 0)    << "," << din(63, 32)   << "," << din(95, 64)   << "," << din(127, 96)  << ","
                                << din(159, 128) << "," << din(191, 160) << "," << din(223, 192) << "," << din(255, 224) << ")\n"
                      << "B: (" << din(287, 256) << "," << din(319, 288) << "," << din(351, 320) << "," << din(383, 352) << ","
                                << din(415, 384) << "," << din(447, 416) << "," << din(479, 448) << "," << din(511, 480) << ")\n" 
                      << "output : (" << (dout(31, 0)) << ")\n"
                      << "expected : (" << (expected_output(31, 0)) << ")\n";
            pass = 0;
            break;
        }
    }

    if (pass)
        std::cout << "Success: results match" << std::endl;

    return 0;
}
