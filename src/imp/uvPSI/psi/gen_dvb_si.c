/*##########################################################################
## gen_dvb_si.c
## grepo@163.com
## 2010-07-06
## Implementation of the interfaces for generating and decoding SI
##########################################################################*/


#include "config.h"

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <fcntl.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

/* The libdvbpsi distribution defines DVBPSI_DIST */
#include "gen_dvb_si.h"



/*---------------------------------------------------------------------------------------------*/
//	Uttilities 
/*---------------------------------------------------------------------------------------------*/

/*
typedef struct _dvb_io{
	fcn_open open;	
	fcn_close close;
	fcn_read read;	
	fcn_write write;
	char devname[64];	
	int flags;
	int hdev;
	unsigned char cha;
} uv_dvb_io;
*/
int dvbSI_Start(uv_dvb_io *io)
{        
	dvb_io_dev.open = io->open;
	dvb_io_dev.close = io->close;
	dvb_io_dev.read = io->read;
	dvb_io_dev.write = io->write;
	dvb_io_dev.ioctl = io->ioctl;
	strcpy(dvb_io_dev.devname, io->devname);
	dvb_io_dev.flags = io->flags;
	dvb_io_dev.cha = io->cha;
	//memcpy(&dvb_io_dev, io, sizeof(uv_dvb_io));
	dvb_io_dev.hdev = dvb_io_dev.open(dvb_io_dev.devname, dvb_io_dev.flags);
	if(dvb_io_dev.hdev < 0)
	{
		dbg_prt("[SI MODULE]: dvbSI_Start()->open dev %s failes!", dvb_io_dev.devname);
		return -1; 
	}
	io->hdev = dvb_io_dev.hdev;
	return 0;
}

int dvbSI_ChangCha(unsigned char cha)
{
	int old_cha = dvb_io_dev.cha;
	dvb_io_dev.cha = cha;
	return old_cha; 
}

void dvbSI_Stop()
{
	dvb_io_dev.close(dvb_io_dev.hdev);
}

// howto is parameter for sending period, such as 400ms(howto=0), 2s(howto=1) and so on.
static void dvb_Send_SI_Pack(uint8_t howto, uint8_t* p_packet, dvbpsi_psi_section_t* p_section);

void dvb_Dump_PAT(void* p_zero, dvbpsi_pat_t* p_pat);

void dvb_Dump_PMT(void* p_zero, dvbpsi_pmt_t* p_pmt);

void dvb_Dump_SDT(void* p_zero, dvbpsi_sdt_t* p_sdt);

void dvb_Dump_NIT(void* p_zero, dvbpsi_nit_t* p_nit);

void dvb_Dump_EIT(void* p_zero, dvbpsi_eit_t* p_eit);

void dvb_Dump_CAT(void* p_zero, dvbpsi_cat_t* p_cat);

void dvb_New_Subtable(void * p_zero, dvbpsi_handle h_dvbpsi,
                 uint8_t i_table_id, uint16_t i_extension);
                 
static int ReadPacket(int i_fd, uint8_t* p_dst);

/*---------------------------------------------------------------------------------------------*/
//	 
/*---------------------------------------------------------------------------------------------*/
int dvbSI_GenSS(unsigned int ss)
{
	dvb_io_dev.ioctl(ss, NULL);
	return 0;
}

static uint8_t sg_share_buf[4096];
/*---------------------------------------------------------------------------------------------*/
//	 
/*---------------------------------------------------------------------------------------------*/

int dvbSI_Gen_PAT(uv_pat_data *p_pat_data, uv_pat_pid_data *p_pid_data, uint16_t i_pid_num)
{
	int ret = 0;
	uint8_t packet[188];
	
	dvbpsi_pat_t pat;
	dvbpsi_psi_section_t* p_section;
	int i;

	/* PAT generation */
	dvbpsi_InitPAT(&pat, p_pat_data->i_tran_stream_id, 0, 0);

	for(i = 0; i < i_pid_num; i++)
	{
		dvbpsi_PATAddProgram(&pat, p_pid_data[i].i_pg_num, p_pid_data[i].i_pid);
	}

	// each section max prog number is 64.
	p_section = dvbpsi_GenPATSections(&pat, 64);

	/* TS packets generation */
	packet[0] = 0x47;
	packet[1] = packet[2] = 0x00;
	packet[3] = 0x00;

	dvb_Send_SI_Pack(0, packet, p_section);

	dvbpsi_DeletePSISections(p_section);

	dvbpsi_EmptyPAT(&pat);
  
	return ret;
}

void outputHex(unsigned char *data, unsigned int len, unsigned char width)
{
	int i, col;
	
	for(i = 0; i < len;)
	{
		for(col = 0; col < width && i < len; col++)
		{
			dbg_prt("%02x", data[i++]);
		}
		dbg_prt("\n");
	}
}

