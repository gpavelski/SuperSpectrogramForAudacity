#include "Model/FFTWContainer.h"

FFTWContainer::FFTWContainer(int numSegments, int windowLength)
    : container(numSegments)
{
    fftw_init_threads();  // optional: ensure it runs once globally

    for (auto& instance : container) {
        instance.signal = std::vector<fftw_complex>(windowLength);
        instance.result = std::vector<fftw_complex>(windowLength);
        instance.plan = fftw_plan_dft_1d(
            windowLength,
            instance.signal.data(),
            instance.result.data(),
            FFTW_FORWARD,
            FFTW_ESTIMATE);
    }
}

FFTWContainer::~FFTWContainer() {
    for (auto& instance : container) {
        fftw_destroy_plan(instance.plan);
    }
    fftw_cleanup_threads();  // optional: cleanup FFTW thread resources
}
