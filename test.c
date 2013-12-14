/* shot plane
* version 1.0 OS group work
* team member : xtt & lyj & fzj & awj
*
* user input: a -move the plane left
* 			  s -move the plane down
* 			  d -move the plane right	
*			  w -move the plane up	
* 			  g -pause..
*			  p -when game over ,play it again
* 			  c -claer screen
* build: cc test.c -lcurses -o test
*/

#include <curses.h>
#include <signal.h>
#include <sys/time.h>
#include "bounce.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//敌机的结构体，tem为当前剩余等待时间，wit为初始等待时间
struct ani_plane{
	int tem,wit,atc;
};

//我方飞机，记个位置就好，别无他求
struct my{
	int x,y;
};

bool game_over = 0;                //判断当前游戏状态是否结束
bool isp = false;				   //判断当前游戏状态是否暂停

void set_up();					   //初始化函数
void wrap_up();					   //游戏结束，后续处理函数
void mov_my_plane();			   //移动我方飞机位置
void add_boom();				   //发射子弹
void del_plane(int x,int y);
void clear_screen();
void add_my_plane();

struct ani_plane ani[105][105];	   //记录屏幕上是否有飞机，以及飞机移动的等待时间

bool isb[105][105];				   //记录屏幕上是否有子弹

int nex_plane = 0;				   //通过取模控制下一架敌机出现


struct my my_plane;				   //我方飞机的位置信息

int score = 0;

int clear_chance = 0;

set_ticker( n_msecs )
{
        struct itimerval new_timeset;
        long n_sec, n_usecs;

        n_sec = n_msecs / 1000 ;
        n_usecs = ( n_msecs % 1000 ) * 1000L ;

        new_timeset.it_interval.tv_sec = n_sec; /* set reload */
        new_timeset.it_interval.tv_usec = n_usecs; /* new ticker value */
        new_timeset.it_value.tv_sec = n_sec ; /* store this */
        new_timeset.it_value.tv_usec = n_usecs ; /* and this */

        return setitimer(ITIMER_REAL, &new_timeset, NULL);
}


int main()
{
	//set_up();
    while(1){
    
    	  char c;
         //int c, dir = 0;

         set_up();
        if(!(start_color() == OK))return ;
		init_pair(1,COLOR_GREEN,COLOR_BLACK);
		init_pair(2,COLOR_BLUE,COLOR_BLACK);
		init_pair(3,COLOR_YELLOW,COLOR_BLACK);
		//init_pair(3,COLOR_WHITE,COLOR_BLUE);
         while (!game_over){
                c = getchar();
				if(c == 'a'){
					if(!isp && !game_over)mov_my_plane(0,-2);
				}
				if(c == 'd'){
					if(!isp && !game_over)mov_my_plane(0,2);
				}
				if(c == 'w'){
					if(!isp && !game_over)mov_my_plane(-2,0);
				}
				if(c == 's'){
					if(!isp && !game_over)mov_my_plane(2,0);
				}
					
				if(c == 'j'){
					if(!isp && !game_over)add_boom();
				}
				if(c == 'c'){
					if(!isp){
						if(clear_chance > 0){
							clear_screen();
							clear_chance --;
						}
					}
				}
				
				//此步是必要的，之前的getchar()函数会影响程序的执行，这里实际上已经game_over
                if ( c == 'Q' ){
                            set_ticker( 0 );
                            endwin();
                            return 0;
                }
                if(c == 'p'){
                 	game_over = 0;
		             //mvaddstr(15, 15, " ");
		             //mvaddstr(16, 15, " ");
		             //mvaddstr(17, 15, " ");
		             //refresh();
                 	break;
                }
                if(c == 'g'){
                         if(!isp){ //如果当前为非暂停状态，转到暂停状态
                                 set_ticker(0);
                                 mvaddstr(13, 20, "pause...");
                                 refresh();
                                 isp = true;
                         }
                         else { //如果当前为暂停状态，转到非暂停状态
                                 set_ticker(20);
                                 mvaddstr(13, 20, "        ");
                                 refresh();
                                 isp = false;
                         }
                 }        
       }
       while(game_over){
             c = getchar();
             if(c == 'Q'){
                    set_ticker( 0 );
                    endwin();
                    return 0;
                }
             else if(c == 'p'){
                    game_over = 0;
                    //mvaddstr(15, 15, " ");
                    //mvaddstr(16, 15, " ");
                    //mvaddstr(17, 15, " ");
                    //refresh();
                }
         }

   // wrap_up();*/
    }
}

