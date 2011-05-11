#include "wu/wu_base_type.h"
#include "wu/message.h"
#include "wu/wu_udp.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>



static msgobj mo = {MSG_INFO, ENCOLOR, "snmp"};


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
	TagResponse = 0xa3, // FIXME
};

union snmp_value {
	uint32_t int_v;
	uint8_t *string;
	uint8_t *var;
};
struct wu_snmp_atom {
	uint8_t tag;
	uint16_t len;
	union snmp_value data;
};

struct wu_snmp_pdu {
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
};

struct wu_snmp_request {
	uint8_t *data;
	uint16_t size;
};

struct wu_snmp_var_bind {
	uint8_t error;

	struct wu_snmp_atom name;
	struct wu_snmp_atom value;
};

struct wu_snmp_client {
	struct wu_snmp_request request;

	struct wu_snmp_var_bind variable_bindings[16];
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
			memcpy(&atom->data.int_v, data, len);
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
		default:
			trace_warn("unimplement tag %#x", atom->tag);
			break;
	}
	atom->len = len;
	trace_info("atom tag %#x, len %#x", atom->tag, atom->len);
}
static int extract_fix_atoms(uint8_t **pdata, uint16_t *psize,
	struct wu_snmp_atom *atoms, int atom_cnt)
{
	int atom_idx = 0;
	uint8_t *data = *pdata;
	uint16_t size = *psize;
	uint8_t *data_end = data + size;
	uint8_t tag, len;

	while (data < data_end && atom_idx < atom_cnt) {
		tag = *data++;
		len = *data++;
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
		trace_info("pop atom: data size 0, no more atom");
		return -1;
	} else if (size < 2) {
		trace_err("pop atom: data size %#x too small!", size);
		return -1;
	}
	atom->tag = *data++;
	atom->len = *data++;
	if (size < 2 + atom->len) {
		trace_err("pop atom: expect data size %#x, but real %#x!",
			2 + atom->len, size);
		hex_dump("raw data", *pdata, *psize);
		return -1;
	}

	atom_set_data(atom, data, atom->len);

	*pdata = data + atom->len;
	*psize -= 2 + atom->len;

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
	hex_dump("var bind", data, size);
	rc = pop_atom(&data, &size, &vb->name);
	if (rc < 0)
		return -1;
	rc = pop_atom(&data, &size, &vb->value);
	if (rc < 0)
		return -1;

	return 0;
}
static void get_request_handler(struct wu_snmp_client *client,
	struct wu_snmp_pdu *pdu)
{
	struct wu_snmp_var_bind vb;
	uint8_t *data = pdu->variable_bindings.data.string;
	uint16_t size = pdu->variable_bindings.len;
	int rc;
	int idx = 0;

	trace_info("get-request: size %#x", size);
	rc = pop_var_bind(&data, &size, &vb);
	while (rc == 0) {
		hex_dump("var name", vb.name.data.string, vb.name.len);
		if (vb.value.len > 0) {
			hex_dump("var value", vb.value.data.string, vb.value.len);
		}

		client->variable_bindings[idx++] = vb;
		rc = pop_var_bind(&data, &size, &vb);
	}
}
static void get_response_handler(struct wu_snmp_client *client,
	struct wu_snmp_pdu *pdu)
{
}
void process_client_request(struct wu_snmp_client *client)
{
	struct wu_snmp_atom header = {TagVar};
	struct wu_snmp_atom version = {TagInt};
	struct wu_snmp_atom com = {TagString};
	char com_str[128] = {0};
	struct wu_snmp_atom method;
	struct wu_snmp_pdu pdu = {
		.request_id = {TagInt},
		.error_status = {TagInt},
		.error_index = {TagInt},
		.variable_bindings = {TagVar}
	};
	uint8_t *data = client->request.data;
	uint16_t size = client->request.size;
	
	// snmp header
	extract_fix_atoms(&data, &size, &header, 1);
	data = header.data.string;
	size = header.len;
	extract_fix_atoms(&data, &size, &version, 1);
	extract_fix_atoms(&data, &size, &com, 1);
	pop_atom(&data, &size, &method);
	memcpy(com_str, com.data.string, com.len);
	trace_info("reqeust snmp: version %#x, com %s, method %#x",
		version.data.int_v, com_str, method.tag);

	data = method.data.string;
	size = method.len;
	// pdu
	extract_fix_atoms(&data, &size, &pdu.request_id, 4);
	trace_info("request-id %#x, error_status %#x, error_index %#x,"
		"variable_bindings %#x bytes",
		pdu.request_id.data.int_v, pdu.error_status.data.int_v,
		pdu.error_index.data.int_v, pdu.variable_bindings.len);

	// variable list
	switch (method.tag) {
		case TagGetRequest:
			hex_dump("variable_bindings", pdu.variable_bindings.data.string,
				pdu.variable_bindings.len);
			get_request_handler(client, &pdu);
			break;
		case TagGetResponse:
			get_response_handler(client, &pdu);
			break;
		default:
			break;
	}
}

static struct udp_context *udp_ctx;
static int agent_sock;
int wu_snmp_agent_init(const char name)
{
	udp_ctx = udp_open("127.0.0.1", 161);
	if (!udp_ctx)
		return -1;
	agent_sock = udp_ctx->sock;

	return 0;
}
static int agent_quit;
#define UDP_PKG_SIZE		2048
void wu_snmp_agent_loop(void *data)
{
	unsigned char buf[UDP_PKG_SIZE];
	int len, rc;
	struct timeval to;
	fd_set read_set;
	struct sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);
	struct wu_snmp_client client;

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
				hex_dump("snmp request data", buf, len);
				client.request.data = buf;
				client.request.size = len;
				process_client_request(&client);
			}
		} else if (rc == 0) {
			// timeout
		} else {
			// error
		}
	}

	trace_info("agent quit");
}

