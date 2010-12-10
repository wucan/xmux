/*****************************************************************************
 * nit.c: NIT decoder/generator
 *----------------------------------------------------------------------------
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
#include "nit.h"
#include "nit_private.h"


/*****************************************************************************
 * dvbpsi_AttachNIT
 *****************************************************************************
 * Initialize a NIT decoder and return a handle on it.
 *****************************************************************************/

dvbpsi_handle dvbpsi_AttachNIT(uint16_t i_net_id,
                               dvbpsi_nit_callback pf_callback,
                               void* p_cb_data)
{
  dvbpsi_handle h_dvbpsi = (dvbpsi_decoder_t*)malloc(sizeof(dvbpsi_decoder_t));
  dvbpsi_nit_decoder_t* p_nit_decoder;
  unsigned int i;

  if(h_dvbpsi == NULL)
    return NULL;

  p_nit_decoder = (dvbpsi_nit_decoder_t*)malloc(sizeof(dvbpsi_nit_decoder_t));

  if(p_nit_decoder == NULL)
  {
    free(h_dvbpsi);
    return NULL;
  }

  /* PSI decoder configuration */
  h_dvbpsi->pf_callback = &dvbpsi_GatherNITSections;
  h_dvbpsi->p_private_decoder = p_nit_decoder;
  h_dvbpsi->i_section_max_size = 1024;
  /* PSI decoder initial state */
  h_dvbpsi->i_continuity_counter = 31;
  h_dvbpsi->b_discontinuity = 1;
  h_dvbpsi->p_current_section = NULL;

  /* NIT decoder configuration */
  p_nit_decoder->i_net_id = i_net_id;
  p_nit_decoder->pf_callback = pf_callback;
  p_nit_decoder->p_cb_data = p_cb_data;

  /* NIT decoder initial state */
  p_nit_decoder->b_current_valid = 0;
  p_nit_decoder->p_building_nit = NULL;
  for(i = 0; i <= 255; i++)
    p_nit_decoder->ap_sections[i] = NULL;

  return h_dvbpsi;
}


/*****************************************************************************
 * dvbpsi_DetachNIT
 *****************************************************************************
 * Close a NIT decoder. The handle isn't valid any more.
 *****************************************************************************/
void dvbpsi_DetachNIT(dvbpsi_handle h_dvbpsi)
{
  dvbpsi_nit_decoder_t* p_nit_decoder
                        = (dvbpsi_nit_decoder_t*)h_dvbpsi->p_private_decoder;
  unsigned int i;

  free(p_nit_decoder->p_building_nit);

  for(i = 0; i <= 255; i++)              
  {
    if(p_nit_decoder->ap_sections[i])
      free(p_nit_decoder->ap_sections[i]);
  }

  free(h_dvbpsi->p_private_decoder);
  free(h_dvbpsi);
}


/*****************************************************************************
 * dvbpsi_InitNIT
 *****************************************************************************
 * Initialize a pre-allocated dvbpsi_nit_t structure.
 *****************************************************************************/
void dvbpsi_InitNIT(dvbpsi_nit_t* p_nit, uint16_t i_net_id,
                    uint8_t i_version, int b_current_next)

{
  p_nit->i_net_id = i_net_id;
  p_nit->i_version = i_version;
  p_nit->b_current_next = b_current_next;
  p_nit->p_first_descriptor = NULL;
  p_nit->p_first_stream = NULL;
}

void dvbpsi_EmptyNIT(dvbpsi_nit_t* p_nit)
{
  dvbpsi_nit_stream_t* p_stream = p_nit->p_first_stream;

  dvbpsi_DeleteDescriptors(p_nit->p_first_descriptor);

  while(p_stream != NULL)
  {
    dvbpsi_nit_stream_t* p_tmp = p_stream->p_next;
    dvbpsi_DeleteDescriptors(p_stream->p_first_descriptor);
    free(p_stream);
    p_stream = p_tmp;
  }

  p_nit->p_first_descriptor = NULL;
  p_nit->p_first_stream= NULL;
}
                        
