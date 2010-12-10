/*****************************************************************************
 * eit.c: EIT decoder/generator
 *
 *****************************************************************************/


#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include "dvbpsi.h"
#include "dvbpsi_private.h"
#include "psi.h"
#include "descriptor.h"
#include "eit.h"
#include "eit_private.h"

dvbpsi_handle dvbpsi_AttachEIT(uint16_t i_service_id,
                               dvbpsi_eit_callback pf_callback,
                               void* p_cb_data)

{
  dvbpsi_handle h_dvbpsi = (dvbpsi_decoder_t*)malloc(sizeof(dvbpsi_decoder_t));
  dvbpsi_eit_decoder_t* p_eit_decoder;
  unsigned int i;

  if(h_dvbpsi == NULL)
    return NULL;

  p_eit_decoder = (dvbpsi_eit_decoder_t*)malloc(sizeof(dvbpsi_eit_decoder_t));

  if(p_eit_decoder == NULL)
  {
    free(h_dvbpsi);
    return NULL;
  }

  /* PSI decoder configuration */
  h_dvbpsi->pf_callback = &dvbpsi_GatherEITSections;
  h_dvbpsi->p_private_decoder = p_eit_decoder;
  h_dvbpsi->i_section_max_size = 1024;
  /* PSI decoder initial state */
  h_dvbpsi->i_continuity_counter = 31;
  h_dvbpsi->b_discontinuity = 1;
  h_dvbpsi->p_current_section = NULL;

  /* EIT decoder configuration */
  p_eit_decoder->i_service_id= i_service_id;
  p_eit_decoder->pf_callback = pf_callback;
  p_eit_decoder->p_cb_data = p_cb_data;
  /* EIT decoder initial state */
  p_eit_decoder->b_current_valid = 0;
  p_eit_decoder->p_building_eit = NULL;
  for(i = 0; i <= 255; i++)
    p_eit_decoder->ap_sections[i] = NULL;

  return h_dvbpsi;
}

void dvbpsi_DetachEIT(dvbpsi_handle h_dvbpsi)
{
  dvbpsi_eit_decoder_t* p_eit_decoder
                        = (dvbpsi_eit_decoder_t*)h_dvbpsi->p_private_decoder;
  unsigned int i;

  free(p_eit_decoder->p_building_eit);

  for(i = 0; i <= 255; i++)              
  {
    if(p_eit_decoder->ap_sections[i])
      free(p_eit_decoder->ap_sections[i]);
  }

  free(h_dvbpsi->p_private_decoder);
  free(h_dvbpsi);
}

void dvbpsi_InitEIT(dvbpsi_eit_t* p_eit, uint16_t i_service_id,
                    uint8_t i_version, int b_current_next, uint16_t i_stream_id, uint16_t i_orig_net_id)
{
  p_eit->i_service_id = i_service_id;
  p_eit->i_stream_id = i_stream_id;
  p_eit->i_version = i_version;
  p_eit->i_orig_net_id = i_orig_net_id;
  p_eit->b_current_next = b_current_next;
  p_eit->p_first_event = NULL;
}

void dvbpsi_EmptyEIT(dvbpsi_eit_t* p_eit)
{
  dvbpsi_eit_event_t* p_event = p_eit->p_first_event;
  while(p_event != NULL)
  {
    dvbpsi_eit_event_t* p_tmp = p_event->p_next;
    dvbpsi_DeleteDescriptors(p_event->p_first_descriptor);
    free(p_event);
    p_event = p_tmp;
  }
  p_eit->p_first_event = NULL;
}
                        

