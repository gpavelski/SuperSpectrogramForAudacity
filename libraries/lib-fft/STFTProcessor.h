/**********************************************************************

  Audacity: A Digital Audio Editor

  STFTProcessor.h

  Guilherme Pavelski

**********************************************************************/

#ifndef STFT_PROCESSOR_H
#define STFT_PROCESSOR_H

#include <vector>
#include <thread>
#include <mutex>
#include "Decimator.h"
#include "FFTWContainer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class STFTProcessor {
public:

    // Constructor accepting sigmaExponent
    explicit STFTProcessor(int sigmaExponent)
        : sigmaExp(sigmaExponent),
        sigma(static_cast<int>(std::pow(2, sigmaExponent))),
        windowLength(16 * sigma),
        halfWindow(windowLength / 2),
        eighthWindow(windowLength / 8),
        thirtySecondWindow(windowLength / 32),
        windowSpectrogramSize(32 * sigma * sigma),
        gwin(computeGaussianWindow()) {
    }
    
    // Main interface methods
    std::vector<double> computeGaussianWindow() const;

    std::vector<std::vector<double>> processFullSTFTMatrix(
          const std::vector<double>& data
    );

    int getSigma() const {
        return sigma;
    }

    int getWindowLength() const {
        return windowLength;
    }

    int getLowerThreshold() const {
        return lowerThreshold;
    }

    void setLowerThreshold(size_t threshold_level) {
       lowerThreshold = threshold_level;
    }

    int getWindowSpectrogramSize() const {
        return windowSpectrogramSize;
    }

    int getResizedSignalLength() const {
       return resizedLength;
    }

private:
    const int sigmaExp;
    const int sigma;
    const int windowLength;
    const int halfWindow;
    const int eighthWindow;
    const int thirtySecondWindow;
    const int windowSpectrogramSize;
    const std::vector<double> gwin;
    int lowerThreshold = -70;
    const int upperThreshold = 0;
    int resizedLength = 0;

    std::vector<int> computeSignalCursors(
        int numSegments,
        int startCursor) const;

    void updateSpectrogramFromSTFT(
        const double* inputSegment,
        FFTData& fftData,
        std::vector<double>& spectrogram,
        size_t baseDestIndex,
        size_t baseSrcIndex,
        size_t rows,
        size_t cols) const;

    template<typename Func>
    void runInParallel(int count, Func&& f) const {
        std::vector<std::thread> threads;
        for (int i = 0; i < count; ++i) {
            threads.emplace_back([&, i]() { f(i); });
        }
        for (auto& t : threads) t.join();
    };

    void processWindows(
        std::vector<double>& spectrogram,
        const std::vector<double>& inputSignal,
        const int numSegments,
        std::vector<FFTData>& fftContainer) const;

    void removeEdgeEffects(
        std::vector<double>& spectrogram,
        const std::vector<double>& inputSignal,
        const int numSegments,
        std::vector<FFTData>& fftContainer) const;

    std::vector<double> computeSTFT(
        const double* timeSignal,
        FFTData& fftData) const;

    void insertTransformedSegment(
        std::vector<double>& spectrogram,
        const std::vector<double>& transform
    ) const;

    std::vector<double> resize_signal(
        const std::vector<double>& signal,
        size_t N) const;

    // Utility
    double applyLogAndThreshold(
        double value) const;

};
#endif // STFT_PROCESSOR_H
