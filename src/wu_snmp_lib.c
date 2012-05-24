#include "wu/wu_base_type.h"
#include "wu/message.h"
#include "wu/wu_udp.h"
#include "wu/wutree.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>

#include "wu_snmp_agent.h"
#include "xmux_snmp.h"


#define SNMP_DEBUG		0
#if SNMP_DEBUG
#define snmp_hex_dump		hex_dump
#else
#define snmp_hex_dump(...)
#endif

static msgobj mo = {MSG_INFO, ENCOLOR, "snmp"};


static struct wu_oid_object * find_oid_object(wu_oid_t *oid, int oid_len);

/*
 * implement base pdu methods:
 * get-request, set-request, response
 */

/*
 * SMI
 */
// tag len data
enum {
	TagInt = 0x02,
	TagString = 0x04,
	TagValue = 0x05,
	TagOID = 0x06,
	TagVar = 0x30,

	TagGetRequest = 0xa0,
	TagGetResponse = 0xa2,
	TagSetRequest = 0xa3,
};

struct snmp_value {
	uint32_t int_v;
	uint8_t *string;
	uint8_t *var;
};
struct wu_snmp_atom {
	uint8_t tag;
	uint16_t len;
	struct snmp_value data;
};
struct wu_snmp_com_atom {
	struct wu_snmp_atom atom;

	uint16_t max_size;

	struct wu_snmp_com_atom *parent_catom;
	struct wu_snmp_com_atom *son_catom;

	uint8_t data_buf[2048];
};

struct wu_snmp_pdu {
	struct wu_snmp_atom header;
	struct wu_snmp_atom version;
	struct wu_snmp_atom com;

	struct wu_snmp_atom method;

	struct wu_snmp_atom request_id;
	struct wu_snmp_atom error_status;
	struct wu_snmp_atom error_index;
	struct wu_snmp_atom variable_bindings;
};

#if 0
struct VarBind {
	ObjectName;
	ObjectSyntax;
};
#endif

enum snmp_error_status {
	noError = 0,
	tooBig = 1,
	noSuchName = 2,
	badValue = 3,
	readOnly = 4,
	genErr = 5,
	noAccess = 6,
	wrongType = 7,
	wrongLength = 8,
	wrongEncoding = 9,
	wrongValue = 10,
	noCreation = 11,
	inconsistentValue = 12,
	resourceUnavailable = 13,
	commitFailed = 14,
	undoFailed = 15,
	authorizationError = 16,
	notWritable = 17,
	inconsistentName = 18,

	/* private error */
	noSuchObject = 100,
};

struct wu_snmp_request {
	uint8_t *data;
	uint16_t size;
};

struct wu_snmp_var_bind {
	uint8_t error;

	struct wu_snmp_atom name;
	struct wu_snmp_atom value;

	wu_oid_t oid[WU_OID_MAX_SIZE];
	int oid_len;

	uint8_t data_buf[2048];
};

#define RESP_DATA_MAX_SIZE		2048
struct wu_snmp_client {
	struct wu_snmp_request request;

	uint8_t error_status[16];
	uint8_t error_index[16];
	uint8_t errors;

	struct wu_snmp_var_bind variable_bindings[16];

	uint8_t resp_data[RESP_DATA_MAX_SIZE];
	uint16_t resp_size;

	int readonly;
};

struct wu_snmp_pdu *build_pdu(void *data)
{
	return NULL;
}
static void atom_set_data(struct wu_snmp_atom *atom,
	uint8_t *data, uint16_t len)
{
	switch (atom->tag) {
		case TagInt:
			atom->data.int_v = 0;
			memcpy(&atom->data.int_v, data, len);
			atom->data.string = data;
			break;
		case TagString:
			atom->data.string = data;
			break;
		case TagValue:
			atom->data.string = data;
			break;
		case TagOID:
			atom->data.string = data;
			break;
		case TagVar:
			atom->data.string = data;
			break;
		case TagGetRequest:
			atom->data.string = data;
			break;
		case TagSetRequest:
			atom->data.string = data;
			break;
		default:
			trace_warn("unimplement tag %#x", atom->tag);
			break;
	}
	atom->len = len;
	trace_dbg("atom tag %#x, len %#x", atom->tag, atom->len);
}
static int extract_fix_atoms(uint8_t **pdata, uint16_t *psize,
	struct wu_snmp_atom *atoms, int atom_cnt)
{
	int atom_idx = 0;
	uint8_t *data = *pdata;
	uint16_t size = *psize;
	uint8_t *data_end = data + size;
	uint8_t tag;
	uint16_t len;

