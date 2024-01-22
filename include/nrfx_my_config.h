#pragma once
#define NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY 6

#undef NRFX_TIMER_ENABLED
#define NRFX_TIMER_ENABLED 1

// #define NRFX_TIMER0_ENABLED 0
// #define NRFX_TIMER1_ENABLED 0

#undef NRFX_TIMER2_ENABLED
#define NRFX_TIMER2_ENABLED 1
// #define NRFX_TIMER3_ENABLED 0

// <e> NRFX_TIMER_CONFIG_LOG_ENABLED - Enables logging in the module.
//==========================================================
#ifndef NRFX_TIMER_CONFIG_LOG_ENABLED
#define NRFX_TIMER_CONFIG_LOG_ENABLED 1
#endif
// <o> NRFX_TIMER_CONFIG_LOG_LEVEL  - Default Severity level

// <0=> Off
// <1=> Error
// <2=> Warning
// <3=> Info
// <4=> Debug

#ifndef NRFX_TIMER_CONFIG_LOG_LEVEL
#define NRFX_TIMER_CONFIG_LOG_LEVEL 3
#endif