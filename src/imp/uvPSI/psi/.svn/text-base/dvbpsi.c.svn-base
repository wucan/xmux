/*****************************************************************************
 * dvbpsi.c: conversion from TS packets to PSI sections
 *----------------------------------------------------------------------------
 * (c)2001-2002 VideoLAN
 * $Id: dvbpsi.c,v 1.4 2002/10/07 14:15:14 sam Exp $
 *
 * Authors: Arnaud de Bossoreille de Ribou <bozo@via.ecp.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *----------------------------------------------------------------------------
 *
 *****************************************************************************/


#include "config.h"

#include <string.h>
#include <stdio.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include "dvbpsi.h"
#include "dvbpsi_private.h"
#include "psi.h"

#define DEBUG_PRT

#ifdef DEBUG_PRT

#ifndef dbg_prt
#define dbg_prt printf
#endif

#else
#ifndef dbg_prt
#define dbg_prt(...)
#endif

#endif

static void UV_SetBIT(uint8_t val[4], uint8_t bitn)
{
	uint32_t y = (1<<bitn);
	
	val[0] |= y;
	val[1] |= y >> 8;
	val[2] |= y >> 16;
	val[3] |= y >> 24;
}

/*****************************************************************************
 * dvbpsi_PushSection
 *****************************************************************************
 * Injection of a section  into a PSI decoder.
 *****************************************************************************/
 // 返回1表示需要继续读取section
 int dvbpsi_PushSection(dvbpsi_handle h_dvbpsi, uint8_t* p_sec, uint16_t sec_len)
{
	 dvbpsi_psi_section_t* p_section;      /* Current section */
  
	 p_section = h_dvbpsi->p_current_section;

  /* If the psi decoder needs a begginning of section and a new section
       begins in the packet then initialize the dvbpsi_psi_section_t structure */
  	if(p_section == NULL)
  	{
  		h_dvbpsi->p_current_section
                       	 = p_section
                        	 = dvbpsi_NewPSISection(h_dvbpsi->i_section_max_size);
			   
		 p_section->i_length
                         =   ((uint16_t)(p_section->p_data[1] & 0xf)) << 8
                           | p_section->p_data[2];
		 
		 memcpy(p_section->p_data, p_sec, sec_len);
		 h_dvbpsi->i_need = 0;
		p_section->p_payload_end = p_section->p_data + sec_len -1;
			
		 p_section->b_syntax_indicator = p_section->p_data[1] & 0x80;
        	 p_section->b_private_indicator = p_section->p_data[1] & 0x40;
			
        	 if(p_section->b_syntax_indicator)
               	p_section->p_payload_end -= 4;
		
		p_section->i_table_id = p_section->p_data[0];
		
		if(p_section->b_syntax_indicator)
		{
			p_section->i_extension =   (p_section->p_data[3] << 8)
			                         | p_section->p_data[4];
			p_section->i_version = (p_section->p_data[5] & 0x3e) >> 1;
			p_section->b_current_next = p_section->p_data[5] & 0x1;
			p_section->i_number = p_section->p_data[6];
			p_section->i_last_number = p_section->p_data[7];
			p_section->p_payload_start = p_section->p_data + 8;
		}
		else
		{
			p_section->p_payload_start = p_section->p_data + 3;
		}

  	}  

	 if(dvbpsi_ValidPSISection(p_section))
	 {
		 h_dvbpsi->pf_callback(h_dvbpsi, p_section);
          	 h_dvbpsi->p_current_section = NULL;
	 }
	 else
	 {
		dvbpsi_DeletePSISections(p_section);
          	h_dvbpsi->p_current_section = NULL;
	 }

FUNC_RETURN:
	  // 还需要读TS包
	  if(p_section->i_number != p_section->i_last_number || (h_dvbpsi->i_need > 0))
	  {
	         dbg_prt("-need-%d,%d\n",p_section->i_number, h_dvbpsi->i_need);
	  	  return 1; 
	  }else
	  {
	  	  return 0;	
	  }

	return 0;
}


/*****************************************************************************
 * dvbpsi_PushPacket
 *****************************************************************************
 * Injection of a TS packet into a PSI decoder.
 *****************************************************************************/
