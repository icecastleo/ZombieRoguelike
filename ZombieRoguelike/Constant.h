#pragma once

#define MS_PER_UPDATE 16

#define START_HP 200
#define LEVEL_UP_BASE 200
#define LEVEL_UP_FACTOR 150

#define SQRTF(a, b) sqrtf((a)*(a) + (b)*(b))

#define DIR 4 // number of possible directions to go at any position

#if DIR == 4
// if dir==4
static int dxx[4] = { 1, 0, -1, 0 };
static int dyy[4] = { 0, 1, 0, -1 };
#endif

// if dir==8
//static int dxx[dir] = { 1, 1, 0, -1, -1, -1, 0, 1 };
//static int dyy[dir] = { 0, 1, 1, 1, 0, -1, -1, -1 };