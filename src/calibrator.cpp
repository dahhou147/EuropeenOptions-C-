#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <nlopt.h>
#include "calibrator.h"
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
          r(r) {
        validate();
    }

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
    
    bool validateFellerCondition() const {
        return (kappa * theta > 0.5 * sigma * sigma);
    }
};

// Surcharge de l'opérateur << pour afficher CalibratedData
ostream& operator<<(ostream& os, const CalibratedData& data) {
    os << "Calibrated Parameters:" << endl;
    os << "  rho: " << data.rho << endl;
    os << "  theta: " << data.theta << endl;
    os << "  sigma: " << data.sigma << endl;
    os << "  kappa: " << data.kappa << endl;
    os << "  drift: " << data.drift << endl;
    os << "  S0: " << data.S0 << endl;
    os << "  K: " << data.K << endl;
    os << "  T: " << data.T << endl;
    os << "  v0: " << data.v0 << endl;
    os << "  r: " << data.r << endl;
    return os;
}

class Calibrator {
public:
    vector<double> prices;
    vector<double> volatilities;
    
    Calibrator(vector<double> prices, vector<double> volatilities)
        : prices(prices), volatilities(volatilities) {}

    static double objective_function(unsigned n, const double* x, double* grad, void* data) {
        Calibrator* cal = static_cast<Calibrator*>(data);
        return cal->log_likelihood(x[0], x[1], x[2], x[3], x[4]);
    } // This function is indeed adapted for nlopt as it requires a static function with 5 parameters:
    // - n: unsigned int for the dimension of x
    // - x: pointer to array of parameters to optimize
    // - grad: pointer to gradient (unused here)
    // - data: pointer to additional data, cast to Calibrator* to access log_likelihood

    double log_likelihood(double rho, double theta, double sigma, double kappa, double drift) {
        double sum = 0;
        for (size_t i = 0; i < prices.size() - 1; i++) {
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
        nlopt_opt opt = nlopt_create(NLOPT_LD_MMA, 5); // ici on a 5 paramètres a estimer
        if (!opt) {
            throw std::runtime_error("Failed to create nlopt optimizer");
        }
        
        double lb[5] = {-1, 0, 0, 0, 0};
        double ub[5] = {1, 100, 1, 100, 1};
        nlopt_set_lower_bounds(opt, lb);
        nlopt_set_upper_bounds(opt, ub);
        
        nlopt_set_xtol_rel(opt, 1e-4);
        nlopt_set_maxeval(opt, 1000);
        nlopt_set_min_objective(opt, objective_function, this);
        
        double x[5] = {0, 1, 1, 1, 1};
        double minf;
        
        nlopt_result result = nlopt_optimize(opt, x, &minf);
        
        if (result < 0) {
            cout << "Warning: Optimization failed with result: " << result << endl;
        }
        
        // Nettoyer
        nlopt_destroy(opt);
        
        // Utiliser la première volatilité comme v0 initial
        double initial_v0 = volatilities[0];
        return CalibratedData(x[0], x[1], x[2], x[3], x[4], prices[0], prices[prices.size()-1], 1, initial_v0, 0);
    }
};

int main() {
    try {
        cout << "=== Calibration du Modèle de Heston ===" << endl;
        
        vector<double> prices = {100, 101.2, 99.8, 102.5, 101.9, 103.2, 100.5, 104.1, 102.8, 105.3, 103.9};
        vector<double> volatilities = {0.15, 0.16, 0.14, 0.17, 0.16, 0.18, 0.15, 0.19, 0.17, 0.20, 0.18};
        
        cout << "Prix: ";
        for (double p : prices) cout << fixed << setprecision(2) << p << " ";
        cout << endl;
        
        cout << "Volatilités: ";
        for (double v : volatilities) cout << fixed << setprecision(3) << v << " ";
        cout << endl;
        
        Calibrator calibrator(prices, volatilities);
        cout << "\nDébut de la calibration..." << endl;
        
        CalibratedData calibrated_data = calibrator.calibrate();
        cout << "\nRésultats de la calibration:" << endl;
        cout << calibrated_data << endl;
        
        // Test de validation des paramètres
        cout << "\n=== Validation des paramètres ===" << endl;
        cout << "Condition de Feller (2κθ > σ²): ";
        if (calibrated_data.validateFellerCondition()) {
            cout << "✓ Satisfaite" << endl;
        } else {
            cout << "✗ Non satisfaite" << endl;
        }
        
        cout << "Contraintes de base:" << endl;
        cout << "  κ > 0: " << (calibrated_data.kappa > 0 ? "✓" : "✗") << endl;
        cout << "  θ > 0: " << (calibrated_data.theta > 0 ? "✓" : "✗") << endl;
        cout << "  σ > 0: " << (calibrated_data.sigma > 0 ? "✓" : "✗") << endl;
        cout << "  |ρ| < 1: " << (abs(calibrated_data.rho) < 1 ? "✓" : "✗") << endl;
        cout << "  v₀ > 0: " << (calibrated_data.v0 > 0 ? "✓" : "✗") << endl;
        
    } catch (const std::exception& e) {
        cerr << "Erreur: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}