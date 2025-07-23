#include <algorithm>
#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
using namespace std;

double norm_cdf(double x) {
    return 0.5 * erfc(-x * sqrt(2) / 2);
}


constexpr double PI = 3.14159265358979323846;

double norm_pdf(double x) {
    return exp(-x * x / 2) / sqrt(2 * M_PI);
}

vector<vector<double>> HestonSimultion(double S0, double drift, double T, double v0, double kappa,
                                       double theta, double sigma, double rho, int N, int M) {
    double dt = T / M;
    double sqrt_dt = sqrt(dt);
    vector<vector<double>> stock_path(M, vector<double>(N + 1));
    vector<vector<double>> variance_path(M, vector<double>(N + 1));
    random_device rd;
    mt19937 gen(rd()); // merton 2
    for (int m = 0; m < M; m++) {
        stock_path[m][0] = S0;
        variance_path[m][0] = v0;
        for (int i = 1; i <= N; i++) {
            normal_distribution<> dW1(0, sqrt_dt);
            normal_distribution<> dw2(0, sqrt_dt);
            double z1 = dW1(gen);
            double z2 = rho * z1 + sqrt(1 - rho * rho) * dw2(gen);
            double v = variance_path[m][i - 1] + kappa * (theta - variance_path[m][i - 1]) * dt +
                       sigma * sqrt(variance_path[m][i - 1]) * z1;
            variance_path[m][i] = max(0.0, v);
            stock_path[m][i] = stock_path[m][i - 1] * exp((drift - 0.5 * variance_path[m][i]) * dt +
                                                          sqrt(variance_path[m][i]) * z2);
        }
    }
    return stock_path;
}

class HestonPricer {
private:
    vector<vector<double>> stock_path;
    vector<vector<double>> variance_path;
    double drift;
    double T;
    double v0;
    double kappa;
    double theta;
    double sigma;
    double rho;
    double r;
    int N;
    int M;

public:
    HestonPricer(double drift, double T, double v0, double kappa, double theta,
                 double sigma, double rho, double r, int N, int M) : 
        S0(S0), drift(drift), T(T), v0(v0), kappa(kappa), theta(theta), sigma(sigma), rho(rho),
        r(r), N(N), M(M) {}

    complex<double> carateristique_function(double u, double V, double X) {
        complex<double> i(0, 1);
        double M = sqrt(pow(rho * sigma * i * u - kappa, 2) + pow(sigma, 2) * (i * u + pow(u, 2)));
        complex<double> N = (rho * sigma * i * u - kappa - M) / (rho * sigma * i * u - kappa + M);
        complex<double> A = r * i * rho * T + (kappa * theta / pow(sigma, 2)) *
                            (-(rho * sigma * i * u - kappa - M) * T -
                             2 * log((1 - N * exp(M * T)) / (1 - N)));
        complex<double> C = (exp(M * T) - 1) * (rho * sigma * i * u - kappa - M) /
                            (pow(sigma, 2) * (1 - N * exp(M * T)));
        return exp(A + C * V + i * u * X);
    }

    double integrand(double u, double V, double X, int j) {
        complex<double> i(0, 1);
        complex<double> f = carateristique_function(u+j, V, X);
        return real(exp(-i * u * log(K)) * f / (i * u));
    }
    double price_call(double S, double v, double tau){
        double x = log(S);
        double P1 = 0.5 * S;
        double P2 = K * exp(-r * tau);
        for (int j = 1; j>=0 ; j--){
            double integral0 = 0;
            double integral1 = 0;
            for (int N= 0; N< 1000;N++){
                if (j==0){
                    double u = 0.0001 + N * 0.1;
                    integral0 + =  integrand(u, v,x,j)
                }
                else{
                    double u = 0.0001 + N * 0.1;
                    integral1 + =  integrand(u, v,x,j)
                }
            }
        }
        P1 +=(exp(-r*tau)/PI)*integral1;
        P2 *= (0.5 + (1/PI)*integral0);
        return P1 - P2;
    }

    double price_put(double K) {
        return price_call(K) - S + K * exp(-r * T);
    }
};






int main() {
    double S0 = 100;
    double drift = 0.05;
    double T = 1;
    double v0 = 0.09;
    double kappa = 2;
    double theta = 0.04;
    double sigma = 0.5;
    double rho = -0.75;
    int N = 100;
    int M = 100000;
    vector<vector<double>> stock_path =
        HestonSimultion(S0, drift, T, v0, kappa, theta, sigma, rho, N, M);
    ofstream file("stock_path.csv");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j <= N; j++) {
            file << stock_path[i][j] << ",";
        }
        file << endl;
    }
    file.close();
    return 0;
}