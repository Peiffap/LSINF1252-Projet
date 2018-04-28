
#include "Text.h"

void read_file(char *filename){
    
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
        struct fractal *newFractal = read_line(line);
        if(newFractal != NULL){
            
            //met la nouvelle struct fractal dans le buffer;
            sem_wait(&empty);
            pthread_mutex_lock(&mutex);
            
            //cherche laquelle partie de buffer est vide et y place la nouvelle fractal;
            //je mets i=-1 pour sortir de la boucle dès que la place libre est trouvée;
            for(int i=0; i<=NTHREADS && i!=-1; i++){
                if(buffer[i]==NULL){
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

int count(){
    
    struct fractal *newFractal;
    int sumVal=0;
    int average;
    
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    
    //prends le premier struct trouvé;
    //je mets i=-1 pour sortir de la boucle dès que la place libre est trouvée;
    for(int i=0; i<=NTHREADS && i!=-1; i++){
        if(buffer[i]!=NULL){
            newFractal=buffer[i];
            i=-1;
        }
    }
    
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    
    //calcule la valeur pour chaque pixel
    int width = fractal_get_width(newFractal);
    int height = fractal_get_height(newFractal);
    int totalPixel = width*height;
    for (int x = 0; x<width; x++) {
        for (int y = 0; y < height; y++) {
            int val = fractal_compute_value(newFractal, x, y);
            void fractal_set_value(newFractal, x, y, val);
            sumVal += val;
        }
    }
    
    average = sumVal/totalPixel;
    
    if(average<maxAverage){
        //ajouter cette fractal dans la liste chainée
        //TO DO(créer struct de la liste chianée)
        
    }
    
}



