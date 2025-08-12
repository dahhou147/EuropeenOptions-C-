#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <vector>
#include <cmath>

struct CalibratedData {
    double rho;
    double theta;
    double sigma;
    double kappa;
    double drift;
    double S0;
    double K;
    double T;
    double v0;
    double r;
    
    CalibratedData(double rho, double theta, double sigma, double kappa, double drift, double S0,
                   double K, double T, double v0, double r);
    
    void validate();
    bool validateFellerCondition() const;
};  

class Calibrator {
public:

    Calibrator(std::vector<double> prices, std::vector<double> volatilities);
    
    static double objective_function(unsigned n, const double* x, double* grad, void* data);
    
    double log_likelihood(double rho, double theta, double sigma, double kappa, double drift);
    
    CalibratedData calibrate();
};

#endif // CALIBRATOR_H