	while (data < data_end && atom_idx < atom_cnt) {
		tag = *data++;
		len = *data++;
		if (len == 0x82) {
			len = data[0] << 8 | data[1];
			data += 2;
		} else if (len == 0x81) {
			len = data[0];
			data += 1;
		}
		if (tag != atoms[atom_idx].tag) {
			trace_err("atom #%d tag mismatch!(recv %#x, expect %#x)",
				atom_idx, tag, atoms[atom_idx].tag);
			// error!
			return -1;
		}
		atom_set_data(&atoms[atom_idx], data, len);
		data += len;
		atom_idx++;
	}

	*pdata = data;
	*psize = data_end - data;

	return 0;
}
int pop_atom(uint8_t **pdata, uint16_t *psize, struct wu_snmp_atom *atom)
{
	uint8_t *data = *pdata;
	uint16_t size = *psize;

	if (size == 0) {
		trace_dbg("pop atom: data size 0, no more atom");
		return -1;
	} else if (size < 2) {
		trace_err("pop atom: data size %#x too small!", size);
		snmp_hex_dump("raw data", *pdata, MIN(*psize, 128));
		return -1;
	}
	atom->tag = *data++;
	atom->len = *data++;
	if (atom->len == 0x82) {
		atom->len = data[0] << 8 | data[1];
		data += 2;
	} else if (atom->len == 0x81) {
		atom->len = data[0];
		data += 1;
	}
	if (size < 2 + atom->len) {
		trace_err("pop atom: expect data size %#x, but real %#x!",
			2 + atom->len, size);
		snmp_hex_dump("raw data", *pdata, *psize);
		return -1;
	}

	atom_set_data(atom, data, atom->len);

	*psize -= (data + atom->len) - *pdata;
	*pdata = data + atom->len;

	return 0;
}
extern uint32_t oid_encoded2subid(const uint8_t *oid_bytes,
	int oid_len, uint32_t *subids);
static int decode_oid(struct wu_snmp_var_bind *vb)
{
	uint32_t *decoded_oid = NULL;

	vb->oid_len = oid_encoded2subid(vb->name.data.string, vb->name.len,
		vb->oid);
	if (vb->oid_len <= 0) {
		return -1;
	}
	// cut off instance
	if (vb->oid[vb->oid_len - 1] == 0)
		vb->oid_len--;

	return 0;
}
int pop_var_bind(uint8_t **pdata, uint16_t *psize,
	struct wu_snmp_var_bind *vb)
{
	uint8_t *data;
	uint16_t size;
	struct wu_snmp_atom var;
	int rc;

	rc = pop_atom(pdata, psize, &var);
	if (rc < 0)
		return -1;
	data = var.data.string;
	size = var.len;
	snmp_hex_dump("var bind", data, size);
	rc = pop_atom(&data, &size, &vb->name);
	if (rc < 0)
		return -1;
	rc = pop_atom(&data, &size, &vb->value);
	if (rc < 0)
		return -1;

	rc = decode_oid(vb);
	if (rc < 0)
		return -1;

