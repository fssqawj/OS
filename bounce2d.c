/* bounce2d.c 1.0
* bounce a character (default is 'o') around the screen
* defined by some parameters
*
* user input: s slow down x component, S: slow y component
* f speed up x component, F: speed y component
* Q quit a: user wall left d: user wall right
*
* blocks on read, but timer tick sends SIGALRM caught by ball_move
* build: cc bounce2d.c -lcurses -o bounce2d
*/

#include <curses.h>
#include <signal.h>
#include <sys/time.h>
#include "bounce.h"

struct ppball the_ball ;
int cnt = 3;
bool game_over = 0;
/** the main loop **/
void set_up();
void wrap_up();
void move_user_wall(int);

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
    while(1){
         int c, dir = 0;

         set_up();
        
         while (!game_over){
                c = getchar();
                if ( c == 'f' ) the_ball.x_ttm--;
                else if ( c == 's' ) the_ball.x_ttm++;
                else if ( c == 'F' ) the_ball.y_ttm--;
                else if ( c == 'S' ) the_ball.y_ttm++;
                else if ( c == 'a' ) {
                	dir = -1;
                	move_user_wall(dir);
                }
                else if ( c == 'd' ) {
                	dir = 1;
                	move_user_wall(dir);
                }
                else if ( c == 'j' ) {
                	if ( --user_wall_len <= 1 ){
                		user_wall_len = 1;
                		move_user_wall(0);
                	}
                }
                else if ( c == 'k' ) {
                	if ( ++user_wall_len >= 15 ){
                		user_wall_len = 15;
                		move_user_wall(0);
                	}
                }
                else if ( c == 'Q' ){
                			set_ticker( 0 );
                            endwin();
                            return 0;
                }
         }
         while(game_over){
                        c = getchar();
                        if(c == 'q'){
                                set_ticker( 0 );
                                endwin();
                                return 0;
                        }
                        else if(c == 'p'){
                                game_over = 0; 
                                //mvaddstr(15, 15, "               ");
                                //mvaddstr(16, 15, "               ");
                                //mvaddstr(17, 15, "               ");
                                refresh();
                        }
         }

   // wrap_up();
    }
}

void set_up()
/*
* init structure and other stuff
*/
{
    void ball_move(int);
	int i;
    the_ball.y_pos = Y_INIT;
    the_ball.x_pos = X_INIT;
    the_ball.y_ttg = the_ball.y_ttm = Y_TTM ;
    the_ball.x_ttg = the_ball.x_ttm = X_TTM ;
    the_ball.y_dir = 1 ;
    the_ball.x_dir = 1 ;
    the_ball.symbol = DFL_SYMBOL ;

    initscr();
    noecho();
    cbreak();
    echo();

    signal( SIGINT , SIG_IGN );
    for(i = 0;i < 50;i ++){
    	mvhline(i, 0, ' ', 40);
    }
    /* draw wall */
    mvhline(5, 5, '_', 40);
    mvvline(6, 5, '|', 15);
    mvvline(6, 45, '|', 15);

    mvprintw( 4, 5, "score:%d left:s right:d -speed:s +speed:f +wall_len:j -wall_len:k quit:Q", 0); /* infomation */
    mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol ); /* init ball */
    mvhline(20, user_wall_pos, '_', user_wall_len); /* init user_wall */
    refresh();

    signal( SIGALRM, ball_move );
    signal( SIGQUIT,SIG_IGN);
    set_ticker( 1000 / TICKS_PER_SEC ); /* send millisecs per tick */
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
    refresh();
    //endwin();
    //exit(0); /* put back to normal */
}

void ball_move(int signum)
{
    int y_cur, x_cur, moved;

    signal( SIGALRM , SIG_IGN ); /* dont get caught now */
    y_cur = the_ball.y_pos ; /* old spot */
    x_cur = the_ball.x_pos ;
    moved = 0 ;

    if ( the_ball.y_ttm > 0 && the_ball.y_ttg-- == 1 ){
        the_ball.y_pos += the_ball.y_dir ; /* move */
        the_ball.y_ttg = the_ball.y_ttm ; /* reset*/
        moved = 1;
    }

    if ( the_ball.x_ttm > 0 && the_ball.x_ttg-- == 1 ){
        the_ball.x_pos += the_ball.x_dir ; /* move */
        the_ball.x_ttg = the_ball.x_ttm ; /* reset*/
        moved = 1;
    }

    if ( moved ){
        mvaddch( y_cur, x_cur, BLANK );
        mvaddch( y_cur, x_cur, BLANK );
        mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol );
        bounce_or_lose( &the_ball );
        move(LINES-1,COLS-1);
        refresh();
    }

    signal( SIGALRM, ball_move); /* for unreliable systems */

}

int bounce_or_lose(struct ppball *bp)
{
    int return_val = 0 ;
    int user_wall_left = user_wall_pos,
        user_wall_right = user_wall_pos + user_wall_len - 1;

    if ( bp->y_pos == TOP_ROW ){
        bp->y_dir = 1 ;
        return_val = 1 ;
    } else if ( (bp->y_pos == BOT_ROW) && (bp->x_pos >= user_wall_left && bp->x_pos <= user_wall_right) ){
        bp->y_dir = -1;
        mvprintw( 4, 5, "score:%d", score++);
        return_val = 1;
    } else if (bp->y_pos > BOT_ROW)
        wrap_up();
        
    if ( bp->x_pos == LEFT_EDGE ){
        bp->x_dir = 1 ;
        return_val = 1 ;
    } else if ( bp->x_pos == RIGHT_EDGE ){
        bp->x_dir = -1;
        return_val = 1;
    }

    return return_val;
}

void move_user_wall(int dir)
{
    int user_wall_left = user_wall_pos,
        user_wall_right = user_wall_pos + user_wall_len - 1;
    int wall_left = 5,
        wall_right = 45;
        
    user_wall_pos += dir;
    
    if(user_wall_pos <= wall_left)
        user_wall_pos = wall_left;
    if( user_wall_pos >= wall_right - user_wall_len + 1 )
        user_wall_pos = wall_right - user_wall_len;
	mvhline( 20, 0, BLANK, 50);
    mvhline( 20, user_wall_pos, USER_WALL, user_wall_len + 1 );
    
    /*if (dir == 1 && user_wall_right < wall_right -1 )
        mvhline( 20, user_wall_left, BLANK, 1 );
    if (dir == -1 && user_wall_left > wall_left )
        mvhline( 20, user_wall_right+1, BLANK, 1 );
    */
    move(LINES - 1, COLS - 1);
    refresh();
}
