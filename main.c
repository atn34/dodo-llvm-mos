#include "api.h"

#define SCREEN_WIDTH_BEGIN 0
#define SCREEN_HEIGHT_BEGIN 0
#define SCREEN_WIDTH_END 128
#define SCREEN_HEIGHT_END 64

#define BLOCK_WIDTH 8
#define BLOCK_HEIGHT 8

#define TILE_ROWS 8
#define TILE_COLUMNS 16

struct GameState {
  int block_x;
  int block_y;
  int block_velocity_x;
  // Fixed point. Right shift by 2 to convert to pixels.
  int block_velocity_y;
  int block_want_jump;
  int block_jump_frames_left;
  int block_moves_available;
  byte last_buttons;
  byte tiles[TILE_ROWS][TILE_COLUMNS];
  int coins_remaining;
  int lastblock_x;
  int last_block_x;
  int last_block_y;
};

static char *const tile_sprites[] = {
    "\x00\x00\x00\x00\x00\x00\x00\x00",
    "\xff\x00\xff\x00\xff\x00\xff\x00",
    "\x81\x42\x24\x18\x18\x24\x42\x81",
    "\x00\x3c\x42\x42\x42\x42\x3c\x00",
};

static char *const block_sprites[] = {
    "\xff\x81\x81\x81\x81\x81\x81\xff",
    "\xff\xff\xc3\xc3\xc3\xc3\xff\xff",
    "\xff\xff\xff\xe7\xe7\xff\xff\xff",
    "\xff\xff\xff\xff\xff\xff\xff\xff",
};

static void processInput(struct GameState *current, byte buttons) {
  if ((buttons & 16) == 0 && (current->last_buttons & 16) != 0) {
    current->block_want_jump = 1;
  }
  if (current->block_want_jump && current->block_moves_available > 0) {
    // Begin jump
    current->block_want_jump = 0;
    current->block_velocity_y = -6;
    current->block_jump_frames_left = 8;
    current->block_moves_available -= 1;
  }
  if ((buttons & 16) == 0 && (current->last_buttons & 16) == 0 &&
      current->block_jump_frames_left > 0) {
    // Continue jump
    current->block_velocity_y = -6;
    current->block_jump_frames_left--;
  }
  if ((buttons & 16) != 0 && (current->last_buttons & 16) == 0) {
    // End jump
    current->block_jump_frames_left = 0;
    current->block_want_jump = 0;
  }
  if ((buttons & 8) == 0) {
    // Move right
    if (current->block_velocity_x < 2) {
      current->block_velocity_x = 2;
    } else {
      current->block_velocity_x = 4;
    }
  } else if ((buttons & 4) == 0) {
    // Move left
    if (current->block_velocity_x > -2) {
      current->block_velocity_x = -2;
    } else {
      current->block_velocity_x = -4;
    }
  } else {
    current->block_velocity_x = 0;
  }
  if ((buttons & 32) == 0 && (current->last_buttons & 32) != 0 &&
      current->block_moves_available > 0) {
    if ((buttons & 8) == 0) {
      // Dash right
      current->block_velocity_y = 0;
      current->block_moves_available -= 1;
      current->block_x += 24;
    }
    if ((buttons & 4) == 0) {
      // Dash left
      current->block_velocity_y = 0;
      current->block_moves_available -= 1;
      current->block_x -= 24;
    }
  }
  if (current->block_moves_available < 0) {
    current->block_moves_available = 0;
  }
  current->last_buttons = buttons;
}

