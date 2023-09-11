#include "libeasydma.h"
#include <boost/thread/thread.hpp>
#include <chrono>
#include <iostream>

#include <cstdlib>

#include "util/shared.h"
#include <cmath>
#include <fstream>
#include <stdlib.h>
#include <vector>

using namespace std;

void host_tx(h2c *&h2c_chan, u32 pkt_size);
void host_rx(c2h *&c2h_chan, u32 pkt_size);

// Limits
#define MAX_BATCH 10
#define MAX_KERNEL_SIZE 7
#define MAX_INPUT_DIMS 384
#define MAX_OUTPUT_DIMS 384
#define MAX_INPUT_WIDTH 230
#define MAX_INPUT_HEIGHT 230
#define MAX_OUTPUT_WIDTH 112
#define MAX_OUTPUT_HEIGHT 112
#define MAX_CONV_INPUT MAX_INPUT_DIMS *MAX_INPUT_WIDTH *MAX_INPUT_HEIGHT
#define MAX_CONV_OUTPUT MAX_OUTPUT_DIMS *MAX_OUTPUT_WIDTH *MAX_OUTPUT_HEIGHT

#define MATRIX_SIZE 1  // matrix size (in number of floats)
#define STREAM_SIZE 16 // stream size (in number of floats)

pkt to_MFMMP_tmp, from_MFMMP_tmp;
hls::stream<pkt> to_MFMMP, from_MFMMP;

float A[MATRIX_SIZE];
float B[MATRIX_SIZE];

int counterhelp = 0;

