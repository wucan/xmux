#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include "wufs.h"

off_t wufs_get_file_size (char *name)
{
	struct stat st;

	if (stat(name, &st)) {
		perror("stat");
		return -1;
	}

	return st.st_size;
}