int dvbSI_Dec_PAT(uv_pat_data *p_pat_data, uv_pat_pid_data *p_pid_data, uint16_t *p_pid_num)
{
	FILE* fp;
	int ret = 0;
	int i=0;
	
	//int i_fd;
	dvbpsi_handle h_dvbpsi;
	int b_ok;
	
	uv_io_param param;
	param.pid = 0x0;
	param.cha = dvb_io_dev.cha;

	//i_fd = open("pat.ts", 0);
	sg_si_param.pcnt = 3;
	sg_si_param.p1 = p_pat_data;
       sg_si_param.p2 = p_pid_data;
	sg_si_param.p3 = p_pid_num;	
	h_dvbpsi = dvbpsi_AttachPAT(dvb_Dump_PAT, &sg_si_param);
	//b_ok = ReadPacket(i_fd, sg_share_buf);

	if( EUV_DEC_FROM_SECTION != sg_si_param.type )
	{
		param.type = 0;
	}
	else
	{
		param.type = 1;
	}

	fp=fopen("./PAT_hubei.ts","wb"); 
	
	b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,1);
	if(188 != fwrite(sg_share_buf, 1, 188, fp))
	{
		dbg_prt("err_write\n");
	}
	dbg_prt("pat_write_%d\n",i);
	//fwrite();
	//outputHex(sg_share_buf, 188, 16);
 	 //dbg_prt("---%---\n");
	while(b_ok)
	{
		uint16_t i_pid = ((uint16_t)(sg_share_buf[1] & 0x1f) << 8) + sg_share_buf[2];
		//dbg_prt("%--- pid: 02x ---\n", i_pid);
		if(i_pid == 0x0)
		{
			if( EUV_DEC_FROM_SECTION != sg_si_param.type )
			{
				dbg_prt("--get a packet--\n");
				if(1!= dvbpsi_PushPacket(h_dvbpsi, sg_share_buf))
				{
					break;
				}
			}
			else
			{
				if(1!= dvbpsi_PushSection(h_dvbpsi, sg_share_buf, param.len))
				{
					break;
				}
			}	
			//outputHex(sg_share_buf, 188, 16);
		}
		//b_ok = ReadPacket(i_fd, sg_share_buf);
		b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,0);
		if(188 != fwrite(sg_share_buf, 1, 188, fp))
		{
			dbg_prt("err_write_02\n");
		}
		i++;
		dbg_prt("pat_write_%d\n",i);

	}
	fclose(fp);

	dvbpsi_DetachPAT(h_dvbpsi);
	
	return ret;
}

int dvbSI_Gen_PMT(uv_pmt_data *p_pmt_data, uv_pmt_es_data *p_pmt_es_data, uint16_t i_es_num)
{
	int ret = 0;
	uint8_t packet[188];
	dvbpsi_pmt_t pmt;
	dvbpsi_pmt_es_t* p_es;
	dvbpsi_psi_section_t* p_section;

	int i, j;
	/* PMT generation */
	dvbpsi_InitPMT(&pmt, p_pmt_data->i_pg_num, 0, 0, p_pmt_data->i_pcr_pid);

	for(i = 0; i < p_pmt_data->i_descr_num; i++)
	{
		dvbpsi_PMTAddDescriptor(&pmt, p_pmt_data->p_descr[i].i_tag, 
			p_pmt_data->p_descr[i].i_length, p_pmt_data->p_descr[i].p_data);
	}
	
	for(i = 0; i < i_es_num; i++)
	{
		p_es = dvbpsi_PMTAddES(&pmt, p_pmt_es_data[i].i_type, p_pmt_es_data[i].i_pid);
		if(NULL == p_es)
		{
			   DVBPSI_ERROR("PMT Gen", "  dvbSI_Gen_PMT()->dvbpsi_PMTAddES()  error returns! ");
			   dvbpsi_EmptyPMT(&pmt);
			   return -1;
		}
		for(j = 0; j < p_pmt_es_data[i].i_descr_num; j++)
		{
			dvbpsi_PMTESAddDescriptor(p_es, p_pmt_es_data[i].p_descr[j].i_tag, p_pmt_es_data[i].p_descr[j].i_length, p_pmt_es_data[i].p_descr[j].p_data);
		}
	}

	p_section = dvbpsi_GenPMTSections(&pmt);
		
	/* TS packets generation */
	packet[0] = 0x47;
	packet[1] = p_pmt_data->i_pmt_pid>>8;
	packet[2] = p_pmt_data->i_pmt_pid&0x0FF;
	packet[3] = 0x00;

	dvb_Send_SI_Pack(0, packet, p_section);

	dvbpsi_DeletePSISections(p_section);
	
	dvbpsi_EmptyPMT(&pmt);
  
	return ret;
}

int dvbSI_Dec_PMT(uv_pmt_data *p_pmt_data, uv_pmt_es_data *p_pmt_es_data, uint16_t *p_es_num)
{
	int ret = 0;
	int i=0;
	FILE* fp;
	dvbpsi_handle h_dvbpsi;
	int b_ok;
	uint16_t i_program_number, i_pmt_pid;

	uv_io_param param;
	param.cha = dvb_io_dev.cha;

	//i_fd = open("pmt.ts", 0);
	
	i_program_number = p_pmt_data->i_pg_num;
	i_pmt_pid = p_pmt_data->i_pmt_pid;
	
	sg_si_param.pcnt = 3;
	sg_si_param.p1 = p_pmt_data;
       sg_si_param.p2 = p_pmt_es_data;
	sg_si_param.p3 = p_es_num;	
	h_dvbpsi = dvbpsi_AttachPMT(i_program_number, dvb_Dump_PMT, &sg_si_param);

	//b_ok = ReadPacket(i_fd, sg_share_buf);

	param.pid = i_pmt_pid;
	if( EUV_DEC_FROM_SECTION != sg_si_param.type )
	{
		param.type = 0;
	}
	else
	{
		param.type = 1;
	}


	fp=fopen("./PMT_test.ts","a");
	if(fp==NULL)	
		printf("err_fopen");
	else printf("ok_fopen");

	//return 0;
   	

	b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,1);
#if 1
	if(b_ok)
	{
		if(188 != fwrite(sg_share_buf, 1, 188, fp))
		{
			dbg_prt("err_write\n");
		}
		dbg_prt("pmt_write_%d\n",i);
	}

	printf("b_ok=%d\n",b_ok);
