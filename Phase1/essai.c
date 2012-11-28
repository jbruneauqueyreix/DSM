#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv[]){

char * file = "machine_file";
FILE * fid_machinefile;
int temp;
int num_procs = 0;

// Lecture du nombre de machine
if ((fid_machinefile = fopen(file,"r")) == 0)
perror("[DSMEXEC] - Error opening file");

while ((temp = fgetc(fid_machinefile)) != EOF)
{
if(temp == '\n')
++num_procs;
}

printf("[Nb machine] : %d\n", num_procs);

fclose(fid_machinefile);

// Récupération du nom des machines

if ((fid_machinefile = fopen(file,"r")) == 0)
perror("[DSMEXEC] - Error opening file");

char *line  = NULL;
size_t j = 0;
ssize_t read;
int i=0;
char **c[num_procs];

while( (read = getline(&line, &j, fid_machinefile)) != -1){
    
    printf("Réception d'une ligne de longueur %zu :\n", read);
    printf("%s", line);
    c[i] = line;
    i++;
}

    printf("%s\n", c[3]);



fclose(fid_machinefile);

return EXIT_FAILURE;

}