static int handlePhysics(struct GameState *current) {
  int i;
  int j;
  int i_end;
  int j_end;
  int last_x;
  int last_y;
  int intersects_blocks = 0;
  current->block_velocity_y += 1;
  current->block_y += current->block_velocity_y >> 2;
  current->block_x += current->block_velocity_x;

  if (current->block_x < SCREEN_WIDTH_BEGIN) {
    current->block_x = SCREEN_WIDTH_BEGIN;
  }
  if (current->block_y < SCREEN_HEIGHT_BEGIN) {
    current->block_y = SCREEN_HEIGHT_BEGIN;
    current->block_velocity_y = 0;
  }
  if (current->block_x + BLOCK_WIDTH > SCREEN_WIDTH_END) {
    current->block_x = SCREEN_WIDTH_END - BLOCK_WIDTH;
  }
  if (current->block_y + BLOCK_HEIGHT > SCREEN_HEIGHT_END) {
    current->block_y = SCREEN_HEIGHT_END - BLOCK_HEIGHT;
    current->block_velocity_y = 0;
  }
  last_x = current->last_block_x;
  last_y = current->last_block_y;
  i_end = current->block_y / BLOCK_HEIGHT + 1 + !!(current->block_y & 7);
  j_end = current->block_x / BLOCK_WIDTH + 1 + !!(current->block_x & 7);
  for (i = current->block_y / BLOCK_HEIGHT; i < i_end; ++i) {
    for (j = current->block_x / BLOCK_WIDTH; j < j_end; ++j) {
      // Indices of all tiles intersecting block
      if (current->tiles[i][j] == 1) {
        intersects_blocks = 1;
        current->block_moves_available = 2;
        if (last_x / 8 + !!(last_x & 7) < j &&
            current->block_x / 8 + !!(current->block_x & 7) == j) {
          current->block_x = j * 8 - 8;
        } else if (last_x / 8 > j && current->block_x / 8 == j) {
          current->block_x = current->block_x / 8 * 8 + 8;
        }
      }
    }
  }
  if (intersects_blocks) {
    i_end = current->block_y / BLOCK_HEIGHT + 1 + !!(current->block_y & 7);
    j_end = current->block_x / BLOCK_WIDTH + 1 + !!(current->block_x & 7);
    for (i = current->block_y / BLOCK_HEIGHT; i < i_end; ++i) {
      for (j = current->block_x / BLOCK_WIDTH; j < j_end; ++j) {
        // Indices of all tiles intersecting block
        if (current->tiles[i][j] == 1) {
          current->block_moves_available = 2;
          if (last_y / 8 + !!(last_y & 7) < i &&
              current->block_y / 8 + !!(current->block_y & 7) == i) {
            current->block_y = i * 8 - 8;
            current->block_velocity_y = 0;
          } else if (last_y / 8 > i && current->block_y / 8 == i) {
            current->block_y = current->block_y / 8 * 8 + 8;
            current->block_velocity_y = 0;
          }
        }
      }
    }
  }
  i_end = current->block_y / BLOCK_HEIGHT + 1 + !!(current->block_y & 7);
  j_end = current->block_x / BLOCK_WIDTH + 1 + !!(current->block_x & 7);
  for (i = current->block_y / BLOCK_HEIGHT; i < i_end; ++i) {
    for (j = current->block_x / BLOCK_WIDTH; j < j_end; ++j) {
      // Indices of all tiles intersecting block
      if (current->tiles[i][j] == 2) {
        return 1;
      } else if (current->tiles[i][j] == 3) {
        DRAW_SPRITE((byte *)tile_sprites[current->tiles[i][j]], j * 8, i * 8, 8,
                    8,
                    /*flip*/ 0, DRAW_XOR);
        if (--current->coins_remaining == 0) {
          return 2;
        }
        current->tiles[i][j] = 0;
      }
    }
  }
  current->last_block_x = current->block_x;
  current->last_block_y = current->block_y;
  return 0;
}

static void drawBlock(struct GameState *current) {
  DRAW_SPRITE((byte *)block_sprites[current->block_moves_available],
              current->block_x, current->block_y, BLOCK_WIDTH, BLOCK_HEIGHT,
              /*flip*/ 0, DRAW_OR);
}

static void eraseBlock(struct GameState *current) {
  DRAW_SPRITE((byte *)block_sprites[current->block_moves_available],
              current->block_x, current->block_y, BLOCK_WIDTH, BLOCK_HEIGHT,
              /*flip*/ 0, DRAW_XOR);
}

static void drawTiles(struct GameState *current) {
  int i;
  int j;
  for (i = 0; i < TILE_ROWS; ++i) {
    for (j = 0; j < TILE_COLUMNS; ++j) {
      if (current->tiles[i][j] == 3) {
        ++current->coins_remaining;
      }
      DRAW_SPRITE((byte *)tile_sprites[current->tiles[i][j]], j * 8, i * 8, 8,
                  8,
                  /*flip*/ 0, DRAW_OR);
    }
  }
}

static void initGameState(struct GameState *current) {
  int i;
  // Memset 0
  for (i = 0; i < (int)sizeof(struct GameState); ++i) {
    ((char *)current)[i] = 0;
  }
  current->block_moves_available = 2;
}