#endif
	while(b_ok)
	{
#if 1
		uint16_t i_pid = ((uint16_t)(sg_share_buf[1] & 0x1f) << 8) + sg_share_buf[2];
		if(i_pid == i_pmt_pid)
		{
		 	if( EUV_DEC_FROM_SECTION != sg_si_param.type )
			{
				if(1!= dvbpsi_PushPacket(h_dvbpsi, sg_share_buf))
				{
					break;
				}
			}
			else
			{
				if(1!= dvbpsi_PushSection(h_dvbpsi, sg_share_buf, param.len))
				{
					break;
				}
			}	
		}
		//b_ok = ReadPacket(i_fd, data);
#endif
		b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,0);
		if(b_ok)
		{
			i++;
			if(188 != fwrite(sg_share_buf, 1, 188, fp))
			{
				dbg_prt("err_write\n");
			}
			dbg_prt("pmt_write_%d\n",i);
		}
		//break;
	}
	fclose(fp);

	dvbpsi_DetachPMT(h_dvbpsi);
	
	
	printf("\n -- end dec pmt -- \n");
	return ret;
}

int dvbSI_Gen_SDT(uv_sdt_data *p_sdt_data, uv_sdt_serv_data *p_sdt_serv_data, uint16_t i_serv_num)
{
	int ret = 0;
	
	uint8_t packet[188];
	dvbpsi_sdt_t sdt;
	dvbpsi_sdt_service_t* p_serv;
	dvbpsi_psi_section_t* p_section;

	int i, j;
	
	/* SDT generation */
	dvbpsi_InitSDT(&sdt, p_sdt_data->i_tran_stream_id, 0, 0, p_sdt_data->i_orig_net_id);
	
	for(i = 0; i < i_serv_num; i++)
	{
		p_serv = dvbpsi_SDTAddService(&sdt, p_sdt_serv_data[i].i_serv_id,
			p_sdt_serv_data[i].i_eit_sched_flag,  p_sdt_serv_data[i].i_eit_pres_foll_flag,  
			p_sdt_serv_data[i].i_running_status,  p_sdt_serv_data[i].i_free_ca_mode);
		if(NULL == p_serv)
		{
			   DVBPSI_ERROR("SDT Gen", "  dvbSI_Gen_SDT()->dvbpsi_SDTAddService()  error returns! ");
			   dvbpsi_EmptySDT(&sdt);
			   return -1;
		}
		for(j = 0; j < p_sdt_serv_data[i].i_descr_num; j++)
		{
			dvbpsi_SDTServiceAddDescriptor(p_serv, p_sdt_serv_data[i].p_descr[j].i_tag, p_sdt_serv_data[i].p_descr[j].i_length, p_sdt_serv_data[i].p_descr[j].p_data);
		}
	}

	p_section = dvbpsi_GenSDTSections(&sdt);
		
	/* TS packets generation */
	packet[0] = 0x47;
	packet[1] = 0x00;
	packet[2] = 0x11;
	packet[3] = 0x00;

	dvb_Send_SI_Pack(1, packet, p_section);

	dvbpsi_DeletePSISections(p_section);
	
	dvbpsi_EmptySDT(&sdt);
	
	return ret;
}

int dvbSI_Dec_SDT(uv_sdt_data *p_sdt_data, uv_sdt_serv_data *p_sdt_serv_data, uint16_t *p_serv_num)
{
	int ret = 0;
	//int i_fd;
	dvbpsi_handle h_dvbpsi;
	int b_ok;

	uv_io_param param;
	param.cha = dvb_io_dev.cha;
	param.pid = 0x11;
	if( EUV_DEC_FROM_SECTION != sg_si_param.type )
	{
		param.type = 0;
	}
	else
	{
		param.type = 1;
	}
	
	//i_fd = open("sdt.ts", 0);

	sg_si_param.pcnt = 3;
	sg_si_param.p1 = p_sdt_data;
       sg_si_param.p2 = p_sdt_serv_data;
	sg_si_param.p3 = p_serv_num;	
	h_dvbpsi = dvbpsi_AttachDemux(dvb_New_Subtable, &sg_si_param);

	//b_ok = ReadPacket(i_fd, sg_share_buf);
	
	b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,1);
	
	while(b_ok)
	{
		uint16_t i_pid = ((uint16_t)(sg_share_buf[1] & 0x1f) << 8) + sg_share_buf[2];
		if(i_pid == 0x11)
		{
			if( EUV_DEC_FROM_SECTION != sg_si_param.type )
			{
				if(1!= dvbpsi_PushPacket(h_dvbpsi, sg_share_buf))
				{
					break;
				}
			}
			else
			{
				if(1!= dvbpsi_PushSection(h_dvbpsi, sg_share_buf, param.len))
				{
					break;
				}
			}	
		}
		//b_ok = ReadPacket(i_fd, sg_share_buf);
		b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,0);
	}

	dvbpsi_DetachDemux(h_dvbpsi);

	return ret;
}

int dvbSI_Gen_CAT(uv_descriptor *p_descr, uint16_t i_descr_num)
{
	int ret = 0;
	
	uint8_t packet[188];
	dvbpsi_cat_t cat;
	dvbpsi_psi_section_t* p_section;

	int i, j;
	
	/* CAT generation */
	dvbpsi_InitCAT(&cat, 0, 1);
	
	for(i = 0; i < i_descr_num; i++)
	{
		dvbpsi_CATAddDescriptor(&cat, p_descr[i].i_tag, p_descr[i].i_length, p_descr[i].p_data);
	}

	p_section = dvbpsi_GenCATSections(&cat);
		
	/* TS packets generation */
	packet[0] = 0x47;
	packet[1] = 0x00;
	packet[2] = 0x01;
	packet[3] = 0x00;

	dvb_Send_SI_Pack(0, packet, p_section);

	dvbpsi_DeletePSISections(p_section);
	
	dvbpsi_EmptyCAT(&cat);
  
	
	return ret;
}