	return 0;
}
static void atom_init(struct wu_snmp_atom *atom,
	uint8_t tag, uint8_t *data, uint16_t size)
{
	atom->tag = tag;
	atom->len = size;
	atom->data.string = data;
}
static void com_atom_init(struct wu_snmp_com_atom *catom, uint8_t tag)
{
	catom->atom.tag = tag;
	catom->atom.len = 0;
	catom->atom.data.string = catom->data_buf;
	catom->atom.data.string[0] = tag;

	catom->max_size = 2048;
	catom->parent_catom = NULL;
	catom->son_catom = NULL;
}
static void com_atom_fix_data(struct wu_snmp_com_atom *catom)
{
	if (catom->atom.len > 0xFF) {
		memcpy(catom->data_buf + 4, catom->data_buf + 2, catom->atom.len);
		catom->data_buf[1] = 0x82;
		catom->data_buf[2] = catom->atom.len >> 8;
		catom->data_buf[3] = catom->atom.len & 0xFF;
		catom->atom.len += 2;
	} else if (catom->atom.len >= 0x80) {
		memcpy(catom->data_buf + 3, catom->data_buf + 2, catom->atom.len);
		catom->data_buf[1] = 0x81;
		catom->data_buf[2] = catom->atom.len;
		catom->atom.len += 1;
	} else {
		catom->data_buf[1] = catom->atom.len;
	}
}
static void com_atom_merge(struct wu_snmp_com_atom *parent,
		struct wu_snmp_com_atom *son)
{
	com_atom_fix_data(son);
	memcpy(parent->data_buf + 2 + parent->atom.len,
		son->data_buf, 2 + son->atom.len);
	parent->atom.len += 2 + son->atom.len;
}
static void com_atom_end(struct wu_snmp_com_atom *catom)
{
	if (catom->son_catom) {
		com_atom_merge(catom, catom->son_catom);
		catom->son_catom = NULL;
	}
	if (catom->parent_catom) {
		com_atom_merge(catom->parent_catom, catom);
		catom->parent_catom->son_catom = NULL;
		catom->parent_catom = NULL;
	} else {
		com_atom_fix_data(catom);
	}
}
static void com_atom_add_atom(struct wu_snmp_com_atom *catom,
	struct wu_snmp_atom *atom)
{
	catom->atom.data.string[2 + catom->atom.len] = atom->tag;
	if (atom->len > 0xFF) {
		catom->atom.data.string[2 + catom->atom.len + 1] = 0x82;
		catom->atom.data.string[2 + catom->atom.len + 2] = atom->len >> 8;
		catom->atom.data.string[2 + catom->atom.len + 3] = atom->len & 0xFF;
		catom->atom.len += 4;
	} else if (atom->len >= 0x80) {
		catom->atom.data.string[2 + catom->atom.len + 1] = 0x81;
		catom->atom.data.string[2 + catom->atom.len + 2] = atom->len;
		catom->atom.len += 3;
	} else {
		catom->atom.data.string[2 + catom->atom.len + 1] = atom->len;
		catom->atom.len += 2;
	}
	if (atom->len > 0) {
		memcpy(catom->atom.data.string + 2 + catom->atom.len,
			atom->data.string, atom->len);
		catom->atom.len += atom->len;
	}
}
static void com_atom_add_atom_data(struct wu_snmp_com_atom *catom,
	uint8_t tag, uint8_t *data, uint16_t size)
{
	struct wu_snmp_atom atom;

	atom_init(&atom, tag, data, size);
	com_atom_add_atom(catom, &atom);
}
static void com_atom_add_com_atom(struct wu_snmp_com_atom *catom,
	struct wu_snmp_com_atom *son_catom, uint8_t tag)
{
	com_atom_init(son_catom, tag);

	catom->son_catom = son_catom;
	son_catom->parent_catom = catom;
}
static void build_error(struct wu_snmp_client *client, int vb_cnt)
{
	int i;
	struct wu_snmp_var_bind *vb;

	/*
	 * build error_status[] and error_index
	 */
	client->errors = 0;
	for (i = 0; i < vb_cnt; i++) {
		vb = &client->variable_bindings[i];
		if (vb->error) {
			client->error_status[client->errors] = vb->error;
			client->error_index[client->errors] = i + 1;
			client->errors++;
		}
	}
	if (!client->errors) {
		client->error_status[0] = 0;
		client->error_index[0] = 0;
		client->errors = 1;
	}
}
static void get_request_process_var_bind(struct wu_snmp_client *clien,
	struct wu_snmp_var_bind *vb)
{
	struct wu_oid_object *obj;
	struct wu_snmp_value v;

	vb->error = noError;

	trace_dbg("vb oid is: %s", oid_str_2(vb->oid, vb->oid_len));
	obj = find_oid_object(vb->oid, vb->oid_len);
	if (!obj) {
		trace_err("no such object: %s", oid_str_2(vb->oid, vb->oid_len));
		vb->error = noSuchName;
		return;
	}

	/* the object's oid should fixed to current one */
	memcpy(obj->oid, vb->oid, sizeof(vb->oid));

