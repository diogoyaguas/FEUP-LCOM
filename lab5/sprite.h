#ifndef __SPRITE
#define __SPRITE

typedef struct {
  int x, y;            // current position
  int width, height;   // dimensions
  int xspeed, yspeed;  // current speed
  char *map;           // the pixmap
} Sprite;

/** Creates a new sprite with pixmap "pic", with specified
* position (within the screen limits) and speed;
* Does not draw the sprite on the screen
* Returns NULL on invalid pixmap.
*/
Sprite *create_sprite(char *pic[], int x, int y, int xspeed, int yspeed);

void destroy_sprite(Sprite *sp);

/** @} end of sprite */

#endif /* __SPRITE */
