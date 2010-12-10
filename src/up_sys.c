#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "up_config.h"


#if HAD_UCBOOTSTRAP
	#include <asm/uCbootstrap.h>

_bsc1 (int, setbenv, char *, a)
_bsc1 (char *, getbenv, char *, a1)


int up_sys_set_benv (char *env, char *value)
{
	char str[100];

	sprintf(str, "%s=%s", env, value);
	setbenv(str);

	return 0;
}
char * up_sys_get_benv (char *env)
{
	return getbenv(env);
}
#else
int up_sys_set_benv (char *env, char *value)
{
	return 0;
}
char * up_sys_get_benv (char *env)
{
	return NULL;
}
#endif