dvbpsi_descriptor_t* dvbpsi_NITAddDescriptor(dvbpsi_nit_t* p_nit,
                                             uint8_t i_tag, uint8_t i_length,
                                             uint8_t* p_data)
{
  dvbpsi_descriptor_t* p_descriptor
                        = dvbpsi_NewDescriptor(i_tag, i_length, p_data);

  if(p_descriptor)
  {
    if(p_nit->p_first_descriptor == NULL)
    {
      p_nit->p_first_descriptor = p_descriptor;
    }
    else
    {
      dvbpsi_descriptor_t* p_last_descriptor = p_nit->p_first_descriptor;
      while(p_last_descriptor->p_next != NULL)
        p_last_descriptor = p_last_descriptor->p_next;
      p_last_descriptor->p_next = p_descriptor;
    }
  }

  return p_descriptor;
}

dvbpsi_nit_stream_t* dvbpsi_NITAddStream(dvbpsi_nit_t * p_nit, uint16_t i_stream_id, uint16_t i_orig_net_id)
{
  dvbpsi_nit_stream_t* p_stream = (dvbpsi_nit_stream_t*)malloc(sizeof(dvbpsi_nit_stream_t));

  if(p_stream)
  {
    p_stream->i_stream_id = i_stream_id;
    p_stream->i_orig_net_id = i_orig_net_id;
    p_stream->p_first_descriptor = NULL;
    p_stream->p_next = NULL;

    if(p_nit->p_first_stream == NULL)
    {
      	p_nit->p_first_stream = p_stream;
    }
    else
    {
      dvbpsi_nit_stream_t* p_last_stream = p_nit->p_first_stream;
      while(p_last_stream->p_next != NULL)
        p_last_stream = p_last_stream->p_next;
      p_last_stream->p_next = p_stream;
    }
  }
  return p_stream;
}


/*****************************************************************************
 * dvbpsi_PMTESAddDescriptor
 *****************************************************************************
 * Add a descriptor in the PMT ES.
 *****************************************************************************/
dvbpsi_descriptor_t* dvbpsi_NITStreamAddDescriptor(dvbpsi_nit_stream_t* p_stream,
                                               uint8_t i_tag, uint8_t i_length,
                                               uint8_t* p_data)
{
  dvbpsi_descriptor_t* p_descriptor
                        = dvbpsi_NewDescriptor(i_tag, i_length, p_data);

  if(p_descriptor)
  {
    if(p_stream->p_first_descriptor == NULL)
    {
      p_stream->p_first_descriptor = p_descriptor;
    }
    else
    {
      dvbpsi_descriptor_t* p_last_descriptor = p_stream->p_first_descriptor;
      while(p_last_descriptor->p_next != NULL)
        p_last_descriptor = p_last_descriptor->p_next;
      p_last_descriptor->p_next = p_descriptor;
    }
  }

  return p_descriptor;
}


