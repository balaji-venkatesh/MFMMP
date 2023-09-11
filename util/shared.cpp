#include <iostream>
#include <fstream>
#include <vector>

#include "shared.h"

using namespace std;

std::vector <int> readFile(const string fname,
                     float *& fptr,
                     const int max_alloc)
{
  int* read_sizes = NULL;

  std::vector<int> read_sizes_vec;
  ifstream in_file(fname.c_str(), ios::in | ios::binary);
  if (in_file.is_open())
  {
    // Read header
    int read_dims;
    in_file.read(reinterpret_cast<char*>(&read_dims), sizeof(int));
    fptr = new float[max_alloc];
    read_sizes = new int[read_dims];

    // Read dimension data
    in_file.read(reinterpret_cast<char*>(read_sizes),
                 sizeof(int)*read_dims);

    // Read the array
    int read_alloc = 1;
    for (int i = 0; i < read_dims; i++){
      read_alloc *= read_sizes[i];
      read_sizes_vec.push_back(read_sizes[i]);
    }

    if (read_alloc <= max_alloc)
      in_file.read(reinterpret_cast<char*>(&fptr[0]), sizeof(float)*read_alloc);
    else
    {
      cerr << "Desired dimensions too large\n";
      delete [] read_sizes;
      delete [] fptr;
      read_sizes = NULL;
    }
    in_file.close();
  }
  else
    cerr << "Couldn't open file: " << fname << endl;

  delete [] read_sizes;
  return read_sizes_vec;
}
