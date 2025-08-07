#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <nlopt.hpp>

using namespace std;

double PI = 3.14159265358979323846;

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
                   double K, double T, double v0, double r)
        : rho(rho),
          theta(theta),
          sigma(sigma),
          kappa(kappa),
          drift(drift),
          S0(S0),
          K(K),
          T(T),
          v0(v0),
          r(r),
          validate() {}

    void validate() {
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
        // feller condition
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
};

class Calibrator {
public:
    vector<double> prices;
    vector<double> volatilities;
    Calibrator(vector<double> prices, vector<double> volatilities)
        : prices(prices), volatilities(volatilities) {}

    double log_likelihood(double rho, double theta, double sigma, double kappa, double drift) {
        double sum = 0;
        for (int i = 0; i < prices.size() - 1; i++) {
            double Q = std::log(prices[i + 1] / prices[i]);
            double V = volatilities[i];
            double V_i = volatilities[i + 1];
            sum += -log(2 * PI) - log(sigma) - log(V) - 0.5 * log(1 - pow(rho, 2)) -
                   (pow(Q - 1 - drift, 2)) / (2 * V * (1 - pow(rho, 2))) +
                   (rho * (Q - 1 - drift) * (V_i - V - theta * kappa + kappa * V) /
                       (V * sigma * (1 - pow(rho, 2)))) -
                   (pow(V_i - V - theta * kappa + kappa * V, 2)) /
                       (2 * sigma * sigma * (1 - pow(rho, 2)));
        }
        return sum;
    }

    CalibratedData calibrate() {
        nlopt::opt opt(nlopt::LD_MMA, 5);
        opt.set_lower_bounds({-1, 0, 0, 0, 0});
        opt.set_upper_bounds({1, 100, 100, 100, 100});
        opt.set_xtol_rel(1e-4);
        opt.set_maxeval(1000);
        opt.set_min_objective(log_likelihood, this);
        std::vector<double> x = {0, 1, 1, 1, 1};
        double minf;
        nlopt::result result = opt.optimize(x, minf);
        return CalibratedData(x[0], x[1], x[2], x[3], x[4], prices[0], prices[prices.size()-1], 1, 0, 0);
    }

    private:
    }