#ifndef _WU_BASE_TYPE_H_
#define _WU_BASE_TYPE_H_

#include <stddef.h>
#include <stdint.h>

#include "up_type.h"

#ifndef bool
	#define bool	int
	#define false	0
	#define true	(!false)
#endif

#define MIN(a,b)			((a) < (b) ? (a) : (b))
#define MAX(a,b)			((a) > (b) ? (a) : (b))
#define ARRAY_ESIZE(a)		(sizeof(a) / sizeof(a[0]))


#endif /* _WU_BASE_TYPE_H_ */

