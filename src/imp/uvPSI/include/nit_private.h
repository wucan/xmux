
/*****************************************************************************
 * nit_private.h: private NIT structures
 *----------------------------------------------------------------------------
 -----------------------------------------------------------
 *
 *****************************************************************************/

#ifndef _DVBPSI_NIT_PRIVATE_H_
#define _DVBPSI_NIT_PRIVATE_H_
/*****************************************************************************
 * dvbpsi_nit_decoder_t
 *****************************************************************************
 * NIT decoder.
 *****************************************************************************/
typedef struct dvbpsi_nit_decoder_s
{
  uint16_t                      i_net_id;

  dvbpsi_nit_callback           pf_callback;
  void *                        p_cb_data;

  dvbpsi_nit_t                  current_nit;
  dvbpsi_nit_t *                p_building_nit;

  int                           b_current_valid;

  uint8_t                       i_last_section_number;
  dvbpsi_psi_section_t *        ap_sections [256];

} dvbpsi_nit_decoder_t;


/*****************************************************************************
 * dvbpsi_GatherNITSections
 *****************************************************************************
 * Callback for the PSI decoder.
 *****************************************************************************/
void dvbpsi_GatherNITSections(dvbpsi_decoder_t* p_decoder,
                              dvbpsi_psi_section_t* p_section);


/*****************************************************************************
 * dvbpsi_DecodeNITSections
 *****************************************************************************
 * NIT decoder.
 *****************************************************************************/
void dvbpsi_DecodeNITSections(dvbpsi_nit_t* p_pmt,
                              dvbpsi_psi_section_t* p_section);


#else
#error "Multiple inclusions of pmt_private.h"
#endif