	trace_info("%s get", oid_str(obj));
	if (!obj->getter) {
		vb->error = noAccess;
	} else if (obj->getter(obj, &v)) {
		vb->error = genErr;
	} else {
		memcpy(vb->data_buf, v.data, v.size);
		vb->value.tag = TagString;
		vb->value.len = v.size;
		vb->value.data.string = vb->data_buf;
	}
}
struct wu_snmp_com_atom header, method, vblist, vb_catom;
static void get_request_handler(struct wu_snmp_client *client,
	struct wu_snmp_pdu *pdu)
{
	struct wu_snmp_var_bind vb;
	uint8_t *data = pdu->variable_bindings.data.string;
	uint16_t size = pdu->variable_bindings.len;
	int rc;
	int idx = 0, i;

	trace_dbg("get-request: size %#x", size);
	rc = pop_var_bind(&data, &size, &vb);
	while (rc == 0) {
		snmp_hex_dump("var name", vb.name.data.string, vb.name.len);
		if (vb.value.len > 0) {
			snmp_hex_dump("var value", vb.value.data.string, vb.value.len);
		}
		// process var
		get_request_process_var_bind(client, &vb);

		client->variable_bindings[idx++] = vb;
		rc = pop_var_bind(&data, &size, &vb);
	}

	build_error(client, idx);

	/*
	 * build get-response pdu
	 */
	com_atom_init(&header, TagVar);
	com_atom_add_atom(&header, &pdu->version);
	com_atom_add_atom(&header, &pdu->com);
	com_atom_add_com_atom(&header, &method, TagGetResponse);
	com_atom_add_atom(&method, &pdu->request_id);

	com_atom_add_atom_data(&method, TagInt,
		client->error_status, client->errors * sizeof(client->error_status[0]));
	com_atom_add_atom_data(&method, TagInt,
		client->error_index, client->errors * sizeof(client->error_index[0]));

	/* add variable-bindings */
	com_atom_add_com_atom(&method, &vblist, TagVar);
	for (i = 0; i < idx; i++) {
		if (!client->variable_bindings[i].error) {
			com_atom_add_com_atom(&vblist, &vb_catom, TagVar);
			com_atom_add_atom(&vb_catom, &client->variable_bindings[i].name);
			com_atom_add_atom(&vb_catom, &client->variable_bindings[i].value);
			com_atom_end(&vb_catom);
		}
	}
	com_atom_end(&vblist);
	com_atom_end(&method);
	com_atom_end(&header);

	snmp_hex_dump("get-response", header.atom.data.string, header.atom.len + 2);
	client->resp_size = header.atom.len + 2;
	memcpy(client->resp_data, header.data_buf, client->resp_size);
}

static void get_response_handler(struct wu_snmp_client *client,
	struct wu_snmp_pdu *pdu)
{
}
static void set_request_process_var_bind(struct wu_snmp_client *clien,
	struct wu_snmp_var_bind *vb)
{
	struct wu_oid_object *obj;
	struct wu_snmp_value v;
	static wu_oid_t load_oid[] = {XMUX_ROOT_OID, 100};
	int instance = vb->oid[vb->oid_len];

	if (clien->readonly) {
		vb->error = readOnly;
		return;
	}

	vb->error = noError;

	trace_dbg("vb oid is: %s", oid_str_2(vb->oid, vb->oid_len));
	obj = find_oid_object(vb->oid, vb->oid_len);
	if (!obj) {
		trace_err("no such object: %s", oid_str_2(vb->oid, vb->oid_len));
		vb->error = noSuchName;
		return;
	}

	/*
	 * error if current in front panel mode or not login
	 */
	if ((sg_mib_heartDevice.flag != SNMP_LOGIN_STATUS_SUCCESS) &&
		!oid_is(obj, load_oid, 7)) {
		if (sg_mib_heartDevice.flag != SNMP_LOGIN_STATUS_SUCCESS) {
			trace_err("not login!");
		} else {
			trace_err("busy!");
		}
		vb->error = resourceUnavailable;
		return;
	}
	xmux_snmp_update_heart_device_time();

	/* the object's oid should fixed to current one */
	memcpy(obj->oid, vb->oid, sizeof(vb->oid));

	v.data = vb->value.data.string;
	v.size = vb->value.len;
	trace_info("%s.%d,  set, data %d bytes", oid_str(obj), instance, v.size);
	hex_dump("snmp set", v.data, MIN(v.size, 48));
	if (!obj->setter) {
		vb->error = readOnly;
	} else if (obj->setter(obj, &v)) {
		vb->error = genErr;
	} else {
		vb->value.len = 0;
	}
}
static void set_request_handler(struct wu_snmp_client *client,
	struct wu_snmp_pdu *pdu)
{
	struct wu_snmp_var_bind vb;
	uint8_t *data = pdu->variable_bindings.data.string;
	uint16_t size = pdu->variable_bindings.len;
	int rc;
	int idx = 0, i;
	struct wu_snmp_com_atom header, method, vblist;

