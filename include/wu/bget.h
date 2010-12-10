/*

    Interface definitions for bget.c, the memory management package.

*/
#include <stdlib.h>

__BEGIN_DECLS

#ifndef _
#ifdef PROTOTYPES
#define  _(x)  x		      /* If compiler knows prototypes */
#else
#define  _(x)  ()                     /* It it doesn't */
#endif /* PROTOTYPES */
#endif

typedef long bufsize;
//void	bpool	    _((void *buffer, bufsize len));
void	bpool	    (void *buffer, bufsize len);
void   *bget	    (bufsize size);
void   *bgetz	    _((bufsize size));
void   *bgetr	    _((void *buffer, bufsize newsize));
//void	brel	    _((void *buf));
void	brel	    (void *buf);
void	bectl	    _((int (*compact)(bufsize sizereq, int sequence),
		       void *(*acquire)(bufsize size),
		       void (*release)(void *buf), bufsize pool_incr));
//void	bstats	    _((bufsize *curalloc, bufsize *totfree, bufsize *maxfree,
//		       long *nget, long *nrel));
void	bstats	    (bufsize *curalloc, bufsize *totfree, bufsize *maxfree,
		       long *nget, long *nrel);
void	bstatse     _((bufsize *pool_incr, long *npool, long *npget,
		       long *nprel, long *ndget, long *ndrel));
void	bufdump     _((void *buf));
//void	bpoold	    _((void *pool, int dumpalloc, int dumpfree));
void	bpoold	    (void *pool, int dumpalloc, int dumpfree);
int	bpoolv	    _((void *pool));

__END_DECLS