void clear_screen(){
	int i,j;
	set_ticker(0);
	for(i = 0;i < 105;i ++){
		for(j = 0;j < 105;j ++){
			ani[i][j].tem = ani[i][j].wit = 0;
		}
	}
	for(i = LINES;i >= 0;i --){
		if(i < LINES){
			for(j = 1;j < 45;j ++){
				mvaddch(i + 1,j,' ');
			}
		}
		attron(COLOR_PAIR(3));
		for(j = 1;j < 45;j ++){
			mvaddch(i,j,'*');
		}
		attroff(COLOR_PAIR(3));
		refresh();
		usleep(20000);
	}
	for(j = 1;j < 45;j ++){
				mvaddch(0,j,' ');
			}
	/*for(i = 0;i < LINES - 1;i ++){
		mvaddch(i,0,'|');
		mvaddch(i,45,'|');
	}
	*/
	add_my_plane();
	mvprintw(20,50,"score:%d", score);
	mvprintw(19,50,"clear chance:%d",clear_chance);
	refresh();
	set_ticker(20);
}
	
/********************************************************/
//判断飞机移动过程中是否撞上了敌机
void judge_lose(){
	//wrap_up();
	int x,y;
	x = my_plane.x;
	y = my_plane.y;
	
	if(x > 1 && y > 1){
		if(ani[x - 1][y].tem > 0 || ani[x][y - 1].tem > 0|| ani[x][y - 2].tem > 0|| ani[x][y + 1].tem > 0|| ani[x][y + 2].tem > 0 || ani[x - 1][y - 2].tem > 0 || ani[x - 1][y + 2].tem > 0){
			if(ani[x - 1][y].tem > 0 && ani[x - 1][y].atc == -1){
				clear_chance ++;
				del_plane(x - 1,y);
				ani[x - 1][y].tem = 0;
			}
			else if(ani[x][y - 1].tem > 0 && ani[x][y - 1].atc == -1){
				clear_chance ++;
				del_plane(x,y - 1);
				ani[x][y - 1].tem = 0;
			}
			else if(ani[x][y + 1].tem > 0 && ani[x][y + 1].atc == -1){
				clear_chance ++;
				del_plane(x,y + 1);
				ani[x][y + 1].tem = 0;
			}
			else if(ani[x - 1][y - 2].tem > 0 && ani[x - 1][y - 2].atc == -1){
				clear_chance ++;
				del_plane(x - 1,y - 2);
				ani[x - 1][y - 2].tem = 0;
			}
			else if(ani[x - 1][y + 2].tem > 0 && ani[x - 1][y + 2].atc == -1){
				clear_chance ++;
				del_plane(x - 1,y + 2);
				ani[x - 1][y + 2].tem = 0;
			}
			else if(ani[x][y + 2].tem > 0 && ani[x][y + 2].atc == -1){
				clear_chance ++;
				del_plane(x,y + 2);
				ani[x][y + 2].tem = 0;
			}
			else if(ani[x][y - 2].tem > 0 && ani[x][y - 2].atc == -1){
				clear_chance ++;
				del_plane(x,y - 2);
				ani[x][y - 2].tem = 0;
			}
			else wrap_up();
		}
	}
}

//判断子弹是否打中了敌机
bool judge_exp(int i,int j){
	if(isb[i][j] || isb[i + 1][j + 1] || isb[i][j + 2])return true;
	return false;
}
/******************************************************************/


//在x,y位置增加一架敌机
void add_plane(int x,int y,int f){
	if(f == 1){
		mvaddstr(x,y,"0 0");
		mvaddstr(x + 1,y," o ");
	}
	else if(f == 2){
		mvaddstr(x,y,"8 8");
		mvaddstr(x + 1,y," o ");
	}
	else {
		attron(COLOR_PAIR(3)|A_BOLD|A_BLINK);
		mvaddstr(x,y,"* *");
		mvaddstr(x + 1,y," _ ");
		attroff(COLOR_PAIR(3)|A_BOLD|A_BLINK);
	}
}

//删除掉x,y位置的敌机
void del_plane(int x,int y){
	mvaddstr(x,y,"   ");
	mvaddstr(x + 1,y,"   ");
}

