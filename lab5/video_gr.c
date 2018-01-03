#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include <math.h>

#include "lmlib.h"
#include "vbe.h"
#include "read_xpm.h"
#include "video_gr.h"
#include "video_test.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */

/* Private global variables */

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */

static unsigned h_res = H_RES; /* Horizontal screen resolution in pixels */
static unsigned v_res = V_RES; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel =
    BITS_PER_PIXEL; /* Number of VRAM bits per pixel */

int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */
  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if (sys_int86(&reg86) != OK) {
    printf("\tvg_exit(): sys_int86() failed \n");
    return -1;
  } else
    return 0;
}

void *vg_init(unsigned short mode) {
  struct reg86u r;
  vbe_mode_info_t mode_info;
  int l;
  struct mem_range mr;
  unsigned int vram_base;
  unsigned int vram_size;

  r.u.w.ax = 0x4F02;           // VBE call, function 02 -- set VBE mode
  r.u.w.bx = 1 << 14 | 0x105;  // set bit 14: linear framebuffer
  r.u.b.intno = 0x10;

  if (sys_int86(&r) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return NULL;
  }

  vbe_get_mode_info(mode, &mode_info);

  h_res = mode_info.XResolution;
  v_res = mode_info.YResolution;
  bits_per_pixel = mode_info.BitsPerPixel;
  vram_base = mode_info.PhysBasePtr;
  vram_size = h_res * v_res * ceil(bits_per_pixel / 8);

  mr.mr_base = (phys_bytes)vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (l = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", l);

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED) panic("couldn't map video memory");

  return video_mem;
}

unsigned long set_pixel(int x, int y) { return h_res * y + x; }

int draw_square(short x, short y, unsigned short size, unsigned long color) {
  int i, j;

  x = h_res / 2 + x - size / 2;
  y = v_res / 2 + y - size / 2;

  if (x + size > h_res || y + size > v_res) return -1;

  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      video_mem[set_pixel(x + j, y + i)] = color;
      video_dump_fb();
    }
  }
  return 0;
}

int draw_line(int xi, int yi, int xf, int yf, int color) {
  // based on the code
  // https://www.tutorialspoint.com/computer_graphics/line_generation_algorithm.htm
  int dx, dy, step, i;
  float xinc, yinc;
  float x, y;

  dx = abs(xf - xi);
  dy = abs(yf - yi);

  if (dx > dy)
    step = dx;
  else
    step = dy;

  xinc = dx / (float)step;
  yinc = dy / (float)step;

  x = xi;
  y = yi;

  for (i = 0; i < step; i++) {
    x += xinc;
    y += yinc;
    video_mem[set_pixel(x, y)] = color;
    video_dump_fb();
  }

  return 0;
}

void draw_sprite(Sprite *sp) {
  unsigned long color;
  int y = sp->height, x = sp->width, i, j, a = 0;

  for (i = 0; i < y; i++) {
    for (j = 0; j < x; j++) {
      color = sp->map[a];

      video_mem[set_pixel(sp->x + j, sp->y + i)] = color;
      video_dump_fb();

      a++;
    }
  }
}

void erase_sprite(Sprite *sp) {
  int y = sp->height, x = sp->width, i, j;

  for (i = 0; i < y; i++) {
    for (j = 0; j < x; j++) {
      video_mem[set_pixel(sp->x + j, sp->y + i)] = BACKGROUND_COLOR;
      video_dump_fb();
    }
  }
}

int draw_xpm(char *xpm[], unsigned short xi, unsigned short yi) {
  int width, height;
  char *sprite = read_xpm(xpm, &width, &height);
  int i, j, a = 0;
  unsigned long color;

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      color = sprite[a];

      video_mem[set_pixel(xi + j, yi + i)] = color;
      video_dump_fb();

      a++;
    }
  }
  return 0;
}
