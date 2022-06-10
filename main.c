// This software is a modified version of the project https://github.com/Gregwar/ASCII-Tetris

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"

// These are tricks because sprites need to be 8 pixels tall and cannot be drawn partially off screen
static const byte BLOCK_LOW[] = { 0x07, 0x07, 0x07 };
static const byte ERASE_LOW[] = { 0xF8, 0xF8, 0xF8 };

static const byte BLOCK[] = { 0xE0, 0xE0, 0xE0 };
static const byte ERASE[] = { 0x1F, 0x1F, 0x1F };

#define GS 149
#define A 140
#define B 125
#define C 118
#define D 104
#define E 93
#define F 88
#define G 78
#define GS2 74
#define A2 69            

static const byte MUSIC[] = { E, 8, B, 4, C, 4, D, 8, C, 4, B, 4,     
                              A, 7, 0, 1, A, 4, C, 4, E, 8, D, 4, C, 4,
                              B, 12, C, 4, D, 8, E, 8,
                              C, 8, A, 7, A, 3, 0, 1, A, 3, 0, 1, B, 4, C, 4,
                              D, 12, F, 4, A2, 8, G, 4, F, 4,
                              E, 12, C, 4, E, 8, D, 4, C, 4,
                              B, 7, 0, 1, B, 4, C, 4, D, 8, E, 8,
                              C, 8, A, 7, 0, 1, A, 8, 0, 8,
                              E, 16, C, 16,
                              D, 16, B, 16,
                              C, 16, A, 16,
                              GS, 16, B, 8, 0, 8,
                              E, 16, C, 16,
                              D, 16, B, 16,
                              C, 8, E, 8, A2, 16, 
                              GS2, 16, 0, 16,
                              0, 0 };

static byte _buffer[10];

// TODO
// be able to swap lines around as a whole

struct tetris;


void tetris_init(struct tetris *t,int w,int h);
void tetris_clean(struct tetris *t);
void tetris_run(byte width, byte height);
void tetris_new_block(struct tetris *t);
void tetris_new_block(struct tetris *t);
void tetris_print_block(struct tetris *t);
void tetris_rotate(struct tetris *t);
void tetris_gravity(struct tetris *t);
void tetris_fall(struct tetris *t, int l);
void tetris_check_lines(struct tetris *t);
int tetris_level(struct tetris *t);
void tetris_draw_score(struct tetris *t);
void tetris_draw_next(struct tetris *t, byte c);

struct tetris_level {
    int score;
    byte mod;
};

struct tetris_block {
    char data[4][4];
    byte w;
    byte h;
};

struct tetris {
    char **game;
    byte w;
    byte h;
    byte level;
    byte gameover;
    int score;
    int lines;
    struct tetris_block current;
    byte next;
    byte x;
    byte y;
};

struct tetris_block blocks[] =
{
    {{"\1\1", 
         "\1\1"},
    2, 2
    },
    {{"\0\1\0",
         "\1\1\1"},
    3, 2
    },
    {{"\1\1\1\1"},
        4, 1},
    {{"\1\1",
         "\1\0",
         "\1\0"},
    2, 3},
    {{"\1\1",
         "\0\1",
         "\0\1"},
    2, 3},
    {{"\1\1\0",
         "\0\1\1"},
    3, 2}
};

// Todo: Figure out points per level again
struct tetris_level const levels[]=
{
    {0,
        16},
    {150,
        12},
    {800,
        8},
    {2000,
        6},
    {4000,
        4},
    {7500,
        2},
    {10000,
        1}
};

#define TETRIS_PIECES (sizeof(blocks)/sizeof(struct tetris_block))
#define TETRIS_LEVELS (sizeof(levels)/sizeof(struct tetris_level))

byte
get_position(struct tetris* t, byte x, byte y) {
    return t->game[x][y];
}

void
set_position(struct tetris* t, byte x, byte y, byte c) {
    byte sx;
    byte sy;
    
    sx = x * 3;
    sy = y * 3;
    t->game[x][y] = c;
    
    if (sy < 5) {
        if (c != 0)
            DRAW_SPRITE((byte*)BLOCK_LOW, sx + 50, sy + 1, 3, 8, 0, DRAW_OR);
        else
            DRAW_SPRITE((byte*)ERASE_LOW, sx + 50, sy + 1, 3, 8, 0, DRAW_AND);
    }
    else
    {
        if (c != 0)
            DRAW_SPRITE((byte*)BLOCK, sx + 50, sy - 4, 3, 8, 0, DRAW_OR);
        else
            DRAW_SPRITE((byte*)ERASE, sx + 50, sy - 4, 3, 8, 0, DRAW_AND);
    }
}

