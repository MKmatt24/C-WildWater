#!/bin/bash


#code couleur ANSI
RED="\033[0;31m"     
GREEN="\033[0;32m"    
BLUE="\033[0;34m"  
CYAN="\033[0;36m" 
MAGENTA="\033[0;35m"
YELLOW="\033[0;33m"
WHITE="\033[1;37m"
BLACK="\033[0;30m" 

#fonction permettant l'affiche de l'ascii art
display ascii_art() {
  cat << "EOF"
   ___  __      ___ _    ___      __    _           
  / __|_\ \    / (_) |__| \ \    / /_ _| |_ ___ _ _ 
 | (_|___\ \/\/ /| | / _` |\ \/\/ / _` |  _/ -_) '_|
  \___|   \_/\_/ |_|_\__,_| \_/\_/\__,_|\__\___|_|  
                                                    
EOF

  
}


start=`date +%s`



echo Bienvenue sur C-WildWater!

end=`date +%s`

runtime=$((end-start))
echo "Temps d'execution: $runtime seconds"