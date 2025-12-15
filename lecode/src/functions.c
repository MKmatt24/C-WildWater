#include"function.h"









char* find_line(const char* filename, const char* target) {

  FILE *f = fopen(filename,"r");
  if (f==NULL) {
        
      printf("Error file do not open");
        exit(2);
    }

    char buffer[MAX_LINE];

    // fscanf lit jusqu'à la fin de ligne (le \n est consommé grâce à \n final)
    while (fscanf(f,"%1023[^\n]\n",buffer) == 1) {

        // Vérifie si la ligne contient la clé recherchée
        if (strstr(buffer,target) != NULL) {

            // Copie dynamique de la ligne trouvée
            char *result = malloc(strlen(buffer) + 1);
            if (result)
                strcpy(result,buffer);

            fclose(f);
            return result;
        }
    }

    fclose(f);
    return NULL;  // Rien trouvé
}

















