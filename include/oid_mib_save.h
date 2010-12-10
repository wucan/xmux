/*
 * save oid mib to cache files
 */

#include <stdio.h>

#include "mem.h"
#include "message.h"

#include "oid_mib.h"


static unsigned char *mib_buf, *oid_buf;
static int mib_buf_off, oid_buf_off;
static int oid_cnt;
static struct oid_info oi;
static struct mib_info mi;
static msgobj mo;

static void save_tag (int tag, unsigned char *data, int size)
{
	mi.tag = tag;
	mi.size = size;
	memcpy(mib_buf + mib_buf_off, &mi, sizeof(mi));
	mib_buf_off += sizeof(mi);
	memcpy(mib_buf + mib_buf_off, data, size);
	mib_buf_off += size;
}
static void save_tag_int (int tag, int value)
{
	save_tag(tag, (unsigned char *)&value, 4);
}
static void save_tag_string (int tag, char *str)
{
	int size = strlen(str);

	save_tag(tag, (unsigned char *)str, size + 1);
}
static void save_tag_end ()
{
	mi.tag = TAG_END;
	mi.size = 0;
	memcpy(mib_buf + mib_buf_off, &mi, sizeof(mi));
	mib_buf_off += sizeof(mi);
}
static void save_tag_only (int tag)
{
	mi.tag = tag;
	mi.size = 0;
	memcpy(mib_buf + mib_buf_off, &mi, sizeof(mi));
	mib_buf_off += sizeof(mi);
}
static void save_tag_enums (struct enum_list *e)
{
	save_tag_only(TAG_ENUMS_BEGIN);
	while (e) {
		save_tag_int(TAG_ENUM_VALUE, e->value);
		save_tag_string(TAG_ENUM_LABEL, e->label);
		e = e->next;
	}
	save_tag_only(TAG_ENUMS_END);
}
static void save_tag_ranges (struct range_list *r)
{
	int values[2];

	save_tag_only(TAG_RANGES_BEGIN);
	while (r) {
		values[0] = r->low;
		values[1] = r->high;
		save_tag(TAG_RANGE_VALUE_PAIR, (unsigned char *)values, 8);
		r = r->next;
	}
	save_tag_only(TAG_RANGES_END);
}
static void save_tree (struct tree *t)
{
	int len;
	oid_t oid[20];
	int oidlen, i;

	oidlen = snmp_data_get_node_oid((snmp_data_tree *)&t, oid);
	oi.oidlen = oidlen - mib_tree_head_deepth;
	if (oi.oidlen <= 0) {
		trace_err("save tree oid stranger!");
		return;
	}

	for (i = 0; i < oi.oidlen; i++) {
		oi.oid[i] = oid[mib_tree_head_deepth + i];
	}
	oi.off = mib_buf_off;
	memcpy(oid_buf + oid_buf_off, &oi, sizeof(oi));
	oid_buf_off += sizeof(oi);

	if (t->label) {
		save_tag_string(TAG_LABEL, t->label);
	}
	if (t->type == 15) {
		t->type = 14;
	}
	save_tag_int(TAG_TYPE, t->type);
	save_tag_int(TAG_ACCESS, t->access);
	save_tag_int(TAG_STATUS, t->status);
	if (t->hint) {
		save_tag_string(TAG_HINT, t->hint);
	}
	if (t->units) {
		save_tag_string(TAG_UNITS, t->units);
	}
	if (t->enums) {
		save_tag_enums(t->enums);
	}
	if (t->ranges) {
		save_tag_ranges(t->ranges);
	}
	save_tag_end();

	oid_cnt++;
}
static void _snmp_data_save (struct tree *t)
{
	while (t) {
		if (!t->child_list) {
		       	if (t->type != TYPE_OTHER) {
				/* save t */
				save_tree(t);
			}
		} else {
			/* go to t's son */
			_snmp_data_save(t->child_list);
		}
		/* goto t's next */
		t = t->next_peer;
	}
}
void snmp_data_save ()
{
	FILE *mib_file, *oid_file;

	msgobj_init(&mo, MSG_INFO, 1, "OID-MIB-SAVE");
	mib_file = fopen(MIB_FILE, "w+");
	if (!mib_file) {
		trace_warn("cannot open %s for write!", MIB_FILE);
		return;
	}
	oid_file = fopen(OID_FILE, "w+");
	if (!oid_file) {
		trace_warn("cannot open %s for write!", OID_FILE);
		fclose(mib_file);
		return;
	}
	mib_buf = mem_malloc(1024 * 1024);
	oid_buf = mem_malloc(1024 * 1024);
	if (!mib_buf || !oid_buf) {
		trace_err("cannot alloc mem as oper buf!");
		goto close_file_out;
	}
	_snmp_data_save(mib_tree_head->child_list);
	/* save */
	fwrite(mib_buf, 1, mib_buf_off, mib_file);
	fwrite(&oid_cnt, 1, 4, oid_file);
	fwrite(oid_buf, 1, oid_buf_off, oid_file);
close_file_out:
	/* close files */
	fclose(mib_file);
	fclose(oid_file);
	/* free bufs */
	if (mib_buf) {
		mem_free(mib_buf);
	}
	if (oid_buf) {
		mem_free(oid_buf);
	}
}

