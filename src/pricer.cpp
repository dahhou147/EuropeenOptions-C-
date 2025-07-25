    #include <time.h>
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
    constexpr double PI = 3.14159265358979323846;

    double norm_cdf(double x) {
        return 0.5 * erfc(-x * sqrt(2) / 2);
    }

    double norm_pdf(double x) {
        return exp(-x * x / 2) / sqrt(2 * PI);
    }

    pair<vector<vector<double>>, vector<vector<double>>> HestonSimulation(double S0, double drift,
                                                                        double T, double v0,
                                                                        double kappa, double theta,
                                                                        double sigma, double rho,
                                                                        int N, int M) {
        double dt = T / N;
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
                normal_distribution<> dW2(0, sqrt_dt);
                double z1 = dW1(gen);
                double z2 = rho * z1 + sqrt(1 - rho * rho) * dW2(gen);

                double vt_prev = max(0.0, variance_path[m][i - 1]);
                double vt = vt_prev + kappa * (theta - vt_prev) * dt + sigma * sqrt(vt_prev) * z1;
                variance_path[m][i] = max(0.0, vt);

                stock_path[m][i] =
                    stock_path[m][i - 1] * exp((drift - 0.5 * vt_prev) * dt + sqrt(vt_prev) * z2);
            }
        }

        return make_pair(stock_path, variance_path);
    }

    class HestonPricer {
    public:
        double S, K, tau, v, kappa, theta, sigma, rho, r;

    public:
        HestonPricer(double S, double K, double tau, double v, double kappa, double theta, double sigma,
                    double rho, double r)
            : S(S), K(K), tau(tau), v(v), kappa(kappa), theta(theta), sigma(sigma), rho(rho), r(r) {}

        HestonPricer(const HestonPricer& other)
            : S(other.S), K(other.K), tau(other.tau), v(other.v), kappa(other.kappa), theta(other.theta), sigma(other.sigma), rho(other.rho), r(other.r) {}

        complex<double> characteristic_function(complex<double> u) {
            complex<double> i(0.0, 1.0);
            double x = log(S);

            complex<double> d =
                sqrt(pow(rho * sigma * u * i - kappa, 2.0) + sigma * sigma * (u * i + u * u));
            complex<double> g = (kappa - rho * sigma * u * i - d) / (kappa - rho * sigma * u * i + d);

            complex<double> C =
                r * u * i * tau + (kappa * theta) / (sigma * sigma) *
                                    ((kappa - rho * sigma * u * i - d) * tau -
                                    2.0 * log((1.0 - g * exp(-d * tau)) / (1.0 - g)));
            complex<double> D = ((kappa - rho * sigma * u * i - d) / (sigma * sigma)) *
                                ((1.0 - exp(-d * tau)) / (1.0 - g * exp(-d * tau)));

            return exp(C + D * v + i * u * x);
        }

        double integral_term(int j) {
            double du = 0.01;
            int N_u = 10000;
            double sum = 0.0;
            complex<double> i(0.0, 1.0);

            for (int n = 1; n <= N_u; ++n) {
                double phi = n * du;
                complex<double> u(phi, 0.0);
                complex<double> numer;

                if (j == 1) {
                    numer = exp(-i * u * log(K)) * characteristic_function(u - i);
                } else {
                    numer = exp(-i * u * log(K)) * characteristic_function(u);
                }

                complex<double> integrand = numer / (i * u);
                sum += real(integrand) * du;
            }
            return sum;
        }

        double price_call() {
            double P1 = 0.5 * S + (exp(-r * tau) / PI) * integral_term(1);
            double P2 = K * exp(-r * tau) * (0.5 + (1.0 / PI) * integral_term(0));
            return P1 - P2;
        }

        double price_put() {
            double call_price = price_call();
            return call_price - S + K * exp(-r * tau);
        }
    };

    class HestonGreeks : public HestonPricer {
    public: 
        HestonGreeks(double S, double K, double tau, double v, double kappa, double theta, double sigma, double rho, double r)
            : HestonPricer(S, K, tau, v, kappa, theta, sigma, rho, r) {}

                
        HestonPricer choc_stock(double Stock){
            return HestonPricer(Stock, K, tau, v, kappa, theta, sigma, rho, r);
        }
        HestonPricer choc_vol(double Vol){
            return HestonPricer(S, K, tau, Vol, kappa, theta, sigma, rho, r);
        }
        HestonPricer choc_time(double Time){
            return HestonPricer(S, K, Time, v, kappa, theta, sigma, rho, r);
        }
        HestonPricer choc_rho(double Rho){
            return HestonPricer(S, K, tau, v, kappa, theta, sigma, Rho, r);
        }
        double delta(){
            double epsilon = 1e-4;
            double S_plus = S + epsilon;
            double S_minus = S - epsilon;
            double call_price_plus = choc_stock(S_plus).price_call();
            double call_price_minus = choc_stock(S_minus).price_call();
            return (call_price_plus - call_price_minus) / (2 * epsilon);
        }

        double gamma(){
            double epsilon = 1e-4;
            double S_plus = S + epsilon;
            double S_minus = S - epsilon;
            double call_price_plus = choc_stock(S_plus).price_call();
            double call_price_minus = choc_stock(S_minus).price_call();
            double call_price = price_call();
            return (call_price_plus -2*call_price + call_price_minus) / (epsilon * epsilon);
        }

        double vega(){
            double epsilon = 1e-4;  
            double v_plus = v + epsilon;
            double v_minus = v - epsilon;
            double call_price_plus = choc_vol(v_plus).price_call();
            double call_price_minus = choc_vol(v_minus).price_call();
            return (call_price_plus - 2 * price_call() + call_price_minus) / (epsilon * epsilon);
        }

        double theta_(){
            double epsilon = 1e-4;
            double tau_plus = tau + epsilon;
            double tau_minus = tau - epsilon;
            double theta_plus = choc_time(tau_plus).price_call();
            double theta_minus = choc_time(tau_minus).price_call();
            return (theta_plus - theta_minus) / (2 * epsilon);
        }
    };
        
        
    
            


    int main() {
        double S0 = 100.0;
        double K = 100.0;
        double v0 = 0.04;
        double kappa = 1.5;
        double theta = 0.04;
        double sigma = 0.3;
        double rho = -0.6;
        double r = 0.01;
        double tau = 0.5; // 1 an
        // ======= PRICING AVEC TON CODE (HestonPricer) =======
        HestonPricer myPricer(S0, K, tau, v0, kappa, theta, sigma, rho, r);
        double price_maison = myPricer.price_call();
        cout << "[Ton code] Prix Call Européen Heston : " << price_maison << endl;
        // ======= PRICING AVEC TON CODE (HestonGreeks) =======
        HestonGreeks myGreeks(S0, K, tau, v0, kappa, theta, sigma, rho, r);
        double delta_maison = myGreeks.delta();
        cout << "[Ton code] Delta : " << delta_maison << endl;
        double gamma_maison = myGreeks.gamma();
        cout << "[Ton code] Gamma : " << gamma_maison << endl;
        double vega_maison = myGreeks.vega();
        cout << "[Ton code] Vega : " << vega_maison << endl;
        double theta_maison = myGreeks.theta_();
        cout << "[Ton code] Theta : " << theta_maison << endl;

        return 0;
    }