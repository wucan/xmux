#ifndef _FAKE_HFPGA_H_
#define _FAKE_HFPGA_H_


int fake_hfpga_init(const char *input_ts_file);

int fake_hfpga_open(const char *name, int flags);
void fake_hfpga_close(int hdev);
int fake_hfpga_readn(unsigned char *p_buf, unsigned int len,
		void *p_param, int nbgn);
int fake_hfpga_writen(unsigned char *p_buf, unsigned int len, void *p_param);
int fake_hfpga_ioctl(unsigned int cmd, void *p_param);


#endif /* _FAKE_HFPGA_H_ */

