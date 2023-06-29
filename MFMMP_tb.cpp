#include "ap_int.h"
#include <cstdlib>
#include <iostream>

#define MFMMP_SIZE 16
#define NUM_SIZE 32

void MFMMP(ap_uint<1024> &din, ap_uint<32> &dout);

int main() {
    
    ap_uint<1024> din;
    ap_uint<32> dout;
    ap_uint<32> expected_output;

    int pass = 1;

    for (int i = 0; i < 1; i++) {

        for (int j = 0; j < MFMMP_SIZE * 2; j++) 
            din(j * 32 + 31, j * 32) = rand() % 100;

        expected_output(31, 0)  = din(31, 0)    * din(31+512, 0+512)
                                + din(63, 32)   * din(63+512, 32+512)
                                + din(95, 64)   * din(95+512, 64+512)
                                + din(127, 96)  * din(127+512, 96+512)
                                + din(159, 128) * din(159+512, 128+512)
                                + din(191, 160) * din(191+512, 160+512)
                                + din(223, 192) * din(223+512, 192+512)
                                + din(255, 224) * din(255+512, 224+512)
                                + din(287, 256) * din(287+512, 256+512)
                                + din(319, 288) * din(319+512, 288+512)
                                + din(351, 320) * din(351+512, 320+512)
                                + din(383, 352) * din(383+512, 352+512)
                                + din(415, 384) * din(415+512, 384+512)
                                + din(447, 416) * din(447+512, 416+512)
                                + din(479, 448) * din(479+512, 448+512)
                                + din(511, 480) * din(511+512, 480+512);

        MFMMP(din, dout);

        std::cout << "TEST " << i << "\n"
                  << "  in : " << din << "\n"
                  << "  out: " << dout << "\n"
                  << "  exp: " << expected_output << "\n";
        if (dout != expected_output)
            pass = 0;
    }

    if (pass)
        std::cout << "ALL RESULTS MATCH" << std::endl;

    return 0;
}
