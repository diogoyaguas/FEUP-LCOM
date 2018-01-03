#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "video_gr.h"
#include "lmlib.h"
#include "i8042.h"
#include "i8254.h"

#define LINEAR_MODEL_BIT 14
#define MODE_INFO 0x4F01
#define BLOCK_INFO 0x4F00
#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x)&0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
  struct reg86u r;
  mmap_t map;

  if (lm_init() == NULL) return -1;

  if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL) return -1;

  r.u.w.ax = MODE_INFO;
  r.u.w.es = PB2BASE(map.phys);
  r.u.w.di = PB2OFF(map.phys);
  r.u.w.cx = mode;
  r.u.b.intno = 0x10;
  if (sys_int86(&r) != OK) return -1;

  *vmi_p = *(vbe_mode_info_t *)map.virtual;

  lm_free(&map);

  return 0;
}

int vbe_get_info_block() {
  struct reg86u r;
  mmap_t map;
  vbe_info_block_t controller;

  if (lm_init() == NULL) return -1;

  if (lm_alloc(sizeof(vbe_info_block_t), &map) == NULL) return -1;

  r.u.w.ax = BLOCK_INFO;
  r.u.w.es = PB2BASE(map.phys);
  r.u.w.di = PB2OFF(map.phys);
  r.u.b.intno = 0x10;
  if (sys_int86(&r) != OK) return -1;

  controller = *(vbe_info_block_t *)map.virtual;

  printf("%d.%d\n", controller.VbeVersion >> 8, controller.VbeVersion & 0x0FF);

  uint16_t *videoInfoBlockPtr = (uint16_t *)controller.Reserved;

  while (*videoInfoBlockPtr != 0x0FFFF) {
    printf("0x%x:", *videoInfoBlockPtr);
    videoInfoBlockPtr++;
  }

  printf("\n%d\n", controller.TotalMemory * 64);

  lm_free(&map);

  return 0;
}