void
tetris_init(struct tetris *t,int w,int h) {
    byte x, y;
    t->level = 1;
    t->score = 0;
    t->lines = 0;
    t->gameover = 0;
    t->w = w;
    t->h = h;
    t->next = rand()%TETRIS_PIECES;
    t->game = malloc(sizeof(char *)*w);
    
    for (x=0; x<w; x++) {
        t->game[x] = malloc(sizeof(char)*h);
        for (y=0; y<h; y++)
            t->game[x][y] = '\0';
    }
    
    tetris_draw_next(t, 1);
}

void
tetris_clean(struct tetris *t) {
    
    byte x;
    for (x=0; x<t->w; x++) {
        free(t->game[x]);
    }
    free(t->game);
}

int
tetris_hittest(struct tetris *t) {
    byte x,y,X,Y;
    struct tetris_block b; 
    b = t->current;
    for (x=0; x<b.w; x++)
        for (y=0; y<b.h; y++) {
            X=t->x+x;
            Y=t->y+y;
            if (X > 128 || X>=t->w)
                return 1;
            if (b.data[y][x]!=0) {
                if ((Y>=t->h) || 
                        (X <= 128 && X<t->w && Y <= 128 && get_position(t, X, Y) != 0)) {
                    return 1;
                }
            }
        }
    return 0;
}

void tetris_draw_next(struct tetris *t, byte c) {
    byte x,y, sx, sy;
    struct tetris_block b;
    b=blocks[t->next];
    for (x=0; x<b.w; x++)
        for (y=0; y<b.h; y++) {
            if (b.data[y][x] != 0) {
                sx = (12 + x) * 3;
                sy = (12 + y) * 3;
                if (c != 0)
                    DRAW_SPRITE((byte*)BLOCK, sx + 50, sy - 4, 3, 8, 0, DRAW_OR);
                else
                    DRAW_SPRITE((byte*)ERASE, sx + 50, sy - 4, 3, 8, 0, DRAW_AND);
            }
        }
}

void
tetris_new_block(struct tetris *t) {
    t->current=blocks[t->next];
    tetris_draw_next(t, 0);
    t->next=rand()%TETRIS_PIECES;
    tetris_draw_next(t, 1);
    t->x=(t->w/2) - (t->current.w/2);
    t->y=0;
    if (tetris_hittest(t)) {
        t->gameover=1;
    }
}

void
tetris_print_block(struct tetris *t) {
    byte x,y;
    struct tetris_block b;
    b=t->current;
    for (x=0; x<b.w; x++)
        for (y=0; y<b.h; y++) {
            if (b.data[y][x]!=0)
                set_position(t, t->x+x, t->y+y, b.data[y][x]);
        }
}

void
tetris_erase_block(struct tetris *t) {
    byte x,y;
    struct tetris_block b;
    b=t->current;
    for (x=0; x<b.w; x++)
        for (y=0; y<b.h; y++) {
            if (b.data[y][x]!=0)
                set_position(t, t->x+x, t->y+y, 0);
        }
}

void
tetris_rotate(struct tetris *t) {
    struct tetris_block b;
    struct tetris_block s;
    byte x,y;
    b=t->current;
    s=b;
    b.w=s.h;
    b.h=s.w;
    for (x=0; x<s.w; x++)
        for (y=0; y<s.h; y++) {
            b.data[x][y]=s.data[s.h-y-1][x];
        }
    x=t->x;
    y=t->y;
    
    if (b.w > s.w) {
        t->x-=(b.w-s.w)/2;
    } else {
        t->x+=(s.w-b.w)/2;
    }
    
    if (b.h > s.h) {
        t->y-=(b.h-s.h)/2;
    } else {
        t->y+=(s.h-b.h)/2;
    }
    
    t->current=b;
    if (tetris_hittest(t)) {
        t->current=s;
        t->x=x;
        t->y=y;
    }
}

void
tetris_gravity(struct tetris *t) {
    t->y++;
    if (tetris_hittest(t)) {
        t->y--;
        tetris_print_block(t);
        DISPLAY();
        tetris_check_lines(t);
        tetris_new_block(t);
    }
}

void
tetris_fall(struct tetris *t, int l) {
    int x,y;
    for (y=l; y>0; y--) {
        for (x=0; x<t->w; x++)
            set_position(t, x, y, get_position(t, x, y-1));
    }
    for (x=0; x<t->w; x++)
        set_position(t, x, 0, 0);
}

void
tetris_check_lines(struct tetris *t) {
    byte x,y,l;
    int p=10;
    // Optimization here: only check the lines that are affected by current block
    for (y=t->h-1; y <= 128; y--) {
        l=1;
        for (x=0; x<t->w && l; x++) {
            if (get_position(t, x, y) == 0) {
                l=0;
            }
        }
        if (l) {
            t->score += p;
            t->lines = t->lines + 1;
            
            tetris_draw_score(t);
            
            p*=2;
            tetris_fall(t, y);
            y++;
        }
    }
}

