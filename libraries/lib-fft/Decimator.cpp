/**********************************************************************

  Audacity: A Digital Audio Editor

  Decimator.cpp

  Guilherme Pavelski

*******************************************************************//**

\class Decimator
\brief Constructs a fractional decimator with an anti-aliasing IIR filter.

This class is responsible for downsampling the selected audio for
reducing the computational overhead of the STFT. Besides, it also
applies a low-pass anti-aliasing filter to the signal.
*//*******************************************************************/

#include "Decimator.h"

/**
 * @brief Construct a Decimator instance.
 *
 * This constructor computes the appropriate decimation factor based on the input
 * sampling frequency (`frequencyRate`) and the desired target frequency (`targetRate`).
 * It then designs a Chebyshev Type I low-pass filter to pre-filter the signal
 * before decimation to prevent aliasing.
 *
 * @param frequencyRate The original sampling rate of the input signal (Hz).
 * @param targetRate The desired target sampling rate after decimation (Hz).
 *
 * @throws std::invalid_argument If either frequency is non-positive.
 * @throws std::runtime_error If the computed decimation factor is invalid (<= 0).
 */
Decimator::Decimator(double frequencyRate, double targetRate)
{
   if (frequencyRate <= 0.0 || targetRate <= 0.0)
      throw std::invalid_argument("Frequencies must be positive");

   step = computeDecimationLevel(frequencyRate, targetRate);

   if (step <= 0.0)
      throw std::runtime_error("Computed decimation factor is invalid");

   // ----------------------------------------------------
   // Chebyshev Type I low-pass filter design
   // ----------------------------------------------------
   const double Wn = 0.8 / step;
   const double rp = 0.05;

   Cheby1LowPassIIRFilter filter(filter_order, rp, Wn);

   b_coeffs = filter.b();
   a_coeffs = filter.a();
}

/**
 * @brief Compute the decimation factor given input and target rates.
 *
 * This method calculates the ratio of the input rate to the target rate.
 * The decimation factor is at least 1 (no upsampling).
 *
 * @param inputRate The sampling rate of the input signal.
 * @param targetRate The desired target sampling rate.
 * @return The computed decimation factor (>= 1).
 *
 * @throws std::invalid_argument If targetRate is non-positive.
 */
double Decimator::computeDecimationLevel(double inputRate, double targetRate) const
{
   if (targetRate <= 0.0)
      throw std::invalid_argument("targetRate must be > 0");

   const double ratio = inputRate / targetRate;
   return ratio < 1.0 ? 1.0 : ratio;
}

/**
 * @brief Apply an IIR filter to a signal using Direct Form I implementation.
 *
 * This function performs standard causal IIR filtering on the input vector `x`
 * using numerator coefficients `b` and denominator coefficients `a`.
 * The filtered output is stored in `y`.
 *
 * @param x Input signal vector.
 * @param y Output filtered signal vector (resized inside the function).
 * @param b Numerator coefficients of the IIR filter.
 * @param a Denominator coefficients of the IIR filter (a[0] should be non-zero).
 */
void Decimator::iirFilter(
   const std::vector<double>& x,
   std::vector<double>& y,
   const std::vector<double>& b,
   const std::vector<double>& a)
{
   const size_t N = x.size();
   const size_t order = a.size() - 1;

   y.assign(N, 0.0);

   for (size_t i = 0; i < N; ++i) {
      y[i] = b[0] * x[i];
      for (size_t j = 1; j <= order; ++j) {
         if (i >= j) {
            y[i] += b[j] * x[i - j];
            y[i] -= a[j] * y[i - j];
         }
      }
   }
}

/**
 * @brief Apply zero-phase IIR filtering (forward-backward) to a signal.
 *
 * This method mimics MATLAB's filtfilt behavior:
 * 1. Filters the signal forward using `iirFilter`.
 * 2. Reverses the filtered signal.
 * 3. Filters the reversed signal (backward pass) using the same IIR coefficients.
 * 4. Reverses the result to produce a zero-phase, non-delayed output.
 *
 * This approach eliminates phase distortion and effectively squares the magnitude
 * response of the filter.
 *
 * @param x Input signal vector.
 * @param b Numerator coefficients of the IIR filter.
 * @param a Denominator coefficients of the IIR filter.
 * @return Filtered signal vector with zero-phase distortion.
 */
std::vector<double> Decimator::filtfilt(
   const std::vector<double>& x,
   const std::vector<double>& b,
   const std::vector<double>& a)
{
   std::vector<double> y, yr, y2;

   // forward
   iirFilter(x, y, b, a);

   // reverse
   yr = y;
   std::reverse(yr.begin(), yr.end());

   // backward
   iirFilter(yr, y2, b, a);

   // reverse back
   std::reverse(y2.begin(), y2.end());

   return y2;
}
