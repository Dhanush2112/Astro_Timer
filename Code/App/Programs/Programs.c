/* Programs.c
 * Minimal placeholder implementation for Programs module
 */

#include "Programs.h"
#include <string.h>

static int program_count = 0;

void Programs_Init(void) {
    program_count = 0;
}

int Programs_Add(ProgramType type, const char *spec) {
    if (program_count >= MAX_PROGRAMS) return -1;
    (void)type; (void)spec; // placeholder
    return program_count++;
}

void Programs_Remove(int id) {
    (void)id; // placeholder
}
