#ifndef DECIMATOR_H
#define DECIMATOR_H

#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>

class Decimator {
public:
    Decimator(int decimationFactor = 10);

    std::vector<double> process(const std::vector<double>& signal);
    std::vector<double> process(const double* signal, size_t length);
    std::vector<double> process(const float* signal, size_t length);
    std::vector<double> process(const std::unique_ptr<double[]>& signal, size_t length);

private:
    const std::vector<double> b_coeffs{
        3.58632432538361e-09 ,	2.86905946030689e-08	, 1.00417081110741e-07 ,	
        2.00834162221482e-07 ,	2.51042702776853e-07 ,	2.00834162221482e-07 ,	
        1.00417081110741e-07 ,	2.86905946030689e-08 ,	3.58632432538361e-09
    };

    const std::vector<double> a_coeffs{
        1	, -7.39772047094363	, 24.0727277609670 ,	
        -44.9989146659833 ,	52.8434667669225 ,	-39.9159143524685 ,	
        18.9376658097605 ,	-5.15917942637568 ,	0.617869501520364
    };

    const int filter_order = 8;
    const int initial_offset = 32;
    int decimation_factor;
};

#endif // DECIMATOR_H
