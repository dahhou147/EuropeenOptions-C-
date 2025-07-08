#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
double square(double x) {
    return x * x;
}

double square_root(double x) {
    return sqrt(x);
}

double peak_value(double x, double y, double z) {
    double max = x;
    if (max <= y) {
        if (y > z) {
            max = y;
        } else {
            max = z;
        }
    } else {
        if (x > z) {
            max = x;
        } else {
            max = z;
        }
    }
    return max;
}

bool is_prime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int fc(int n) {
    if (n == 0) {
        return 1;
    } else {
        return n * fc(n - 1);
    }
}

void table_de_multiplication(int n) {
    for (int i = 1; i <= 10; i++) {
        std ::cout << n << "*" << i << "=" << n * i << std ::endl;
    }
}
int fibonacci(int n) {
    if (n == 0 || n == 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int table_manipulation(int n) {
    std::vector<int> tab(n);
    for (int i = 0; i < n; i++) {
        tab[i] = rand() % 100;
    }
    int max_table = *std ::max_element(tab.begin(), tab.end());
    return max_table;
}

class Personne {
public:
    std::string nom;
    int age;

    // Constructeur par défaut (sans paramètres)
    Personne() {
        nom = "Inconnu";
        age = 0;
    }

    // Constructeur avec paramètres
    Personne(std::string n, int a) {
        nom = n;
        age = a;
    }

    // Constructeur de copie
    Personne(const Personne& autre) {
        nom = autre.nom;
        age = autre.age;
    }

    void afficher() {
        std::cout << "je m'appelle " << nom << "et j'ai " << age << " ans" << std::endl;
    }
};

// ici on a creer une structure de données qu'on a appelé vecteur
struct vecteur {
    int sz;
    double* elem;
};
// ici on définie une fonction qui va creer un espace de stockage pour le vecteur
// et qui initialise la taille du vecteur

void vectuer_init(vecteur& v, int s) {
    v.sz = s;
    v.elem = new double[s];
}