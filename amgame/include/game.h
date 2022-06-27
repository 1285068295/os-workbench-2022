#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>

#ifndef SPEED
  #define SPEED 1
#endif
/**
 * @brief  方块  4x4大小
 * 注意 typedef struct 和 struct 区别
 */
typedef struct SQUARE
{
  // x,y坐标
  int x, y;
  // x，y 方向速度
  int vx, vy;
} Square;

void splash();
void print_key();
int  readkey(AM_INPUT_KEYBRD_T* event);
void kbd_event(AM_INPUT_KEYBRD_T event, Square *square);
void game_progress(Square* square);
void screen_update(Square square);

static inline void puts(const char *s)
{
  for (; *s; s++)
    putch(*s);
}
