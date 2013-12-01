#ifndef BOUNCH_H
#define BOUNCH_H

/* some settings for the game    */

#define    BLANK        ' '
#define    WALL        '.'
#define    USER_WALL    '_'
#define    DFL_SYMBOL    'o'
#define    TOP_ROW        6
#define    BOT_ROW     19
#define    LEFT_EDGE    6
#define    RIGHT_EDGE    44
#define    X_INIT        10        /* starting col        */
#define    Y_INIT        10        /* starting row        */
#define    TICKS_PER_SEC    25        /* affects speed    */

#define    X_TTM        5
#define    Y_TTM        8

int score = 0;
int user_wall_pos = 5;
int user_wall_len = 9;

/** the ping pong ball **/

struct ppball {
        int    y_pos, x_pos,
                y_ttm, x_ttm,
                y_ttg, x_ttg,
                y_dir, x_dir;
        char    symbol ;

    } ;

#endif
