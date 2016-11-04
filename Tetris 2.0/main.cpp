#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//int sizeXstage=18;
//int sizeYstage=24;
#define sizeXstage 20
#define sizeYstage 24

#define sizeYblock 4 //x deve ser igual y
#define sizeXblock 4

//declara classe
typedef   struct tp_block tp_block;
typedef   struct tp_stage tp_stage;
typedef   struct tp_mechanics tp_mechanics;

//inclui classes
#include "stage.h"
#include "block.h"
#include "mechanics.h"

unsigned int currentscore;
unsigned int record;
int pontosporzeraravidacaralho=5;
int menustate=0;
int firsttimeingame=1;

int blococheio=254;
int escala=2;
int tempoparaandar=5;
int velocidadedojogo=100;
int setacima=65;
int setabaixo=66;
int setadireito=67;
int setaequerda=68;

//declara powerclasses
tp_block Bloco;
tp_stage Fase;
tp_mechanics Mecanicas;

//delcara funções
void setup();
void game();
void load();
void save();
void menu();


 #ifdef __linux__
void* getinput(void *arg);
#endif // __linux__

#ifdef _WIN32
void getinput(void *arg);
#endif
int main(){
    setup();
    while(Mecanicas.zueiraneverends)
        game();
   return 0;
}

void setup(){
    if(firsttimeingame){
        Fase.init=tp_stage_init;
        Bloco.init=tp_block_init;
        Mecanicas.init=tp_mechanics_init;
        firsttimeingame=0;
        #ifdef __linux__
        pthread_t tid[2];
        pthread_create(&(tid[0]), NULL, &getinput,NULL);
        #endif // __linux__

        #ifdef _WIN32
         _beginthread(getinput,0,(void*)NULL);
        #endif // _WIN32
        //Mecanicas.startThread(&Mecanicas,&Fase,&Bloco);  ---------não funciona
    }
    Fase.init(&Fase,sizeXstage,sizeYstage);
    Bloco.init(&Bloco,&Fase,1);
	if(OS){
		setacima=119;
		setabaixo=115;
		setadireito=100;
		setaequerda=97;
		velocidadedojogo=1;
		tempoparaandar=1;

	}
    Mecanicas.init(&Mecanicas,setacima,setabaixo,setadireito,setaequerda,blococheio,escala,1,tempoparaandar,velocidadedojogo);
}

void game(){
    menu();
    int rows;
    load();
    currentscore=0;
    Fase.clear(&Fase,Mecanicas.randColor(&Mecanicas));
    Mecanicas.begin =(long int) clock();
    Bloco.genblock(&Bloco,Mecanicas.random(7),Mecanicas.randColor(&Mecanicas));
    Mecanicas.clear();
    Mecanicas.printStage(&Mecanicas,&Fase,&Bloco,currentscore,record);
    do{
        Mecanicas.end = (long int)clock();
        if(Mecanicas.end-Mecanicas.begin>=Mecanicas.timetomovefoward){
            currentscore+=2;
            Mecanicas.begin = clock();
            if(!Bloco.foward(&Bloco)){
                Mecanicas.mix(&Fase,&Bloco);
                if(Fase.stage[1][((sizeXstage-sizeXblock)/2)+1]==1)
                    break;
                Bloco.genblock(&Bloco,Mecanicas.random(7),Mecanicas.randColor(&Mecanicas));
                currentscore+=5;
            }
            //Mecanicas.clear();
            Mecanicas.printStage(&Mecanicas,&Fase,&Bloco,currentscore,record);
        }
        rows=Fase.checkstagerows(&Fase);
        if(Mecanicas.pause>0){
            Mecanicas.clear();
            printf("\n\n\n\n               pause, press p\n");
        }
        while(Mecanicas.pause>0);
        for(int b=0;b<rows;b++)
            pontosporzeraravidacaralho*=10;
        if(rows){
            currentscore+=pontosporzeraravidacaralho;
            pontosporzeraravidacaralho=5;
        }
        Mecanicas.wait(&Mecanicas);
    }while(!Fase.ggchecker(&Fase));
    Mecanicas.clear();
    if(record<currentscore){
        printf("GG WP\nVoce bateu o record, o novo valor é %d",currentscore);
        save();
    }else{
        printf("GG EZ\n");
    }

    Mecanicas.flush();
    Mecanicas.getch();
}

 #ifdef __linux__
