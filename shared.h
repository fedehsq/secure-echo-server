#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

#define CONFIG "./settings.conf"
#define CONFIG_LEN 128
#define bool int
#define true 1
#define false 0

extern int BUFFER_SIZE;
extern int BLOCK_SIZE;

char *strdup(const char *s);

// client & server variables
void setup_params();
