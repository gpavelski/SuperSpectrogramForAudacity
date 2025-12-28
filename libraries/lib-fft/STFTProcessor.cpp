#include "STFTProcessor.h"

std::vector<double> STFTProcessor::computeGaussianWindow() const {
    const int size = 16 * sigma;
    const int center = 8 * sigma;
    const double denom = std::sqrt(2 * M_PI) * sigma;
    const double twoSigmaSq = 2.0 * sigma * sigma;

    std::vector<double> gwin(size);
    for (int i = 0; i < size; ++i) {
        int x = i - center;
        gwin[i] = std::exp(-(x * x) / twoSigmaSq) / denom;
    }
    return gwin;
}

std::vector<int> STFTProcessor::computeSignalCursors(
    int numSegments, 
    int startCursor) const {
    std::vector<int> cursors(numSegments);
    int cursor = startCursor;
    for (int i = 0; i < numSegments; ++i) {
        cursors[i] = cursor;
        cursor += windowLength;
    }
    return cursors;
}

void STFTProcessor::updateSpectrogramFromSTFT(
    const double* inputSegment,
    FFTData& fftData,
    std::vector<double>& spectrogram,
    size_t baseDestIndex,
    size_t baseSrcIndex,
    size_t rows,
    size_t cols) const
{
    std::vector<double> spectrogramSegment(windowSpectrogramSize, -INFINITY);

    const std::vector<double> mainTransform = computeSTFT(inputSegment, fftData);

    insertTransformedSegment(
        spectrogramSegment, mainTransform
    );

    for (size_t j = 0; j < rows; ++j) {
        for (size_t k = 0; k < cols; ++k) {
            size_t dstIndex = baseDestIndex + j * cols + k;
            size_t srcIndex = baseSrcIndex + j * windowLength + k;

            if (dstIndex < spectrogram.size() && srcIndex < spectrogramSegment.size()) {
                    spectrogram[dstIndex] = spectrogramSegment[srcIndex];
            }
        }
    }
}

void STFTProcessor::processWindows(
    std::vector<double>& spectrogram,
    const std::vector<double>& inputSignal,
    const int numSegments,
    std::vector<FFTData>& fftContainer) const
{
    auto signalCursors = computeSignalCursors(numSegments, 0);

    runInParallel(numSegments, [&](int i) {
        int signalCursor = signalCursors[i];

        const double* signalSegment = inputSignal.data() + signalCursor;
        
        updateSpectrogramFromSTFT(signalSegment, fftContainer[i], spectrogram,
            sigma * signalCursor, 0, eighthWindow, 8 * sigma);
    });
}

void STFTProcessor::removeEdgeEffects(
    std::vector<double>& spectrogram,
    const std::vector<double>& inputSignal,
    const int numSegments,
    std::vector<FFTData>& fftContainer) const
{
    auto signalCursors = computeSignalCursors(numSegments, windowLength);
    
    runInParallel(numSegments - 1, [&](int i) {
            int signalCursor = signalCursors[i];

            const double* edgeWindowSegment = inputSignal.data() + signalCursor - 8 * sigma;

            updateSpectrogramFromSTFT(edgeWindowSegment, fftContainer[i], spectrogram,
                sigma * signalCursor - 4 * sigma * sigma,
                8 * sigma * sigma, sigma, 8 * sigma);
    });
}

std::vector<double> STFTProcessor::computeSTFT(
    const double* timeSignal,
    FFTData& fftData
    ) const {
    const int cntr = windowLength / 2;
    const int outputCols = windowLength;
    const int outputRows = windowLength;
    std::vector<double> out(outputCols * outputRows, 0.0);

    std::vector<double> x(2 * windowLength, 0.0);

    for (int i = 0; i < windowLength; ++i) {
        x[cntr + i] = timeSignal[i];
    }
    
    for (int i = 0; i < windowLength; ++i) {
        for (int j = 0; j < windowLength; ++j) {
            double val = gwin[j] * x[j + i];
            fftData.signal[j][0] = val;
            fftData.signal[j][1] = 0.0;
        }

        fftw_execute(fftData.plan);

        for (int j = 0; j < windowLength; ++j) {
            double re = fftData.result[j][0];
            double im = fftData.result[j][1];
            out[i * windowLength + j] = std::sqrt(re * re + im * im);
        }
    }

    return out;
}

double STFTProcessor::applyLogAndThreshold(double value) const {
    double db = 20 * std::log10(value);
    if (db < lowerThreshold || db > upperThreshold) {
        return -std::numeric_limits<double>::infinity();
    }
    return db;
}

void STFTProcessor::insertTransformedSegment(
    std::vector<double>& spectrogram,
    const std::vector<double>& transform
) const {
    for (int row = 0; row < eighthWindow; ++row) {
        for (int col = 0; col < windowLength; ++col) {
            int dstIdx = row * windowLength + col;
            int srcIdx = 8 * row * windowLength + col;
            spectrogram[dstIdx] = applyLogAndThreshold(transform[srcIdx]);
        }
    }
}

std::vector<double> STFTProcessor::resize_signal(
    const std::vector<double>& signal, 
    size_t N) const {
    
    std::vector<double> resized_signal;
    resized_signal.reserve(N); // Reserve space to avoid multiple allocations
    resized_signal.insert(resized_signal.end(), signal.begin(), signal.end());
    resized_signal.insert(resized_signal.end(), N - signal.size(), 0.0);

    return resized_signal;
}


std::vector<double> STFTProcessor::processFullSTFT(
    const std::vector<double>& data) {
    
    int signal_length = data.size();
    int numSegments = std::ceil(static_cast<double>(signal_length) / windowLength);
    int resized_length = numSegments * windowLength;
    
    std::vector<double> spectrogram(sigma * resized_length, -INFINITY);
    std::vector<double> resizedSignal = resize_signal(data, resized_length);

    FFTWContainer fftContainer(numSegments, windowLength);

    processWindows(spectrogram, resizedSignal, numSegments, fftContainer.data());
    removeEdgeEffects(spectrogram, resizedSignal, numSegments, fftContainer.data());

    return spectrogram;
}

