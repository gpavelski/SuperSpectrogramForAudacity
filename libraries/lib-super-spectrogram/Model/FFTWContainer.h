#ifndef FFTW_CONTAINER_H
#define FFTW_CONTAINER_H

#include <vector>

#include "fftw3.h"
#include "Model/FFTData.h"

class FFTWContainer {
public:
    explicit FFTWContainer(int numSegments, int windowLength);
    ~FFTWContainer();

    FFTData& operator[](size_t i) { return container[i]; }
    const FFTData& operator[](size_t i) const { return container[i]; }

    std::vector<FFTData>& data() { return container; }
    const std::vector<FFTData>& data() const { return container; }

    size_t size() const { return container.size(); }
  
private:  
    std::vector<FFTData> container;
};

#endif // FFTW_CONTAINER_H
