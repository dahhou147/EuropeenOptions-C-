# Configuration C++

Ce projet contient une configuration complète pour développer en C++ avec Makefile, gestion des dépendances et outils de debug.

## 📋 Prérequis

### Installation des dépendances

**Ubuntu/Debian :**
```bash
make install-deps
```

**Fedora :**
```bash
make install-deps-fedora
```

**Arch Linux :**
```bash
make install-deps-arch
```

**Installation manuelle :**
```bash
# Compilateur C++
sudo apt install build-essential

# Debugger
sudo apt install gdb

# Outil de détection de fuites mémoire
sudo apt install valgrind
```

## 🚀 Utilisation

### Compilation

**Mode Debug (par défaut) :**
```bash
make debug
# ou simplement
make
```

**Mode Release :**
```bash
make release
```

### Exécution

```bash
make run
```

### Nettoyage

```bash
make clean
```

## 📁 Structure du projet

```
.
├── Makefile          # Configuration de compilation
├── .gitignore        # Fichiers à ignorer par Git
├── README.md         # Ce fichier
└── src/
    └── main.cpp      # Fichier source principal
```

## 🔧 Configuration

### Options de compilation

- **Standard C++ :** C++17
- **Avertissements :** -Wall -Wextra -pedantic
- **Mode Debug :** -g -O0 -DDEBUG
- **Mode Release :** -O2 -DNDEBUG

### Ajouter de nouveaux fichiers

1. Placez vos fichiers `.cpp` dans le dossier `src/`
2. Le Makefile les détectera automatiquement
3. Compilez avec `make`

### Exemple d'utilisation

```bash
# Compiler en mode debug
make debug

# Exécuter le programme
./bin/main

# Ou utiliser la commande run
make run

# Nettoyer les fichiers de compilation
make clean
```

## 🐛 Debug

### Avec GDB

```bash
# Compiler en mode debug
make debug

# Lancer le debugger
gdb bin/main
```

### Avec Valgrind (détection de fuites mémoire)

```bash
# Compiler en mode debug
make debug

# Vérifier les fuites mémoire
valgrind --leak-check=full ./bin/main
```

## 📝 Extensions VSCode recommandées

Si vous utilisez VSCode, installez ces extensions :

- **C/C++** (Microsoft)
- **C/C++ Extension Pack** (Microsoft)
- **CMake Tools** (Microsoft)
- **Code Runner** (Jun Han)

## 🔍 Vérification de l'installation

Après avoir installé les dépendances, testez la configuration :

```bash
make run
```

Vous devriez voir :
```
=== Configuration C++ Test ===
Calculatrice: Ma Calculatrice
10 + 5 = 15
10 * 5 = 50
Vecteur: 1 2 3 4 5
Mode DEBUG activé
Configuration C++ installée avec succès!
```

## 📚 Ressources utiles

- [Documentation C++](https://en.cppreference.com/)
- [GCC Documentation](https://gcc.gnu.org/onlinedocs/)
- [GDB Documentation](https://sourceware.org/gdb/documentation/)
- [Makefile Tutorial](https://makefiletutorial.com/) # EuropeenOptions-C-
