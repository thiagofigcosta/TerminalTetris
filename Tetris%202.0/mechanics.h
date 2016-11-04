//THIAGO POWER CLASS FILE - tpClass
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> ///ver se funfa com windows

#include <time.h>
#include <string.h>

#include <assert.h>
#include <string.h>

#include <stdexcept>

#define OS 0
#ifdef __linux__
	#define OS 0
	#include <stdio_ext.h>
	#include <termios.h>
	#include <pthread.h>

	int getch(){
    int c=0;

      struct termios org_opts, new_opts;
      int res=0;
          //-----  store old settings -----------
      res=tcgetattr(STDIN_FILENO, &org_opts);
      assert(res==0);
          //---- set new terminal parms --------
      memcpy(&new_opts, &org_opts, sizeof(new_opts));
      new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
      tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
      c=getchar();
          //------  restore old settings ---------
      res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
      assert(res==0);
      return(c);
}
#elif _WIN32
	#define OS 1
        #include <Windows.h>
        #include <process.h>
        #include <conio.h>
#endif

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

//declara funções
typedef   void (*function_initMech)(tp_mechanics *self, int up, int down, int right, int left, int block, int scale, int zueira, int timetomove, int sleeptime);
typedef   int  (*function_random)(int max);
typedef   void (*function_flush)();
typedef   void (*function_clear)();
typedef   char*(*function_randColor)(tp_mechanics *self);
typedef   int  (*function_getch)();
typedef   int  (*function_getarrow)(tp_mechanics *self);
typedef   void (*function_printfc)(char *content,char *color);
typedef   void (*function_startThread)(tp_mechanics *self,tp_stage *stage,tp_block *block);
typedef   void (*function_wait)(tp_mechanics *self);
typedef   void (*function_printscreen)(tp_mechanics *self,tp_stage *stage,tp_block *block,int scoreatual,int maxscore);
typedef   void (*function_mix)(tp_stage *self,tp_block *block);

//declarar variaveis privadas
typedef struct{
    int up;                                  //=65;
    int down;                                //=66;
    int right;                               //=67;
    int left;                                //=68;
}tp_mechanics_private;

typedef struct{
    tp_mechanics *self;
    tp_stage *stage;
    tp_block *block;
}tp_mechanics_SB;


//classe
struct tp_mechanics{
       //variaveis publicas
       char fullBlock;                       //=(char)254;
       int scale;                            //=1;
       int zueiraneverends;                  //=1;
       int timetomovefoward;                 //=1000;
       int sleeptime;                        //=1;
       int pause;
       unsigned long int begin,end;
#ifdef __linux__
       pthread_t tid[2];
#endif
       //ponteiro para variaveis privadas
       tp_mechanics_private arrows;

       //ponteiro para funções
       function_initMech init;
       function_random random;
       function_flush flush;
       function_clear clear;
       function_randColor randColor;
       function_getch getch;
       function_getarrow getarrow;
       function_printfc printfc;
       function_startThread startThread;
       function_wait wait;
       function_printscreen printStage;
       function_mix mix;
};

//funções
int random(int max){
    srand(time(NULL));
    return rand() % max;
}
void flush(){
#ifdef __linux__
	__fpurge(stdin);
#elif _WIN32
	fflush(stdin);
#endif
}
void clear(){
    if(OS)
		system("cls");
	else
		system("clear");
}
char* randColor(tp_mechanics *self){
    char *out=(char*)malloc(10);
    switch(self->random(7)){
        default:
            strcpy(out,"default");
            return out;
        case 0:
            strcpy(out,"red");
            return out;
        case 1:
            strcpy(out,"green");
            return out;
        case 2:
            strcpy(out,"yellow");
            return out;
        case 3:
            strcpy(out,"blue");
            return out;
        case 4:
            strcpy(out,"pink");
            return out;
        case 5:
            strcpy(out,"cyan");
            return out;
        case 6:
            strcpy(out,"white");
            return out;
    }
}

int getarrow(tp_mechanics *self){
    int out=self->getch();
    if(out==27){
        out=self->getch();
        if(out==91)
            out=self->getch();
    }
    return out;
}


void printfC(char *content,char *color){
	    if(!strcmp(color,"red"))
	        printf(KRED "%s",content);//, RESET);
	    else if (!strcmp(color,"green"))
	        printf(KGRN "%s",content);//, RESET);
	    else if (!strcmp(color,"yellow"))
	        printf(KYEL "%s",content);//, RESET);
	    else if (!strcmp(color,"blue"))
	        printf(KBLU "%s",content);//, RESET);
	    else if (!strcmp(color,"pink"))
	        printf(KMAG "%s",content);//, RESET);
	    else if (!strcmp(color,"cyan"))
	        printf(KCYN "%s",content);//, RESET);
	    else if (!strcmp(color,"white"))
	        printf(KWHT "%s",content);//, RESET);
	    else
	        printf(KNRM "%s",content);//printf(KNRM "%s",content, RESET);
}

