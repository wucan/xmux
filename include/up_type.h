#ifndef _UP_TYPE_H_
#define _UP_TYPE_H_

#include "up_config.h"


#ifndef BOOL
	#define BOOL		int
#endif

#ifndef TRUE 
	#define FALSE	0
	#define TRUE	(!FALSE)
#endif

#define OID_MAX_SIZE		20

typedef unsigned long oid_t;
typedef unsigned long IPAddress;
#ifndef BOOL_DEFED
#define BOOL_DEFED
typedef int	bool;
#endif

#define ARRAY_CNT(t)	(sizeof(t) / sizeof(t[0]))


#endif /* _UP_TYPE_H_ */

