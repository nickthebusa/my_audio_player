#ifndef _EVENTS_H_
#define _EVENTS_H_

#include "main.h"

#define CTRL(x) ((x) & 0x1f)

void process_events(AppState *app);

#endif
