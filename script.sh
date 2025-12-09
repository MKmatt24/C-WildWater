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

echo "Vérification et compilation du programme C..."
make

if [ $? -ne 0 ]; then
  error_exit error_exit "La compilation du programme C a échoué. Vérifiez votre Makefile." 
fi
echo "Compilation réussie. Exécutable: $C_EXECUTABLE"



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

case "$COMMAND" in
    "histo")
        
        HISTO_TYPE=$ARG3
        
        #vérification of histo argument 
        if [ -z "$HISTO_TYPE" ]; then
            error_exit "Type d'histogramme (max, src, real) manquant pour la commande 'histo'."
        fi
        
        # Bonus work to complete
        if [ "$HISTO_TYPE" != "max" ] && [ "$HISTO_TYPE" != "src" ] && [ "$HISTO_TYPE" != "real" ] && [ "$HISTO_TYPE" != "all" ]; then
            error_exit "Type d'histogramme '$HISTO_TYPE' non valide. Doit être 'max', 'src', 'real' (ou 'all' pour le bonus)."
        fi
        
        #verification of ARG4 
        if [ ! -z "$ARG4" ]; then
             error_exit "Argument(s) supplémentaire(s) inattendu(s) pour la commande 'histo'."
        fi

        echo "Traitement : Génération d'histogramme pour le type '$HISTO_TYPE'."

        echo "Fichier de données CSV généré avec succès. Lancement de GnuPlot..."

        #TrucGnuplot

        echo "Génération des images d'histogramme terminée."
        ;;

        "leaks")
        
        FACTORY_ID=$ARG3
        
        
        if [ -z "$FACTORY_ID" ]; then
            error_exit "Identifiant d'usine manquant pour la commande 'leaks'." [cite: 192]
        fi
        
        
        if [ ! -z "$ARG4" ]; then
             error_exit "Argument(s) supplémentaire(s) inattendu(s) pour la commande 'leaks'."
        fi

        echo "Traitement : Calcul des fuites pour l'usine '$FACTORY_ID'."
        
        $C_EXECUTABLE leaks "$DATA_FILE" "$FACTORY_ID"
        
        
        C_RETURN_CODE=$?
        if [ $C_RETURN_CODE -ne 0 ]; then
            error_exit "Le programme C a retourné une erreur ($C_RETURN_CODE) lors du traitement 'leaks'."
        fi
        
        echo "Calcul des fuites et mise à jour du fichier historique de rendement terminée."
        ;;

    *)
        # principal command not recognised
        error_exit "Commande '$COMMAND' non reconnue. Doit être 'histo' ou 'leaks'."
        ;;
esac

exit 0