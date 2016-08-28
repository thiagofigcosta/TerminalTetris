//THIAGO POWER CLASS FILE - tpClass
#include <stdlib.h>
#include <string.h>

//declara funções
typedef   void (*function_initStage)(tp_stage *self,int x, int y);
typedef   int (*function_checkstagerows)(tp_stage *self);
typedef   void (*function_clearStage)(tp_stage *self,char* color);
typedef   int (*function_ggchecker)(tp_stage *self);
    
//declarar variaveis privadas
typedef struct{
    int x;//=sizeXstage;                                              
    int y;//=sizeYstage;                                              
}tp_stage_private;

//classe
struct tp_stage{
        //variaveis publicas
       char color[10];
       int stage[sizeYstage][sizeXstage];
       
       //ponteiro para variaveis privadas
        tp_stage_private size;
       
       //ponteiro para funções
       function_initStage init;
       function_checkstagerows checkstagerows;
       function_clearStage clear;
       function_ggchecker ggchecker;
};

//funções
int checkstagerows(tp_stage *self){        
    int onlyOnes, out=0;
   for(int y=sizeYstage-1;y>=0;y--){
        onlyOnes=1;
        for(int x=sizeXstage-1;x>=0;x--){
            if(self->stage[y][x]==0){
                onlyOnes=0;
                break;
            }
        }
        if(onlyOnes){ 
            out++;
            for(int x=0;x<sizeXstage;x++)
                self->stage[y][x]=0;
            
            for(int y2=y;y2>0;y2--)
                for(int x=0;x<sizeXstage;x++)
                    self->stage[y2][x]=self->stage[y2-1][x];
        }
    }
    return out;
}
void clearS(tp_stage *self,char* color){
    strcpy(self->color,color);
    for(int y=0;y<sizeYstage;y++){
        for(int x=0;x<sizeXstage;x++){
            self->stage[y][x]=0;
        }
    }
}

int ggCheker(tp_stage *self){
    int gg=0;
    for(int x=0;x<sizeXstage;x++)
        if(self->stage[0][x]==1){
            gg=1;
            break;
        }
    return gg;
}
//inicia o objeto
void tp_stage_init(tp_stage *self,int x, int y){
    //atribui funções
    self->checkstagerows=checkstagerows;
    self->clear=clearS;
    self->ggchecker=ggCheker;
    
    self->size.x=x;
    self->size.y=y;
    
}
//THIAGO POWER CLASS FILE - tpClass
