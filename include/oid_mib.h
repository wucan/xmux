#ifndef _OID_MIB_H_
#define _OID_MIB_H_

#include <string.h>

#include "up_mib.h"


#define MIB_FILE		"mib_file"
#define OID_FILE		"oid_file"

struct oid_info {
	char oid[7];
	char oidlen;
	int off;
};
enum {
	TAG_END = 0,

	TAG_LABEL,
	TAG_HINT,
	TAG_UNITS,

	TAG_TYPE,
	TAG_ACCESS,
	TAG_STATUS,

	TAG_ENUMS_BEGIN,
	TAG_ENUMS_END,
	TAG_RANGES_BEGIN,
	TAG_RANGES_END,

	TAG_ENUM_VALUE,
	TAG_ENUM_LABEL,

	TAG_RANGE_VALUE_PAIR,

	TAG_MAX,
};
struct mib_info {
	char tag;
	/* max 256 */
	unsigned char size;
};
static inline int get_tag (unsigned char *data)
{
	int tag;

	tag = *data;
	if (tag < TAG_END || tag >= TAG_MAX) {
		return -1;
	}

	return tag;
}
static inline void goto_next_tag (unsigned char **data)
{
	*data += (*data)[1] + 2;
}
static inline char * get_tag_string (unsigned char *data)
{
	return (char *)(data + 2);
}
static inline int get_tag_int (unsigned char *data)
{
	int value;

	memmove(&value, data + 2, 4);

	return value;
}
static inline void
get_tag_range_value_pair (unsigned char *data, int *low, int *high)
{
	int value[2];
	
	memmove(value, data + 2, 8);
	*low = value[0];
	*high = value[1];
}
int oid_mib_load_from_file ();
int oidops_load_oid_mib (OIDOps *oidops);
void oid_mib_dump_info ();


#endif /* _OID_MIB_H_ */