int dvbSI_Dec_CAT(uv_descriptor *p_descr, uint16_t *p_descr_num)
{
	int ret = 0;
	//int i_fd;
	dvbpsi_handle h_dvbpsi;
	int b_ok;

	uv_io_param param;
	param.cha = dvb_io_dev.cha;
	param.pid = 0x0001;
	if( EUV_DEC_FROM_SECTION != sg_si_param.type )
	{
		param.type = 0;
	}
	else
	{
		param.type = 1;
	}
	
	//i_fd = open("sdt.ts", 0);

	sg_si_param.pcnt = 2;
	sg_si_param.p1 = p_descr;
       sg_si_param.p2 = p_descr_num;	
	h_dvbpsi = dvbpsi_AttachCAT(dvb_Dump_CAT, &sg_si_param);

	//b_ok = ReadPacket(i_fd, data);
	b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,1);
	
	while(b_ok)
	{
		uint16_t i_pid = ((uint16_t)(sg_share_buf[1] & 0x1f) << 8) + sg_share_buf[2];
		if(i_pid == 0x0001)
		{
			if( EUV_DEC_FROM_SECTION != sg_si_param.type )
			{
				if(1!= dvbpsi_PushPacket(h_dvbpsi, sg_share_buf))
				{
					break;
				}
			}
			else
			{
				if(1!= dvbpsi_PushSection(h_dvbpsi, sg_share_buf, param.len))
				{
					break;
				}
			}	
		}
		//b_ok = ReadPacket(i_fd, sg_share_buf);
		b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,0);
	}

	dvbpsi_DetachCAT(h_dvbpsi);
	
	return ret;
}

int dvbSI_Gen_NIT(uv_nit_data *p_nit_data, uv_nit_stream_data *p_stream_data, uint16_t i_stream_num)
{
	int ret = 0;
	uint8_t packet[188];
	dvbpsi_nit_t nit;
	dvbpsi_nit_stream_t* p_stream;
	dvbpsi_psi_section_t* p_section;

	int i, j;

	/* NIT generation */
	dvbpsi_InitNIT(&nit, p_nit_data->i_net_id, 0, 0);

	for(i = 0; i < p_nit_data->i_descr_num; i++)
	{
		dvbpsi_NITAddDescriptor(&nit, p_nit_data->p_descr[i].i_tag, 
			p_nit_data->p_descr[i].i_length, p_nit_data->p_descr[i].p_data);
	}
	
	for(i = 0; i < i_stream_num; i++)
	{
		p_stream = dvbpsi_NITAddStream(&nit, p_stream_data[i].i_tran_stream_id, p_stream_data[i].i_orig_net_id);
		if(NULL == p_stream)
		{
			   DVBPSI_ERROR("NIT Gen", "  dvbSI_Gen_NIT()->dvbpsi_NITAddStream()  error returns! ");
			   dvbpsi_EmptyNIT(&nit);
			   return -1;
		}
		for(j = 0; j < p_stream_data[i].i_descr_num; j++)
		{
			dvbpsi_NITStreamAddDescriptor(p_stream, p_stream_data[i].p_descr[j].i_tag, p_stream_data[i].p_descr[j].i_length, p_stream_data[i].p_descr[j].p_data);
		}
	}

	p_section = dvbpsi_GenNITSections(&nit);
		
	/* TS packets generation */
	packet[0] = 0x47;
	packet[1] = 0x00;
	packet[2] = 0x10;
	packet[3] = 0x00;

	dvb_Send_SI_Pack(1, packet, p_section);

	dvbpsi_DeletePSISections(p_section);
	
	dvbpsi_EmptyNIT(&nit);
	
	return ret;
}

int dvbSI_Dec_NIT(uv_nit_data *p_nit_data, uv_nit_stream_data *p_stream_data, uint16_t *p_stream_num)
{
	int ret = 0;
	//int i_fd;
	dvbpsi_handle h_dvbpsi;
	int b_ok;

	uv_io_param param;
	param.cha = dvb_io_dev.cha;
       param.pid = 0x10;
	if( EUV_DEC_FROM_SECTION != sg_si_param.type )
	{
		param.type = 0;
	}
	else
	{
		param.type = 1;
	}
	
	//i_fd = open("pmt.ts", 0);
	
	sg_si_param.pcnt = 3;
	sg_si_param.p1 = p_nit_data;
       sg_si_param.p2 = p_stream_data;
	sg_si_param.p3 = p_stream_num;	
       // ***********************************  input i_net_id !!!!!!!!!!!!! 
	h_dvbpsi = dvbpsi_AttachNIT(p_nit_data->i_net_id, dvb_Dump_NIT, &sg_si_param);

	//b_ok = ReadPacket(i_fd, sg_share_buf);
	
	b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,1);

	while(b_ok)
	{
		uint16_t i_pid = ((uint16_t)(sg_share_buf[1] & 0x1f) << 8) + sg_share_buf[2];
		if(i_pid == 0x10)
		{
			if( EUV_DEC_FROM_SECTION != sg_si_param.type )
			{
				if(1!= dvbpsi_PushPacket(h_dvbpsi, sg_share_buf))
				{
					break;
				}
			}
			else
			{
				if(1!= dvbpsi_PushSection(h_dvbpsi, sg_share_buf, param.len))
				{
					break;
				}
			}	
		}
		//b_ok = ReadPacket(i_fd, sg_share_buf);
		b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,0);
	}

	dvbpsi_DetachNIT(h_dvbpsi);
	
	return ret;
}

