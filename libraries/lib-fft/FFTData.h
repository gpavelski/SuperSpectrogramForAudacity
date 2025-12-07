#ifndef FFT_DATA_H
#define FFT_DATA_H

#include "fftw3.h"
#include <vector>

struct FFTData {
    std::vector<fftw_complex> signal;
    std::vector<fftw_complex> result;
    fftw_plan plan;
};

#endif // FFT_DATA_H