void* getInput(void *argsvoid){             //thread não funciona dentro do objeto....
    #ifdef __linux__
    pthread_t id = pthread_self();
    #endif
    tp_mechanics_SB *args=(tp_mechanics_SB*)argsvoid;

    char input;
    while(args->self->zueiraneverends){
        args->self->flush();
        input=args->self->getarrow(args->self);
        if(input==args->self->arrows.left){//<
            args->block->move(args->block,(char*)"left");
            args->self->clear();
            args->self->printStage(args->self,args->stage,args->block,0,0);
        }else if(input==args->self->arrows.right){//>
            args->block->move(args->block,(char*)"right");
            args->self->clear();
            args->self->printStage(args->self,args->stage,args->block,0,0);
        }else if(input==args->self->arrows.up){//^
            args->block->rotate(args->block);
            args->self->clear();
            args->self->printStage(args->self,args->stage,args->block,0,0);
        }else if(input==args->self->arrows.down){//v
            if(!args->block->foward(args->block)){
                args->self->mix(args->stage,args->block);
                args->block->genblock(args->block,args->self->random(7),args->self->randColor(args->self));
                args->self->clear();
                args->self->printStage(args->self,args->stage,args->block,0,0);
            }
        }
    }

    return NULL;
}

void startThread(tp_mechanics *self,tp_stage *stage,tp_block *block){
    tp_mechanics_SB* args;
    args->self=self;
    args->block=block;
    args->stage=stage;
    int err;
    #ifdef __linux__
    err=pthread_create(&(self->tid[0]), NULL, &getInput,(void*)args);
    #endif
    if (err != 0){
        printf("\ncan't create thread\nto try again press any key?");
        self->flush();
        self->getch();
        self->startThread(self,stage,block);
    }
}

void sleepWait(tp_mechanics *self){
    #ifdef __linux__
	usleep(self->sleeptime*1000);
    #else
	Sleep(self->sleeptime);
    #endif
}


void imprimirFase(tp_mechanics *self,tp_stage *stage,tp_block *block,int scoreatual,int maxscore){
    self->clear();
    int FT1=1,FT2=1,FT3=1,FT4=1;
    char buffer[10];
    sprintf(buffer,"%c",self->fullBlock);
    for(int t=0;t<self->scale*sizeXstage+2;t++)
        self->printfc(buffer,stage->color);
    printf("\n");
    for(int y=0;y<sizeYstage;y++){
        for(int t=0;t<self->scale;t++){
            self->printfc(buffer,stage->color);
            for(int x=0;x<sizeXstage;x++){
                if(x>=block->pos.x&&y>=block->pos.y&&x<block->pos.x+sizeXblock&&y<block->pos.y+sizeYblock&&block->block[y-block->pos.y][x-block->pos.x]){
                        for(int k=0;k<self->scale;k++)
                        self->printfc(buffer,block->color);
                }else{
                    if(stage->stage[y][x])
                        for(int k=0;k<self->scale;k++)
                        self->printfc(buffer,stage->color);
                    else
                        for(int k=0;k<self->scale;k++)
                        self->printfc((char*)" ",(char*)"");
                }
            }
            self->printfc(buffer,stage->color);
            if(maxscore<scoreatual)
                maxscore=scoreatual;
            if(y==4&&FT1){
                printf(" Score Atual:\n");
                FT1=0;
            }else if(y==5&&FT2){
                printf(" %d\n",scoreatual);
                FT2=0;
            }else if(y==6&&FT3){
                printf(" RECORD:\n");
                FT3=0;
            }else if(y==7&&FT4){
                printf(" %d\n",maxscore);
                FT4=0;
            }else
            printf("\n");
        }
    }
    for(int t=0;t<self->scale*sizeXstage+2;t++)
        self->printfc(buffer,stage->color);
       printf("\n");
}

void mix(tp_stage *stage,tp_block *block){
try{
    for(int y=0;y<sizeYblock;y++)
    for(int x=0;x<sizeXblock;x++)
        if(block->block[y][x]&&y+block->pos.y>=0)
	if(y+block->pos.y<sizeYstage&&x+block->pos.x<sizeXstage)
            stage->stage[y+block->pos.y][x+block->pos.x]=block->block[y][x];
}catch(const std::exception& e){
}catch(...){}
}
//inicia o objeto
void tp_mechanics_init(tp_mechanics *self, int up, int down, int right, int left, int block, int scale, int zueira, int timetomove, int sleeptime){
    //atribui funções
    self->random=random;
    self->flush=flush;
    self->clear=clear;
    self->randColor=randColor;
    self->getch=getch;
    self->getarrow=getarrow;
    self->printfc=printfC;
    self->startThread=startThread;
    self->wait=sleepWait;
    self->printStage=imprimirFase;
    self->mix=mix;

    self->arrows.up=up;
    self->arrows.down=down;
    self->arrows.left=left;
    self->arrows.right=right;

    self->fullBlock=(char)block;
    self->scale=scale;
    self->zueiraneverends=zueira;
    self->timetomovefoward=timetomove;
    self->sleeptime=sleeptime;
    self->pause=-1;
}
//THIAGO POWER CLASS FILE - tpClass
