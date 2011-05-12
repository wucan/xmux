#include "wu/wu_base_type.h"


#define guint		uint32_t
#define guint8		uint8_t
#define guint32		uint32_t
#define guint64		uint64_t
#define gint		int
#define gboolean	bool

/*
 * steal these codes from wireshard epan/oids.c
 */
guint oid_encoded2subid(const guint8 *oid_bytes, gint oid_len, guint32* subids) {
	gint i;
	guint n = 1;
	gboolean is_first = TRUE;
	guint32* subid_overflow;
	/*
	 * we cannot handle sub-ids greater than 32bytes
	 * have the subid in 64 bytes to be able to check the limit
	 */
	guint64 subid = 0;

	for (i=0; i<oid_len; i++) { if (! (oid_bytes[i] & 0x80 )) n++; }

	subid_overflow = subids+n;

	for (i=0; i<oid_len; i++){
		guint8 byte = oid_bytes[i];

		subid <<= 7;
		subid |= byte & 0x7F;

		if (byte & 0x80) {
			continue;
		}

		if (is_first) {
			guint32 subid0 = 0;

			if (subid >= 40) { subid0++; subid-=40; }
			if (subid >= 40) { subid0++; subid-=40; }

			*subids++ = subid0;

			is_first = FALSE;
		}

		if( subids >= subid_overflow || subid > 0xffffffff) {
			return 0;
		}

		*subids++ = (guint32)subid;
		subid = 0;
	}

	return n;
}