int dvbSI_Gen_EIT(uv_eit_data *p_eit_data, uv_eit_event_data *p_event_data,  uint16_t i_event_num)
{
	int ret = 0;

	uint8_t packet[188];
	dvbpsi_eit_t eit;
	dvbpsi_eit_event_t* p_event;
	dvbpsi_psi_section_t* p_section;

	int i, j;

	/* EIT generation */
	dvbpsi_InitEIT(&eit, p_eit_data->i_serv_id, 0, 0, p_eit_data->i_tran_stream_id, p_eit_data->i_orig_net_id);
	
	for(i = 0; i < i_event_num; i++)
	{
		p_event = dvbpsi_EITAddEvent(&eit, p_event_data[i].i_event_id,
		p_event_data[i].i_start_time,  p_event_data[i].i_duration,  
		p_event_data[i].i_running_status,  p_event_data[i].i_free_ca_mode);
		if(NULL == p_event)
		{
			   DVBPSI_ERROR("EIT Gen", "  dvbSI_Gen_EIT()->dvbpsi_EITAddEvent()  error returns! ");
			   dvbpsi_EmptyEIT(&eit);
			   return -1;
		}
		for(j = 0; j < p_event_data[i].i_descr_num; j++)
		{
			dvbpsi_EITEventAddDescriptor(p_event, p_event_data[i].p_descr[j].i_tag, p_event_data[i].p_descr[j].i_length, p_event_data[i].p_descr[j].p_data);
		}
	}

	p_section = dvbpsi_GenEITSections(&eit);
		
	/* TS packets generation */
	packet[0] = 0x47;
	packet[1] = 0x00;
	packet[2] = 0x12;
	packet[3] = 0x00;

	dvb_Send_SI_Pack(1, packet, p_section);

	dvbpsi_DeletePSISections(p_section);
	
	dvbpsi_EmptyEIT(&eit);
	return ret;
}

int dvbSI_Dec_EIT(uv_eit_data *p_eit_data, uv_eit_event_data *p_event_data,  uint16_t *p_event_num)
{
	int ret = 0;
	//int i_fd;
	dvbpsi_handle h_dvbpsi;
	int b_ok;

	uv_io_param param;
	param.cha = dvb_io_dev.cha;
	param.pid = 0x12;
	if( EUV_DEC_FROM_SECTION != sg_si_param.type )
	{
		param.type = 0;
	}
	else
	{
		param.type = 1;
	}
	
	//i_fd = open("sdt.ts", 0);

	sg_si_param.pcnt = 3;
	sg_si_param.p1 = p_eit_data;
       sg_si_param.p2 = p_event_data;
	sg_si_param.p3 = p_event_num;
	// *********************** input service id !!!!!!!!!!!!!!!!!!!!	
	h_dvbpsi = dvbpsi_AttachEIT(p_eit_data->i_serv_id, dvb_Dump_EIT, &sg_si_param);

	//b_ok = ReadPacket(i_fd, data);
	b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,1);
	
	while(b_ok)
	{
		uint16_t i_pid = ((uint16_t)(sg_share_buf[1] & 0x1f) << 8) + sg_share_buf[2];
		if(i_pid == 0x12)
		{
			if( EUV_DEC_FROM_SECTION != sg_si_param.type )
			{
				if(1!= dvbpsi_PushPacket(h_dvbpsi, sg_share_buf))
				{
					break;
				}
			}
			else
			{
				if(1!= dvbpsi_PushSection(h_dvbpsi, sg_share_buf, param.len))
				{
					break;
				}
			}	
		}
		//b_ok = ReadPacket(i_fd, sg_share_buf);
		b_ok = dvb_io_dev.read(sg_share_buf, 188, &param,0);
	}

	dvbpsi_DetachEIT(h_dvbpsi);
	
	return ret;
}


/*---------------------------------------------------------------------------------------------*/
//	dvb_Send_SI_Pack 
/*---------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------*/
//	dvb_Send_SI_Pack 
/*---------------------------------------------------------------------------------------------*/
void dvbSI_Send_Pack(uint8_t howto, unsigned short pid, unsigned char* p_section, unsigned int len)
{
	uint8_t packet[188];
	uint8_t *p_packet = packet;

       uint8_t* p_pos_in_ts;
	uint8_t* p_byte = p_section;
	uint8_t* p_end =  p_section+len-1;

	p_packet[0] = 0x47;
	p_packet[1] = 0x1F&pid>>8;
	p_packet[2] = pid;
	p_packet[3] = 0x00;
		
	if(p_section)
	{
		
		p_packet[1] |= 0x40; // payload_unit_start_indicator
		p_packet[3] = (p_packet[3] & 0x0f) | 0x10;

		p_packet[4] = 0x00; /* pointer_field */
		p_pos_in_ts = p_packet + 5;

		while((p_pos_in_ts < p_packet + 188) && (p_byte < p_end))
		  *(p_pos_in_ts++) = *(p_byte++);
		while(p_pos_in_ts < p_packet + 188)
		  *(p_pos_in_ts++) = 0xff;

		dvb_io_dev.write(p_packet, 188, &howto);

		p_packet[3] = (p_packet[3] + 1) & 0x0f;

		while(p_byte < p_end)
		{
			  p_packet[1] &= 0xbf;
			  p_packet[3] = (p_packet[3] & 0x0f) | 0x10;

			  p_pos_in_ts = p_packet + 4;

			  while((p_pos_in_ts < p_packet + 188) && (p_byte < p_end))
			  	*(p_pos_in_ts++) = *(p_byte++);
			  while(p_pos_in_ts < p_packet + 188)
			    	*(p_pos_in_ts++) = 0xff;

			  dvb_io_dev.write(p_packet, 188, &howto);

			  p_packet[3] = (p_packet[3] + 1) & 0x0f;
		}
	}
}