bool candel(int i,int j){
	return ani[i][j].atc == 0;
}
//移动我方飞机
void mov_plane(){
	int i,j,y,wit,hapnes;
	signal( SIGALRM , SIG_IGN );
	
	//每隔20个alarm信号，增加一架敌机
	if(nex_plane % 40 == 0){
		y = rand() % 40 + 1;
		wit = rand() % 30 + 2;
		hapnes = rand() % 4;
		ani[0][y].tem = ani[0][y].wit = wit;
		if(wit > 30)ani[0][y].atc = 2;
		else ani[0][y].atc = 1;
		if(hapnes == 0){
			ani[0][y].atc = -1;
		}
		attron(COLOR_PAIR(2));
		add_plane(0,y,ani[0][y].atc);
		attroff(COLOR_PAIR(2));
	}
	
	//从上往下扫描子弹并移动
	for(i = 0;i < LINES - 1;i ++){
		for(j = 0;j < 40;j ++){
			if(isb[i][j]){
				mvaddch(i,j,' ');
				attron(COLOR_PAIR(3));
				mvaddch(i - 1,j,'*');
				attron(COLOR_PAIR(3));
				isb[i][j] = false;
				if(i >=1)isb[i - 1][j] = true;
			}
		}
	}
	
	//从下往上扫描敌机并移动或删除(碰到子弹)
	for(i = LINES - 1;i >= 0;i --){
		for(j = 0;j < 40;j ++){
			if(ani[i][j].tem > 0){
				if(judge_exp(i,j)){
					if(ani[i][j].atc > 0)ani[i][j].atc --;
					if(candel(i,j)){
						del_plane(i,j);
						ani[i][j].tem = 0;
						score ++;
						mvprintw(20,50,"score:%d", score);
						continue;
					}
				}
				ani[i][j].tem --;
				if(ani[i][j].tem == 0){
					del_plane(i,j);
					attron(COLOR_PAIR(2));
					add_plane(i + 1,j,ani[i][j].atc);
					attroff(COLOR_PAIR(2)); 
					ani[i + 1][j].tem = ani[i + 1][j].wit = ani[i][j].wit;
					ani[i + 1][j].atc = ani[i][j].atc;
					ani[i][j].atc = 0;
					ani[i][j].wit = 0;
				}
			}
		}
	}
	attron(COLOR_PAIR(1)); 
	mov_my_plane(0,0);
	attroff(COLOR_PAIR(1)); 
	//判断我方飞机是否装上敌机
	judge_lose();
	mvprintw(19,50,"clear chance:%d", clear_chance);
	nex_plane ++;
	move(LINES - 1,COLS - 1);
	refresh();
	signal( SIGALRM, mov_plane);
}
/*****************************************************/
int judge_x(int x){
	if(x < 1)return 1;
	if(x > LINES - 2)return LINES - 2;
	return x;
}
int judge_y(int x){
	if(x < 1)return 1;
	if(x > 40)return 40;
	return x;
}
//删除我方飞机
void del_my_plane(){
	int x,y;
	x = my_plane.x;
	y = my_plane.y;
	mvaddstr(x,y,"   ");
	mvaddstr(x + 1,y,"   ");
}

//在新的位置重画飞机
void add_my_plane(){
	int x,y;
	x = my_plane.x;
	y = my_plane.y;
	mvaddstr(x,y," o ");
	mvaddstr(x + 1,y,"0 0");
}


void mov_my_plane(int x,int y){
	del_my_plane();
	my_plane.x += x;
	my_plane.y += y;
	my_plane.x = judge_x(my_plane.x);
	my_plane.y = judge_y(my_plane.y);
	attron(COLOR_PAIR(1));
	add_my_plane();
	attroff(COLOR_PAIR(1));
	
	move(LINES - 1,COLS - 1);
	refresh();
}
/********************************************************/


void add_boom(){
	int x,y;
	x = my_plane.x;
	y = my_plane.y;
	if(x > 1)isb[x - 1][y + 1] = true;
}



void set_up()
/*
* init structure and other stuff
*/
{
	int i,j;
	
	//初始化函数
    initscr();
    noecho();
    cbreak();
    echo();
    
    //重画整个屏幕
	for(i = 0;i < 105;i ++){
		for(j = 0;j < 105;j ++){
			ani[i][j].tem = ani[i][j].wit = 0;
		}
	}
	for(i = 0;i < LINES;i ++){
		for(j = 0;j < COLS - 1;j ++){
			mvaddch(i,j,' ');
		}
	}
	for(i = 0;i < LINES - 1;i ++){
		mvaddch(i,0,'|');
		mvaddch(i,45,'|');
	}	
	my_plane.x = LINES - 2;
	my_plane.y = 20;
	add_my_plane();
	
	mvprintw(20,50,"score:%d", score);
	mvprintw(19,50,"clear chance:%d",clear_chance);
	memset(isb,false,sizeof(isb));
	
	
    signal( SIGALRM, mov_plane );
    //signal( SIGQUIT,SIG_IGN);
    set_ticker( 20 ); 
}

void wrap_up()
{
    set_ticker( 0 );
    mvaddstr(15, 15, "GAME OVER!.....");
    mvaddstr(16, 15, "P:Play again!");
    mvaddstr(17, 15, "Q:quit");
    refresh();
    //while(1){
    // if ( getchar() == 'Q') break;
        //else solve();
    //}
    game_over = 1;
    //refresh();
    //endwin();
    //exit(0); /* put back to normal */
}

