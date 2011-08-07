/*
 * create by why @ 2009 06 10 10:56
 * libci
 */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <semaphore.h>
#include "ci_define.h"

static sem_t ci_inited;

unsigned int load_libci()
{
    unsigned int ret = 0;

    ret = sem_init(&ci_inited, 0, 0);

    if(-1 == ret)
    {
        return 0;
    }
    
    return init_ci();
}

