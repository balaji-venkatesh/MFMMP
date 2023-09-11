#include "libeasydma.h"
#include <boost/thread/thread.hpp>
#include <chrono>
#include <iostream>

using namespace std;

void host_tx(h2c *&h2c_chan, u32 pkt_size);
void host_rx(c2h *&c2h_chan, u32 pkt_size);

int main(int argc, char **argv) {
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

    void *ptr = h2c_chan->get_chan_ptr();
    /*     for (int i = 0; i < TRAFFIC_SIZE; i+=4) {
            *(u32 *)((char *)ptr + i) = i/4;
        }
     */

    *(float *)((char *)ptr + 0) = 21.2;
    *(float *)((char *)ptr + 4) = 21.3;
    *(float *)((char *)ptr + 8) = 21.4;
    *(float *)((char *)ptr + 12) = 21.5;

    u32 err = h2c_chan->commit(64);
    cout << "send status: " << err << endl;

    u32 size;

    /*     for (;;) {
     */
    // c2h_chan->get_pkt_ptr
    size = c2h_chan->get_new_pkt();
    cout << "response sz: " << size << endl;

    void *recptr = c2h_chan->get_pkt_ptr();
    float rec = *((float *)recptr);

    cout << "output: " << rec << endl;
    /// EEEEEE HELP!!!!
    // size = c2h_chan->get_new_pkt();
    // cout << "response sz: " << size << endl;

    // recptr = c2h_chan->get_chan_ptr();
    rec = *((float *)(recptr + 4));
    rec = *((float *)(recptr + 8));
    rec = *((float *)(recptr + 12));

    cout << "output: " << rec << endl;

    return 0;
}