void dvbpsi_GatherNITSections(dvbpsi_decoder_t* p_decoder,
                              dvbpsi_psi_section_t* p_section)
{
  dvbpsi_nit_decoder_t* p_nit_decoder
                        = (dvbpsi_nit_decoder_t*)p_decoder->p_private_decoder;
  int b_append = 1;
  int b_reinit = 0;
  unsigned int i;

  DVBPSI_DEBUG_ARG("NIT decoder",
                   "Table version %2d, " "i_extension %5d, "
                   "section %3d up to %3d, " "current %1d",
                   p_section->i_version, p_section->i_extension,
                   p_section->i_number, p_section->i_last_number,
                   p_section->b_current_next);

  if(p_section->i_table_id != 0x02)
  {
    /* Invalid table_id value */
    DVBPSI_ERROR_ARG("PMT decoder",
                     "invalid section (table_id == 0x%02x)",
                     p_section->i_table_id);
    b_append = 0;
  }

  if(b_append && !p_section->b_syntax_indicator)
  {
    /* Invalid section_syntax_indicator */
    DVBPSI_ERROR("PMT decoder",
                 "invalid section (section_syntax_indicator == 0)");
    b_append = 0;
  }

  /* Now if b_append is true then we have a valid NIT section */
  if(b_append && (p_nit_decoder->i_net_id != p_section->i_extension))
  {
    /* Invalid program_number */
    //DVBPSI_ERROR("PMT decoder", "'program_number' don't match");
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
      if(p_nit_decoder->p_building_nit)
      {
        if(p_nit_decoder->p_building_nit->i_version != p_section->i_version)
        {
          /* version_number */
          DVBPSI_ERROR("NIT decoder",
                       "'version_number' differs"
                       " whereas no discontinuity has occured");
          b_reinit = 1;
        }
        else if(p_nit_decoder->i_last_section_number
                                                != p_section->i_last_number)
        {
          /* last_section_number */
          DVBPSI_ERROR("NIT decoder",
                       "'last_section_number' differs"
                       " whereas no discontinuity has occured");
          b_reinit = 1;
        }
      }
      else
      {
        if(    (p_nit_decoder->b_current_valid)
            && (p_nit_decoder->current_nit.i_version == p_section->i_version))
        {
          /* Signal a new PMT if the previous one wasn't active */
          if(    (!p_nit_decoder->current_nit.b_current_next)
              && (p_section->b_current_next))
          {
            dvbpsi_nit_t* p_nit = (dvbpsi_nit_t*)malloc(sizeof(dvbpsi_nit_t));

            p_nit_decoder->current_nit.b_current_next = 1;
            *p_nit = p_nit_decoder->current_nit;
            p_nit_decoder->pf_callback(p_nit_decoder->p_cb_data, p_nit);
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
    p_nit_decoder->b_current_valid = 0;
    /* Free structures */
    if(p_nit_decoder->p_building_nit)
    {
      free(p_nit_decoder->p_building_nit);
      p_nit_decoder->p_building_nit = NULL;
    }
    /* Clear the section array */
    for(i = 0; i <= 255; i++)
    {
      if(p_nit_decoder->ap_sections[i] != NULL)
      {
        dvbpsi_DeletePSISections(p_nit_decoder->ap_sections[i]);
        p_nit_decoder->ap_sections[i] = NULL;
      }
    }
  }

  /* Append the section to the list if wanted */
  if(b_append)
  {
    int b_complete;

    /* Initialize the structures if it's the first section received */
    if(!p_nit_decoder->p_building_nit)
    {
      p_nit_decoder->p_building_nit =
                                (dvbpsi_nit_t*)malloc(sizeof(dvbpsi_nit_t));
      dvbpsi_InitNIT(p_nit_decoder->p_building_nit,
                     p_nit_decoder->i_net_id,
                     p_section->i_version,
                     p_section->b_current_next);
      p_nit_decoder->i_last_section_number = p_section->i_last_number;
    }

    /* Fill the section array */
    if(p_nit_decoder->ap_sections[p_section->i_number] != NULL)
    {
      DVBPSI_DEBUG_ARG("NIT decoder", "overwrite section number %d",
                       p_section->i_number);
      dvbpsi_DeletePSISections(p_nit_decoder->ap_sections[p_section->i_number]);
    }
    p_nit_decoder->ap_sections[p_section->i_number] = p_section;

    /* Check if we have all the sections */
    b_complete = 0;
    for(i = 0; i <= p_nit_decoder->i_last_section_number; i++)
    {
      if(!p_nit_decoder->ap_sections[i])
        break;

      if(i == p_nit_decoder->i_last_section_number)
        b_complete = 1;
    }

    if(b_complete)
    {
      /* Save the current information */
      p_nit_decoder->current_nit = *p_nit_decoder->p_building_nit;
      p_nit_decoder->b_current_valid = 1;
      /* Chain the sections */
      if(p_nit_decoder->i_last_section_number)
      {
        for(i = 0; i <= p_nit_decoder->i_last_section_number - 1; i++)
          p_nit_decoder->ap_sections[i]->p_next =
                                        p_nit_decoder->ap_sections[i + 1];
      }
      /* Decode the sections */
      dvbpsi_DecodeNITSections(p_nit_decoder->p_building_nit,
                               p_nit_decoder->ap_sections[0]);
      /* Delete the sections */
      dvbpsi_DeletePSISections(p_nit_decoder->ap_sections[0]);
      /* signal the new NIT */
      p_nit_decoder->pf_callback(p_nit_decoder->p_cb_data,
                                 p_nit_decoder->p_building_nit);
      /* Reinitialize the structures */
      p_nit_decoder->p_building_nit = NULL;
      for(i = 0; i <= p_nit_decoder->i_last_section_number; i++)
        p_nit_decoder->ap_sections[i] = NULL;
    }
  }
  else
  {
    dvbpsi_DeletePSISections(p_section);
  }
}


void dvbpsi_DecodeNITSections(dvbpsi_nit_t* p_nit,
                              dvbpsi_psi_section_t* p_section)
{
  uint8_t* p_byte, * p_end;

  while(p_section)
  {
    /* - NIT descriptors */
    p_byte = p_section->p_payload_start + 4;
    p_end = p_byte + (   ((uint16_t)(p_section->p_payload_start[2] & 0x0f) << 8)
                       | p_section->p_payload_start[3]);
    while(p_byte + 2 < p_end)
    {
      uint8_t i_tag = p_byte[0];
      uint8_t i_length = p_byte[1];
      if(i_length + 2 <= p_end - p_byte)
        dvbpsi_NITAddDescriptor(p_nit, i_tag, i_length, p_byte + 2);
      p_byte += 2 + i_length;
    }

    /* - treams */
    for(p_byte = p_end; p_byte + 5 <= p_section->p_payload_end;)
    {
      uint8_t i_type = p_byte[0];
      uint16_t i_pid = ((uint16_t)(p_byte[1] & 0x1f) << 8) | p_byte[2];
      uint16_t i_length = ((uint16_t)(p_byte[3] & 0x0f) << 8) | p_byte[4];
      dvbpsi_nit_stream_t* p_stream = dvbpsi_NITAddStream(p_nit, i_type, i_pid);
      /* - ES descriptors */
      p_byte += 5;
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
          dvbpsi_NITStreamAddDescriptor(p_stream, i_tag, i_length, p_byte + 2);
        p_byte += 2 + i_length;
      }
    }

    p_section = p_section->p_next;
  }
}

dvbpsi_psi_section_t* dvbpsi_GenNITSections(dvbpsi_nit_t* p_nit)
{
  dvbpsi_psi_section_t* p_result = dvbpsi_NewPSISection(1024);
  dvbpsi_psi_section_t* p_current = p_result;
  dvbpsi_psi_section_t* p_prev;
  dvbpsi_descriptor_t* p_descriptor = p_nit->p_first_descriptor;
  dvbpsi_nit_stream_t* p_stream = p_nit->p_first_stream;
  uint16_t i_net_descr_length = 0;
  uint16_t i_stream_length = 0; 

  p_current->i_table_id = 0x40;
  p_current->b_syntax_indicator = 1;
  p_current->b_private_indicator = 0;
  p_current->i_length = 11;   // after header + CRC32               
  p_current->i_extension = p_nit->i_net_id;
  p_current->i_version = p_nit->i_version;
  p_current->b_current_next = p_nit->b_current_next;
  p_current->i_number = 0;
  p_current->p_payload_end += 10;        
  p_current->p_payload_start = p_current->p_data + 8;

  // NIT Net Descr Length
  p_current->p_data[8] = 0xF0;
  p_current->p_data[9] = 0x00;

  /* NIT descriptors */
  while(p_descriptor != NULL)
  {
    /* New section if needed */
    /* written_data_length + descriptor_length + 2 > 1024 - CRC_32_length */
    if(   (p_current->p_payload_end - p_current->p_data)
                                + p_descriptor->i_length > 1018)
    {
  
      i_net_descr_length = (p_current->p_payload_end - p_current->p_data) - 10;
      p_current->p_data[8] = (i_net_descr_length >> 8) | 0xf0;
      p_current->p_data[9] = i_net_descr_length;

      p_prev = p_current;
      p_current = dvbpsi_NewPSISection(1024);
      p_prev->p_next = p_current;

      p_current->i_table_id = 0x40;
      p_current->b_syntax_indicator = 1;
      p_current->b_private_indicator = 0;
      p_current->i_length = 11;                 /* header + CRC_32 */
      p_current->i_extension = p_nit->i_net_id;
      p_current->i_version = p_nit->i_version;
      p_current->b_current_next = p_nit->b_current_next;
      p_current->i_number = p_prev->i_number + 1;
      p_current->p_payload_end += 10;           /* just after the header */
      p_current->p_payload_start = p_current->p_data + 8;

    }

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

  i_net_descr_length = (p_current->p_payload_end - p_current->p_data) - 10;
  p_current->p_data[8] = (i_net_descr_length >> 8) ;
  p_current->p_data[9] = i_net_descr_length;

  p_current->p_payload_end += 2;
  p_current->i_length += 2;

  /* NIT Streams */
  while(p_stream != NULL)
  {
	uint8_t* p_stream_start = p_current->p_payload_end;
	uint16_t i_stream_descr_length = 6;

	/* Can the current section carry all the descriptors ? */
	p_descriptor = p_stream->p_first_descriptor;
	while(    (p_descriptor != NULL)
		&& ((p_stream_start - p_current->p_data) + i_stream_length <= 1020))
	{
		i_stream_descr_length += p_descriptor->i_length + 2;
		p_descriptor = p_descriptor->p_next;
	}

	/* If _no_ and the current section isn't empty and an empty section
	may carry one more descriptor
	then create a new section */
	if(    (p_descriptor != NULL)
		&& (p_stream_start - p_current->p_data != 12)
		&& (i_stream_descr_length <= 1008))
	{
		/* will put more descriptors in an empty section */
		DVBPSI_DEBUG("NIT generator",
		"create a new section to carry more descriptors");
		p_prev = p_current;
		p_current = dvbpsi_NewPSISection(1024);
		p_prev->p_next = p_current;

		p_current->i_table_id = 0x40;
		p_current->b_syntax_indicator = 1;
		p_current->b_private_indicator = 0;
		p_current->i_length = 11;                 /* header + CRC_32 */
		p_current->i_extension = p_nit->i_net_id;
		p_current->i_version = p_nit->i_version;
		p_current->b_current_next = p_nit->b_current_next;
		p_current->i_number = p_prev->i_number + 1;
		p_current->p_payload_end += 10;           /* just after the header */
		p_current->p_payload_start = p_current->p_data + 8;

		p_current->p_data[10] = 0xF0;
		p_current->p_data[11] = 0;
		p_stream_start = p_current->p_payload_end+2;
	}

	/* p_stream_start is where the Stream begins */
	p_stream_start[0] = p_stream->i_stream_id >> 8;
	p_stream_start[1] = p_stream->i_stream_id;
	p_stream_start[2] = p_stream->i_orig_net_id >> 8;
	p_stream_start[3] = p_stream->i_orig_net_id;

	/* Increase the length by 6 */
	i_stream_length += 6;
	p_current->p_payload_end += 6;
	p_current->i_length += 6;

	/* NIT Stream descriptors */
	p_descriptor = p_stream->p_first_descriptor;
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
		DVBPSI_ERROR("NIT generator", "unable to carry all the Stream descriptors");

		/*transport descriptor length */
		i_stream_descr_length = p_current->p_payload_end - p_stream_start - 6;
		p_stream_start[4] = (i_stream_length >> 8) | 0xF0;
		p_stream_start[5] = i_stream_length;

		i_stream_length += i_stream_descr_length;

		p_stream = p_stream->p_next;
  }// for each stream
  
  p_current->p_data[10+i_net_descr_length] = (i_stream_length >> 8) | 0xF0;
  p_current->p_data[10+i_net_descr_length+1] = i_stream_length;
 
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
