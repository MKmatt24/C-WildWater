#!/bin/bash

START_TIME=$(date +%s%3N)

C_EXECUTABLE="./C-Wild"


#affiche la durée totale en millisecondes
function display_time {
  if [ ! -z "$START_TIME" ]; then
    END_TIME=$(date +%s%3N)
    DURATION=$((END_TIME - START_TIME))
    echo "Temps d'éxecution total: ${DURATION} ms"
  fi
}

#affiche le temps d'éxecution en fin de script ou en cas d'erreur
trap display_time EXIT


cat << "EOF"
   ___  __      ___ _    ___      __    _           
  / __|_\ \    / (_) |__| \ \    / /_ _| |_ ___ _ _ 
 | (_|___\ \/\/ /| | / _` |\ \/\/ / _` |  _/ -_) '_|
  \___|   \_/\_/ |_|_\__,_| \_/\_/\__,_|\__\___|_|  
                                                    
EOF

echo Bienvenue sur C-WildWater!

echo "---"

#vérification de la compilation du programme C
echo "Vérification et compilation du programme C..."

make

if [ $? -ne 0 ]; then
  error_exit error_exit "La compilation du programme C a échoué. Vérifiez votre Makefile." 
fi
echo "Compilation réussie. Exécutable: $C_EXECUTABLE"

#vérification et parsing des arguments

DATA_FILE=$1
COMMAND=$2
ARG3=$3
ARG4=$4

if [ -z "$DATA_FILE" ] || [ -z "$COMMAND" ]; then
    error_exit "Arguments incomplets. Utilisation: myScript.sh <fichier_csv> <commande> [args...]"
fi

if [ "$COMMAND" != "histo" ] && [ ! -z "$ARG4" ]; then
    error_exit "Argument(s) supplémentaire(s) inattendu(s) après '$ARG3'."
fi