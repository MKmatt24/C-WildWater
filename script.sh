#!/bin/bash

start=`date +%s`

cat << "EOF"
   ___  __      ___ _    ___      __    _           
  / __|_\ \    / (_) |__| \ \    / /_ _| |_ ___ _ _ 
 | (_|___\ \/\/ /| | / _` |\ \/\/ / _` |  _/ -_) '_|
  \___|   \_/\_/ |_|_\__,_| \_/\_/\__,_|\__\___|_|  
                                                    
EOF

echo Bienvenue sur C-WildWater!

end=`date +%s`

runtime=$((end-start))
echo "Temps d'execution: $runtime seconds"
