#ifndef _SHARED_H
#define _SHARED_H

#include <string>
#include <vector>

#define MAX_BATCH 10
#define NUM_BIAS_DIMENSIONS 1
std::vector<int> readFile(const std::string fname,
                          float *& fptr,
                          const int max_alloc);

#endif
