#ifndef WATCHDOG_H
#define WATCHDOG_H
#include "error_code.h"
ErrorCode watchdog_init(void);  /* ~4s timeout, call from task context after scheduler */
void watchdog_feed(void);       /* feed in display task loop */
#endif
