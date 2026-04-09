/* Modes.h
 * Operating modes API
 */

#ifndef MODES_H
#define MODES_H

typedef enum { MODE_AUTO, MODE_RANDOM, MODE_HOLIDAY, MODE_MANUAL } ModeType;

void Modes_Init(void);
ModeType Modes_GetCurrent(void);

#endif /* MODES_H */