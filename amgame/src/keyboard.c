#include <game.h>

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = {
    AM_KEYS(KEYNAME)};

void print_key()
{
  AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown)
  {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}

/**
 * @brief  TODO 这里不能用 参数传进来的event来接收键盘事件 why???
 *         指针的地址不能改变 但是可改变指向的内容
 * @param event 
 * @return int 
 */
int readkey(AM_INPUT_KEYBRD_T* event)
{
  // 用新的s来接收返回值 
  AM_INPUT_KEYBRD_T new_event = {.keycode = AM_KEY_NONE};
  ioe_read(AM_INPUT_KEYBRD, &new_event);
  if (new_event.keycode != AM_KEY_NONE)
  {
    puts("Key pressed: ");
    puts(key_names[new_event.keycode]);
    puts("\n");
  }
  event->keycode = new_event.keycode;
  event->keydown = new_event.keydown;
  return event->keycode;
}


/**
 * @brief 处理键盘事件
 * 
 * @param event 
 * @param square 
 */
void kbd_event(AM_INPUT_KEYBRD_T event, Square *square)
{
  switch (event.keycode)
  {
  case AM_KEY_A:
  case AM_KEY_LEFT:
    square->vx = -SPEED;
    break;
  case AM_KEY_D:
  case AM_KEY_RIGHT:
    square->vx = SPEED;
    break;
  case AM_KEY_S:
  case AM_KEY_DOWN:
    square->vy = SPEED;
    break;
  case AM_KEY_W:
  case AM_KEY_UP:
    square->vx = -SPEED;
    break;
  case AM_KEY_ESCAPE:
    halt(0);
    break;
  default:
    break;
  }
}