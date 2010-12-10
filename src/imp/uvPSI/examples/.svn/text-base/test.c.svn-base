#include "gen_dvb_si.h"

#define Test_Gen_NIT_main main

int main()
{
	uv_nit_data nit_data;
	uv_nit_stream_data nit_stream_data[10];
	uint16_t stream_num = 1;
	uv_descriptor *p_descr;
	uint16_t nit_descr_num = 0;
	uv_descriptor *p_stream_descr;
	int stream_descr_num = 1;
	
	int i, j;
	uint8_t buf[256];
	uint8_t buf_stream[256];

	nit_data.i_table_id = 0x40;
	nit_data.i_net_id = 0x1234;
	nit_data.i_descr_num = nit_descr_num;
	//printf(" ^ ");

	nit_data.p_descr = (uv_descriptor *)malloc(nit_data.i_descr_num * sizeof(uv_descriptor));
 	for(i = 0; i < nit_descr_num; i++)
       {
       	nit_data.p_descr[i].i_tag = 0x43;
		nit_data.p_descr[i].i_length = 11;
		nit_data.p_descr[i].p_data = buf;	
		
 	}
       for(i = 0; i < stream_num; i++)
       {	
		nit_stream_data[i].i_tran_stream_id =  i+1;
		nit_stream_data[i].i_orig_net_id = 0x123;
		nit_stream_data[i].i_descr_num = stream_descr_num;
		nit_stream_data[i].p_descr = (uv_descriptor *)malloc(nit_stream_data[i].i_descr_num * sizeof(uv_descriptor));
		p_stream_descr = nit_stream_data[i].p_descr;
		
		for(j = 0; j < nit_stream_data[i].i_descr_num; j++)
		{
			p_stream_descr[j].i_tag = 0x4A;
			p_stream_descr[j].i_length = 15;
			p_stream_descr[j].p_data = buf_stream;
			buf_stream[0] = 0x00;
			buf_stream[1] = 0x02;
			buf_stream[2] = 0x01;
			buf_stream[3] = 0x23; 
			buf_stream[4] = 0x02;
		    memcpy(buf_stream+5, "helloworld", 10);
		}

	}
  
	dvbSI_Gen_NIT(&nit_data, nit_stream_data, stream_num);

	return 0;
	
}


int Test_Gen_SDT_main(void)
{
	uv_sdt_data sdt_data;
       uv_sdt_serv_data sdt_serv_data[10];
       uint16_t serv_num = 2;
       int i, j;
	uv_descriptor *p_descr;
	int num = 2;
	uint8_t buf[256];
	
	//printf("Starting packing SDT ts....\n");

	sdt_data.i_table_id = 0x42;
       sdt_data.i_tran_stream_id = 0x1234;
	sdt_data.i_orig_net_id = 0xEAEA;

       for(i = 0; i < serv_num; i++)
       {
		sdt_serv_data[i].i_serv_id = i+1;
		sdt_serv_data[i].i_eit_pres_foll_flag = 0;
		sdt_serv_data[i].i_eit_sched_flag = 0;
		sdt_serv_data[i].i_free_ca_mode = 0;
		sdt_serv_data[i].i_running_status = 1;
		sdt_serv_data[i].i_descr_num = num;
		sdt_serv_data[i].p_descr = (uv_descriptor *)malloc(sdt_serv_data[i].i_descr_num * sizeof(uv_descriptor));
		p_descr = sdt_serv_data[i].p_descr;
		for(j = 0; j < num; j++)
		{
			p_descr[j].i_tag = 0x48;
			p_descr[j].i_length = 14;
			p_descr[j].p_data = buf;
			buf[0] = 0x01; // serivece type
			buf[1] = 6; // 
			memcpy(buf+2, "CCTV-1", 6);
			buf[8] = 5;
			memcpy(buf+9, "human", 5);
		}
	}

	dvbSI_Gen_SDT(&sdt_data, sdt_serv_data, serv_num);


	//printf("End of packing");

	//pritnf("Starting depacking SDT ts...\n");
	//printf("End of depacking");

	return 0;
}