dvbpsi_eit_event_t* dvbpsi_EITAddEvent(dvbpsi_eit_t* p_eit,
                                 uint16_t i_event_id, uint32_t i_start_time, uint32_t i_duration, uint8_t i_running_status, uint8_t i_free_CA_mode)
{
  dvbpsi_eit_event_t* p_event = (dvbpsi_eit_event_t*)malloc(sizeof(dvbpsi_eit_event_t));

  if(p_event)
  {
	p_event->i_event_id = i_event_id;
	p_event->i_start_time = i_start_time;
	p_event->i_duration = i_duration;
	p_event->i_running_status = i_running_status;
	p_event->b_free_ca = i_free_CA_mode;
	p_event->p_first_descriptor = NULL;
	p_event->p_next = NULL;

	if(p_eit->p_first_event == NULL)
	{
	  	p_eit->p_first_event = p_event;
	}
	else
	{
		dvbpsi_eit_event_t* p_last_event = p_eit->p_first_event;
		while(p_last_event->p_next != NULL)
			p_last_event = p_last_event->p_next;
		p_last_event->p_next = p_event;
	}
  }

  return p_event;
}

dvbpsi_descriptor_t* dvbpsi_EITEventAddDescriptor(dvbpsi_eit_event_t* p_event,
                                               uint8_t i_tag, uint8_t i_length,
                                               uint8_t* p_data)
{
	dvbpsi_descriptor_t* p_descriptor
                        = dvbpsi_NewDescriptor(i_tag, i_length, p_data);

  if(p_descriptor)
  {
    if(p_event->p_first_descriptor == NULL)
    {
      p_event->p_first_descriptor = p_descriptor;
    }
    else
    {
      dvbpsi_descriptor_t* p_last_descriptor = p_event->p_first_descriptor;
      while(p_last_descriptor->p_next != NULL)
        p_last_descriptor = p_last_descriptor->p_next;
      p_last_descriptor->p_next = p_descriptor;
    }
  }

  return p_descriptor;
}

