#include <game.h>

// 屏幕是 640 * 480
// 实际的 x  y 是  40*SIDE 30 * SIDE
#define SIDE 16
static int w, h;

static void init() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;
}

static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash() {
  init();
  // 初始化为黑白相间网格
  // for (int x = 0; x * SIDE <= w; x ++) {
  //   for (int y = 0; y * SIDE <= h; y++) {
  //     if ((x & 1) ^ (y & 1)) {
  //       draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
  //     }
  //   }
  // }
}




/**
 * @brief 处理方块的坐标信息
 *        (x', y')  <-- (x + vx/FPS,  y + vy/FPS)
 *        到达边缘后向相反方向移动
 * @param square 
 */
void game_progress(Square* square)
{
  // int FPS = 60;
  // int xx = square->x + square->vx / FPS;
  int xx = square->x + square->vx ;
  if (xx < 0)
  {
    xx = 0;
    square->vx = -square->vx;
  }
  else if (xx * SIDE >= w)
  {
    xx = w / SIDE;
    square->vx = -square->vx;
  }
  else
  {
    square->x = xx;
  }
 
  // int yy = square->y + square->vy / FPS;
  int yy = square->y + square->vy;
  if (yy <= 0)
  {
    yy = 0;
    square->vy = -square->vy;
  }
  else if (yy * SIDE >= h)
  {
    yy = h / SIDE;
    square->vy = -square->vy;
  }
  else
  {
    square->y = yy;
  }
}


/**
 * @brief 刷新屏幕
 * 
 * @param square 
 */
void screen_update(Square square)
{
  for (int x = 0; x * SIDE <= w; x++)
  {
    for (int y = 0; y * SIDE <= h; y++)
    {
      draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0); // white
      if (x == square.x && y == square.y)
      {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      }
    }
  }
}

/**
 * @brief   阅读时间戳计数器
 *          TSC寄存器统计了CPU自启动以来的运行时间，每个时钟信号到来时，TSC递增1。
 *          cpu频率 = 1秒内TSC寄存器统计的数值。
 *          (下一时刻读取统计值 - 上一刻统计值 ) * 1000(ms) / CPU主频
 * @param current_rdtsc 
 * @return int 
 */
int uptime(uint64_t current_rdtsc){
  for (int volatile i = 0; i < 100000; i++);
  return  (1000 * (cpu_rdtsc() - current_rdtsc) >> 20) / cpu_fre();
} 