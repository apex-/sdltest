#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <stdint.h>

const float DEG2RAD = 3.14159265f/180.0f;

const uint32_t VIEWPORT_WIDTH = 1280;
const uint32_t VIEWPORT_HEIGHT = 720;
const uint32_t VIEWPORT_PIXELS = VIEWPORT_WIDTH * VIEWPORT_HEIGHT;


const float screenx_conv_factor = (VIEWPORT_WIDTH -1) / 2.0;
const float screeny_conv_factor = (VIEWPORT_HEIGHT -1) / 2.0;


#endif // GLOBAL_H_INCLUDED