void dvbpsi_GatherEITSections(dvbpsi_decoder_t * p_decoder,  void* p_private_decoder, dvbpsi_psi_section_t * p_section)
{
  dvbpsi_eit_decoder_t* p_eit_decoder
                        = (dvbpsi_eit_decoder_t*)p_decoder->p_private_decoder;
  int b_append = 1;
  int b_reinit = 0;
  unsigned int i;

  DVBPSI_DEBUG_ARG("EIT decoder",
                   "Table version %2d, " "i_extension %5d, "
                   "section %3d up to %3d, " "current %1d",
                   p_section->i_version, p_section->i_extension,
                   p_section->i_number, p_section->i_last_number,
                   p_section->b_current_next);

  if(p_section->i_table_id != 0x4E)
  {
    /* Invalid table_id value */
    DVBPSI_ERROR_ARG("EIT decoder",
                     "invalid section (table_id == 0x%02x)",
                     p_section->i_table_id);
    b_append = 0;
  }

  if(b_append && !p_section->b_syntax_indicator)
  {
    /* Invalid section_syntax_indicator */
    DVBPSI_ERROR("EIT decoder",
                 "invalid section (section_syntax_indicator == 0)");
    b_append = 0;
  }

  /* Now if b_append is true then we have a valid EIT section */
  if(b_append && (p_eit_decoder->i_service_id != p_section->i_extension))
  {
    //DVBPSI_ERROR("EIT decoder", "'service_id' don't match");
    b_append = 0;
  }

  if(b_append)
  {
    /* TS discontinuity check */
    if(p_decoder->b_discontinuity)
    {
      b_reinit = 1;
      p_decoder->b_discontinuity = 0;
    }
    else
    {
      /* Perform some few sanity checks */
      if(p_eit_decoder->p_building_eit)
      {
        if(p_eit_decoder->p_building_eit->i_version != p_section->i_version)
        {
          /* version_number */
          DVBPSI_ERROR("EIT decoder",
                       "'version_number' differs"
                       " whereas no discontinuity has occured");
          b_reinit = 1;
        }
        else if(p_eit_decoder->i_last_section_number
                                                != p_section->i_last_number)
        {
          /* last_section_number */
          DVBPSI_ERROR("EIT decoder",
                       "'last_section_number' differs"
                       " whereas no discontinuity has occured");
          b_reinit = 1;
        }
      }
      else
      {
        if(    (p_eit_decoder->b_current_valid)
            && (p_eit_decoder->current_eit.i_version == p_section->i_version))
        {
          /* Signal a new EIT if the previous one wasn't active */
          if(    (!p_eit_decoder->current_eit.b_current_next)
              && (p_section->b_current_next))
          {
            dvbpsi_eit_t* p_eit = (dvbpsi_eit_t*)malloc(sizeof(dvbpsi_eit_t));

            p_eit_decoder->current_eit.b_current_next = 1;
            *p_eit = p_eit_decoder->current_eit;
            p_eit_decoder->pf_callback(p_eit_decoder->p_cb_data, p_eit);
          }

          /* Don't decode since this version is already decoded */
          b_append = 0;
        }
      }
    }
  }

  /* Reinit the decoder if wanted */
  if(b_reinit)
  {
    /* Force redecoding */
    p_eit_decoder->b_current_valid = 0;
    /* Free structures */
    if(p_eit_decoder->p_building_eit)
    {
      free(p_eit_decoder->p_building_eit);
      p_eit_decoder->p_building_eit = NULL;
    }
    /* Clear the section array */
    for(i = 0; i <= 255; i++)
    {
      if(p_eit_decoder->ap_sections[i] != NULL)
      {
        dvbpsi_DeletePSISections(p_eit_decoder->ap_sections[i]);
        p_eit_decoder->ap_sections[i] = NULL;
      }
    }
  }

  /* Append the section to the list if wanted */
  if(b_append)
  {
    int b_complete;

    /* Initialize the structures if it's the first section received */
    if(!p_eit_decoder->p_building_eit)
    {
      p_eit_decoder->p_building_eit =
                                (dvbpsi_eit_t*)malloc(sizeof(dvbpsi_eit_t));
      dvbpsi_InitEIT(p_eit_decoder->p_building_eit,
                     p_eit_decoder->i_service_id,
                     p_section->i_version,
                     p_section->b_current_next,
                       (uint16_t)(p_section->p_payload_start[0]),  // transport stream id
                       (uint16_t)(p_section->p_payload_start[2]) );// original network id
                  
      p_eit_decoder->i_last_section_number = p_section->i_last_number;
    }

    /* Fill the section array */
    if(p_eit_decoder->ap_sections[p_section->i_number] != NULL)
    {
      DVBPSI_DEBUG_ARG("EIT decoder", "overwrite section number %d",
                       p_section->i_number);
      dvbpsi_DeletePSISections(p_eit_decoder->ap_sections[p_section->i_number]);
    }
    p_eit_decoder->ap_sections[p_section->i_number] = p_section;

    /* Check if we have all the sections */
    b_complete = 0;
    for(i = 0; i <= p_eit_decoder->i_last_section_number; i++)
    {
      if(!p_eit_decoder->ap_sections[i])
        break;

      if(i == p_eit_decoder->i_last_section_number)
        b_complete = 1;
    }

    if(b_complete)
    {
      /* Save the current information */
      p_eit_decoder->current_eit = *p_eit_decoder->p_building_eit;
      p_eit_decoder->b_current_valid = 1;
      /* Chain the sections */
      if(p_eit_decoder->i_last_section_number)
      {
        for(i = 0; i <= p_eit_decoder->i_last_section_number - 1; i++)
          p_eit_decoder->ap_sections[i]->p_next =
                                        p_eit_decoder->ap_sections[i + 1];
      }
      /* Decode the sections */
      dvbpsi_DecodeEITSections(p_eit_decoder->p_building_eit,
                               p_eit_decoder->ap_sections[0]);
      /* Delete the sections */
      dvbpsi_DeletePSISections(p_eit_decoder->ap_sections[0]);
      /* signal the new EIT */
      p_eit_decoder->pf_callback(p_eit_decoder->p_cb_data,
                                 p_eit_decoder->p_building_eit);
      /* Reinitialize the structures */
      p_eit_decoder->p_building_eit = NULL;
      for(i = 0; i <= p_eit_decoder->i_last_section_number; i++)
        p_eit_decoder->ap_sections[i] = NULL;
    }
  }
  else
  {
    dvbpsi_DeletePSISections(p_section);
  }
}

