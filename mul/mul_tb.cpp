#include <iostream>
#include <cstdlib>
#include "ap_int.h"
void mul(
    ap_uint<32> &din0,
    ap_uint<32> &din1,
    ap_uint<32> &dout
);

int main() {
    ap_uint<32> din0;
    ap_uint<32> din1;
    ap_uint<32> dout;

    ap_uint<32> expected_output;

    int pass = 1;

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 4; j++) {
            din0(j*8+7,j*8) = rand()%256;
            din1(j*8+7,j*8) = rand()%256;
            expected_output(j*8+7,j*8) = ap_uint<8>(din0(j*8+7,j*8)) * ap_uint<8>(din1(j*8+7,j*8));
        }
        mul(din0,din1,dout);
        if (dout != expected_output) {
            std::cout << "Test failed at iteration " << i << "\n" <<
            "input 0 : (" << (din0(7,0)) << "," << (din0(15,8)) << "," << (din0(23,16)) << "," << (din0(31,24)) << ")\n" <<
            "input 1 : (" << (din1(7,0)) << "," << (din1(15,8)) << "," << (din1(23,16)) << "," << (din1(31,24)) << ")\n" <<
            "output : (" << (dout(7,0)) << "," << (dout(15,8)) << "," << (dout(23,16)) << "," << (dout(31,24)) << ")\n" <<
            "expected : (" << (expected_output(7,0)) << "," << (expected_output(15,8)) << "," << (expected_output(23,16)) << "," << (expected_output(31,24)) << ")\n";
            pass = 0;
            break;
        }
    }
    if (pass)
        std::cout << "Success: results match" << std::endl;

    return 0;
}