void  dvb_Send_SI_Pack(uint8_t howto, uint8_t* p_packet, dvbpsi_psi_section_t* p_section)
{
	p_packet[0] = 0x47;

	while(p_section)
	{
		uint8_t* p_pos_in_ts;
		uint8_t* p_byte = p_section->p_data;
		uint8_t* p_end =  p_section->p_payload_end
		                 + (p_section->b_syntax_indicator ? 4 : 0);

		p_packet[1] |= 0x40;
		p_packet[3] = (p_packet[3] & 0x0f) | 0x10;

		p_packet[4] = 0x00; /* pointer_field */
		p_pos_in_ts = p_packet + 5;

		while((p_pos_in_ts < p_packet + 188) && (p_byte < p_end))
		  *(p_pos_in_ts++) = *(p_byte++);
		while(p_pos_in_ts < p_packet + 188)
		  *(p_pos_in_ts++) = 0xff;

		dvb_io_dev.write(p_packet, 188, &howto);

		p_packet[3] = (p_packet[3] + 1) & 0x0f;

		while(p_byte < p_end)
		{
			  p_packet[1] &= 0xbf;
			  p_packet[3] = (p_packet[3] & 0x0f) | 0x10;

			  p_pos_in_ts = p_packet + 4;

			  while((p_pos_in_ts < p_packet + 188) && (p_byte < p_end))
			  	*(p_pos_in_ts++) = *(p_byte++);
			  while(p_pos_in_ts < p_packet + 188)
			    	*(p_pos_in_ts++) = 0xff;

			  dvb_io_dev.write(p_packet, 188, &howto);

			  p_packet[3] = (p_packet[3] + 1) & 0x0f;
		}
		p_section = p_section->p_next;
	}
}


/*---------------------------------------------------------------------------------------------*/
//	 
/*---------------------------------------------------------------------------------------------*/

/*****************************************************************************
 * DumpPAT
 *****************************************************************************/
void dvb_Dump_PAT(void* p_zero, dvbpsi_pat_t* p_pat)
{
  int i = 0;
  uv_param *p_param = (uv_param *)p_zero;
  uv_pat_data *p_pat_data = (uv_pat_data *)p_param->p1;
  uv_pat_pid_data *p_pid_data = (uv_pat_pid_data *)p_param->p2;
  uint16_t *p_pid_num = (uint16_t *)p_param->p3;

  dvbpsi_pat_program_t* p_program = p_pat->p_first_program;
  while(p_program)
  {
    p_pid_data[i].i_pg_num = p_program->i_number;
    p_pid_data[i].i_pid = p_program->i_pid;
    p_program = p_program->p_next;
    i++;
  }
  *p_pid_num = i;
  p_pat_data->i_tran_stream_id = p_pat->i_ts_id;
  dvbpsi_DeletePAT(p_pat);
}


/*****************************************************************************
 * DumpDescriptors
 *****************************************************************************/
static void DumpDescriptors(const char* str, dvbpsi_descriptor_t* p_descriptor)
{
  while(p_descriptor)
  {
    int i;
    dbg_prt("%s 0x%02x : \"", str, p_descriptor->i_tag);
    for(i = 0; i < p_descriptor->i_length; i++)
      dbg_prt("%c", p_descriptor->p_data[i]);
    dbg_prt("\"\n");
    p_descriptor = p_descriptor->p_next;
  }
};


/*****************************************************************************
 * DumpPMT
 *****************************************************************************/
