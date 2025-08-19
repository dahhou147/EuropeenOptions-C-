#include "calibrator.h"
#include <nlopt.h>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

double PI = 3.14159265358979323846;
using namespace std;

CalibratedData ::CalibratedData(double rho, double theta, double sigma, double kappa, double drift,
                                double S0, double K, double T, double v0, double r)
    : rho(rho),
      theta(theta),
      sigma(sigma),
      kappa(kappa),
      drift(drift),
      S0(S0),
      K(K),
      T(T),
      v0(v0),
      r(r) {
    validate();
}

void CalibratedData ::validate() {
    if (rho < -1 || rho > 1) {
        throw std::invalid_argument("rho must be between -1 and 1");
    }
    if (theta < 0) {
        throw std::invalid_argument("theta must be positive");
    }
    if (sigma <= 0) {
        throw std::invalid_argument("sigma must be positive");
    }
    if (kappa <= 0) {
        throw std::invalid_argument("kappa must be positive");
    }
    if (kappa * theta < 0.5 * sigma * sigma) {
        throw std::invalid_argument("feller condition not satisfied");
    }
    if (S0 <= 0) {
        throw std::invalid_argument("S0 must be positive");
    }
    if (K <= 0) {
        throw std::invalid_argument("K must be positive");
    }
    if (T <= 0) {
        throw std::invalid_argument("T must be positive");
    }
    if (v0 <= 0) {
        throw std::invalid_argument("v0 must be positive");
    }
}

bool CalibratedData ::validateFellerCondition() const {
    return (kappa * theta > 0.5 * sigma * sigma);
}

Calibrator ::Calibrator(std::vector<double> prices, std::vector<double> volatilities)
    : prices(prices), volatilities(volatilities) {}

double Calibrator ::objective_function(unsigned n, const double* x, double* grad, void* data) {
    (void)n;
    (void)grad;
    Calibrator* cal = static_cast<Calibrator*>(data);
    return cal->log_likelihood(x[0], x[1], x[2], x[3], x[4]);
}

double Calibrator ::log_likelihood(double rho, double theta, double sigma, double kappa,
                                   double drift) {
    double sum = 0;
    for (size_t i = 0; i < prices.size() - 1; i++) {
        double Q = std::log(prices[i + 1] / prices[i]);
        double V = volatilities[i];
        double V_i = volatilities[i + 1];
        sum +=
            -log(2 * PI) - log(sigma) - log(V) - 0.5 * log(1 - pow(rho, 2)) -
            (pow(Q - 1 - drift, 2)) / (2 * V * (1 - pow(rho, 2))) +
            (rho * (Q - 1 - drift) * (V_i - V - theta * kappa + kappa * V) /
             (V * sigma * (1 - pow(rho, 2)))) -
            (pow(V_i - V - theta * kappa + kappa * V, 2)) / (2 * sigma * sigma * (1 - pow(rho, 2)));
    }
    return sum;
}

CalibratedData Calibrator ::calibrate() {
    nlopt_opt opt = nlopt_create(NLOPT_LD_MMA, 5); // ici on a 5 paramètres a estimer
    if (!opt) {
        throw std::runtime_error("Failed to create nlopt optimizer");
    }

    double lb[5] = {-1, 0, 1e-8, 0, 0};
    double ub[5] = {1, 100, 1, 100, 1};
    nlopt_set_lower_bounds(opt, lb);
    nlopt_set_upper_bounds(opt, ub);

    nlopt_set_xtol_rel(opt, 1e-4);
    nlopt_set_maxeval(opt, 1000);
    nlopt_set_max_objective(opt, objective_function, this);

    double x[5] = {0, 1, 1, 1, 1};
    double minf;

    nlopt_result result = nlopt_optimize(opt, x, &minf);

    if (result < 0) {
        cout << "Warning: Optimization failed with result: " << result << endl;
    }

    nlopt_destroy(opt);

    double initial_v0 = volatilities[0];
    return CalibratedData(x[0], x[1], x[2], x[3], x[4], prices[0], prices[prices.size() - 1], 1,
                          initial_v0, 0);
}
