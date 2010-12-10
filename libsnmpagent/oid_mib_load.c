#include <stdio.h>

#include "wu/wufs.h"
#include "wu/mem.h"
#include "wu/message.h"

#include "oid_mib.h"


#define ENUMS_MAX_CNT		2000
#define RANGES_MAX_CNT		1000

static unsigned char *mib_buf, *oid_buf;
static int oid_cnt;
static struct enum_list enums[ENUMS_MAX_CNT];
static struct range_list ranges[RANGES_MAX_CNT];
static int next_enum_idx;
static int next_range_idx;
static msgobj mo;

/*
 * oid mib load from files
 */
int oid_mib_load_from_file ()
{
	FILE *mib_file, *oid_file;
	off_t filesize;
	int rc = -1;

	msgobj_init(&mo, MSG_INFO, 1, "OID-MIB");
	mib_file = fopen(MIB_FILE, "r");
	if (!mib_file) {
		trace_err("could not open %s!", MIB_FILE);
		return -1;
	}
	oid_file = fopen(OID_FILE, "r");
	if (!oid_file) {
		trace_err("could not open %s!", OID_FILE);
		fclose(mib_file);
		return -1;
	}
	/* load mib file */
	filesize = wufs_get_file_size("mib_file");
	mib_buf = mem_malloc(filesize);
	if (!mib_buf) {
		trace_err("could not alloc mib buf, size %d", filesize);
		goto close_file_out;
	}
	fread(mib_buf, 1, filesize, mib_file);
	/* load oid file */
	filesize = wufs_get_file_size("oid_file");
	fread(&oid_cnt, 1, 4, oid_file);
	filesize -= 4;
	oid_buf = mem_malloc(filesize);
	if (!oid_buf) {
		trace_err("could not alloc oid buf, size %d", filesize);
		mem_free(mib_buf);
		goto close_file_out;
	}
	fread(oid_buf, 1, filesize, oid_file);
	/* all ok */
	trace_info("sucess load, oid count is %d", oid_cnt);
	rc = 0;

close_file_out:
	/* close files */
	fclose(mib_file);
	fclose(oid_file);

	return rc;
}
static unsigned char * find_mib_info (oid_t *oid, int oidlen)
{
	struct oid_info *poi = (struct oid_info *)oid_buf;
	int i, j;

	oid += 8;
	oidlen -= 8;
	for (i = 0; i < oid_cnt; i++) {
		if (poi[i].oidlen != oidlen) {
			continue;
		}
		for (j = 0; j < oidlen; j++) {
			if (poi[i].oid[j] != oid[j]) {
				break;
			}
		}
		if (j == oidlen) {
			/* found */
			return mib_buf + poi[i].off;
		}
	}

	return NULL;
}
static int fill_enums (OIDOps *oidops, unsigned char **data)
{
	unsigned char *p = *data;
	int tag, over = 0;
	int ivalue;
	struct enum_list *e = &enums[next_enum_idx], *laste = NULL;
	struct enum_list *enums = e;
	char *pstr;
	
	while (!over) {
		tag = get_tag(p);
		if (tag == -1) {
			return -1;
		}
		switch (tag) {
			case TAG_ENUM_VALUE:
				ivalue = get_tag_int(p);
				e->value = ivalue;
				goto_next_tag(&p);
				break;
			case TAG_ENUM_LABEL:
				pstr = get_tag_string(p);
				e->label = pstr;
				goto_next_tag(&p);
				if (laste) {
					laste->next = e;
				}
				laste = e;
				next_enum_idx++;
				if (next_enum_idx >= ENUMS_MAX_CNT) {
					trace_err("enums exceed! max enum max is %d, enlarge it!",
							ENUMS_MAX_CNT);
					exit(1);
				}
				e++;
				break;
			case TAG_ENUMS_END:
				goto_next_tag(&p);
				over = 1;
				break;
			default:
				over = 1;
				break;
		}
	}
	oidops->enums = enums;
	*data = p;

	return 0;
}
static int fill_ranges (OIDOps *oidops, unsigned char **data)
{
	unsigned char *p = *data;
	int tag, over = 0;
	struct range_list *r = &ranges[next_range_idx], *lastr = NULL;
	struct range_list *ranges = r;
	
	while (!over) {
		tag = get_tag(p);
		if (tag == -1) {
			return -1;
		}
		switch (tag) {
			case TAG_RANGE_VALUE_PAIR:
				get_tag_range_value_pair(p, &r->low, &r->high);
				goto_next_tag(&p);
				if (lastr) {
					lastr->next = r;
				}
				lastr = r;
				next_range_idx++;
				if (next_range_idx >= RANGES_MAX_CNT) {
					trace_err("ranges exceed! max range max is %d, enlarge it!",
							RANGES_MAX_CNT);
					exit(1);
				}
				r++;
				break;
			case TAG_RANGES_END:
				goto_next_tag(&p);
				over = 1;
				break;
			default:
				over = 1;
				break;
		}
	}
	oidops->ranges = ranges;
	*data = p;

	return 0;
}
static int
parse_and_fill_oidops (OIDOps *oidops, unsigned char *mibinfo)
{
	unsigned char *p = mibinfo;
	int tag, over = 0;
	char *pstr;
	int ivalue;

	while (!over) {
		tag = get_tag(p);
		if (tag == -1) {
			return -1;
		}
		switch (tag) {
			case TAG_END:
				over = 1;
				break;
			case TAG_LABEL:
				pstr = get_tag_string(p);
				oidops->label = pstr;
				goto_next_tag(&p);
				break;
			case TAG_HINT:
				pstr = get_tag_string(p);
				oidops->hint = pstr;
				goto_next_tag(&p);
				break;
			case TAG_UNITS:
				pstr = get_tag_string(p);
				oidops->units = pstr;
				goto_next_tag(&p);
				break;
			case TAG_TYPE:
				ivalue = get_tag_int(p);
				oidops->oidtype = ivalue;
				goto_next_tag(&p);
				break;
			case TAG_ACCESS:
				ivalue = get_tag_int(p);
				oidops->access = ivalue;
				goto_next_tag(&p);
				break;
			case TAG_STATUS:
				ivalue = get_tag_int(p);
				oidops->status = ivalue;
				goto_next_tag(&p);
				break;
			case TAG_ENUMS_BEGIN:
				goto_next_tag(&p);
				fill_enums(oidops, &p);
				break;
			case TAG_RANGES_BEGIN:
				goto_next_tag(&p);
				fill_ranges(oidops, &p);
				break;
			default:
				over = 1;
				break;
		}
	}
}
/*
 * fill the oidops with oid mib
 */
int oidops_load_oid_mib (OIDOps *oidops)
{
	unsigned char *mibinfo;

	mibinfo = find_mib_info(oidops->oid, oidops->oidlen);
	if (!mibinfo) {
		return -1;
	}
	parse_and_fill_oidops(oidops, mibinfo);

	return 0;
}
/*
 * dump oid mib cache info
 */
void oid_mib_dump_info ()
{
	trace_info("there are %d oid in the cache", oid_cnt);
	trace_info("and had %d enum item, %d range item",
		next_enum_idx, next_range_idx);
}

