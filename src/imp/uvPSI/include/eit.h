/*****************************************************************************
 * eit.h
 */

#ifndef _DVBPSI_EIT_H_
#define _DVBPSI_EIT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dvbpsi_eit_event_s
{
  uint16_t                  i_event_id;          
  uint32_t                  i_start_time;     
   uint32_t                 i_duration;   
  uint8_t                   i_running_status;       /*!< Running status */
  uint8_t                    b_free_ca;              /*!< Free CA mode flag */
  uint16_t                  i_descriptors_length;  
  dvbpsi_descriptor_t *     p_first_descriptor;    

  struct dvbpsi_eit_event_s * p_next;            

} dvbpsi_eit_event_t;


typedef struct dvbpsi_eit_s
{
  uint16_t                  i_service_id;            /*!< transport_stream_id */
  uint8_t                   i_version;          /*!< version_number */
  uint8_t                   b_current_next;     /*!< current_next_indicator */
  uint16_t                  i_stream_id;       /*!< original network id */
  uint16_t                 i_orig_net_id;
  
  dvbpsi_eit_event_t *    p_first_event;    /*!< service description
                                                     list */

} dvbpsi_eit_t;


typedef void (* dvbpsi_eit_callback)(void* p_cb_data, dvbpsi_eit_t* p_new_eit);


dvbpsi_handle dvbpsi_AttachEIT(uint16_t i_service_id,
                               dvbpsi_eit_callback pf_callback,
                               void* p_cb_data);


void dvbpsi_DetachEIT(dvbpsi_handle h_dvbpsi);



void dvbpsi_InitEIT(dvbpsi_eit_t* p_eit, uint16_t i_service_id,
                    uint8_t i_version, int b_current_next, uint16_t i_stream_id, uint16_t i_orig_net_id);


#define dvbpsi_NewEIT(p_eit, i_service_id, i_version, b_current_next, i_stream_id, i_orig_net_id)  \
  			p_eit = (dvbpsi_eit_t*)malloc(sizeof(dvbpsi_eit_t));                  \
  			if(p_eit != NULL)                                                     \
    				dvbpsi_InitEIT(p_eit, i_service_id, i_version, b_current_next, i_stream_id, i_orig_net_id);



void dvbpsi_EmptyEIT(dvbpsi_eit_t* p_eit);

#define dvbpsi_DeleteEIT(p_eit)                                         \
  dvbpsi_EmptyEIT(p_eit);                                               \
  free(p_eit);

dvbpsi_eit_event_t* dvbpsi_EITAddEvent(dvbpsi_eit_t* p_eit,
                                 uint16_t i_event_id, uint32_t i_start_time, uint32_t i_duration, uint8_t i_running_status, uint8_t i_free_CA_mode);


dvbpsi_descriptor_t* dvbpsi_EITEventAddDescriptor(dvbpsi_eit_event_t* p_event,
                                               uint8_t i_tag, uint8_t i_length,
                                               uint8_t* p_data);


dvbpsi_psi_section_t* dvbpsi_GenEITSections(dvbpsi_eit_t* p_eit);


#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of pmt.h"
#endif

