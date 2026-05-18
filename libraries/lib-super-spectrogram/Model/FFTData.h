/**********************************************************************

  Audacity: A Digital Audio Editor

  FFTData.h

  Guilherme Pavelski

**********************************************************************/

#pragma once

#include <vector>

#include "fftw3.h"

struct FFTData {
    std::vector<fftw_complex> signal;
    std::vector<fftw_complex> result;
    fftw_plan plan;
};