void dvb_Dump_PMT(void* p_zero, dvbpsi_pmt_t* p_pmt)
{
  int i,j;
  dvbpsi_pmt_es_t* p_es = p_pmt->p_first_es;

  uv_param *p_param = (uv_param *)p_zero;
  uv_pmt_data *p_pmt_data = (uv_pmt_data *)p_param->p1;
  uv_pmt_es_data *p_pmt_es_data = (uv_pmt_es_data *)p_param->p2;
  uint16_t *p_es_num = (uint16_t *)p_param->p3;
  
  dvbpsi_descriptor_t *p_descr;
  

  dbg_prt(  "\n");
 
  dbg_prt(  "  program_number : %d\n",
         p_pmt->i_program_number);
 
  dbg_prt(  "  PCR_PID        : 0x%x (%d)\n",
         p_pmt->i_pcr_pid, p_pmt->i_pcr_pid);

  //--------------------------------------------------------------------------------------
  p_pmt_data->i_pcr_pid = p_pmt->i_pcr_pid;
  
  //==========
  // ===
  p_descr = p_pmt->p_first_descriptor;
  i = 0; 
  while(p_descr)
  {
	if(i < 5)
        { // max descr is 5	
		p_pmt_data->p_descr[i].i_tag = p_descr->i_tag;
		p_pmt_data->p_descr[i].i_length = p_descr->i_length;
		memcpy(p_pmt_data->p_descr[i].p_data, p_descr->p_data, p_descr->i_length);
		p_descr = p_descr->p_next;
		i++;
	}else{
		break;
	}
  }
  // ===
  // ===========
  p_pmt_data->i_descr_num = i;
  
  j = 0;
  while(p_es)
  {
	if(j < 32)
	{// max ele is 32
		  p_pmt_es_data[j].i_type = p_es->i_type;
		  p_pmt_es_data[j].i_pid = p_es->i_pid;

		  //dbg_prt("     0x%02x 0x%02x  \n",p_es->i_type, p_es->i_pid);   
                                                     
		  p_descr = p_es->p_first_descriptor;
		  i = 0; 
		  while(p_descr)
		  {
			if(i < 5)
			{ // max descr is 5	
				p_pmt_es_data[j].p_descr[i].i_tag = p_descr->i_tag;
				p_pmt_es_data[j].p_descr[i].i_length = p_descr->i_length;
				dbg_prt("-------%d===%d \n", p_descr->i_tag, p_descr->i_length);
				memcpy(p_pmt_es_data[j].p_descr[i].p_data, p_descr->p_data, p_descr->i_length);
				p_descr = p_descr->p_next;
				i++;
				
			}else{
				break;
			}
		  }
		  //dbg_prt("----* i = %d \n", i);
      p_pmt_es_data[j].i_descr_num = i;	
		
		  p_es = p_es->p_next;
		  j++;		
	}
	else
	{
		break;
	}
  }
  *p_es_num = j;

  // dbg_prt("---- es-j = %d \n", j);
  //--------------------------------------------------------------------------------------
  /*
  DumpDescriptors("    ]", p_pmt->p_first_descriptor);
  dbg_prt(  "    | type @ elementary_PID\n");
  while(p_es)
  {
    dbg_prt("    | 0x%02x @ 0x%x (%d)\n",
           p_es->i_type, p_es->i_pid, p_es->i_pid);
    DumpDescriptors("    |  ]", p_es->p_first_descriptor);
    p_es = p_es->p_next;
  }
  */
  dvbpsi_DeletePMT(p_pmt);
}

/*****************************************************************************
 * DumpSDT
 *****************************************************************************/
void dvb_Dump_SDT(void* p_zero, dvbpsi_sdt_t* p_sdt)
{
/*
  dvbpsi_sdt_service_t* p_service = p_sdt->p_first_service;
  dbg_prt(  "\n");
  dbg_prt(  "New active SDT\n");
  dbg_prt(  "  ts_id : %d\n",
         p_sdt->i_ts_id);
  dbg_prt(  "  version_number : %d\n",
         p_sdt->i_version);
  dbg_prt(  "  network_id        : %d\n",
         p_sdt->i_network_id);
  dbg_prt(  "    | service_id \n");
  while(p_service)
  {
    dbg_prt("    | 0x%02x \n",
           p_service->i_service_id);
    DumpDescriptors("    |  ]", p_service->p_first_descriptor);
    p_service = p_service->p_next;
  }
  */
  	int i,j;  

	dvbpsi_sdt_service_t* p_service = p_sdt->p_first_service;

	uv_param *p_param = (uv_param *)p_zero;
        uv_sdt_data *p_sdt_data = (uv_eit_data *)p_param->p1;
	uv_sdt_serv_data *p_service_data = (uv_sdt_serv_data *)p_param->p2;
	uint16_t *p_service_num =  (uint16_t *)p_param->p3;
	
	dvbpsi_descriptor_t *p_descr;

        p_sdt_data->i_tran_stream_id = p_sdt->i_ts_id; 	
	p_sdt_data->i_orig_net_id = p_sdt->i_network_id;

	j = 0;
	while(p_service)
	{
		if(j < 32)
		{// max service is 32
			  p_service_data[j].i_serv_id = p_service->i_service_id;
			  p_service_data[j].i_eit_sched_flag = p_service->b_eit_schedule;
			  p_service_data[j].i_eit_pres_foll_flag = p_service->b_eit_present; 
			  p_service_data[j].i_running_status = p_service->i_running_status;
			  p_service_data[j].i_free_ca_mode = p_service->b_free_ca;
	
			  p_descr = p_service->p_first_descriptor;
			  i = 0; 
			  while(p_descr)
			  {
				if(i < 5)
				{ // max descr is 5	
					p_service_data[j].p_descr[i].i_tag = p_descr->i_tag;
					p_service_data[j].p_descr[i].i_length = p_descr->i_length;
					memcpy(p_service_data[j].p_descr[i].p_data, p_descr->p_data, p_descr->i_length);
					p_descr = p_descr->p_next;
					i++;
				}else{
					break;
				}
			  }
		    p_service_data[j].i_descr_num = i;		
		    
		    p_service = p_service->p_next;
		    j++;	
		}
		else
	  {
		 break;
	  }
	}
	*p_service_num = j;
	
	dvbpsi_DeleteSDT(p_sdt);
}

