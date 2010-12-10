#ifndef _PSI_WORKER_H_
#define _PSI_WORKER_H_


int psi_worker_open();
int psi_worker_close();
int psi_worker_run();

int psi_worker_request_parse_psi(int chan_mask, void (*parse_done)(void *),
		int request_manager);
int psi_worker_request_download_psi(int chid, void *psi_data, int len);

void psi_worker_stop_parse_psi();

int read_pid_map(void *buf);
int write_pid_map(void *pid_map_data, int size);


#endif /* _PSI_WORKER_H_ */