void* getinput(void *arg){
pthread_t id = pthread_self();
#endif // __linux__

#ifdef _WIN32
void getinput(void *arg){
#endif
    char input;
    while(Mecanicas.zueiraneverends){
        if(menustate==0){
            Mecanicas.flush();
            input=Mecanicas.getarrow(&Mecanicas);
            if(input=='p'||input=='P')
                Mecanicas.pause*=-1;
            if(Mecanicas.pause<0)
                if(input==Mecanicas.arrows.left){//<
                    Bloco.move(&Bloco,(char*)"left");
                    //Mecanicas.clear();
                    Mecanicas.printStage(&Mecanicas,&Fase,&Bloco,currentscore,record);
                }else if(input==Mecanicas.arrows.right){//>
                    Bloco.move(&Bloco,(char*)"right");
                    //Mecanicas.clear();
                    Mecanicas.printStage(&Mecanicas,&Fase,&Bloco,currentscore,record);
                }else if(input==Mecanicas.arrows.up){//^
                    Bloco.rotate(&Bloco);
                    //Mecanicas.clear();
                    Mecanicas.printStage(&Mecanicas,&Fase,&Bloco,currentscore,record);
                }else if(input==Mecanicas.arrows.down){//v
                    if(!Bloco.foward(&Bloco)){
                        Mecanicas.mix(&Fase,&Bloco);
                        Bloco.genblock(&Bloco,Mecanicas.random(7),Mecanicas.randColor(&Mecanicas));
                        //Mecanicas.clear();
                        Mecanicas.printStage(&Mecanicas,&Fase,&Bloco,currentscore,record);
                    }
                }
        }
    }
}

void load(){
    record=0;
    FILE *VLWNEGREROS;

    VLWNEGREROS=fopen("record.cr","r+");
    if(VLWNEGREROS!=NULL){
        fscanf(VLWNEGREROS,"%d",&record);
    }
    fclose(VLWNEGREROS);
}

void save(){
    FILE *VLWNEGREROS;

    VLWNEGREROS=fopen("record.cr","w");
    if(VLWNEGREROS!=NULL){
        fprintf(VLWNEGREROS,"%d",currentscore);
    }
    fclose(VLWNEGREROS);
}

void menu(){
    menustate=1;
    Mecanicas.clear();
    printf("BEM VINDOS AO TETRIS, JOGO SUPER DIVERTIDO:\nPor Thiago, Ian,Pedro Bizerra, Alexandre Pinho\n\nDurante o jogo voce deve usar as setas do teclado para controlar a peca\nP para pausar, Control+C para sair\n...pronto :D, so isso Pressione \"o\" para ir para opcoes e qualquer outra para iniciar\n, Boa sorte(pq é dificil) e bom jogo :D");
    Mecanicas.flush();
    char in=Mecanicas.getch();
    if(in=='o'||in=='O'){
        while(in!='e'||in!='E'){
            Mecanicas.clear();
            printf("Para modificar um valor digite a letra entre parenteses e depois entre com o valor\n");
            //printf("(1)Tamanho X: %d\n",sizeXstage);
            //printf("(2)Tamanho Y: %d\n",sizeYstage);
            printf("(1)Bloco cheio: %d\n",blococheio);
            printf("(2)Scale: %d\n",escala);
            printf("(3)Tempo para andar: %d\n",tempoparaandar);
            printf("(4)Velocidade do jogo: %d\n",velocidadedojogo);
            printf("(5)Valor de UP: %d\n",setacima);
            printf("(6)Valor de DOWN: %d\n",setabaixo);
            printf("(7)Valor de LEFT: %d\n",setaequerda);
            printf("(8)Valor de RIGHT: %d\n",setadireito);
            printf("(E)SAIR das opcoes\n");
            Mecanicas.flush();
            in=Mecanicas.getch();
            switch(in){
                //case '1':scanf("%d",&sizeXstage);  break;
                //case '2':scanf("%d",&sizeYstage);  break;
                case '1':scanf("%d",&blococheio);  break;
                case '2':scanf("%d",&escala);  break;
                case '3':scanf("%d",&tempoparaandar);  break;
                case '4':scanf("%d",&velocidadedojogo);  break;
                case '5':scanf("%d",&setacima);  break;
                case '6':scanf("%d",&setabaixo);  break;
                case '7':scanf("%d",&setaequerda);  break;
                case '8':scanf("%d",&setadireito);  break;
                case 'e': setup(); menu(); return;
                case 'E': setup(); menu(); return;
            }
           setup();
        }
    }
    menustate=0;
}

