/*****************************************************************************
 * cat.h
 *****************************************************************************/

#ifndef _DVBPSI_CAT_H_
#define _DVBPSI_CAT_H_

#ifdef __cplusplus
extern "C" {
#endif


/*!
 * \struct dvbpsi_cat_s
 * \brief CAT structure.
 *
 * This structure is used to store a decoded CAT.
 * (ISO/IEC 13818-1 section 2.4.4.8).
 */
/*!
 * \typedef struct dvbpsi_pmt_s dvbpsi_cat_t
 * \brief dvbpsi_cat_t type definition.
 */
typedef struct dvbpsi_cat_s
{
  uint8_t                   i_version;          /*!< version_number */
  int                      	 b_current_next;     /*!< current_next_indicator */
  dvbpsi_descriptor_t *     p_first_descriptor; /*!< descriptor list */

} dvbpsi_cat_t;


/*****************************************************************************
 * dvbpsi_pmt_callback
 *****************************************************************************/
/*!
 * \typedef void (* dvbpsi_pmt_callback)(void* p_cb_data,
                                         dvbpsi_pmt_t* p_new_pmt)
 * \brief Callback type definition.
 */
typedef void (* dvbpsi_cat_callback)(void* p_cb_data, dvbpsi_cat_t* p_new_cat);


/*****************************************************************************
 * dvbpsi_AttachCAT
 *****************************************************************************/
/*!
 * \brief Creation and initialization of a CAT decoder.
 * \param pf_callback function to call back on new CAT
 * \param p_cb_data private data given in argument to the callback
 * \return a pointer to the decoder for future calls.
 */
dvbpsi_handle dvbpsi_AttachCAT(dvbpsi_cat_callback pf_callback, void* p_cb_data);


/*****************************************************************************
 * dvbpsi_DetachCAT
 *****************************************************************************/
/*!
 * \fn void dvbpsi_DetachCAT(dvbpsi_handle h_dvbpsi)
 * \brief Destroy a CAT decoder.
 * \param h_dvbpsi handle to the decoder
 * \return nothing.
 *
 * The handle isn't valid any more.
 */
void dvbpsi_DetachCAT(dvbpsi_handle h_dvbpsi);


/*****************************************************************************
 * dvbpsi_InitCAT/dvbpsi_NewCAT
 *****************************************************************************/
/*!
 * \brief Initialize a user-allocated dvbpsi_cat_t structure.
 * \param p_pmt pointer to the CAT structure
 * \param i_version CAT version
 * \param b_current_next current next indicator
 * \return nothing.
 */
void dvbpsi_InitCAT(dvbpsi_cat_t* p_cat, uint8_t i_version, int b_current_next);

#define dvbpsi_NewCAT(p_cat,                       						\
                      i_version, b_current_next)            	\
  p_cat = (dvbpsi_cat_t*)malloc(sizeof(dvbpsi_cat_t));                  \
  if(p_cat != NULL)                                                     \
    dvbpsi_InitCAT(p_cat, i_version, b_current_next);
                


void dvbpsi_EmptyCAT(dvbpsi_cat_t* p_cat);


#define dvbpsi_DeleteCAT(p_cat)                                         \
  dvbpsi_EmptyCAT(p_cat);                                               \
  free(p_cat);


dvbpsi_descriptor_t* dvbpsi_CATAddDescriptor(dvbpsi_cat_t* p_cat,
                                             uint8_t i_tag, uint8_t i_length,
                                             uint8_t* p_data);



dvbpsi_psi_section_t* dvbpsi_GenCATSections(dvbpsi_cat_t* p_cat);


#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of cat.h"
#endif

