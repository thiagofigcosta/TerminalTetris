#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

#include <stdio_ext.h>

#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

/* mechanics
 * salvar Score em texto e exibir
 */
/*ERROSSSSSSSSSS
 
 * bloco começando do meio do mapa
 * block gen bugado
 * checkStageRows nao funciona
 * verificar blockFoward
 * ascII nao funciona
 *bloco nao gira
 */

#define OS 0
#ifdef __linux__
	#define OS 0
#elif _WIN32
	#define OS 1
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

#define VK_LEFT 68//??
#define VK_RIGHT 67//??
#define VK_UP 65//??
#define VK_DOWN 66//??

#define moveSteps 1//passos que os blocos movem
#define stageSizeY 25//tamanho vertical
#define stageSizeX 25//tamanho horizontal  7 para teste
#define scale 1 //escala de impressao
#define layers 2 //0-stage;1-blocks;
#define fullBlock 254 //178, 219, 254
#define zueiraneverends 1
#define timeToMoveFow 100000
#define sleepTime 0.5
#define maxBlockSize 20

int stage[stageSizeX][stageSizeY][layers];

char colorStage[10];
char colorBlock[10];

void* getInput(void *arg);//le o teclado
void rotateBlock();//rotaciona o bloco
void printfC(char *content,char *color);//imprime colorido na tela
int random(int max);//randomiza um numero
void clearBlocks();//limpa a matriz dos blocos
void genBlock(int max_size);//gera um bloco aleatoriamente
int ggChecker();//verifica se perdeu o jogo
void checkStageRows();//verifica se alguma linha esta completa
void blockMove(char *dir,int steps);//move o bloco para os lados
int blockFoward(int steps);//move o bloco para baixo
void mixBlocks();//mistura a matriz dos blocos com a matriz do fundo
void clearStage();//limpa a matriz dos blocos
void flush();//limpa o buffer
void clear();//limpa a tela
char* random_Color();//randomiza uma cor
void printStage();//escreve na tela
void mechanics();//mecanicas do jogo
int getch(void);//função getchar sem o enter
int getArrow();//le flechas do teclado
int lineClear(int y,int steps);//ve se a linha está ok

pthread_t tid[2];
//clock_t start_t, end_t;

int main(){
    int err= pthread_create(&(tid[0]), NULL, &getInput, NULL);
    if (err != 0){
        printf("\ncan't create thread\nto try again press any key?");
        flush();
        getch();
        main();
    }else{
        while(zueiraneverends)
            mechanics();
    }
    return 0;
}

void mechanics(){
    unsigned long int begin,end;
    clearStage();
    begin =(long int) clock();
    genBlock(maxBlockSize);
    clear();
    printStage();
    do{
        end = (long int)clock();
        if(end-begin>=timeToMoveFow){
            begin = clock(); 
            if(!blockFoward(moveSteps)){
                mixBlocks();
                genBlock(maxBlockSize);
            }
            clear();
            printStage();
        }
        checkStageRows();
        if(sleepTime>0)
        sleep(sleepTime);
    }while(!ggChecker());
    clear();
    printf("GG EZ\n");
    flush();
    getch();
    
}
int getArrow(){
    int out=getch();
    if(out==27){
        out=getch();
        if(out==91)
            out=getch();
    }
    return out;
}

void* getInput(void *arg){
    char input;
    pthread_t id = pthread_self();

    while(zueiraneverends){
        flush();
        input=getArrow();
        if(input==VK_LEFT){//<
            blockMove((char*)"left",moveSteps);
            clear();
            printStage();
        }else if(input==VK_RIGHT){//>
            blockMove((char*)"right",moveSteps);
            clear();
            printStage();
        }else if(input==VK_UP){//^
            rotateBlock();
            clear();
            printStage();
        }else if(input==VK_DOWN){//v
            if(!blockFoward(moveSteps))
                genBlock(maxBlockSize);
                clear();
                printStage();
        }
    }
    return NULL;
}

