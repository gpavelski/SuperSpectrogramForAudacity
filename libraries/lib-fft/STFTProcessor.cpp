/**********************************************************************

  Audacity: A Digital Audio Editor

  STFTProcessor.cpp

  Guilherme Pavelski

*******************************************************************//**

\class STFTProcessor
\brief Responsible for the steps of computing an STFT.

This is a low-level class that is responsible for performing a
Short-Term Fourier Transform on the input audio signal and returning
a 2-Dimensional array representing the results of the calculation.

*//*******************************************************************/

#include "STFTProcessor.h"

/**
 * Computes a Gaussian window of length 16*sigma centered at 8*sigma.
 * Used for windowing signal segments before FFT.
 */
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

/**
 * Computes starting positions (cursors) for each window segment in the signal.
 * @param numSegments Number of segments/windows.
 * @param startCursor Initial starting index in the signal.
 * @return Vector of starting indices for each segment.
 */
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

/**
 * Updates a portion of the spectrogram matrix from a single STFT computation.
 * Handles placement of the transformed segment into the correct indices.
 */
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

/**
 * Processes all signal windows, computing the STFT for each, and fills the flat spectrogram.
 * Can execute window processing in parallel for performance.
 */
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

/**
 * Processes overlapping edge windows to remove boundary effects in the spectrogram.
 */
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

/**
 * Computes the Short-Time Fourier Transform (STFT) of a single time-domain segment.
 * Applies the Gaussian window and executes FFT on each windowed portion.
 * @param timeSignal Input signal segment.
 * @param fftData FFT container with plan and buffers.
 * @return Flattened STFT magnitude vector for this segment.
 */
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

/**
 * Converts a linear magnitude to dB and applies a lower/upper threshold.
 * Values outside thresholds are set to -INFINITY.
 */
double STFTProcessor::applyLogAndThreshold(double value) const {
    double db = 20 * std::log10(value);
    if (db < lowerThreshold || db > upperThreshold) {
        return -std::numeric_limits<double>::infinity();
    }
    return db;
}

/**
 * Inserts a transformed STFT segment into the spectrogram with log scaling and thresholding.
 * Maps the flat transformed vector into a smaller segment of the full spectrogram.
 */
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

/**
 * Resizes the input signal to length N by padding with zeros if necessary.
 * Useful to ensure integer number of windows for STFT.
 */
std::vector<double> STFTProcessor::resize_signal(
    const std::vector<double>& signal, 
    size_t N) const {
    
    std::vector<double> resized_signal;
    resized_signal.reserve(N); // Reserve space to avoid multiple allocations
    resized_signal.insert(resized_signal.end(), signal.begin(), signal.end());
    resized_signal.insert(resized_signal.end(), N - signal.size(), 0.0);

    return resized_signal;
}

/**
 * Performs full STFT on the input signal and returns a 2D spectrogram matrix.
 * Handles windowing, FFT computation, edge effect removal, and flatten -> matrix conversion.
 */
std::vector<std::vector<double>> STFTProcessor::processFullSTFTMatrix(
   const std::vector<double>& data)
{
   int signal_length = static_cast<int>(data.size());
   int numSegments = std::ceil(static_cast<double>(signal_length) / windowLength);
   resizedLength = numSegments * windowLength;

   // Resize signal to match integer number of segments
   std::vector<double> resizedSignal = resize_signal(data, resizedLength);

   // Spectrogram stored as flat vector
   std::vector<double> flatSpectrogram(sigma * resizedLength, -INFINITY);

   FFTWContainer fftContainer(numSegments, windowLength);

   processWindows(flatSpectrogram, resizedSignal, numSegments, fftContainer.data());
   removeEdgeEffects(flatSpectrogram, resizedSignal, numSegments, fftContainer.data());

   // Convert to 2D matrix: 
   int rows = sigma * 8;
   int cols = static_cast<int>(flatSpectrogram.size()) / rows;

   std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));

   for (int col = 0; col < cols; ++col) {
      for (int row = 0; row < rows; ++row) {
         size_t idx = col * rows + row;
         matrix[row][col] = flatSpectrogram[idx];
      }
   }

   return matrix;
}


