#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

vector<vector<double>> GBM(double S0, double T, double sigma, double drift, int N, int M) {
    double dt = T / N;
    double sqrt_dt = sqrt(dt);
    random_device rd;
    mt19937 gen(rd());
    vector<vector<double>> stock_path(M, vector<double>(N + 1));
    for (int m = 0; m < M; m++) {
        stock_path[m][0] = S0;
        for (int i = 1; i <= N; i++) {
            normal_distribution<> dW(0, sqrt_dt);
            double x = dW(gen);
            stock_path[m][i] =
                stock_path[m][i - 1] * exp((drift - 0.5 * sigma * sigma) * dt + sigma * x);
        }
    }
    return stock_path;
}

class PricerBS {
public:
    double S0;
    double T;
    double sigma;
    double r;
    double K;
    double q;

    PricerBS(double S0, double T, double sigma, double r, double K, std::string type,
             double deviden)
        : S0(S0), T(T), sigma(sigma), r(r), K(K), q(deviden) {}

    PricerBS(const PricerBS& other)
        : S0(other.S0), T(other.T), sigma(other.sigma), r(other.r), K(other.K), q(other.q) {}

    double get_price_call(std ::string type) {
        double d1 = (log(S0 / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
        double d2 = d1 - sigma * sqrt(T);
        switch (type) {
            case "call":
                return S0 * exp(-q * T) * norm_cdf(d1) - K * exp(-r * T) * norm_cdf(d2);
            case "put":
                return K * exp(-r * T) * norm_cdf(-d2) - S0 * exp(-q * T) * norm_cdf(-d1);
            default:
                throw std::invalid_argument("Invalid option type");
        }
    }
};

class Greeks {
    PricerBS pricer;
    double tau;
    Greeks(PricerBS pricer, double tau) : pricer(pricer), tau(tau) {}

    double delta(std::string type) {
        double d1 = (log(pricer.S0 / pricer.K) +
                     (pricer.r - pricer.q + 0.5 * pricer.sigma * pricer.sigma) * tau) /
                    (pricer.sigma * sqrt(tau));
        switch (type) {
            case "call":
                return exp(-pricer.q * tau) * norm_cdf(d1);
            case "put":
                return exp(-pricer.q * tau) * (norm_cdf(d1) - 1);
            default:
                throw std::invalid_argument("Invalid option type");
        }
    }

    double gamma() {
        double d1 = (log(pricer.S0 / pricer.K) +
                     (pricer.r - pricer.q + 0.5 * pricer.sigma * pricer.sigma) * tau) /
                    (pricer.sigma * sqrt(tau));
        return exp(-pricer.q * tau) * norm_pdf(d1) / (pricer.S0 * pricer.sigma * sqrt(tau));
    }

    double theta(std::string type) {
        double d1 = (log(pricer.S0 / pricer.K) +
                     (pricer.r - pricer.q + 0.5 * pricer.sigma * pricer.sigma) * tau) /
                    (pricer.sigma * sqrt(tau));
        switch (type) {
            case "call":
                return -pricer.S0 * pricer.sigma * exp(-pricer.q * tau) * norm_pdf(d1) /
                           (2 * sqrt(tau)) -
                       pricer.r * pricer.K * exp(-pricer.r * tau) * norm_cdf(d2);
            case "put":
                return -pricer.S0 * pricer.sigma * exp(-pricer.q * tau) * norm_pdf(d1) /
                           (2 * sqrt(tau)) +
                       pricer.r * pricer.K * exp(-pricer.r * tau) * norm_cdf(-d2);
            default:
                throw std::invalid_argument("Invalid option type");
        }
    }

    double vega() {
        double d1 = (log(pricer.S0 / pricer.K) +
                     (pricer.r - pricer.q + 0.5 * pricer.sigma * pricer.sigma) * tau) /
                    (pricer.sigma * sqrt(tau));
        return pricer.S0 * exp(-pricer.q * tau) * norm_pdf(d1) * sqrt(tau);
    }

    double rho(std::string type) {
        double d2 = (log(pricer.S0 / pricer.K) +
                     (pricer.r - pricer.q + 0.5 * pricer.sigma * pricer.sigma) * tau) /
                    (pricer.sigma * sqrt(tau));
        switch (type) {
            case "call":
                return pricer.K * tau * exp(-pricer.r * tau) * norm_cdf(d2);
            case "put":
                return -pricer.K * tau * exp(-pricer.r * tau) * norm_cdf(-d2);
            default:
                throw std::invalid_argument("Invalid option type");
        }
    }
    PricerBS get_pricer(double S, double K, double tau, double implied_volatility) {
        PricerBS new_pricer = pricer;
        new_pricer.S0 = S;
        new_pricer.K = K;
        new_pricer.T = tau; 
        new_pricer.sigma = implied_volatility;
        return new_pricer;
    }
    Greeks get_greeks(double S, double K, double tau, double implied_volatility) {
        PricerBS new_pricer = get_pricer(S, K, tau, implied_volatility);
        return Greeks(new_pricer, tau);
    }
    
};

class ConstructPortfolio {
    PricerBS pricer;
    double K1;
    vector<vector<double>> stock_path;
    double K2;
    vector<string> option_types;
    vector<double> implied_volatilities;

    ConstructPortfolio(PricerBS pricer, double tau, double K1, double K2,
                       vector<string> option_types, vector<double> implied_volatilities, vector<vector<double>> stock_path)
        : pricer(pricer),
          tau(tau),
          K1(K1),
          K2(K2),
          option_types(option_types),
          implied_volatilities(implied_volatilities),
          stock_path(stock_path) {}

    
}









int main() {
    double S0 = 100;
    double T = 1;
    double sigma = 0.2;
    double r = 0.05;
    int N = 100;
    int M = 1000;
    vector<vector<double>> stock_path = GBM(S0, T, sigma, r, N, M);
    cout << stock_path[0][0] << endl;
    ofstream file("stock_path.csv");
    for (int m = 0; m < M; m++) {
        for (int i = 0; i <= N; i++) {
            file << stock_path[m][i] << ",";
        }
        file << endl;
    }
    file.close();
    return 0;
}