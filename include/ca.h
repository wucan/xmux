#include "wu/wu_base_type.h"


enum {
	CA_DR_TAG = 0x09,
};

/*
 * libdvbpsi/src/descriptors/dr_09.h
 */
typedef struct dvbpsi_ca_dr_s
{
  uint16_t      i_ca_system_id;         /*!< CA_system_ID */
  uint16_t      i_ca_pid;               /*!< CA_PID */
  uint8_t       i_private_length;       /*!< length of the i_private_data
                                             array */
  uint8_t       i_private_data[251];    /*!< private_data_byte */

} dvbpsi_ca_dr_t;

