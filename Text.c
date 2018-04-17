
#include "Text.h"

int read_file(*filename){
    int fd=open(filename,O_RDONLY);
    if(fd==-1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    char *line=(char *)malloc(100*sizeof(char));
    for(r!=0){
        if(read(fd, (void *) &line, sizeof(char))==-1){
            exit_on_error("read filename");
        }
        
        
    
}

    
struct fractal* read_line(*line){
    
    if(*line=='#'){
        return NULL;
    }
    
    else if(*line=='\n'){
        return NULL
    }
    
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