void dvbpsi_DecodeEITSections(dvbpsi_eit_t* p_eit,
                              dvbpsi_psi_section_t* p_section)
{
  uint8_t* p_byte, * p_end;

  while(p_section)
  {
  	// Waiting for checking
  	uint16_t i_length = ((uint16_t)(p_byte[3] & 0x0f) << 8) | p_byte[4];
    /* - Events */
    for(p_byte = p_end; p_byte + 12 <= p_section->p_payload_end;)
    {
		uint32_t i_duration;
		uint32_t i_start_time;
		uint16_t i_event_id = p_byte[0];
		uint8_t i_running_status;
		uint8_t i_free_CA_mode;
		dvbpsi_eit_event_t* p_event;

		i_event_id <= 8;
		i_event_id |=  p_byte[1];
		i_start_time = p_byte[2];
		i_start_time <<= 8;
		i_start_time |= p_byte[3];
		i_start_time <<= 8;
		i_start_time |= p_byte[4]; 
		i_start_time <<= 8;
		i_start_time |= p_byte[5]; 
      
      i_duration = p_byte[6];
      i_duration <<= 8;
      i_duration |= p_byte[7];
      i_duration <<= 8;
      i_duration |= p_byte[8]; 
      i_duration <<= 8;
      i_duration |= p_byte[9]; 
      
      i_running_status = p_byte[10]>>5;
      i_free_CA_mode = (p_byte[10]>>4) & 0x01;
        
      p_event = dvbpsi_EITAddEvent(p_eit, i_event_id, i_start_time, i_duration, i_running_status, i_free_CA_mode);

	/* - Event  descriptors */
      p_byte += 12;
      p_end = p_byte + i_length;
      if( p_end > p_section->p_payload_end )
      {
            p_end = p_section->p_payload_end;
      }
      while(p_byte + 2 < p_end)
      {
        uint8_t i_tag = p_byte[0];
        uint8_t i_length = p_byte[1];
        if(i_length + 2 <= p_end - p_byte)
          dvbpsi_EITEventAddDescriptor(p_event, i_tag, i_length, p_byte + 2);
        p_byte += 2 + i_length;
      }
    }

    p_section = p_section->p_next;
  }
}


