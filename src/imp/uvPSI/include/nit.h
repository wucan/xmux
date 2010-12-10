/*****************************************************************************
 * nit.h
 *****************************************************************************/


#ifndef _DVBPSI_NIT_H_
#define _DVBPSI_NIT_H_

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * dvbpsi_pmt_es_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_pmt_es_s
 * \brief PMT ES structure.
 *
 * This structure is used to store a decoded ES description.
 * (ISO/IEC 13818-1 section 2.4.4.8).
 */
/*!
 * \typedef struct dvbpsi_pmt_es_s dvbpsi_pmt_es_t
 * \brief dvbpsi_pmt_es_t type definition.
 */
typedef struct dvbpsi_nit_stream_s
{
  uint16_t                      i_stream_id;                
  uint16_t                      i_orig_net_id;                 

  dvbpsi_descriptor_t *         p_first_descriptor;     /*!< descriptor list */

  struct dvbpsi_nit_stream_s *      p_next;                 /*!< next element of
                                                             the list */

} dvbpsi_nit_stream_t;


/*****************************************************************************
 * dvbpsi_pmt_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_pmt_s
 * \brief PMT structure.
 *
 * This structure is used to store a decoded PMT.
 * (ISO/IEC 13818-1 section 2.4.4.8).
 */
/*!
 * \typedef struct dvbpsi_pmt_s dvbpsi_pmt_t
 * \brief dvbpsi_pmt_t type definition.
 */
typedef struct dvbpsi_nit_s
{
  uint16_t                  i_net_id;  
  uint8_t                   i_version;          /*!< version_number */
  int                       b_current_next;     /*!< current_next_indicator */
  
  dvbpsi_descriptor_t *     p_first_descriptor; /*!< descriptor list */

  dvbpsi_nit_stream_t *         p_first_stream;         /*!< ES list */

} dvbpsi_nit_t;


/*****************************************************************************
 * dvbpsi_nit_callback
 *****************************************************************************/
/*!
 * \typedef void (* dvbpsi_nit_callback)(void* p_cb_data,
                                         dvbpsi_nit_t* p_new_pmt)
 * \brief Callback type definition.
 */
typedef void (* dvbpsi_nit_callback)(void* p_cb_data, dvbpsi_nit_t* p_new_nit);


/*****************************************************************************
 * dvbpsi_AttachNIT
 *****************************************************************************/
/*!
 * \fn dvbpsi_handle dvbpsi_AttachNIT(uint16_t i_net_id,
                                      dvbpsi_nit_callback pf_callback,
                                      void* p_cb_data)
 * \brief Creation and initialization of a PMT decoder.
 * \param i_net_id
 * \param pf_callback function to call back on new CAT
 * \param p_cb_data private data given in argument to the callback
 * \return a pointer to the decoder for future calls.
 */
dvbpsi_handle dvbpsi_AttachNIT(uint16_t i_net_id,
                               dvbpsi_nit_callback pf_callback,
                               void* p_cb_data);


/*****************************************************************************
 * dvbpsi_DetachNIT
 *****************************************************************************/
/*!
 * \fn void dvbpsi_DetachNIT(dvbpsi_handle h_dvbpsi)
 * \brief Destroy a NIT decoder.
 * \param h_dvbpsi handle to the decoder
 * \return nothing.
 *
 * The handle isn't valid any more.
 */
void dvbpsi_DetachNIT(dvbpsi_handle h_dvbpsi);


void dvbpsi_InitNIT(dvbpsi_nit_t* p_nit, uint16_t i_net_id,
                    uint8_t i_version, int b_current_next);

 
#define dvbpsi_NewNIT(p_nit, i_net_id,                          \
                      i_version, b_current_next)             \
  p_nit = (dvbpsi_nit_t*)malloc(sizeof(dvbpsi_nit_t));                  \
  if(p_nit != NULL)                                                     \
    dvbpsi_InitNIT(p_nit, i_net_id, i_version, b_current_next);


void dvbpsi_EmptyNIT(dvbpsi_nit_t* p_nit);

/*!
 * \def dvbpsi_DeleteNIT(p_nit)
 * \brief Clean and free a dvbpsi_nit_t structure.
 * \param p_nit pointer to the NIT structure
 * \return nothing.
 */
#define dvbpsi_DeleteNIT(p_nit)                                         \
  dvbpsi_EmptyNIT(p_nit);                                               \
  free(p_nit);


/*****************************************************************************
 * dvbpsi_NITTAddDescriptor
 *****************************************************************************/
/*!
 * \fn dvbpsi_descriptor_t* dvbpsi_NITTAddDescriptor(dvbpsi_nit_t* p_nit,
                                                    uint8_t i_tag,
                                                    uint8_t i_length,
                                                    uint8_t* p_data)
 * \brief Add a descriptor in the NIT.
 * \param p_pmt pointer to the NIT structure
 * \param i_tag descriptor's tag
 * \param i_length descriptor's length
 * \param p_data descriptor's data
 * \return a pointer to the added descriptor.
 */
dvbpsi_descriptor_t* dvbpsi_NITAddDescriptor(dvbpsi_nit_t* p_nit,
                                             uint8_t i_tag, uint8_t i_length,
                                             uint8_t* p_data);


dvbpsi_nit_stream_t* dvbpsi_NITAddStream(dvbpsi_nit_t* p_nit,
                                 uint16_t i_stream_id, uint16_t i_orig_net_id);


/*****************************************************************************
 * dvbpsi_PMTESAddDescriptor
 *****************************************************************************/

dvbpsi_descriptor_t* dvbpsi_NITStreamAddDescriptor(dvbpsi_nit_stream_t* p_stream,
                                               uint8_t i_tag, uint8_t i_length,
                                               uint8_t* p_data);


/*
 * Generate NIT sections based on the dvbpsi_nit_t structure.
 */
dvbpsi_psi_section_t* dvbpsi_GenNITSections(dvbpsi_nit_t* p_nit);


#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of nit.h"
#endif

