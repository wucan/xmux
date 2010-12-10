/*****************************************************************************
 * descriptor.h
 * (c)2001-2002 VideoLAN
 * $Id: descriptor.h,v 1.5 2002/05/08 13:00:40 bozo Exp $
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
 *****************************************************************************/

/*!
 * \file <descriptor.h>
 * \author Arnaud de Bossoreille de Ribou <bozo@via.ecp.fr>
 * \brief Common descriptor tools.
 *
 * Descriptor structure and its Manipulation tools.
 */

#ifndef _DVBPSI_DESCRIPTOR_H_
#define _DVBPSI_DESCRIPTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * from dvbsnoop
  MPEG
  DVB descriptors: tags and names
 *****************************************************************************/
 
#ifdef USE_MSG_DESCR 
typedef struct _decr_table {
    uint8_t    tag;          
    uint8_t   *str;          
} str_descr_table;

static str_descr_table  descr_mpeg_tags[] = {
	{  0x00, "Reserved" },
	{  0x02, "video_stream_descriptor" },
	{  0x03, "audio_stream_descriptor" },
	{  0x04, "hierarchy_descriptor" },
	{  0x05, "registration_descriptor" },
	{  0x06, "data_stream_alignment_descriptor" },
	{  0x07, "target_background_grid_descriptor" },
	{  0x08, "video_window_descriptor" },
	{  0x09, "CA_descriptor" },
	{  0x0A, "ISO_639_language_descriptor" },
	{  0x0B, "system_clock_descriptor" },
	{  0x0C, "multiplex_buffer_utilization_descriptor" },
	{  0x0D, "copyright_descriptor" },
	{  0x0E, "maximum_bitrate_descriptor" },
	{  0x0F, "private_data_indicator_descriptor" },
	{  0x10, "smoothing_buffer_descriptor" },
	{  0x11, "STD_descriptor" },
	{  0x12, "IBP_descriptor" },
	  /* MPEG DSM-CC */
	{  0x13, "carousel_identifier_descriptor" },
	{  0x14, "association_tag_descriptor" },
	{  0x15, "deferred_association_tag_descriptor" },
	{  0x16, 0x16,  "ISO/IEC13818-6 Reserved" },
	  /* DSM-CC stream descriptors */
	{  0x17, "NPT_reference_descriptor" },
	{  0x18, "NPT_endpoint_descriptor" },
	{  0x19, "stream_mode_descriptor" },
	{  0x1A, "stream_event_descriptor" },
	  /* MPEG-4 descriptors */
	{  0x1B, "MPEG4_video_descriptor" },
	{  0x1C, "MPEG4_audio_descriptor" },
	{  0x1D, "IOD_descriptor" },
	{  0x1E, "SL_descriptor" },
	{  0x1F, "FMC_descriptor" },
	{  0x20, "External_ES_ID_descriptor" },
	{  0x21, "MuxCode_descriptor" },
	{  0x22, "FMXBufferSize_descriptor" },
	{  0x23, "MultiplexBuffer_descriptor" },
	{  0x24, "Content_labeling_descriptor" },
	/* TV ANYTIME TS 102 323 descriptors, ISO 13818-1 */
	{  0x25, "metadata_pointer_descriptor" },
	{  0x26, "metadata_descriptor" },
	{  0x27, "metadata_STD_descriptor" },
	{  0x28, "AVC_video_descriptor" },
	{  0x29, "IPMP_descriptor (MPEG-2 IPMP, ISO 13818-11)" },
	{  0x2A, "AVC_timing_and_HRD_descriptor" },
	{  0x2B, "ITU-T.Rec.H.222.0|ISO/IEC13818-1 Reserved" },

	{  0x40, "Forbidden descriptor in MPEG context" },	// DVB Context
	{  0, 0, NULL }
};