	trace_dbg("set-request: size %#x", size);
	rc = pop_var_bind(&data, &size, &vb);
	while (rc == 0) {
		snmp_hex_dump("var name", vb.name.data.string, vb.name.len);
		if (vb.value.len > 0) {
			snmp_hex_dump("var value", vb.value.data.string, vb.value.len);
		}
		// process var
		set_request_process_var_bind(client, &vb);

		client->variable_bindings[idx++] = vb;
		rc = pop_var_bind(&data, &size, &vb);
	}

	build_error(client, idx);

	/*
	 * build set-response pdu
	 */
	com_atom_init(&header, TagVar);
	com_atom_add_atom(&header, &pdu->version);
	com_atom_add_atom(&header, &pdu->com);
	com_atom_add_com_atom(&header, &method, TagGetResponse);
	com_atom_add_atom(&method, &pdu->request_id);

	com_atom_add_atom_data(&method, TagInt,
		client->error_status, client->errors * sizeof(client->error_status[0]));
	com_atom_add_atom_data(&method, TagInt,
		client->error_index, client->errors * sizeof(client->error_index[0]));

	/* add variable-bindings */
	com_atom_add_com_atom(&method, &vblist, TagVar);
	for (i = 0; i < idx; i++) {
		struct wu_snmp_com_atom vb_catom;
		if (1) {
			com_atom_add_com_atom(&vblist, &vb_catom, TagVar);
			com_atom_add_atom(&vb_catom, &client->variable_bindings[i].name);
			com_atom_add_atom(&vb_catom, &client->variable_bindings[i].value);
			com_atom_end(&vb_catom);
		}
	}
	com_atom_end(&vblist);
	com_atom_end(&method);
	com_atom_end(&header);

	snmp_hex_dump("set-response", header.atom.data.string, header.atom.len + 2);
	client->resp_size = header.atom.len + 2;
	memcpy(client->resp_data, header.data_buf, client->resp_size);
}
void process_client_request(struct wu_snmp_client *client)
{
	char com_str[128] = {0};
	uint8_t *data = client->request.data;
	uint16_t size = client->request.size;

	struct wu_snmp_pdu pdu = {
		.header = {TagVar},
		.version = {TagInt},
		.com = {TagString},

		.request_id = {TagInt},
		.error_status = {TagInt},
		.error_index = {TagInt},
		.variable_bindings = {TagVar},
	};

	// snmp header
	extract_fix_atoms(&data, &size, &pdu.header, 1);
	data = pdu.header.data.string;
	size = pdu.header.len;
	extract_fix_atoms(&data, &size, &pdu.version, 1);
	extract_fix_atoms(&data, &size, &pdu.com, 1);
	pop_atom(&data, &size, &pdu.method);
	memcpy(com_str, pdu.com.data.string, pdu.com.len);
	trace_dbg("reqeust snmp: version %#x, com %s, method %#x",
		pdu.version.data.int_v, com_str, pdu.method.tag);

	data = pdu.method.data.string;
	size = pdu.method.len;
	// pdu
	extract_fix_atoms(&data, &size, &pdu.request_id, 4);
	trace_dbg("request-id %#x, error_status %#x, error_index %#x,"
		"variable_bindings %#x bytes",
		pdu.request_id.data.int_v, pdu.error_status.data.int_v,
		pdu.error_index.data.int_v, pdu.variable_bindings.len);

	// variable list
	switch (pdu.method.tag) {
		case TagGetRequest:
			snmp_hex_dump("variable_bindings", pdu.variable_bindings.data.string,
				pdu.variable_bindings.len);
			get_request_handler(client, &pdu);
			break;
		case TagGetResponse:
			get_response_handler(client, &pdu);
			break;
		case TagSetRequest:
			snmp_hex_dump("variable_bindings", pdu.variable_bindings.data.string,
				pdu.variable_bindings.len);
			set_request_handler(client, &pdu);
			break;
		default:
			break;
	}
}

static WuTree *tree;
static WuTreeNode *root;
#define NODE_SIZE	((PROGRAM_MAX_NUM / 32) * 1000)

