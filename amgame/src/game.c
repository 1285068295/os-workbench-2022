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

  int next_frame = 0;

  // 坐标系  
  // o-----→x
  // ¦
  // ¦
  // ↓y

  // 初始化坐标位置和速度
  Square square = {10, 10, 0, 0};
  // 初始化一个键盘事件
  AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
  // 键盘事件中的按键信息
  int key = 0;
  while (1)
  {
    // 等待一帧的到来
    uint64_t pre_rdtsc = cpu_rdtsc();

    while (uptime(pre_rdtsc) < next_frame);

    // 这里虽然while循环，但是每次只处理一个按键信息
    while ((key = readkey(&event)) != AM_KEY_NONE && event.keydown) {
      // 处理键盘事件 根据按键更新方块速度值
      kbd_event(event, &square);         
    }

    game_progress(&square); // 处理一帧游戏逻辑，更新物体的位置等

    screen_update(square); // 重新绘制屏幕  绘制屏幕应该是异步的这里

    // FPS = 50 一秒内绘制图像50次
    next_frame += 1000 / FPS; // 计算下一帧的时间  20ms
  }
}
