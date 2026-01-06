/**********************************************************************

  Audacity: A Digital Audio Editor

  Cheby1LowPassIIRFilter.h

  Guilherme Pavelski

**********************************************************************/

#include <vector>
#include <complex>
#include <cmath>
#include <numeric>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // !M_PI

class Cheby1LowPassIIRFilter {
public:
   Cheby1LowPassIIRFilter(int order, double rp, double Wn) {
      design(order, rp, Wn);
   }

   const std::vector<double>& b() const { return b_; }
   const std::vector<double>& a() const { return a_; }

private:
   using cdouble = std::complex<double>;
   std::vector<double> b_, a_;

   // ------------------------------------------------------------
   // Polynomial from roots
   // ------------------------------------------------------------
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
#include <complex>
#include <vector>
#include <cmath>

   using cdouble = std::complex<double>;

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

   // ------------------------------------------------------------
   // Low-pass frequency scaling
   // ------------------------------------------------------------
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

   // ------------------------------------------------------------
   // Bilinear transform
   // ------------------------------------------------------------
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

   // ------------------------------------------------------------
   // ZPK -> transfer function
   // ------------------------------------------------------------
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

   // ------------------------------------------------------------
   // Full design pipeline
   // ------------------------------------------------------------
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