void dvb_Dump_NIT(void* p_zero, dvbpsi_nit_t* p_nit)
{
	int i,j;
	dvbpsi_nit_stream_t* p_stream = p_nit->p_first_stream;

	uv_param *p_param = (uv_param *)p_zero;
	uv_nit_data *p_nit_data = (uv_nit_data *)p_param->p1;
	uv_nit_stream_data *p_stream_data = (uv_nit_stream_data *)p_param->p2;
	uint16_t *p_stream_num = (uint16_t *)p_param->p3;

	dvbpsi_descriptor_t *p_descr;

	p_nit_data->i_net_id = p_nit->i_net_id;

	//==========
	// ===
	p_descr = p_nit->p_first_descriptor;
	i = 0; 
	while(p_descr)
	{
		if(i < 5)
		    { // max descr is 5	
			p_nit_data->p_descr[i].i_tag = p_descr->i_tag;
			p_nit_data->p_descr[i].i_length = p_descr->i_length;
			memcpy(p_nit_data->p_descr[i].p_data, p_descr->p_data, p_descr->i_length);
			p_descr = p_descr->p_next;
			i++;
		}else{
			break;
		}
	}
	// ===
	// ===========
	p_nit_data->i_descr_num = i;
	  
	j = 0;
	while(p_stream)
	{
		if(j < 32)
		{// max stream is 32
			  p_stream_data[j].i_tran_stream_id = p_stream->i_stream_id;
			  p_stream_data[j].i_orig_net_id = p_stream->i_orig_net_id;

			  p_descr = p_stream->p_first_descriptor;
			  i = 0; 
			  while(p_descr)
			  {
					if(i < 5)
					{ // max descr is 5	
						p_stream_data[j].p_descr[i].i_tag = p_descr->i_tag;
						p_stream_data[j].p_descr[i].i_length = p_descr->i_length;
						memcpy(p_stream_data[j].p_descr[i].p_data, p_descr->p_data, p_descr->i_length);
						p_descr = p_descr->p_next;
						i++;
					}else{
						break;
					}
			  }
		    p_stream_data[j].i_descr_num = i;			
		      
		    p_stream = p_stream->p_next;
		    j++;	
		}
		else
	  {
		 break;
	  }
	}
	*p_stream_num = j;
	
	dvbpsi_DeleteNIT(p_nit);
}

void dvb_Dump_EIT(void* p_zero, dvbpsi_eit_t* p_eit)
{
	int i,j;
	
	dvbpsi_eit_event_t* p_event = p_eit->p_first_event;

	uv_param *p_param = (uv_param *)p_zero;
    uv_eit_data *p_eit_data = (uv_eit_data *)p_param->p1;
	uv_eit_event_data *p_event_data = (uv_eit_event_data *)p_param->p2;
	uint16_t *p_event_num =  (uint16_t *)p_param->p3;
	
	dvbpsi_descriptor_t *p_descr;

    p_eit_data->i_serv_id = p_eit->i_service_id; 	
	p_eit_data->i_tran_stream_id = p_eit->i_stream_id;
	p_eit_data->i_orig_net_id = p_eit->i_orig_net_id;

	j = 0;
	while(p_event)
	{
		if(j < 32)
		{// max event is 32
			  p_event_data[j].i_event_id = p_event->i_event_id;
			  p_event_data[j].i_start_time = p_event->i_start_time;
			  //???????????????????????????????? need to change
			  p_event_data[j].i_duration = p_event->i_duration; 
			  p_event_data[j].i_running_status = p_event->i_running_status;
			  p_event_data[j].i_free_ca_mode = p_event-> b_free_ca;
	
			  p_descr = p_event->p_first_descriptor;
			  i = 0; 
			  while(p_descr)
			  {
					if(i < 5)
					{ // max descr is 5	
						p_event_data[j].p_descr[i].i_tag = p_descr->i_tag;
						p_event_data[j].p_descr[i].i_length = p_descr->i_length;
						memcpy(p_event_data[j].p_descr[i].p_data, p_descr->p_data, p_descr->i_length);
						p_descr = p_descr->p_next;
						i++;
					}else{
						break;
					}
			  }
		    p_event_data[j].i_descr_num = i;		
		      
		    p_event = p_event->p_next;  
		    j++;	
		}
		else
	  {
		 break;
	  }
	}
	*p_event_num = j;
	
	dvbpsi_DeleteEIT(p_eit);
}

void dvb_Dump_CAT(void* p_zero, dvbpsi_cat_t* p_cat)
{
	int i, j;
	uv_param *p_param = (uv_param *)p_zero;
        uv_descriptor *p_mdescr = (uv_pmt_data *)p_param->p1;
	uint16_t *p_descr_num = (uv_pmt_data *)p_param->p2;
        
  dvbpsi_descriptor_t *p_descr = p_cat->p_first_descriptor;

	i = 0; 
	while(p_descr)
	{
		if(i < 5)
		{ // max descr is 5	
			p_mdescr[i].i_tag = p_descr->i_tag;
			p_mdescr[i].i_length = p_descr->i_length;
			memcpy(p_mdescr[i].p_data, p_descr->p_data, p_descr->i_length);
			
			p_descr = p_descr->p_next;
			i++;
		}
		else
		{
			break;
		}
	}
  *p_descr_num = i;
	
	dvbpsi_DeleteNIT(p_cat);
}
	
void dvb_New_Subtable(void * p_zero, dvbpsi_handle h_dvbpsi,
                 uint8_t i_table_id, uint16_t i_extension)
{
	if(i_table_id == 0x42)
	{
		dvbpsi_AttachSDT(h_dvbpsi, i_table_id, i_extension, dvb_Dump_SDT, p_zero);
	}
	//else if(i_table_id == 0x4E)
	//{
	//	dvbpsi_AttachEIT(h_dvbpsi, dvb_Dump_EIT, p_zero);
	//}
	else
	{
		dbg_prt("--- Unresolved Extension Table Id: %d \n ---", i_table_id);
	}
}

int ReadPacket(int i_fd, uint8_t* p_dst)
{
	int i = 187;
	int i_rc = 1;
	
	p_dst[0] = 0;
	
	while((p_dst[0] != 0x47) && (i_rc > 0))
	{
		i_rc = read(i_fd, p_dst, 1);
	}
	
	while((i != 0) && (i_rc > 0))
	{
		i_rc = read(i_fd, p_dst + 188 - i, i);
		if(i_rc >= 0)
		i -= i_rc;
	}
	
	return (i == 0) ? 1 : 0;
}
