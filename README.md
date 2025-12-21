# 🌊 C-WildWater Project

**C-WildWater** est une application d'analyse de réseaux de distribution d'eau. Elle combine la performance du **C** (avec des structures de données optimisées de type AVL) pour le traitement des données et la flexibilité du **Shell/GnuPlot** pour l'automatisation et la visualisation graphique.

##  Prérequis

Pour exécuter ce projet, votre système Linux doit disposer des outils suivants :

* **GCC** : Compilateur C standard.
* **Make** : Pour l'automatisation de la compilation.
* **GnuPlot** : Pour la génération des graphiques (fichiers PNG).
* **Outils CLI** : `bash`, `awk`, `sed`, `sort`.

### Installation des dépendances (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential gnuplot
```
### Installation
Placez-vous à la racine du projet.

Rendez le script principal exécutable :

```bash
chmod +x script.sh
```
### Utilisation
Tout le projet se pilote via le script script.sh.

Syntaxe Générale
```Bash
./script.sh <fichier_donnees> <commande> [argument]
```
#### 1. Générer des Histogrammes (histo)
Cette commande génère des graphiques montrant les volumes d'eau par station. Elle produit deux images :

Un graphique des **10 plus grandes** stations.

Un graphique des **50 plus petites** stations.

Syntaxe :

```Bash
./script.sh <fichier.dat> histo <mode>
```

Modes disponibles :

**src** : Trie selon le volume provenant des sources (source volume).

**max** : Trie selon la capacité maximale (max capacity).

**real** : Trie selon le volume réel reçu (real volume).

Exemple :

```Bash
./script.sh data/c-wildwater_v3.dat histo src
Les images histo_source_volume_largest.png et smallest.png seront générées à la racine.
```

#### 2. Analyser les Fuites (leaks)
Cette commande calcule le **pourcentage** de pertes d'eau pour une station donnée en analysant tout son réseau en amont.

Syntaxe :

```Bash
./script.sh <fichier.dat> leaks "<Identifiant_Station>"
```

Attention : Si l'identifiant contient des espaces ou des caractères spéciaux (comme #), utilisez impérativement des guillemets.
Exemple :

```Bash
./script.sh data/c-wildwater_v3.dat leaks "Factory #AA400033V"
Le résultat sera affiché dans la console et ajouté au fichier leaks.dat.
```