static int oid_finder (void *node_data, void *data)
{
	if (node_data == data) {
		return 0;
	}

	return -1;
}

static struct udp_context *udp_ctx;
static int agent_sock;
int wu_agent_init(char *name)
{
	udp_ctx = udp_open("127.0.0.1", 161);
	if (!udp_ctx)
		return -1;
	agent_sock = udp_ctx->sock;

	tree = wutree_new(NODE_SIZE);
	root = wutree_node_pool_get_node(tree);
	wutreenode_set_data(root, name);
	wutree_set_root(tree, root);
	wutree_set_finder(tree, oid_finder);

	return 0;
}
#define UDP_PKG_SIZE		2048
void wu_agent_loop(void *data)
{
	bool agent_quit = (bool)data;
	unsigned char buf[UDP_PKG_SIZE];
	int len, rc;
	struct timeval to;
	fd_set read_set;
	struct sockaddr_in addr, login_addr;
	int addr_len = sizeof(struct sockaddr_in);
	struct wu_snmp_client *client;

	client = (struct wu_snmp_client *)malloc(sizeof(*client));

	trace_info("agent running");

	while (!agent_quit) {
		to.tv_sec = 1;
		to.tv_usec = 0;
		FD_ZERO(&read_set);
		FD_SET(agent_sock, &read_set);

		rc = select(agent_sock + 1, &read_set, NULL, NULL, &to);
		if (rc > 0) {
			len = recvfrom(agent_sock, buf, UDP_PKG_SIZE, 0,
				(struct sockaddr *)&addr, (socklen_t *)&addr_len);
			if (len > 0) {
				trace_info("client address %s, port %d",
					inet_ntoa(addr.sin_addr), addr.sin_port);

				/*
				 * if other client logined, make it readonly
				 */
				if (sg_mib_heartDevice.flag == SNMP_LOGIN_STATUS_SUCCESS &&
					addr.sin_addr.s_addr != login_addr.sin_addr.s_addr) {
					trace_warn("manager logined! readonly granted");
					client->readonly = 1;
				} else {
					// this is the candicate manager
					login_addr = addr;
					client->readonly = 0;
				}

				snmp_hex_dump("snmp request data", buf, len);
				client->request.data = buf;
				client->request.size = len;
				process_client_request(client);
				len = sendto(agent_sock, client->resp_data, client->resp_size, 0,
					(struct sockaddr *)&addr, (socklen_t)addr_len);
				trace_dbg("resp size %d, len %d", client->resp_size, len);
			}
		} else if (rc == 0) {
			// timeout
		} else {
			// error
		}
	}

	trace_info("agent quit");
	free(client);
}

int wu_agent_register(struct wu_oid_object *reg_obj)
{
	int i;
	wu_oid_t o;
	WuTreeNode *node = root, *added, *tmp_node;

	trace_dbg("register oid %s", oid_str(reg_obj));
	for (i = 0; i < reg_obj->oid_len; i++) {
		o = reg_obj->oid[i];
		tmp_node = wutree_find_in_sons(tree, node, (void *)o);
		if (tmp_node) {
			node = tmp_node;
			continue;
		}
		added = wutree_node_pool_get_node(tree);
		if (!added) {
			trace_err("wutree node pool empty, expand it!");
			return -1;
		}
		trace_dbg("add node %d under node %d", o, (int)node->data);
		added->data = (void *)o;
		wutreenode_add_littleson(node, added);
		node = added;
	}

	if (added) {
		added->ext_data = reg_obj;
	}


	return 0;
}
static WuTreeNode * find_node(wu_oid_t *oid, int oid_len)
{
	WuTreeNode *node = root;
	wu_oid_t o;
	int i;

	if (oid_len <= 0) {
		trace_err("oid length error %d", oid_len);
		return NULL;
	}

	for (i = 0; i < oid_len; i++) {
		o = oid[i];
		node = wutree_find_in_sons(tree, node, (void *)o);
		if (!node) {
			return NULL;
		}
	}

	if (node->eldestson) {
		return NULL;
	}

	return node;
}
static struct wu_oid_object * find_oid_object(wu_oid_t *oid, int oid_len)
{
	WuTreeNode *node;

	node = find_node(oid, oid_len);
	if (!node) {
		return NULL;
	}
	return (struct wu_oid_object *)node->ext_data;
}

