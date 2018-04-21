
#include "Text.h"

int global NTHREADS; // donnees pendant l'appel du programme (TO DO)
struct fractal *buffer[NTHREADS+1]; //buffer pour stocker les fractal
pthread_t thread[NTHREADS]; //nombre de thread qu'on lance par fichier


pthread_mutex_t mutex;
sem_t empty;
sem_t full;

//j'ai mit un main, mais il est évidement pas complet;
//il y a juste une partie que j'en avait besoin pour faire la fonction read_file


int main (int argc, char *argv[])  {
    
    //initialisation de mutex
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0 , NTHREADS+1);
    sem_init(&full, 0 , 0);
    
    //creation des threads; argc[i+1] c'est les arguments pour la fonction read_file càd les fichirs qu'on doit lire
    for(int i=0;i<NTHREADS;i++){
        err=pthread_create(&(thread[i]),NULL,&read_file,argc[i+1]);
        if(err!=0){
            error(err,"pthread_create");
        }
    }
    
    //join le thread
    for(int i=NTHREADS-1;i>=0;i--) {
        err=pthread_join(thread[i],NULL);
        if(err!=0){
            error(err,"pthread_join");
        }
    }
}

int read_file(char *filename){
    
    //ouvre le fichier
    int fd=open(filename,O_RDONLY);
    if(fd==-1) {
        perror("open file");
        exit(EXIT_FAILURE);
    }
    
    //pour allouer la mémoire, je savais pas ça sera quoi la multiplication de sizeof le plus raisonable;
    //j'ai mit 100 mais c'est encore à réfléchir;
    char *line=(char *)malloc(100*sizeof(char));
    
    //lit fichier ligne pas ligne
    int read=read(fd, (void *) &line, sizeof(char));
    if(read==-1){
        exit_on_error("read file");
    }
    for(r!=0){
        //création d'une nouvelle structure fractal;
        struct fractal newFractal = read_line(line);
        if(newFractal != NULL){
            
            //met la nouvelle structure dans le buffer;
            sem_wait(&empty);
            pthread_mutex_lock(&mutex);
            
            //cherche laquelle partie de buffer est vide et y place la nouvelle fractal;
            //je mets i=-1 pour sortir de la boucle dès que la place libre est trouvée;
            for(int i=0; i<=NTHREADS && i!=-1; i++){
                if(*buffer[i]==NULL){
                    buffer[i]=newFractal;
                    i=-1;
                }
            }
            
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
        }
        
        if(read(fd, (void *)&line, sizeof(char))==-1){
            exit_on_error("read file");
        }
    }
    
    //ferme le fichier
    if (close(fd)==-1){
        perror("close file");
        exit(EXIT_FAILURE);
    }
}

//lit la ligne  et retourne la structure fractal, NULL si la ligne commence pas # ou la ligne est vide
struct fractal* read_line(char *line){
    
    if(*line=='#'){
        return NULL;
    }
    
    else if(*line=='\n'){
        return NULL
    }
    //sépare les mots
    const char *sep = ' ';
    char *fractal = strtok(line, sep);
    
    char *name;
    int width;
    int height;
    double a;
    double b;
    
    for(int i==0; istr != NULL; i++)
    {
        if(i==0){
            name = istr;
        }
        else if(i==1){
            width = (int)strtol(istr, (char **) NULL, 10);
        }
        else if(i==2){
            height = (int)strtol(istr, (char **) NULL, 10);
        }
        else if(i==3){
            a = strtod(istr, (char **) NULL);
        }
        else if(i==4){
            b = strtod(istr, (char **) NULL);
        }
        
        istr = strtok (NULL,sep);
    }
    return struct fractal *fractal_new(name, width, height, a, b);
}
    

