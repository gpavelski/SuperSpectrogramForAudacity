/**********************************************************************

  Audacity: A Digital Audio Editor

  Cheby1LowPassIIRFilter.h

  Guilherme Pavelski

**********************************************************************/

#pragma once

#include <algorithm>
#include <cmath>
#include <complex>
#include <vector>
#include <numeric>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

/**
 * @class Cheby1LowPassIIRFilter
 * @brief Designs a digital Chebyshev Type I low-pass IIR filter.
 *
 * This class computes the coefficients of a digital low-pass filter
 * using the Chebyshev Type I prototype. The filter is represented
 * in transfer-function form (numerator `b_` and denominator `a_` coefficients)
 * suitable for direct filtering of discrete-time signals.
 *
 * Features:
 * - Chebyshev Type I prototype generation.
 * - Low-pass frequency scaling.
 * - Bilinear transformation to digital domain.
 * - Conversion from zero-pole-gain (ZPK) to transfer function form.
 * - Coefficient normalization (a[0] == 1).
 */

class Cheby1LowPassIIRFilter {
public:
   /**
   * @brief Construct a Chebyshev Type I low-pass filter.
   *
   * This constructor runs the full design pipeline:
   * 1. Generate Chebyshev Type I analog low-pass prototype poles.
   * 2. Scale to desired cutoff frequency.
   * 3. Transform to digital domain using bilinear transform.
   * 4. Convert ZPK representation to transfer-function coefficients.
   *
   * @param order Filter order (number of poles).
   * @param rp Passband ripple in dB.
   * @param Wn Normalized cutoff frequency (0 < Wn < 1, where 1 corresponds to Nyquist frequency).
   */
   Cheby1LowPassIIRFilter(int order, double rp, double Wn) {
      design(order, rp, Wn);
   }

   const std::vector<double>& b() const { return b_; }
   const std::vector<double>& a() const { return a_; }

private:
   using cdouble = std::complex<double>;
   std::vector<double> b_, a_;

   /**
    * @brief Compute polynomial coefficients from roots.
    *
    * Given a vector of roots, returns the coefficients of the polynomial
    * with those roots, i.e., \prod (x - r_i).
    *
    * @param roots Vector of complex roots.
    * @return Polynomial coefficients as complex numbers.
    */
   static std::vector<cdouble> poly(const std::vector<cdouble>& roots) {
      std::vector<cdouble> p{ 1.0 };
      for (auto r : roots) {
         std::vector<cdouble> next(p.size() + 1);
         for (size_t i = 0; i < p.size(); ++i) {
            next[i] += -r * p[i];
            next[i + 1] += p[i];
         }
         p = next;
      }
      return p;
   }

   // ------------------------------------------------------------
   // Digital Chebyshev Type I prototype
   // ------------------------------------------------------------

   /**
   * @brief Generate analog Chebyshev Type I low-pass prototype poles.
   *
   * Computes poles, zeros (empty for Type I), and gain k for an N-th order
   * Chebyshev Type I analog low-pass filter with passband ripple `rp`.
   *
   * @param N Filter order.
   * @param rp Passband ripple in dB.
   * @param z Output vector of zeros (empty for Type I).
   * @param p Output vector of poles.
   * @param k Output filter gain.
   */
   static void cheb1ap(
      int N, double rp,
      std::vector<cdouble>& z,
      std::vector<cdouble>& p,
      double& k
   ) {
      z.clear();
      p.clear();

      if (N == 0) {
         k = std::pow(10.0, -rp / 20.0);
         return;
      }

      double eps = std::sqrt(std::pow(10.0, 0.1 * rp) - 1.0);
      double mu = std::asinh(1.0 / eps) / N;

      for (int m = -N + 1; m < N; m += 2) {
         double theta = M_PI * m / (2.0 * N);
         p.emplace_back(-std::sinh(mu) * std::cos(theta),
            -std::cosh(mu) * std::sin(theta));
      }

      k = 1.0;
      for (const auto& pole : p) {
         k *= (-pole).real();
      }

      std::complex<double> k_complex = 1.0;
      for (const auto& pole : p) {
         k_complex *= (-pole);
      }
      k = k_complex.real();

      if (N % 2 == 0) {
         k /= std::sqrt(1.0 + eps * eps);
      }
   }

