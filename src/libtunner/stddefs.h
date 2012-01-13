/******************************************************************************

File Name   : stddefs.h

Description : Contains a number of generic type declarations and defines.

Copyright (C) 1999 STMicroelectronics

******************************************************************************/

/* Define to prevent recursive inclusion */
#ifndef __STDDEFS_H
#define __STDDEFS_H


/* Includes ---------------------------------------------------------------- */
#ifndef HOST_PC
//    #include "stlite.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Types ---------------------------------------------------------- */

/* Common unsigned types */
typedef unsigned char  U8;
typedef unsigned int U16;
typedef unsigned long   U32;

/* Common signed types */
typedef signed char  S8;
typedef signed int S16;
typedef signed long   S32;

/* Boolean type (values should be among TRUE and FALSE constants only) */
typedef int BOOL;

/* General purpose string type */
typedef char* ST_String_t;

/* Function return error code */
typedef U32 ST_ErrorCode_t;

/* Revision structure */
typedef const char * ST_Revision_t;

/* Device name type */
#define ST_MAX_DEVICE_NAME 16  /* 15 characters plus '\0' */
typedef char ST_DeviceName_t[ST_MAX_DEVICE_NAME];

/* Generic partition type */
//#ifndef HOST_PC
 //   typedef partition_t ST_Partition_t;
//#endif

/* Exported Constants ------------------------------------------------------ */

/* BOOL type constant values */
#ifndef TRUE
    #define TRUE (1 == 1)
#endif
#ifndef FALSE
    #define FALSE (!TRUE)
#endif

/* Common driver error constants */
#define ST_DRIVER_ID   0
#define ST_DRIVER_BASE (ST_DRIVER_ID << 16)
enum
{
    ST_NO_ERROR = 0,
    ST_ERROR_BAD_PARAMETER,             /* Bad parameter passed       */
    ST_ERROR_NO_MEMORY,                 /* Memory allocation failed   */
    ST_ERROR_UNKNOWN_DEVICE,            /* Unknown device name        */
    ST_ERROR_ALREADY_INITIALIZED,       /* Device already initialized */
    ST_ERROR_NO_FREE_HANDLES,           /* Cannot open device again   */
    ST_ERROR_OPEN_HANDLE,               /* At least one open handle   */
    ST_ERROR_INVALID_HANDLE,            /* Handle is not valid        */
    ST_ERROR_FEATURE_NOT_SUPPORTED,     /* Feature unavailable        */
    ST_ERROR_INTERRUPT_INSTALL,         /* Interrupt install failed   */
    ST_ERROR_INTERRUPT_UNINSTALL,       /* Interrupt uninstall failed */
    ST_ERROR_TIMEOUT,                   /* Timeout occured            */
    ST_ERROR_DEVICE_BUSY                /* Device is currently busy   */
};

/* Exported Variables ------------------------------------------------------ */


/* Exported Macros --------------------------------------------------------- */


/* Exported Functions ------------------------------------------------------ */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __STDDEFS_H */

/* End of stddefs.h  ------------------------------------------------------- */