int
tetris_level(struct tetris *t) {
    int i;
    for (i=0; i<TETRIS_LEVELS; i++) {
        if (t->score>=levels[i].score) {
            t->level = i+1;
        } else break;
    }
    return levels[t->level-1].mod;
}

void 
tetris_draw_score(struct tetris *t) {
    SET_CURSOR(1, 0);
    DRAW_STRING(itoa(t->score, (char*)_buffer, 10));
    
    SET_CURSOR(4, 0);
    DRAW_STRING(itoa(t->lines, (char*)_buffer, 10));
    
    SET_CURSOR(1, 12);
    DRAW_STRING(itoa(t->level, (char*)_buffer, 10));
}

void center(byte row, char *text) {
    byte len = strlen(text);
    byte x = ((17 - len) / 2) + 1;
    SET_CURSOR(row, x);
    DRAW_STRING(text);
}

void waitForADown() {
    byte keys = 0;
    unsigned int counter = 0;
    for (;;) {
        keys = READ_BUTTONS();
        if ((keys & 16) == 0)
            break;
        
        WAIT();
        
        ++counter;
    }
    
    srand(counter); // Re-initialize random number generator with length of time before user hits button (pseudo-random)
}

void waitForAUp() {
    byte keys = 0;
    for (;;) {
        keys = READ_BUTTONS();
        if ((keys & 16) == 16)
            break;
        
        WAIT();
    }
}

void prompt(char *text) {
    CLEAR();
    SET_CURSOR(2, 2);
    DRAW_STRING("\311\315\315\315\315\315\315\315\315\315\315\315\315\315\273");
    center(3, text);
    SET_CURSOR(3, 2);
    DRAW_STRING("\272");
    SET_CURSOR(3, 16);
    DRAW_STRING("\272");
    SET_CURSOR(4, 2);
    DRAW_STRING("\310\315\315\315\315\315\315\315\315\315\315\315\315\315\274");
    center(6, "Press (A)");
    DISPLAY();
    waitForADown();
    waitForAUp();
}

void
tetris_run(byte w, byte h) {
    byte keys;
    struct tetris t;
    int count=0;
    int buttonCount = 0;
    byte canMoveLeftRight = 0;
    byte mod = 16;

    api_init();
    
    for (;;) {
        prompt("Tetris");
        
        tetris_init(&t, w, h);
        
        LOAD_MUSIC((unsigned char*)MUSIC);
        
        CLEAR();
        
        DRAW_LINE(49, 0, 49, 63, 1);
        DRAW_LINE(80, 0, 80, 63, 1);
    
        tetris_new_block(&t);
        
        SET_CURSOR(0, 0);
        DRAW_STRING("Score");
        SET_CURSOR(3, 0);
        DRAW_STRING("Lines");
        SET_CURSOR(0, 12);
        DRAW_STRING("Level");
        SET_CURSOR(3, 12);
        DRAW_STRING("Next");
        
        tetris_draw_score(&t);
        
        while (!t.gameover) {
            DISPLAY();
            
            count++;
    
            tetris_erase_block(&t);
            if (count%mod == 0) {
                tetris_gravity(&t); // tetris_check_lines now inside of tetris_gravity
            }
            
            canMoveLeftRight = (buttonCount == 1) || (buttonCount > 4 /*&& (buttonCount % 2) == 0*/);
            keys = READ_BUTTONS();
            ++buttonCount;
            if ((keys & 4) == 0) {
                if (/*buttonCount == 1 || buttonCount > 4*/ canMoveLeftRight) {
                    t.x--;
                    if (tetris_hittest(&t))
                        t.x++;
                }
            } else if ((keys & 8) == 0) {
                if (/*buttonCount == 1 || buttonCount > 4*/ canMoveLeftRight) {
                    t.x++;
                    if (tetris_hittest(&t))
                        t.x--;
                }
            } else if ((keys & 2) == 0) {
                tetris_gravity(&t);
            } else if ((keys & 16) == 0) {
                if (buttonCount == 1) {
                    tetris_rotate(&t);
                }
            } else {
                buttonCount = 0;
            }
            
            tetris_print_block(&t);
            
            mod = tetris_level(&t);
            
            WAIT();
        }
        
        prompt("Game Over");
        prompt(itoa(t.score, (char*)_buffer, 10));
    
        tetris_clean(&t);
    }
}

int main() {
    tetris_run(10, 21);
    return 0;
}

/*
Copyright (c) <2010-2015> Grégoire Passault

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/