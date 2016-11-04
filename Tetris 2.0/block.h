//THIAGO POWER CLASS FILE - tpClass
#include <stdlib.h>
#include <string.h>

//declara funções
typedef   void (*function_initBlock)(tp_block *self,tp_stage *fase,int steps);
typedef   int  (*function_foward)(tp_block *self);
typedef   void (*function_rotate)(tp_block *self);
typedef   void (*function_genblock)(tp_block *self, int n,char* color);
typedef   void (*function_move)(tp_block *self,char *dir);
    
//declarar variaveis privadas
typedef struct{
    int x;
    int y;
}tp_block_private;

//classe
struct tp_block{
       //variaveis publicas
       int moveSteps;//                     1
       char color[10];
       int block[sizeYblock][sizeXblock];
       tp_stage *fase;
       
       //ponteiro para variaveis privadas
       tp_block_private pos;
       
       //ponteiro para funções
       function_initBlock init;
       function_foward foward;
       function_genblock genblock;
       function_rotate rotate;
       function_move move;
};

//funções
int blockFoward(tp_block *self){
    int lastY=0;
    for(int y=sizeYblock-1;y>=0;y--){
        for(int x=sizeXblock-1;x>=0;x--){
            if(self->block[y][x]==1&&!lastY){
                lastY=y;
                break;
            }
        }
    }
    int canMoveFow=1;
    for(int y=0;y<lastY+1;y++)
        for(int x=0;x<sizeXblock;x++){
            if(self->block[y][x])
                if(self->fase->stage[self->pos.y+y+self->moveSteps][self->pos.x+x]||self->pos.y+y+self->moveSteps>=sizeYstage){
                    canMoveFow=0;
                }
        }
    if(canMoveFow){
        self->pos.y+=self->moveSteps;
    }  
    return canMoveFow;
}
void blockRotate(tp_block *self){
    try{
        int buffer[sizeYblock][sizeXblock];
    int backup[sizeYblock][sizeXblock];
    for(int y=0;y<sizeYblock;y++)
        for(int x=0;x<sizeXblock;x++){
            backup[y][x]=self->block[y][x];
            buffer[y][sizeXblock-x-1]=self->block[y][x];
            self->block[y][x]=0;
        }
        
    for(int y=0;y<sizeYblock;y++)
        for(int x=0;x<sizeXblock;x++)
            self->block[y][x]=buffer[x][y];
        
        
    int rowClear=0, rowSignal=1;    
    for(int x=0;x<sizeXblock;x++){
        int funcionapls=1;
        for(int y=0;y<sizeYblock;y++){
            if(backup[y][x]==1){
                funcionapls=0;
                break;
            }
        }
        if(funcionapls){
            rowClear++;
            if(x==0)
                rowSignal*=-1;
        }
    }
    int lineClear=0, lineSignal=1;    
    for(int y=0;y<sizeYblock;y++){
        int funcionapls=1;
        for(int x=0;x<sizeXblock;x++){
            if(backup[y][x]==1){
                funcionapls=0;
                break;
            }
        }
        if(funcionapls){
            lineClear++;
            if(y==sizeYblock)
                lineSignal*=-1;
        }
    }  
    for(int y=0;y<sizeYblock;y++){
        for(int x=0;x<sizeXblock;x++){
            buffer[y][x]=self->block[y][x];
            //printf("%d",self->block[y][x]);
            self->block[y][x]=0;
        }//printf("\n");
    }
    
    //printf("row%d   line%d",rowClear*rowSignal,lineClear*lineSignal);printf("\n");
    
    for(int y=0;y<sizeYblock-rowClear;y++){  //-row
        for(int x=0;x<sizeXblock-lineClear;x++){  //-line
            int posix, posiy;
            if(rowSignal=1)
                posix=x;
            else
                posix=x+lineClear;
            
            if(lineSignal=1)
                posiy=y+rowClear;
            else
                posiy=y;
            
            
            self->block[y][x]=buffer[posiy][posix];
                
            //printf("%d",self->block[y][x]);
            
        }//printf("\n");
        
    }//getchar();
        /*
         xxxx  
        y0000
        y0000
        y0001
        y0111
         xxxx
        y0000
        y0000
        y1100
        y1100
        */
    int needToRestore=0;
    for(int y=0;y<sizeYblock;y++)
        for(int x=0;x<sizeXblock;x++){
            if(self->block[y][x]==1){
               /*if(self->pos.y+4>=sizeYblock||self->pos.y<0||self->pos.x<0||self->pos.x+4>=sizeXstage){
                   needToRestore=1;
                   break;
               }*/
               if(self->fase->stage[(self->pos.y+y)][(self->pos.x+x)]==1){
                   needToRestore=1;
                   break;
                }
        }
    }
    if(needToRestore)
        for(int y=0;y<sizeYblock;y++)
        for(int x=0;x<sizeXblock;x++)
            self->block[y][x]=backup[y][x];
    }catch(const std::exception& e){
        for(int y=0;y<sizeYblock;y++)
        for(int x=0;x<sizeXblock;x++)
            self->block[y][x]=backup[y][x];
    }  
}
void blockGen(tp_block *self,int n,char* color){
    strcpy(self->color,color);
    for(int y=0;y<sizeYblock;y++)
        for(int x=0;x<sizeXblock;x++)
            self->block[y][x]=0;
        
    switch(n){
        case 1:
        self->block[0][0]=1;
        self->block[0][1]=1;
        self->block[0][2]=1;
        self->block[0][3]=1;
        break;
        case 2:
        self->block[0][0]=1;
        self->block[1][1]=1;
        self->block[1][2]=1;
        self->block[1][0]=1;
        break;
        case 3:
        self->block[0][2]=1;
        self->block[1][1]=1;
        self->block[1][2]=1;
        self->block[1][0]=1;
        break;
        case 4:
        self->block[0][0]=1;
        self->block[0][1]=1;
        self->block[1][0]=1;
        self->block[1][1]=1;
        break;
        case 5:
        self->block[0][1]=1;
        self->block[0][2]=1;
        self->block[1][0]=1;
        self->block[1][1]=1;
        break;
        case 6:
        self->block[0][1]=1;
        self->block[1][1]=1;
        self->block[1][2]=1;
        self->block[1][0]=1;
        break;
        default:
        self->block[0][0]=1;
        self->block[0][1]=1;
        self->block[1][1]=1;
        self->block[1][2]=1;
        break;
    }
    
    int lastY=0;
    for(int y=sizeYblock-1;y>=0;y--){
        for(int x=sizeXblock-1;x>=0;x--){
            if(self->block[y][x]==1&&!lastY){
                lastY=y;
                break;
            }
        }
    }
    self->pos.y=-1*(lastY+1);
    self->pos.x=((sizeXstage-sizeXblock)/2)+1;    
}
void blockMove(tp_block *self,char *dir){
    if(!strcmp(dir,"right")){
        int lastX=0;
        for(int x=sizeXblock-1;x>=0;x--){
            for(int y=sizeYblock-1;y>=0;y--){
                if(self->block[y][x]==1&&!lastX){
                    lastX=x;
                    break;
                }
            }
        }
        int canMoveR=1;
        for(int y=0;y<sizeYblock;y++)
            for(int x=0;x<lastX+1;x++){
                if(self->block[y][x])
                    if(self->fase->stage[self->pos.y+y][self->pos.x+x+self->moveSteps]||self->pos.x+x+self->moveSteps>=sizeXstage){
                        canMoveR=0;
                    }
            }
        if(canMoveR){
            self->pos.x+=self->moveSteps;
        }  
    }else{
        int firstX=sizeXblock-1;
        for(int x=0;x<sizeXblock;x++){
            for(int y=sizeYblock-1;y>=0;y--){
                if(self->block[y][x]==1&&firstX!=sizeXblock-1){
                    firstX=x;
                    break;
                }
            }
        }
        int canMoveR=1;
        for(int y=0;y<sizeYblock;y++)
            for(int x=firstX;x>=0;x--){
                if(self->block[y][x])
                    if(self->fase->stage[self->pos.y+y][self->pos.x+x-self->moveSteps]||self->pos.x+x-self->moveSteps<0){
                        canMoveR=0;
                    }
            }
        if(canMoveR){
            self->pos.x-=self->moveSteps;
        }  
    }
}

//inicia o objeto
void tp_block_init(tp_block *self,tp_stage *fase, int steps){
    //atribui funções
    self->foward=blockFoward;
    self->genblock=blockGen;
    self->rotate=blockRotate;
    self->move=blockMove;
    
    self->fase=fase;
    
    self->moveSteps=steps;
    
}
//THIAGO POWER CLASS FILE - tpClass
