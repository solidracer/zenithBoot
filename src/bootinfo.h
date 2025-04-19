#ifndef _bootinfo_h
#define _bootinfo_h

#include <stdint.h>
#include <stddef.h>

typedef unsigned int pixel_t;

typedef struct {
    pixel_t *framebuffer;
    size_t width, height;
} bootInfo;

#endif
