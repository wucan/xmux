#include "wu/wu_base_type.h"
#include "wu/thread.h"
#include "wu/message.h"

#include "wu_snmp_agent.h"

#include "xmux.h"
#include "xmux_snmp.h"


static Thread *agent_thr;
static bool agent_thread_quit;

/* netsnmp stuff */
extern int netsnmp_agent_init();
extern void netsnmp_agent_loop(void *data);
extern int netsnmp_agent_register(struct wu_oid_object *reg_obj);

extern int wu_agent_init();
extern void wu_agent_loop(void *data);
extern int wu_agent_register(struct wu_oid_object *reg_obj);

int wu_snmp_agent_init()
{
	netsnmp_agent_init();

	return 0;
}

void wu_snmp_agent_fini()
{
	if (agent_thr) {
		agent_thread_quit = true;
		thread_join(agent_thr);
		agent_thr = NULL;
		agent_thread_quit = false;
	}
}

static int agent_thread(void *data)
{
	netsnmp_agent_loop(agent_thread_quit);

	return 0;
}
int wu_snmp_agent_run()
{
	agent_thr = thread_create(agent_thread, NULL);
	if (!agent_thr) {
		return -1;
	}

	return 0;
}

void wu_oid_object_free(struct wu_oid_object *obj)
{
	free(obj);
}
struct wu_oid_object * wu_oid_object_dup(struct wu_oid_object *obj)
{
	struct wu_oid_object *new_obj;

	new_obj = (struct wu_oid_object *)malloc(sizeof(*obj));
	if (new_obj) {
		*new_obj = *obj;
	}

	return new_obj;
}
const char *oid_str(struct wu_oid_object *obj)
{
	static char buf[128];
	int i, off = 0;

	for (i = 0; i < obj->oid_len; i++) {
		off += sprintf(buf + off, "%d,", obj->oid[i]);
	}

	return buf;
}
bool oid_is(struct wu_oid_object *obj, wu_oid_t *oid, int oid_len)
{
	if (oid_len != obj->oid_len ||
		memcmp(oid, obj->oid, sizeof(wu_oid_t) * oid_len)) {
		return false;
	}

	return true;
}
int wu_snmp_agent_register(struct wu_oid_object *reg_obj)
{
	return netsnmp_agent_register(reg_obj);
}