int dvbpsi_PushPacket(dvbpsi_handle h_dvbpsi, uint8_t* p_data)
{
  uint8_t i_expected_counter;           /* Expected continuity counter */
  dvbpsi_psi_section_t* p_section;      /* Current section */
  uint8_t* p_payload_pos;               /* Where in the TS packet */
  uint8_t* p_new_pos = NULL;            /* Beginning of the new section,
                                           updated to NULL when the new
                                           section is handled */
  int i_available;                      /* Byte count available in the
                                           packet */

  /* TS start code */
  // 并不是完整的TS包
  if(p_data[0] != 0x47)
  {
    DVBPSI_ERROR("PSI decoder", "not a TS packet");
    return -1;
  }

  /* Continuity check */
  i_expected_counter = (h_dvbpsi->i_continuity_counter + 1) & 0xf;
  
  h_dvbpsi->i_continuity_counter = p_data[3] & 0xf;

	// 连续计数错误
  if(i_expected_counter != h_dvbpsi->i_continuity_counter)
  {
    DVBPSI_ERROR_ARG("PSI decoder",
                     "TS discontinuity (received %d, expected %d)",
                     h_dvbpsi->i_continuity_counter, i_expected_counter);
    h_dvbpsi->b_discontinuity = 1;
    if(h_dvbpsi->p_current_section)
    {
      dvbpsi_DeletePSISections(h_dvbpsi->p_current_section);
      h_dvbpsi->p_current_section = NULL;
    }
  }

  dbg_prt("--go on --\n");
  /* Return if no payload in the TS packet */
  // Adaption data
  // 没有有效载荷
  if(!(p_data[3] & 0x10))
  {
    return -2;
  }

  /* Skip the adaptation_field if present */
  if(p_data[3] & 0x20)
    p_payload_pos = p_data + 5 + p_data[4];
  else
    p_payload_pos = p_data + 4;

  /* Unit start -> skip the pointer_field and a new section begins */
  // 新的Section开始
  if(p_data[1] & 0x40)
  {
    p_new_pos = p_payload_pos + *p_payload_pos + 1;
    p_payload_pos += 1;
  }

  p_section = h_dvbpsi->p_current_section;

  dbg_prt("---begin to decode section---\n");
  /* If the psi decoder needs a begginning of section and a new section
     begins in the packet then initialize the dvbpsi_psi_section_t structure */
  if(p_section == NULL)
  {
    if(p_new_pos)
    {
      /* Allocation of the structure */
      h_dvbpsi->p_current_section
                        = p_section
                        = dvbpsi_NewPSISection(h_dvbpsi->i_section_max_size);
      /* Update the position in the packet */
      p_payload_pos = p_new_pos;
      /* New section is being handled */
      p_new_pos = NULL;
      /* Just need the header to know how long is the section */
      h_dvbpsi->i_need = 3;
      h_dvbpsi->b_complete_header = 0;
    }
    else
    {
      /* No new section => return */
      dbg_prt("-K-"); 
      return 0;
    }
  }

  /* Remaining bytes in the payload */
  i_available = 188 + p_data - p_payload_pos;

  while(i_available > 0)
  {
    if(i_available >= h_dvbpsi->i_need)
    {
      /* There are enough bytes in this packet to complete the
         header/section */
      memcpy(p_section->p_payload_end, p_payload_pos, h_dvbpsi->i_need);
      p_payload_pos += h_dvbpsi->i_need;
      p_section->p_payload_end += h_dvbpsi->i_need;
      i_available -= h_dvbpsi->i_need;

      if(!h_dvbpsi->b_complete_header)
      {
        /* Header is complete */
        h_dvbpsi->b_complete_header = 1;
        /* Compute p_section->i_length and update h_dvbpsi->i_need */
        h_dvbpsi->i_need = p_section->i_length
                         =   ((uint16_t)(p_section->p_data[1] & 0xf)) << 8
                           | p_section->p_data[2];
        /* Check that the section isn't too long */
        if(h_dvbpsi->i_need > h_dvbpsi->i_section_max_size - 3)
        {
          DVBPSI_ERROR("PSI decoder", "PSI section too long");
          dvbpsi_DeletePSISections(p_section);
          h_dvbpsi->p_current_section = NULL;
          /* If there is a new section not being handled then go forward
             in the packet */
          if(p_new_pos)
          {
            h_dvbpsi->p_current_section
                        = p_section
                        = dvbpsi_NewPSISection(h_dvbpsi->i_section_max_size);
            p_payload_pos = p_new_pos;
            p_new_pos = NULL;
            h_dvbpsi->i_need = 3;
            h_dvbpsi->b_complete_header = 0;
            i_available = 188 + p_data - p_payload_pos;
          }
          else
          {
            i_available = 0;
          }
        }
      }
      else
      {
        /* PSI section is complete */
	 h_dvbpsi->i_need = 0;
        p_section->b_syntax_indicator = p_section->p_data[1] & 0x80;
        p_section->b_private_indicator = p_section->p_data[1] & 0x40;
        /* Update the end of the payload if CRC_32 is present */
        if(p_section->b_syntax_indicator)
          p_section->p_payload_end -= 4;
		
        if(dvbpsi_ValidPSISection(p_section))
        {
        
          //  save the complete section to mib buffer
	   if((sg_si_param.type == EUV_SECTION)||(sg_si_param.type == EUV_BOTH))
	   {
	   	dbg_prt("----enter section from000---\n");
		// firstly copy section data
		memcpy((uint8_t *)sg_si_param.sec[sg_si_param.cur_cnt]+2, p_section->p_data,  p_section->i_length+3);
	       
		// secondly copy section length
		sg_si_param.sec_len[sg_si_param.cur_cnt] = p_section->i_length+3;
		memcpy((uint8_t *)sg_si_param.sec[sg_si_param.cur_cnt], &sg_si_param.sec_len[sg_si_param.cur_cnt],  2);
		
		// thirdly reflesh global state info
		switch(sg_si_param.tbl_type)
		{
			case EUV_TBL_PAT:
				sg_si_param.cur_stat->tbl_s[sg_si_param.cha][0] |= 0x80;
				break;
				
			case EUV_TBL_PMT:
				UV_SetBIT(&sg_si_param.cur_stat->tbl_s[sg_si_param.cha][1], sg_si_param.tbl_which_pmt);
				break;
				
			case EUV_TBL_CAT:
				sg_si_param.cur_stat->tbl_s[sg_si_param.cha][0] |= 0x40;
				break;
				
			case EUV_TBL_SDT:
				sg_si_param.cur_stat->tbl_s[sg_si_param.cha][0] |= (0x01<< sg_si_param.cur_cnt);
				break;
				
			case EUV_TBL_NIT:
				sg_si_param.cur_stat->tbl_s[sg_si_param.cha][0] |= 0x20;
				break;
				
			case EUV_TBL_EIT:
				
				break;
				
			default:
				break;
		}
		
		 sg_si_param.cur_cnt++;

		 if(sg_si_param.type == EUV_SECTION)
		 {
			goto FUNC_RETURN;
		 }
	   }
         
          //  
          /* PSI section is valid */
          p_section->i_table_id = p_section->p_data[0];
          if(p_section->b_syntax_indicator)
          {
            p_section->i_extension =   (p_section->p_data[3] << 8)
                                     | p_section->p_data[4];
            p_section->i_version = (p_section->p_data[5] & 0x3e) >> 1;
            p_section->b_current_next = p_section->p_data[5] & 0x1;
            p_section->i_number = p_section->p_data[6];
            p_section->i_last_number = p_section->p_data[7];
            p_section->p_payload_start = p_section->p_data + 8;
          }
          else
          {
            p_section->p_payload_start = p_section->p_data + 3;
          }

          h_dvbpsi->pf_callback(h_dvbpsi, p_section);
          h_dvbpsi->p_current_section = NULL;
        }
        else
        {
          /* PSI section isn't valid => trash it */
          dvbpsi_DeletePSISections(p_section);
          h_dvbpsi->p_current_section = NULL;
        }

	 dbg_prt("--a new section--\n");
        /* If there is a new section not being handled then go forward
           in the packet */
        if(p_new_pos)
        {
          h_dvbpsi->p_current_section
                        = p_section
                        = dvbpsi_NewPSISection(h_dvbpsi->i_section_max_size);
          p_payload_pos = p_new_pos;
          p_new_pos = NULL;
          h_dvbpsi->i_need = 3;
          h_dvbpsi->b_complete_header = 0;
          i_available = 188 + p_data - p_payload_pos;
        }
        else
        {
          i_available = 0;
        }
      }
    }
    else
    {
      /* There aren't enough bytes in this packet to complete the
         header/section */
      memcpy(p_section->p_payload_end, p_payload_pos, i_available);
      p_section->p_payload_end += i_available;
      h_dvbpsi->i_need -= i_available;
      i_available = 0;
    }
  }

FUNC_RETURN:
  // 还需要读TS包
  if(p_section->i_number != p_section->i_last_number || (h_dvbpsi->i_need > 0))
  {
         dbg_prt("-need-%d,%d\n",p_section->i_number, p_section->i_last_number,  h_dvbpsi->i_need);
  	  return 1; 
  }else
  {
  	  return 0;	
  }
}