   /**
   * @brief Frequency-scale analog low-pass filter.
   *
   * Scales the poles and zeros of an analog prototype filter to achieve
   * the desired cutoff frequency `wo`.
   *
   * @param z Zeros of the prototype filter (scaled in-place).
   * @param p Poles of the prototype filter (scaled in-place).
   * @param k Filter gain (adjusted in-place).
   * @param wo Desired analog cutoff frequency.
   */
   static void lp2lp_zpk(
      std::vector<cdouble>& z,
      std::vector<cdouble>& p,
      double& k,
      double wo
   ) {
      for (auto& zi : z) zi *= wo;
      for (auto& pi : p) pi *= wo;
      k *= std::pow(wo, static_cast<int>(p.size() - z.size()));
   }

   /**
   * @brief Transform analog ZPK filter to digital using bilinear transform.
   *
   * Converts poles and zeros from the s-plane to the z-plane. Adds
   * additional zeros at z=-1 if the number of poles exceeds the number of zeros.
   *
   * @param z Zeros of the analog filter (transformed in-place).
   * @param p Poles of the analog filter (transformed in-place).
   * @param k Filter gain (adjusted in-place).
   * @param fs Sampling frequency.
   */
   static void bilinear_zpk(
      std::vector<cdouble>& z,
      std::vector<cdouble>& p,
      double& k,
      double fs
   ) {
      const double fs2 = 2.0 * fs;
      int degree = static_cast<int>(p.size() - z.size());

      // Calculate gain compensation using ORIGINAL z and p values
      cdouble num = 1.0, den = 1.0;
      for (const auto& zi : z) num *= (fs2 - zi);
      for (const auto& pi : p) den *= (fs2 - pi);

      k *= (num / den).real();

      // Now transform the poles and zeros
      for (auto& zi : z)
         zi = (fs2 + zi) / (fs2 - zi);

      for (auto& pi : p)
         pi = (fs2 + pi) / (fs2 - pi);

      // Add zeros at -1 for excess poles
      for (int i = 0; i < degree; ++i)
         z.emplace_back(-1.0, 0.0);
   }

   /**
    * @brief Convert zero-pole-gain representation to transfer-function coefficients.
    *
    * Converts the ZPK representation into standard numerator (b) and denominator (a)
    * coefficient vectors for direct-time-domain filtering. Coefficients are normalized
    * such that a[0] == 1.
    *
    * @param z Zeros of the digital filter.
    * @param p Poles of the digital filter.
    * @param k Filter gain.
    * @param b Output numerator coefficients.
    * @param a Output denominator coefficients.
    *
    * @throws std::runtime_error if the denominator normalization fails (a[0] too small).
    */
   static void zpk2tf(
      const std::vector<cdouble>& z,
      const std::vector<cdouble>& p,
      double k,
      std::vector<double>& b,
      std::vector<double>& a
   ) {
      auto bz = poly(z);
      auto ap = poly(p);

      // Convert to real vectors
      b.resize(bz.size());
      a.resize(ap.size());

      for (size_t i = 0; i < bz.size(); ++i)
         b[i] = (bz[i] * k).real();

      for (size_t i = 0; i < ap.size(); ++i)
         a[i] = ap[i].real();

      // ------------------------------------------------
      // Reverse coefficient order
      // ------------------------------------------------
      std::reverse(a.begin(), a.end());
      std::reverse(b.begin(), b.end());

      // ------------------------------------------------
      // Normalize so a[0] == 1
      // ------------------------------------------------
      double a0 = a[0];
      if (std::abs(a0) < 1e-12)
         throw std::runtime_error("Invalid IIR denominator normalization");

      for (auto& bi : b) bi /= a0;
      for (auto& ai : a) ai /= a0;
   }

   /**
    * @brief Full filter design pipeline.
    *
    * Implements the complete sequence for designing a digital low-pass
    * Chebyshev Type I filter:
    * 1. Generate analog Chebyshev prototype (cheb1ap).
    * 2. Frequency scale to desired cutoff (lp2lp_zpk).
    * 3. Apply bilinear transform to digital domain (bilinear_zpk).
    * 4. Convert to transfer-function coefficients (zpk2tf).
    *
    * @param N Filter order.
    * @param rp Passband ripple in dB.
    * @param Wn Normalized cutoff frequency (0 < Wn < 1).
    */
   void design(int N, double rp, double Wn) {
      constexpr double fs = 2.0;

      double warped = 2.0 * fs * std::tan(M_PI * Wn / fs);

      std::vector<cdouble> z, p;
      double k;

      cheb1ap(N, rp, z, p, k);
      lp2lp_zpk(z, p, k, warped);
      bilinear_zpk(z, p, k, fs);
      zpk2tf(z, p, k, b_, a_);
   }
};
