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
    mt19937 gen(rd());
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
            stock_path[m][i] =
                stock_path[m][i - 1] * (1 + drift * dt + sqrt(variance_path[m][i]) * z2);
        }
    }
    return stock_path;
}

class HestonPricer {
private:
    double S0;
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
    HestonPricer(double S0, double drift, double T, double v0, double kappa, double theta,
                 double sigma, double rho, double r, int N, int M) {
        this->S0 = S0;
        this->drift = drift;
        this->T = T;
        this->v0 = v0;
        this->kappa = kappa;
        this->theta = theta;
        this->sigma = sigma;
        this->rho = rho;
        this->r = r;
        this->N = N;
        this->M = M;
    }

    complex<double> carateristique_function(double u, double V, double X) {
        complex<double> i(0, 1);
        double M = sqrt(pow(rho * sigma * i * u - kappa, 2) + pow(sigma, 2) * (i * u + pow(u, 2)));
        complex<double> N = (rho * sigma * i * u - kappa - M) / (rho * sigma * i * u - kappa + M);
        complex<double> A = r * i * u * T + (kappa * theta / pow(sigma, 2)) *
                                                ((rho * sigma * i * u - kappa - M) * T -
                                                 2 * log((1 - N * exp(M * T)) / (1 - N)));
        complex<double> C = (exp(M * T) - 1) * (rho * sigma * i * u - kappa - M) /
                            (pow(sigma, 2) * (1 - N * exp(M * T)));
        return exp(A + C * V + i * u * X);
    }
}

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
        int M = 10000;
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