void rotateBlock(){
    int buffer[stageSizeX][stageSizeY];
    int firstY=stageSizeY;
    int lastY=0;
    for(int i=0;i<stageSizeX;i++)
        for(int j=0;j<stageSizeY;j++)
            buffer[i][j]=stage[i][j][1];
     for(int i=0;i<stageSizeX;i++)
        for(int j=0;j<stageSizeY;j++){
            if(buffer[i][j]==1||j<firstY)
                firstY=j;
            if(buffer[i][j]==1||j>lastY)
                lastY=j;
        }
    clearBlocks();
    for(int j=lastY;j>=firstY;j--)
        for(int i=0;i<stageSizeX;i++)
            stage[i][j][1]=buffer[i][j];
    
}

void printfC(char *content,char *color){
    if(!strcmp(color,"red"))
        printf(KRED "%s",content,RESET);
    else if (!strcmp(color,"green"))
        printf(KGRN "%s",content,RESET);
    else if (!strcmp(color,"yellow"))
        printf(KYEL "%s",content,RESET);
    else if (!strcmp(color,"blue"))
        printf(KBLU "%s",content,RESET);
    else if (!strcmp(color,"pink"))
        printf(KMAG "%s",content,RESET);
    else if (!strcmp(color,"cyan"))
        printf(KCYN "%s",content,RESET);
    else if (!strcmp(color,"white"))
        printf(KWHT "%s",content,RESET);
    else 
        printf(KNRM "%s",content,RESET);    
}

int random(int max){
	srand(time(NULL));
	return rand() % max;
}
void clearBlocks(){
	for(int i=0; i<stageSizeX;i++)
		for(int j=0; j<stageSizeY;j++)
			stage[i][j][1]=0;
}
void genBlock(int max_size){//falha de segmentacao--------------------------------
	clearBlocks();
        /////////////////////////////////////////////////////
        strcpy(colorBlock,random_Color());

        stage[6][5][1]=1;
        stage[0][5][1]=1;
        stage[1][5][1]=1;
        stage[2][5][1]=1;
        stage[3][5][1]=1;
        stage[4][5][1]=1;
        stage[5][5][1]=1;
        stage[5][6][1]=1;

        
        /* ta com pau essa porra
        strcpy(colorBlock,random_Color());
	int startPosX=(stageSizeX-max_size)/2;
	int currentPosY=0;
	int currentPosX=0;
	do{
		currentPosX++;	
		if (currentPosX>=startPosX){
			currentPosY++;
			currentPosX=0;
		}
	}
	while(random(8)!=1);
	int form=1;
	do{
	if(form)
		if(random(2)==0){
			for(int i=0;i<random(max_size/5*2);i++){
				if(currentPosX<max_size)
				currentPosX++;
				stage[currentPosX][currentPosY][1]=1;
				}
			form=0;
		}else{
			for(int i=0;i<random(max_size/5*2);i++){
				if(currentPosY<max_size)
				currentPosY++;
				stage[currentPosX][currentPosY][1]=1;
				}
		}
	else{
		int rand=random(3);
		if(rand==0){
			for(int i=0;i<random(max_size/5*2);i++){
				if(currentPosX>0)
				currentPosX--;
				stage[currentPosX][currentPosY][1]=1;
				}
			form=1;
		}else if(rand==1){
			for(int i=0;i<random(max_size/5*2);i++){
				if(currentPosY<max_size)
				currentPosY++;
				stage[currentPosX][currentPosY][1]=1;
				}
		}else{
			for(int i=0;i<random(max_size/5*2);i++){
				if(currentPosX<max_size)
				currentPosX++;
				stage[currentPosX][currentPosY][1]=1;
				}
		}
	}
	}while(random(20)<17);*/
}

int ggChecker(){
	int gg=0;
	for(int i=0;i<stageSizeX;i++)
		if(stage[i][0][0]==1){
			gg=1;
			break;
		}
	return gg;
}

