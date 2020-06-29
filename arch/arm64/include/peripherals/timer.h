#ifndef	_P_TIMER_H
#define	_P_TIMER_H

#include "peripherals/base.h"

#define LOCAL_TIMER_ROUTING (VA1_START + 0x40000024)
#define LOCAL_TIMER_RELOAD 0x249f000
#define LOCAL_TIMER_ROUTING_TARGET 0x7
#define LOCAL_TIMER_RESET (0x3 << 30)

#endif  /*_P_TIMER_H */
