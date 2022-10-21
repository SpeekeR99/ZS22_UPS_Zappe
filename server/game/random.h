#ifndef SERVER_RANDOM_H
#define SERVER_RANDOM_H

#include <stdlib.h>
#include <time.h>
#include "typedefs.h"

#define MAX_DIE_NUM 6
#define MIN_DIE_NUM 1

void init_random();
byte roll_a_die();

#endif
