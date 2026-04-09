/* Programs.h
 * Placeholder for Programs module
 * Manages time programs (daily, yearly) as per Design Reference
 */

#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_PROGRAMS 100

typedef enum {
    PROGRAM_DAILY,
    PROGRAM_YEARLY
} ProgramType;

void Programs_Init(void);
int Programs_Add(ProgramType type, const char *spec);
void Programs_Remove(int id);

#endif /* PROGRAMS_H */
