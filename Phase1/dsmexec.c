#include "common_impl.h"

#define DEBUG 1
#define STR_SIZE 128

typedef char my_type_t[STR_SIZE]; 

/* variables globales */

/* un tableau gerant les infos d'identification */
/* des processus dsm */
static dsm_proc_t *proc_array = NULL;

/* le nombre de processus effectivement crees */
static volatile int num_procs_creat = 0;

void usage(void)
{
  fprintf(stdout,"Usage : dsmexec machine_file executable arg1 arg2 ...\n");
  fflush(stdout);
  exit(EXIT_FAILURE);
}

void sigchld_handler(int sig)
{
	/* on traite les fils qui se terminent */
   /* pour eviter les zombies */
	int status;
	wait(&status);
}

int read_number_proc(char * file){
     int temp;
     int num_procs = 0;
     FILE * fid_machinefile;

     if ((fid_machinefile = fopen(file,"r")) == 0)
		perror("[DSMEXEC] - Error opening file");

     while ((temp = fgetc(fid_machinefile)) != EOF)
     {
		if(temp == '\n')
         ++num_procs;
	 }
	 fclose(fid_machinefile);
	 return num_procs;	
}

void read_proc_name(char *file, my_type_t *nom_procs[]){
	char *line  = NULL;
	size_t j = 0;
	ssize_t read;
	FILE * fid_machinefile;
	int i = 0;

	if ((fid_machinefile = fopen(file,"r")) == 0)
		perror("[DSMEXEC] - Error opening file\n");
	
	while( (read = getline(&line, &j, fid_machinefile)) != -1){
		if (DEBUG){
			printf("Ligne de longueur %d :\n", read);
			printf("%s", line);
		}
		nom_procs[i++] = line;
	}
	fclose(fid_machinefile);
}

int main(int argc, char * argv[])
{
  if (argc < 3){
    usage();
  }
  else{
     pid_t pid;
     int num_procs = 0;
     int i = 0;
     struct sigaction action_zombie;
     int sock_ecoute;
     struct sockaddr_in sockaddr_ecoute;
     int port = 33000;
     int fd_stdout[2], fd_stderr[2];
     
     /* Mise en place d'un traitant pour recuperer les fils zombies*/      
     action_zombie.sa_handler = sigchld_handler;
     action_zombie.sa_flags = 0;
     
     sigaction(SIGCHLD, &action_zombie, NULL);
     
    /* lecture du fichier de machines */
    /* 1- on recupere le nombre de processus a lancer */
    
    num_procs = read_number_proc(argv[1]);

     /* 2- on recupere les noms des machines : le nom de */
     /* la machine est un des elements d'identification */
	my_type_t *nom_procs[num_procs]; // Création d'un poiteur sur un tableau de tableau de caractere
     	
	read_proc_name(argv[1], nom_procs);

	if (DEBUG){
		fprintf(stdout, "[DSMEXEC] : %d procs a lancer\n ", num_procs);
		for (i = 0; i<num_procs; i++)
			printf("%s", nom_procs[i]);
	}

     /* creation de la socket d'ecoute */
	
	memset(&sockaddr_ecoute, 0, sizeof(sockaddr_ecoute)); 
	sockaddr_ecoute.sin_family = AF_INET;
	sockaddr_ecoute.sin_port = htons(port);
	sockaddr_ecoute.sin_addr.s_addr = INADDR_ANY;	
	sock_ecoute = socket(AF_INET, SOCK_STREAM, 0);

     /* + ecoute effective */ 
	if( bind(sock_ecoute, (struct sockaddr *) &sockaddr_ecoute, sizeof(sockaddr_ecoute))) 
		fprintf(stdout, "[DSMEXEC] : Error binding socket \n");
		
	if (listen(sock_ecoute, num_procs) == -1)
		fprintf(stdout, "[DSMEXEC] : Error listening \n");
		
     /* creation des fils */
     for(i = 0; i < num_procs ; i++) {
	
	/* creation du tube pour rediriger stdout */
	if (pipe(fd_stdout) == -1)
		fprintf(stdout, "[DSMEXEC] : Error creating pipe stdout \n");

	/* creation du tube pour rediriger stderr */
	if (pipe(fd_stderr) == -1)
		fprintf(stdout, "[DSMEXEC] : Error creating pipe stderr\n");
	
	pid = fork();
	if(pid == -1) ERROR_EXIT("fork");
	
	if (pid == 0) { /* fils */
	   
	   /* redirection stdout */	      
		close(fd_stdout[0]);
		dup2(STDOUT_FILENO,fd_stdout[1]);
		
	   /* redirection stderr */	      	      
		close(fd_stderr[0]);
		dup2(STDERR_FILENO,fd_stdout[1]);
		
	   /* Creation du tableau d'arguments pour le ssh */ 
	   char *newargv;
	   strcat(newargv, nom_procs[i]);
	   strcat(newargv," truc");
	   
	   if(DEBUG)
		fprintf(stdout, "%s\n", newargv);
	   
	   /* jump to new prog : */
	   execvp("ssh",newargv);
	   
	} else  if(pid > 0) { /* pere */		      
	   /* fermeture des extremites des tubes non utiles */
	   close(fd_stdout[1]);
	   close(fd_stderr[1]);
	   
	   //~ char *c;
	   //~ read(fd_stdout[0], c, 1);
	   //~ printf("%c\n",c);
	   num_procs_creat++;    
	}
    }
     
     for(i = 0; i < num_procs ; i++){
	
	/* on accepte les connexions des processus dsm */
	
	/*  On recupere le nom de la machine distante */
	/* 1- d'abord la taille de la chaine */
	/* 2- puis la chaine elle-meme */
	
	/* On recupere le pid du processus distant  */
	
	/* On recupere le numero de port de la socket */
	/* d'ecoute des processus distants */
     }
     
     /* envoi du nombre de processus aux processus dsm*/
     
     /* envoi des rangs aux processus dsm */
     
     /* envoi des infos de connexion aux processus */
     
     /* gestion des E/S : on recupere les caracteres */
     /* sur les tubes de redirection de stdout/stderr */     
     /* while(1)
         {
            je recupere les infos sur les tubes de redirection
            jusqu'à ce qu'ils soient inactifs (ie fermes par les
            processus dsm ecrivains de l'autre cote ...)
       
         };
      */
     
     /* on attend les processus fils */
     
     /* on ferme les descripteurs proprement */
     
     /* on ferme la socket d'ecoute */
     }   
   exit(EXIT_SUCCESS);  
}