void checkStageRows(){
	int allOne,rows=0;
	for(int j=stageSizeY-1; j>=0;j--){
		allOne=1;
		for(int i=stageSizeX-1; i>=0;i--)
			if(stage[i][j][0]==0){
				allOne=0;
				break;
			}
		if(allOne){
			for(int k=j;k<stageSizeY-1;k++)
				for(int j=stageSizeY-1; j>=0;j--){
                                    //////////////////?????SEGMENTACAO NO K
					stage[k][j][0]=stage[k+1][j][0];
					stage[k+1][j][0]=0;
				}
			rows++;
		}
		
	}
	for(;rows>0;rows--)
		for(int i=stageSizeX-1; i>=0;i--)
		for(int j=stageSizeY-2; j>=0;j--){
				stage[i][j+1][0]=stage[i][j][0];
				stage[i][j][0]=0;
			}
		

}

void blockMove(char *dir,int steps){
    if(!strcmp(dir,"right")){
        for(int i=stageSizeX-1-steps; i>=0;i--)
		for(int j=stageSizeY-1; j>=0;j--){
			if(stage[i+steps][j][0]==0){
				stage[i+steps][j][1]=stage[i][j][1];
				stage[i][j][1]=0;
			}else{
				for(int k=j+1; k>stageSizeY;k++){
					stage[i][j][1]=stage[i+steps][j][1];
					stage[i+steps][j][1]=0;
				}
				if(steps==1){
					break;
				}else{
					steps--;
					j++;
				}
			}
		}
    }else{
        for(int i=0+steps;i<stageSizeX;i++)
		for(int j=stageSizeY-1; j>=0;j--){
			if(stage[i-steps][j][0]==0){
				stage[i-steps][j][1]=stage[i][j][1];
				stage[i][j][1]=0;
			}else{
				for(int k=j+1; k>stageSizeY;k++){
					stage[i][j][1]=stage[i-steps][j][1];
					stage[i-steps][j][1]=0;
				}
				if(steps==1){
					break;
				}else{
					steps--;
					j++;
				}
			}
		} 
    }
}

int lineClear(int y,int steps){
    int out=1;
    for(int i=stageSizeX-1; i>=0;i--)
        if(stage[i][y+steps+1][0]==1&&stage[i][y+steps][1]==1){
            out=0;
            break;
        }
    return out;
}

int blockFoward(int steps){
	int out=1;
	for(int i=stageSizeX-1; i>=0;i--){
		for(int j=stageSizeY-1-steps; j>=0;j--){
			if(lineClear(j+1,steps)&&j+steps<stageSizeY-1){
				stage[i][j+steps][1]=stage[i][j][1];
				stage[i][j][1]=0;
			}else if(stage[i][j][1]==1){                                
				if(steps==1){
                                    for(int t=j; t>=0;t--){
                                        stage[i][t+steps][1]=stage[i][t][1];
                                        stage[i][t][1]=0;
                                    }
					out=0;
					break;
				}else{
					steps--;
					j++;
				}
			}
		}
            
        }
	return out;

}

void mixBlocks(){
	for(int i=stageSizeX-1; i>=0;i--)
		for(int j=stageSizeY-1; j>=0;j--)
			if(stage[i][j][1]==1){
				stage[i][j][0]=stage[i][j][1];                            
                        }
	clearBlocks();
}

void clearStage(){
        strcpy(colorStage,random_Color());
	for(int i=0; i<stageSizeX;i++)
		for(int j=0; j<stageSizeY;j++)
                    if(j==stageSizeY-1)
			stage[i][j][0]=0;
                    else
                        stage[i][j][0]=0;
}               

void flush(){
	if(OS)
		fflush(stdin);
	else
		__fpurge(stdin);
}

void clear(){
	if(OS)
		system("cls");
	else
		system("clear");
}

char* random_Color(){
    char *out=(char*)malloc(10);
    switch(random(7)){
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

void printStage(){
	clear();
        char buffer[10];
        sprintf(buffer,"%c",fullBlock);
	
	for(int j=0; j<stageSizeY;j++){
                for(int i=0; i<stageSizeX;i++)
                    for(int k=0;k<scale;k++)
			if(stage[i][j][0]==0&&stage[i][j][1]==0)
				printfC((char*)" ",(char*)"");
			else
                            for(int t=0;t<scale;t++)
                                if(stage[i][j][0]==1)
                                    printfC(buffer,colorStage);
                                else
                                    printfC(buffer,colorBlock);
		printf("\n");
	}
	
}

int getch(void) {
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
