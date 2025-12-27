#ifndef DECIMATOR_H
#define DECIMATOR_H

#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>

class Decimator {
public:
   Decimator(int decimationFactor = 10);

   std::vector<double> process(const double* signal, size_t length) {
      return processImpl(signal, length);
   }

   std::vector<double> process(const float* signal, size_t length) {
      return processImpl(signal, length);
   }

   std::vector<double> process(const std::vector<float>& signal) {
      return processImpl(signal.data(), signal.size());
   }

   std::vector<double> process(const std::vector<double>& signal) {
      return processImpl(signal.data(), signal.size());
   }

   std::vector<double> process(const std::unique_ptr<float[]>& signal, size_t length) {
      return processImpl(signal.get(), length);
   }

   std::vector<double> process(const std::unique_ptr<double[]>& signal, size_t length) {
      return processImpl(signal.get(), length);
   }
private:
   // Internal template implementation
   template<typename T>
   std::vector<double> processImpl(const T* signal, size_t length) {
      const size_t state_size = initial_offset + filter_order;
      const size_t buffer_size = length + state_size + 1;
      const size_t output_size = length / decimation_factor;

      std::vector<double> X(buffer_size, 0.0);
      std::vector<double> Y(buffer_size, 0.0);
      std::vector<double> output;
      output.reserve(output_size);

      // Convert input to double
      for (size_t i = filter_order + 1; i <= length + filter_order + 1; ++i) {
         X[i] = static_cast<double>(signal[i - filter_order - 1]);
      }

      size_t st = initial_offset + filter_order;
      size_t written = 0;

      for (size_t i = filter_order + 1; i < buffer_size; ++i) {
         Y[i] = b_coeffs[0] * X[i];
         for (size_t j = 1; j <= filter_order; ++j) {
            Y[i] += b_coeffs[j] * X[i - j];
            Y[i] -= a_coeffs[j] * Y[i - j];
         }

         if (i == st && written < output_size) {
            output.push_back((Y[i] > -1 && Y[i] < 1) ? Y[i] : 0.0);
            ++written;
            st += decimation_factor;
         }

         if (written == output_size) break;
      }

      return output;
   }

   const std::vector<double> b_coeffs{
       3.58632432538361e-09 , 2.86905946030689e-08 , 1.00417081110741e-07 ,
       2.00834162221482e-07 , 2.51042702776853e-07 , 2.00834162221482e-07 ,
       1.00417081110741e-07 , 2.86905946030689e-08 , 3.58632432538361e-09
   };

   const std::vector<double> a_coeffs{
       1 , -7.39772047094363 , 24.0727277609670 ,
       -44.9989146659833 , 52.8434667669225 , -39.9159143524685 ,
       18.9376658097605 , -5.15917942637568 , 0.617869501520364
   };

   const int filter_order = 8;
   const int initial_offset = 32;
   int decimation_factor;
};

#endif // DECIMATOR_H