static void initLevel1(struct GameState *current) {
  int i;
  int j;
  initGameState(current);

  for (i = 0; i < TILE_ROWS; ++i) {
    for (j = 0; j < TILE_COLUMNS; ++j) {
      current->tiles[i][j] = 3;
    }
  }

  for (i = 0; i < TILE_ROWS; ++i) {
    current->tiles[i][0] = 1;
    current->tiles[i][15] = 1;
  }

  for (j = 0; j < TILE_COLUMNS; ++j) {
    current->tiles[0][j] = 1;
    current->tiles[7][j] = 1;
  }

  current->block_x = 12;
  current->block_y = 8;
}

static void initLevel2(struct GameState *current) {
  int j;
  initLevel1(current);

  for (j = 0; j < 4; ++j) {
    current->tiles[j][7] = 2;
    current->tiles[j + 4][8] = 2;
  }
}

static void initLevel3(struct GameState *current) {
  int i;
  initLevel1(current);

  for (i = 0; i < TILE_COLUMNS; ++i) {
    current->tiles[7][i] = 2;
  }
}

static void initLevel4(struct GameState *current) {
  int i;
  initLevel3(current);

  for (i = 0; i < TILE_COLUMNS; ++i) {
    current->tiles[0][i] = 2;
  }
}

static void initLevel5(struct GameState *current) {
  int i;
  int j;
  initLevel4(current);

  for (i = 0; i < TILE_COLUMNS; ++i) {
    current->tiles[0][i] = 2;
  }
  for (j = 0; j < TILE_ROWS; ++j) {
    current->tiles[j][0] = 2;
    current->tiles[j][15] = 2;
  }
  for (j = 4; j < 12; ++j) {
    current->tiles[3][j] = 1;
    current->tiles[4][j] = 1;
  }
}

static void initLevel6(struct GameState *current) {
  int j;
  initLevel5(current);

  for (j = 0; j < TILE_ROWS; ++j) {
    current->tiles[j][7] = 2;
    current->tiles[j][8] = 2;
  }
}

static void initLevel7(struct GameState *current) {
  initLevel6(current);

  current->tiles[1][6] = 2;
  current->tiles[2][6] = 2;
  current->tiles[1][9] = 2;
  current->tiles[2][9] = 2;
}

static void initLevel8(struct GameState *current) {
  int i;
  int j;
  initLevel1(current);
  for (i = 0; i < TILE_ROWS; ++i) {
    current->tiles[i][0] = 2;
    current->tiles[i][15] = 2;
  }
  for (j = 3; j < TILE_COLUMNS - 3; ++j) {
    current->tiles[3][j] = 2;
    current->tiles[4][j] = 2;
  }
  for (j = 0; j < TILE_COLUMNS; ++j) {
    current->tiles[0][j] = 2;
    current->tiles[7][j] = 2;
  }
  current->tiles[3][0] = 1;
  current->tiles[4][0] = 1;
  current->tiles[3][15] = 1;
  current->tiles[4][15] = 1;
  current->tiles[0][7] = 1;
  current->tiles[0][8] = 1;
  current->tiles[7][7] = 1;
  current->tiles[7][8] = 1;
}

static void (*levels[])(struct GameState *) = {
    initLevel1, initLevel2, initLevel3, initLevel4,
    initLevel5, initLevel6, initLevel7, initLevel8};
static char *const levelMsgs[] = {
    "A to jump", "B + <dir> to dash", "Level 3", "Level 4",
    "Level 5",   "Level 6",           "Level 7", "Level 8"};

int main() {
  int i;
  int status;
  int level = 0;
  struct GameState state;

  // Initialize the API
  api_init();

BEGIN_GAME:
  if (level >= (int)(sizeof(levels) / sizeof(levels[0]))) {
    CLEAR();
    SET_CURSOR(0, 0);
    DRAW_STRING("You Win!");
    DISPLAY();
    return 0;
  } else {
    CLEAR();
    SET_CURSOR(0, 0);
    DRAW_STRING(levelMsgs[level]);
    DISPLAY();
    for (i = 0; i < 16; ++i) {
      WAIT();
    }
  }
  levels[level](&state);

  // Clear the graphics in video memory
  CLEAR();

  drawTiles(&state);

  for (;;) {
    processInput(&state, READ_BUTTONS());

    status = handlePhysics(&state);

    drawBlock(&state);

    // Push video memory to the OLED
    DISPLAY();

    // Wait for next interrupt
    WAIT();

    eraseBlock(&state);

    if (status == 1) {
      goto BEGIN_GAME;
    }
    if (status == 2) {
      ++level;
      goto BEGIN_GAME;
    }
  }
  return 0;
}

