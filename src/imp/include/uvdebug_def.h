// uvdebug_def.h

#ifndef _UVDEBUG_DEF_H
#define _UVDEBUG_DEF_H

#define UV_DEBUG
#ifdef UV_DEBUG
#define uv_dbg_printf(x) (x)
#else
#define uv_dbg_printf(x) ()
#endif

#define UV_TEST_DEBUG
#ifdef UV_TEST_DEBUG
#define uv_test_printf(x) (x)
#else
#define uv_test_printf(x) ()
#endif

#define DEBUG_PRT

#ifdef DEBUG_PRT
#ifndef dbg_prt
#define dbg_prt printf
#endif
#else

#ifndef dbg_prt
#define dbg_prt(...)
#endif
#endif

#endif
