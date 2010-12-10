/*****************************************************************************
 * eit_private.h: private SDT structures
 *----------------------------------------------------------------------------
 ------------------------------------------------
 *****************************************************************************/

#ifndef _DVBPSI_EIT_PRIVATE_H_
#define _DVBPSI_EIT_PRIVATE_H_


/*****************************************************************************
 * dvbpsi_eit_decoder_t
 *****************************************************************************
 * EIT decoder.
 *****************************************************************************/
typedef struct dvbpsi_eit_decoder_s
{
  uint16_t                      i_service_id;
  dvbpsi_eit_callback           pf_callback;
  void *                        p_cb_data;

  dvbpsi_eit_t                  current_eit;
  dvbpsi_eit_t *                p_building_eit;

  int                           b_current_valid;

  uint8_t                       i_last_section_number;
  dvbpsi_psi_section_t *        ap_sections [256];

} dvbpsi_eit_decoder_t;


/*****************************************************************************
 * dvbpsi_GatherEITSections
 *****************************************************************************
 * Callback for the PSI decoder.
 *****************************************************************************/
void dvbpsi_GatherEITSections(dvbpsi_decoder_t* p_psi_decoder,
		              void* p_private_decoder,
                              dvbpsi_psi_section_t* p_section);


/*****************************************************************************
 * dvbpsi_DecodeEITSections
 *****************************************************************************
 * SDT decoder.
 *****************************************************************************/
void dvbpsi_DecodeEITSections(dvbpsi_eit_t* p_eit,
                              dvbpsi_psi_section_t* p_section);


#else
#error "Multiple inclusions of eit_private.h"
#endif
