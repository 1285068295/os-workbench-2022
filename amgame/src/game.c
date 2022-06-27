#include <game.h>



// Operating system is a C program!
int main(const char *args)
{
  ioe_init();
  puts("mainargs = \"");
  puts(args); // make run mainargs=xxx
  puts("\"\n");
  splash();
  puts("Press any key to see its key code...\n");
// while (1)
// {
//   print_key();
// }



  //uint32_t next_frame = 0;

  // 坐标待定  
  // o-----→x
  // ¦
  // ¦
  // ↓y

  // 初始化坐标位置和速度
  Square square = {0, 0, 0, 0};
  AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
  int key = 0;
  while (1)
  {
    // 等待一帧的到来
    // while (uptime() < next_frame);

    // 这里虽然while循环，但是每次只处理一个按键信息
    while ((key = readkey(&event)) != AM_KEY_NONE && event.keydown) {
      // 处理键盘事件 根据按键更新方块速度值
      kbd_event(event, &square);         
    }

    game_progress(&square); // 处理一帧游戏逻辑，更新物体的位置等

    screen_update(square); // 重新绘制屏幕

 //   next_frame += 1000 / FPS; // 计算下一帧的时间
  }
}
