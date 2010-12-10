#include "wu/wu_base_type.h"
#include "wu/thread.h"
#include "wu/wu_errno.h"
#include "wu/message_queue.h"
#include "psi_worker.h"
#include "psi_parse.h"


enum {
	PSI_WORKER_MB_TYPE_BASE = 0x1000,

	PSI_WORKER_MB_TYPE_PARSE_PSI,
};

static Thread *psi_worker_thr;
static MessageQueue *psi_worker_mq;

int psi_worker_open()
{
	psi_worker_mq = message_queue_create();

	return 0;
}

int psi_worker_close()
{
	if (psi_worker_thr) {
		MessageBlock *mb = message_block_new(NULL);
		mb->type = MB_TYPE_QUIT;
		message_queue_put_message_block(psi_worker_mq, mb);
		thread_join(psi_worker_thr);
		psi_worker_thr = NULL;
	}
	if (psi_worker_mq) {
		message_queue_destroy(psi_worker_mq);
		psi_worker_mq = NULL;
	}

	return 0;
}

static int psi_worker_thread(void *data)
{
	MessageBlock *mb;
	bool psi_worker_thread_quit = false;

	while (!psi_worker_thread_quit) {
		mb = message_queue_get_message_block(psi_worker_mq, MQ_WAIT_FOREVER);
		switch (mb->type) {
			case PSI_WORKER_MB_TYPE_PARSE_PSI:
				psi_parsing();
				break;
			case MB_TYPE_QUIT:
				psi_worker_thread_quit = true;
				break;
			default:
				break;
		}

		message_block_release(mb);
	}

	return 0;
}

int psi_worker_run()
{
	psi_worker_thr = thread_create(psi_worker_thread, NULL);
	if (!psi_worker_thr) {
		return -1;
	}

	return 0;
}

/*
 * request psi worker to do psi parsing
 * return error if it's busy or it's not the time to performance parseing.
 */
int psi_worker_request_parse_psi(int chan_mask, void (*parse_done)(void *))
{
	MessageBlock *mb;

	mb = message_block_new(NULL);
	if (!mb) {
		return -ENOMEM;
	}
	mb->type = PSI_WORKER_MB_TYPE_PARSE_PSI;
	message_queue_put_message_block(psi_worker_mq, mb);

	return 0;
}

/*
 * request psi worker to download psi data
 */
int psi_worker_request_download_psi(int chid, void *psi_data, int len)
{
	return 0;
}

void psi_worker_stop_parse_psi()
{
}

/*
 * read pid map
 */
int read_pid_map(void *buf)
{
	return -1;
}

/*
 * write pid map
 */
int write_pid_map(void *pid_map_data, int size)
{
	return 0;
}

