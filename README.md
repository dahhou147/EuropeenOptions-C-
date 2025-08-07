# Calibration du Modèle de Heston

Ce projet implémente plusieurs méthodes de calibration pour le modèle de Heston, un modèle de volatilité stochastique largement utilisé en finance quantitative.

## Modèle de Heston

Le modèle de Heston décrit l'évolution du prix d'un actif et de sa volatilité par les équations différentielles stochastiques suivantes :

```
dS_t = μS_t dt + √v_t S_t dW_t^S
dv_t = κ(θ - v_t) dt + ξ√v_t dW_t^v
```

Où :
- `S_t` : Prix de l'actif sous-jacent
- `v_t` : Variance stochastique
- `κ` : Taux de retour à la moyenne
- `θ` : Variance de long terme
- `ξ` : Volatilité de la volatilité
- `ρ` : Corrélation entre les mouvements du prix et de la variance

## Méthodes de Calibration Implémentées

### 1. Maximum Likelihood Estimation (MLE)

Cette méthode maximise la vraisemblance des observations données les paramètres du modèle. Elle est particulièrement adaptée quand on dispose de séries temporelles de prix et de variances.

**Avantages :**
- Méthode statistiquement rigoureuse
- Efficace avec des données temporelles
- Fournit des estimations asymptotiquement non-biaisées

**Inconvénients :**
- Nécessite des données de variance observées
- Sensible aux erreurs de mesure

### 2. Calibration par Prix d'Options

Cette méthode ajuste les paramètres pour que les prix théoriques du modèle correspondent aux prix de marché observés.

**Avantages :**
- Utilise directement les prix de marché
- Plus robuste aux erreurs de mesure
- Permet de calibrer sur différentes maturités et strikes

**Inconvénients :**
- Peut être sensible aux erreurs de prix de marché
- Nécessite un nombre suffisant d'options

### 3. Algorithme de Levenberg-Marquardt

Algorithme d'optimisation non-linéaire qui combine les avantages de la méthode de Gauss-Newton et de la descente de gradient.

**Avantages :**
- Convergence rapide
- Robuste aux mauvaises initialisations
- Gère automatiquement la régularisation

**Inconvénients :**
- Calcul coûteux des dérivées
- Peut converger vers des minima locaux

## Utilisation du Code

### Compilation

```bash
make
```

### Exécution

```bash
./heston_calibrator
```

### Exemple d'utilisation

```cpp
#include "calibrator.h"

// Calibration par MLE
vector<double> asset_prices = {100.0, 101.0, 99.5, 102.0, 100.5};
vector<double> variances = {0.04, 0.042, 0.038, 0.045, 0.041};
vector<double> params = calibrate(asset_prices, variances);

// Calibration par prix d'options
vector<OptionData> options = {
    {100.0, 100.0, 0.25, 0.05, 5.0, true},   // Call ATM
    {100.0, 110.0, 0.25, 0.05, 2.0, true},   // Call OTM
    {100.0, 90.0, 0.25, 0.05, 12.0, true},   // Call ITM
};
vector<double> params_options = calibrate_options(options);
```

## Validation des Paramètres

Le code inclut une validation des paramètres pour s'assurer de la stabilité du modèle :

1. **Contraintes de base :** κ > 0, θ > 0, ξ > 0, |ρ| < 1, v₀ > 0
2. **Condition de Feller :** 2κθ > ξ² (assure que la variance reste positive)

## Comparaison des Méthodes

| Méthode | Données Requises | Robustesse | Vitesse | Précision |
|---------|------------------|------------|---------|-----------|
| MLE | Prix + Variances | Moyenne | Rapide | Élevée |
| Options (Grille) | Prix d'options | Élevée | Lente | Moyenne |
| Options (LM) | Prix d'options | Élevée | Rapide | Élevée |

## Améliorations Possibles

1. **Filtre de Kalman :** Pour une calibration dynamique qui s'adapte aux changements de marché
2. **Optimisation parallèle :** Pour accélérer la recherche par grille
3. **Méthodes bayésiennes :** Pour incorporer des connaissances a priori
4. **Calibration multi-actifs :** Pour calibrer simultanément plusieurs actifs

## Références

1. Heston, S. L. (1993). "A Closed-Form Solution for Options with Stochastic Volatility with Applications to Bond and Currency Options." *Review of Financial Studies*, 6(2), 327-343.

2. Cui, Y., del Baño Rollin, S., & Germano, G. (2017). "Full and Fast Calibration of the Heston Stochastic Volatility Model." *European Journal of Operational Research*.

3. Wang, X., He, X., Bao, Y., & Zhao, Y. (2018). "Parameter Estimates of Heston Stochastic Volatility Model with MLE and Consistent EKF Algorithm." *Science China Information Sciences*.

## Structure du Code

```
src/
├── calibrator.h      # Définitions des classes et structures
├── calibrator.cpp    # Implémentation des méthodes de calibration
├── pricer.h          # Définitions des classes de pricing
├── pricer.cpp        # Implémentation du pricing Heston
└── main.cpp          # Programme principal

Makefile              # Configuration de compilation
README.md             # Documentation
```

## Auteur

Ce code a été développé pour l'étude des méthodes de calibration du modèle de Heston en finance quantitative. 