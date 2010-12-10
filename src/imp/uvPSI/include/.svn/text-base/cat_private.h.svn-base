/*****************************************************************************
 * cat_private.h: private CAT structures
 *****************************************************************************/

#ifndef _DVBPSI_CAT_PRIVATE_H_
#define _DVBPSI_CAT_PRIVATE_H_

/*****************************************************************************
 * dvbpsi_cat_decoder_t
 *****************************************************************************
 * CAT decoder.
 *****************************************************************************/
typedef struct dvbpsi_cat_decoder_s
{
  dvbpsi_cat_callback   pf_callback;
  void *                        p_cb_data;

  dvbpsi_cat_t                  current_cat;
  dvbpsi_cat_t *                p_building_cat;

  int                           b_current_valid;

  uint8_t                       i_last_section_number;
  dvbpsi_psi_section_t *        ap_sections [256];

} dvbpsi_cat_decoder_t;


/*****************************************************************************
 * dvbpsi_GatherCATSections
 *****************************************************************************
 * Callback for the PSI decoder.
 *****************************************************************************/
void dvbpsi_GatherCATSections(dvbpsi_decoder_t* p_decoder,
                              dvbpsi_psi_section_t* p_section);


/*****************************************************************************
 * dvbpsi_DecodeCATSections
 *****************************************************************************
 * CAT decoder.
 *****************************************************************************/
void dvbpsi_DecodeCATSections(dvbpsi_cat_t* p_pmt,
                              dvbpsi_psi_section_t* p_section);


#else
#error "Multiple inclusions of cat_private.h"
#endif

