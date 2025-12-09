#include<stdio.h>
#include<stdlib.h>
#include<string.h>



#define MAX_LINE 1024


char *find_line(const char *filename, const char *target) {
    FILE *f = fopen(filename, "r");
    
    if (!f) {
        printf("Error file do not open");
        exit(2);
    }

    char buffer[MAX_LINE];

    // fscanf read the entiere line
    while (fscanf(f, "%1023[^\n]\n", buffer) == 1) {

        // check if the line is the target
        if (strstr(buffer, target) != NULL) {

            // copies the line found
            char *result = malloc(strlen(buffer) + 1);
            if (result){
                
                strcpy(result, buffer);
                fclose(f);
                return result;
            }    
        }
    }

    fclose(f);
    return NULL;  // nothing find
}


char* search(const char* filename) {

    char factory[40];

    printf("\n Wich factory you want to find ? \n");
    scanf("%s",factory);

    char* result=find_line(filename,factory);

    if(result==NULL) {

        printf("\n %s was not found \n",factory);
        free(result);
        return NULL;
    }

    else {




        printf("\n %s was found and archive with success \n",factory);
        return result;
    }

}
