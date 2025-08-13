#include <iostream>
#include <vector>
#include "calibrator.h"

int main() {
    std::vector<double> prices = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110};
    std::vector<double> volatilities = {0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6};
    Calibrator calibrator(prices, volatilities);
    CalibratedData calibrated_data = calibrator.calibrate();
    std::cout << "Calibrated data: " << calibrated_data.rho << ", " << calibrated_data.theta << ", " << calibrated_data.sigma << ", " << calibrated_data.kappa << ", " << calibrated_data.drift << std::endl;
    return 0;
}