int main(int argc, char **argv) {

    // EASYDMA SETUP

    int dev_id;
    if (argc != 2) {
        cerr << "require exactly 1 argument" << endl;
        return -1;
    }
    dev_id = atoi(argv[1]);
    easydma_dev dev = {dev_id};
    huge_pages buffer_pool = {{0}};

    dev.bind_buffer_pool(&buffer_pool);
    dev.build_channels();

    c2h *c2h_chan = dev.c2h_chans[0].get();
    h2c *h2c_chan = dev.h2c_chans[0].get();

    if (c2h_chan->is_occupied()) {
        cerr << "Error: channel has been occupied by other processes" << endl;
        return -1;
    } else if (c2h_chan->is_allocated()) {
        c2h_chan->request();
    } else {
        c2h_chan->allocate(0, HUGE_PAGE_SIZE, 0, 0);
    }

    if (h2c_chan->is_occupied()) {
        cerr << "Error: channel has been occupied by other processes" << endl;
        return -1;
    } else if (h2c_chan->is_allocated()) {
        h2c_chan->request();
    } else {
        h2c_chan->allocate(0, HUGE_PAGE_SIZE, 0, 0, 1024);
    }

    // CNN Setup

    float *weights;
    float *biases;
    float *inputs;
    float *gold_outputs;
    float *outputs;
    int retval = 0;

    string imageDir = "nn_params/inception_3a_1x1/";
    if (argc == 2)
        imageDir = argv[1];
    cout << "Using test " << imageDir << endl;

    cout << "Starting Convolution Test " << endl;

    vector<int> input_params = readFile(imageDir + "/input", inputs, MAX_CONV_INPUT * MAX_BATCH);
    vector<int> output_params = readFile(imageDir + "/output", gold_outputs, MAX_CONV_OUTPUT * MAX_BATCH);
    vector<int> weight_params = readFile(imageDir + "/weights", weights, MAX_OUTPUT_DIMS * MAX_INPUT_DIMS * MAX_KERNEL_SIZE * MAX_KERNEL_SIZE);
    vector<int> bias_params = readFile(imageDir + "/biases", biases, MAX_OUTPUT_DIMS);

    int num_inputs = 1;
    int num_outputs = 1;
    int num_weights = 1;
    int num_biases = 1;

    for (int i = 1; i < input_params.size(); i++)
        num_inputs *= input_params[i];

    for (int i = 1; i < output_params.size(); i++)
        num_outputs *= output_params[i];

    for (int i = 0; i < weight_params.size(); i++)

        num_weights *= weight_params[i];
    for (int i = 0; i < bias_params.size(); i++)
        num_biases *= bias_params[i];

    if (num_inputs < 0 || num_inputs > MAX_CONV_INPUT ||                                           // check num input size
        num_outputs < 0 || num_outputs > MAX_CONV_OUTPUT ||                                        // check num output size
        num_weights != input_params[1] * output_params[1] * weight_params[2] * weight_params[3] || // num weight = id *od *k *k
        num_biases != output_params[1] ||                                                          // one bias per output dimension
        output_params[2] != output_params[3] ||                                                    // output width (ox ) must equal output height (oy)
        input_params[2] != input_params[3] ||                                                      // input width (ix ) must equal output height (yy)
        weight_params[2] != weight_params[3]                                                       // kernel width (kx ) must equal kernel height (ky)
    ) {
        cerr << "Problem with input files\n";
        retval = 1;
    } else {
        outputs = new float[MAX_BATCH * MAX_CONV_OUTPUT];

        int b = input_params[0];
        int od = output_params[1];
        int ox = output_params[2];
        int oy = output_params[3];
        int id = input_params[1];
        int ix = input_params[2];
        int iy = input_params[3];
        int k = weight_params[3];
        int s = (ix - k) / (ox - 1);

        cout << "Begin Test\n"
             << "Batch Size: " << b << endl
             << "Num Inputs: " << num_inputs << endl
             << "Num Outputs: " << num_outputs << endl
             << "Num Weights: " << num_weights << endl
             << "Num Biases: " << num_biases << endl
             << "Input Dimensions " << b << " x " << id << " x " << ix << " x " << iy << endl
             << "Output Dimensions " << b << " x " << od << " x " << ox << " x " << oy << endl
             << "Kernel Dimensions " << od << " x " << id << " x " << k << " x " << k << endl
             << "Stride Size: " << s << endl;

        // CONV LAYER!!!

        // Batch
        for (int b_ = 0; b_ < b; b_++) {
            // Output Dimensions (Feature Maps)
            for (int o_d = 0; o_d < od; o_d++) {
                // Output Y Dimension
                for (int o_y = 0; o_y < oy; o_y++) {
                    // Output X Dimension
                    for (int o_x = 0; o_x < ox; o_x++) {
                        // Set bias
                        outputs[b_ * od * ox * oy + o_d * ox * oy + o_y * ox + o_x] = biases[o_d];

                        // Weighted Sum: MARKER

                        // Input Dimensions (Feature Maps)
                        for (int i_d = 0; i_d < id; i_d++) {
                            // Input Y Dimension
                            for (int i_y = o_y * s, iiy = 0; i_y < o_y * s + k; i_y++, iiy++) {
                                // Input X Dimension
                                float output = 0.0;
                                for (int i_x = o_x * s, iix = 0; i_x < o_x * s + k; i_x++, iix++) {
                                    /* outputs[b_ * od * ox * oy + o_d * ox * oy + o_y * ox + o_x] +=
                                        (inputs[b_ * id * ix * iy + i_d * ix * iy + i_y * ix + i_x] *
                                         weights[o_d * id * k * k + i_d * k * k + iiy * k + iix]); */
                                    A[i_x] = inputs[b_ * id * ix * iy + i_d * ix * iy + i_y * ix + i_x];
                                    B[iix] = weights[o_d * id * k * k + i_d * k * k + iiy * k + iix];
                                    // cout << i_x << "\t" << i_y << "\t" << i_d << endl;

                                    void *ptr = h2c_chan->get_chan_ptr();

                                    *(float *)((char *)ptr + 0) = A[i_x];
                                    *(float *)((char *)ptr + 4) = A[iix];

                                    u32 err = h2c_chan->commit(64);
                                    // cout << "send status: " << err << endl;

                                    u32 size = c2h_chan->get_new_pkt();
                                    // cout << "response sz: " << size << endl;

                                    void *recptr = c2h_chan->get_pkt_ptr();
                                    output += *((float *)recptr);
                                    // cout << "output: " << rec << endl;
                                }
                                outputs[b_ * od * ox * oy + o_d * ox * oy + o_y * ox + o_x] += output;
                            }
                        }

                        // Activaton Function
                        outputs[b_ * od * ox * oy + o_d * ox * oy + o_y * ox + o_x] =
                            std::max(0.0f, outputs[b_ * od * ox * oy + o_d * ox * oy + o_y * ox + o_x]);
                    }
                }
            }
        }

        // Check outputs
        float total = 0.0f;
        for (int i = 0; i < b * num_outputs; i++) {
            float err = fabs(outputs[i] - gold_outputs[i]);
            total += err * err;
        }
        float avg_error = total / (b * num_outputs);
        cout << "Mean Square Error " << avg_error << endl;
    }

    return retval;
}
