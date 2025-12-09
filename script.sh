#!/bin/bash

START_TIME=$(date +%s%3N)


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

end=`date +%s`

runtime=$((end-start))

echo "Press enter to quit..."
read dummy

echo $$ $BASHPID - | cat -
echo "Temps d'execution: $runtime seconds"