static str_descr_table  descr_dvb_tags[] = {
     {  0x00, "Forbidden descriptor in DVB context" },   // MPEG Context
	// ETSI 300 468
	// updated EN 302 192 v 1.4.1
     {  0x40, "network_name_descriptor" },
     {  0x41, "service_list_descriptor" },
     {  0x42, "stuffing_descriptor" },
     {  0x43, "satellite_delivery_system_descriptor" },
     {  0x44, "cable_delivery_system_descriptor" },
     {  0x45, "VBI_data_descriptor" },
     {  0x46, "VBI_teletext_descriptor" },
     {  0x47, "bouquet_name_descriptor" },
     {  0x48, "service_descriptor" },
     {  0x49, "country_availibility_descriptor" },
     {  0x4A, "linkage_descriptor" },
     {  0x4B, "NVOD_reference_descriptor" },
     {  0x4C, "time_shifted_service_descriptor" },
     {  0x4D, "short_event_descriptor" },
     {  0x4E, "extended_event_descriptor" },
     {  0x4F, "time_shifted_event_descriptor" },
     {  0x50, "component_descriptor" },
     {  0x51, "mosaic_descriptor" },
     {  0x52, "stream_identifier_descriptor" },
     {  0x53, "CA_identifier_descriptor" },
     {  0x54, "content_descriptor" },
     {  0x55, "parental_rating_descriptor" },
     {  0x56, "teletext_descriptor" },
     {  0x57, "telephone_descriptor" },
     {  0x58, "local_time_offset_descriptor" },
     {  0x59, "subtitling_descriptor" },
     {  0x5A, "terrestrial_delivery_system_descriptor" },
     {  0x5B, "multilingual_network_name_descriptor" },
     {  0x5C, "multilingual_bouquet_name_descriptor" },
     {  0x5D, "multilingual_service_name_descriptor" },
     {  0x5E, "multilingual_component_descriptor" },
     {  0x5F, "private_data_specifier_descriptor" },
     {  0x60, "service_move_descriptor" },
     {  0x61, "short_smoothing_buffer_descriptor" },
     {  0x62, "frequency_list_descriptor" },
     {  0x63, "partial_transport_stream_descriptor" },
     {  0x64, "data_broadcast_descriptor" },
     {  0x65, "CA_system_descriptor" },
     {  0x66, "data_broadcast_id_descriptor" },
     {  0x67, "transport_stream_descriptor" },
     {  0x68, "DSNG_descriptor" },
     {  0x69, "PDC_descriptor" },
     {  0x6A, "AC3_descriptor" },
     {  0x6B, "ancillary_data_descriptor" },
     {  0x6C, "cell_list_descriptor" },
     {  0x6D, "cell_frequency_list_descriptor" },
     {  0x6E, "announcement_support_descriptor" },
     {  0x6F, "application_signalling_descriptor" },
     {  0x70, "adaptation_field_data_descriptor" },
     {  0x71, "service_identifier_descriptor" },
     {  0x72, "service_availability_descriptor" },
     {  0x73, "default_authority_descriptor" }, 		// TS 102 323
     {  0x74, "related_content_descriptor" }, 		// TS 102 323
     {  0x75, "TVA_id_descriptor" }, 			// TS 102 323
     {  0x76, "content_identifier_descriptor" }, 	// TS 102 323
     {  0x77, "time_slice_fec_identifier_descriptor" }, 	// EN 300 468 v1.6.1
     {  0x78, "ECM_repetition_rate_descriptor" }, 	// EN 300 468 v1.6.1
     {  0x79, "reserved_descriptor" },
     {  0x80, "User defined/ATSC reserved" },		/* ETR 211e02 */
     {  0xB0, "User defined" },
     {  0xFF, "Forbidden" },
	 {  0,0, NULL }
  }; 
  
#endif
/*****************************************************************************
 * dvbpsi_descriptor_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_descriptor_s
 * \brief Descriptor structure.
 *
 * This structure is used to store a descriptor.
 * (ISO/IEC 13818-1 section 2.6).
 */
/*!
 * \typedef struct dvbpsi_descriptor_s dvbpsi_descriptor_t
 * \brief dvbpsi_descriptor_t type definition.
 */
typedef struct dvbpsi_descriptor_s
{
  uint8_t                       i_tag;          /*!< descriptor_tag */
  uint8_t                       i_length;       /*!< descriptor_length */

  uint8_t *                     p_data;         /*!< content */

  struct dvbpsi_descriptor_s *  p_next;         /*!< next element of
                                                     the list */

  void *                        p_decoded;      /*!< decoded descriptor */

} dvbpsi_descriptor_t;


/*****************************************************************************
 * dvbpsi_NewDescriptor
 *****************************************************************************/
/*!
 * \fn dvbpsi_descriptor_t* dvbpsi_NewDescriptor(uint8_t i_tag,
                                                 uint8_t i_length,
                                                 uint8_t* p_data)
 * \brief Creation of a new dvbpsi_descriptor_t structure.
 * \param i_tag descriptor's tag
 * \param i_length descriptor's length
 * \param p_data descriptor's data
 * \return a pointer to the descriptor.
 */
dvbpsi_descriptor_t* dvbpsi_NewDescriptor(uint8_t i_tag, uint8_t i_length,
                                          uint8_t* p_data);


/*****************************************************************************
 * dvbpsi_DeleteDescriptors
 *****************************************************************************/
/*!
 * \fn void dvbpsi_DeleteDescriptors(dvbpsi_descriptor_t* p_descriptor)
 * \brief Destruction of a dvbpsi_descriptor_t structure.
 * \param p_descriptor pointer to the first descriptor structure
 * \return nothing.
 */
void dvbpsi_DeleteDescriptors(dvbpsi_descriptor_t* p_descriptor);


#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of descriptor.h"
#endif