dvbpsi_psi_section_t* dvbpsi_GenEITSections(dvbpsi_eit_t* p_eit)
{
  dvbpsi_psi_section_t* p_result = dvbpsi_NewPSISection(1024);
  dvbpsi_psi_section_t* p_current = p_result;
  dvbpsi_psi_section_t* p_prev;
  dvbpsi_eit_event_t* p_event = p_eit->p_first_event;
   dvbpsi_descriptor_t* p_descriptor;
   
  p_current->i_table_id = 0x04E;
  p_current->b_syntax_indicator = 1;
  p_current->b_private_indicator = 0;
  p_current->i_length = 15;                     /* header + CRC_32 */
  p_current->i_extension = p_eit->i_service_id;
  p_current->i_version = p_eit->i_version;
  p_current->b_current_next = p_eit->b_current_next;
  p_current->i_number = 0;
  p_current->p_payload_end += 12;               /* just after the header */
  p_current->p_payload_start = p_current->p_data + 8;

  // p_current->p_payload_start[0] = p_eit->i_stream_id>>8;
  p_current->p_data[8] = p_eit->i_stream_id>>8;
  p_current->p_data[9] |= (p_eit->i_stream_id & 0x00FF);
  p_current->p_data[10] = p_eit->i_orig_net_id>>8;
  p_current->p_data[11] |= (p_eit->i_orig_net_id & 0x00FF);

  while(p_event != NULL)
  {
    uint8_t* p_event_start = p_current->p_payload_end;
    uint16_t i_event_length = 12;
    uint16_t i_event_descr_length = 0;

    /* Can the current section carry all the descriptors ? */
    p_descriptor = p_event->p_first_descriptor;
    while(    (p_descriptor != NULL)
           && ((p_event_start - p_current->p_data) + i_event_length <= 1020))
    {
      i_event_length += p_descriptor->i_length + 2;
      p_descriptor = p_descriptor->p_next;
    }

    /* If _no_ and the current section isn't empty and an empty section
       may carry one more descriptor
       then create a new section */
    if(    (p_descriptor != NULL)
        && (p_event_start - p_current->p_data != 12)
        && (i_event_length <= 1008))
    {
	/* will put more descriptors in an empty section */
					DVBPSI_DEBUG("EIT generator",
					           "create a new section to carry more ES descriptors");
					p_prev = p_current;
					p_current = dvbpsi_NewPSISection(1024);
					p_prev->p_next = p_current;
				
					p_current->i_table_id = 0x04E;
					p_current->b_syntax_indicator = 1;
					p_current->b_private_indicator = 0;
					p_current->i_length = 15;                     /* header + CRC_32 */
					p_current->i_extension = p_eit->i_service_id;
					p_current->i_version = p_eit->i_version;
					p_current->b_current_next = p_eit->b_current_next;
					p_current->i_number = 0;
					p_current->p_payload_end += 12;               /* just after the header */
					p_current->p_payload_start = p_current->p_data + 8;
				
					p_event_start = p_current->p_payload_end;
    }

    /* p_event_start is where the event begins */
    p_event_start[0] = p_event->i_event_id >> 8;
    p_event_start[1] = p_event->i_event_id & 0x00FF;
    p_event_start[2] = p_event->i_start_time >> 24;
    p_event_start[3] = p_event->i_start_time >> 16;
    p_event_start[4] = p_event->i_start_time >> 8;
    p_event_start[5] = p_event->i_start_time & 0x00FF;
    
    p_event_start[6] = p_event->i_duration >> 24;
    p_event_start[7] = p_event->i_duration >> 16;
    p_event_start[8] = p_event->i_duration >> 8;
    p_event_start[9] = p_event->i_duration & 0x00FF;
    
    p_event_start[10] = p_event->i_running_status << 5;
    p_event_start[10] |= (p_event->b_free_ca ? 0x10 : 0);
    
    /* Increase the length by 5 */
    p_current->p_payload_end += 12;
    p_current->i_length += 12;

    /* Event descriptors */
    p_descriptor = p_event->p_first_descriptor;
    while(    (p_descriptor != NULL)
           && (   (p_current->p_payload_end - p_current->p_data)
                + p_descriptor->i_length <= 1018))
    {
      /* p_payload_end is where the descriptor begins */
      p_current->p_payload_end[0] = p_descriptor->i_tag;
      p_current->p_payload_end[1] = p_descriptor->i_length;
      memcpy(p_current->p_payload_end + 2,
             p_descriptor->p_data,
             p_descriptor->i_length);

      /* Increase length by descriptor_length + 2 */
      p_current->p_payload_end += p_descriptor->i_length + 2;
      p_current->i_length += p_descriptor->i_length + 2;

      p_descriptor = p_descriptor->p_next;
    }

    if(p_descriptor != NULL)
      DVBPSI_ERROR("EIT generator", "unable to carry all the ES descriptors");

    /* event descr length */
    i_event_descr_length = p_current->p_payload_end - p_event_start - 12;
    p_event_start[10] = (i_event_descr_length >> 8) | 0xF0;
    p_event_start[11] = i_event_descr_length;

    p_event = p_event->p_next;
  }

  /* Finalization */
  p_prev = p_result;
  while(p_prev != NULL)
  {
    p_prev->i_last_number = p_current->i_number;
    dvbpsi_BuildPSISection(p_prev);
    p_prev = p_prev->p_next;
  }

  return p_result;
}

