#ifndef INCLUDE_H
#define INCLUDE_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <math.h>
#include <map>
#include <typeinfo>
#include "class.h"
#include "misc/vec/vec.h"

#define SetBit(p, i) ((p)[(i) >> 5] |= (1 << ((i) & 31)))
#define GetBit(p, i) (((p)[(i) >> 5] & (1 << ((i) & 31))) > 0)
#define ALLOC(type, num) ((type *)malloc(sizeof(type) * (num)))
#define RANDOM_UNSIGNED                                        \
    ((((unsigned)rand()) << 24) ^ (((unsigned)rand()) << 12) ^ \
     ((unsigned)rand()))

#endif
