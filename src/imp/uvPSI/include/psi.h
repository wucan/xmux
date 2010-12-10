/*****************************************************************************
 * psi.h
 * (c)2001-2002 VideoLAN
 * $Id: psi.h,v 1.6 2002/04/02 17:55:30 bozo Exp $
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
 * \file <psi.h>
 * \author Arnaud de Bossoreille de Ribou <bozo@via.ecp.fr>
 * \brief Common PSI tools.
 *
 * PSI section structure and its Manipulation tools.
 */

#ifndef _DVBPSI_PSI_H_
#define _DVBPSI_PSI_H_

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * dvbpsi_psi_section_t
 *****************************************************************************/
/*!
 * \struct dvbpsi_psi_section_s
 * \brief PSI section structure.
 *
 * This structure is used to store a PSI section. The common information are
 * decoded (ISO/IEC 13818-1 section 2.4.4.10).
 *
 * dvbpsi_psi_section_s::p_data stores the complete section including the
 * header.
 *
 * When dvbpsi_psi_section_s::b_syntax_indicator == 0,
 * dvbpsi_psi_section_s::p_payload_start points immediately after the
 * section_length field and dvbpsi_psi_section_s::p_payload_end points
 * immediately after the end of the section (don't try to access this byte).
 *
 * When dvbpsi_psi_section_s::b_syntax_indicator != 0,
 * dvbpsi_psi_section_s::p_payload_start points immediately after the
 * last_section_number field and dvbpsi_psi_section_s::p_payload_end points to
 * the first byte of the CRC_32 field.
 *
 * When dvbpsi_psi_section_s::b_syntax_indicator == 0
 * dvbpsi_psi_section_s::i_extension, dvbpsi_psi_section_s::i_version,
 * dvbpsi_psi_section_s::b_current_next, dvbpsi_psi_section_s::i_number,
 * dvbpsi_psi_section_s::i_last_number, and dvbpsi_psi_section_s::i_crc are
 * undefined.
 */


// define TS analyse operation type
// ---- default is decode the content,
// ---- section denotes to only input or output section
typedef enum {
	EUV_DEFAULT = 0, 
	EUV_SECTION,
	EUV_BOTH,
	EUV_DEC_FROM_SECTION
} UV_SI_OPS_TYPE;

typedef enum {
	EUV_TBL_PAT = 0,
	EUV_TBL_PMT,
	EUV_TBL_CAT,
	EUV_TBL_NIT,
	EUV_TBL_SDT,
	EUV_TBL_EIT,
} UV_SI_TBL_TYPE;

#define UV_MAX_SI_SECTION_CNT (6)

//#pragma pack(push)
//#pragma pack(1)

/*ͨ��״̬��ͨ��1-8�Ƿ�������ָʾ��־����־0δ������1Ϊ������ɡ�MSBΪ8ͨ����LSBΪ1ͨ����
ͨ����״̬����Ӧ���Ƿ������ɱ�־����MIB�ı����SECTION�ڵ�ֱ�Ӷ�Ӧ��PAT��CAT��NIT��Ӧ1���ڵ㣬
SDT��Ӧ5���ڵ㣬PMT��Ӧ32���ڵ㣨ÿ���ڵ��Ӧһ��section����
�ڵ�SECTION������ɶ�Ӧλ��1��
δ�����0��5B�е�����ֽڱ�ʾPAT(bit7)��CAT(bit6)��NIT(bit5)��SDT��bit4-bit0��;
��4�ֽ�ΪPMT��MIB�ڵ�1��Ӧ���λ��MIB�ڵ�32��Ӧ���λ����
*/
typedef struct {
	uint8_t ch_s; 				// MSB is ch8
	uint8_t tbl_s[8][5];  
} uv_cha_si_stat __attribute__((packed));

typedef struct _uv_param{
	//--------------------------------------------------------------
	// ����ָ��
	void *p1;
	void *p2;
	void *p3;
	void *p4;
	unsigned int pcnt;
  //--------------------------------------------------------------
  // ���ͨ��cha����tbl_type�� section��
  // ����PMT������Ҫָ�������PMT����tbl_which_pmt
	UV_SI_OPS_TYPE type;														//  ��ʼ��Ϊ�����㣨EUV_SECTION����EUV_DEFAULT��
  	uint8_t *sec[UV_MAX_SI_SECTION_CNT];//  ����ʱ����ֻ�õ�section����һ��Ҫ��ָ��ָ����Ч�ڴ�,�洢section��̬����ָ��
	unsigned short sec_len[UV_MAX_SI_SECTION_CNT];  //  ��ʼ��Ϊ0��ÿ��section���ȣ�psi������ʼʱ��ʼ��ʱӦΪ0
	unsigned char cur_cnt; 												  // section�������psi������ʼʱ ��ʼ��Ϊ0
	unsigned char cha; 															//  ͨ������ʾ0 - 7
	UV_SI_TBL_TYPE tbl_type;  											//  ָ��������table ����0Ϊpid��1Ϊsection
	unsigned char tbl_which_pmt;  									//  �����pmt��Ҫ����ָ������һ��PMT
	//--------------------------------------------------------------
	// ȫ��ͨ��״̬��ͨ������Ϣ
	uv_cha_si_stat *cur_stat; //����ͨ��״̬
} uv_param;

//#pragma pack(pop)

uv_param sg_si_param;

struct dvbpsi_psi_section_s
{
  /* non-specific section data */
  uint8_t       i_table_id;             /*!< table_id */
  int           b_syntax_indicator;     /*!< section_syntax_indicator */
  int           b_private_indicator;    /*!< private_indicator */
  uint16_t      i_length;               /*!< section_length */

  /* used if b_syntax_indicator is true */
  uint16_t      i_extension;            /*!< table_id_extension */
                                        /*!< transport_stream_id for a
                                             PAT section */
  uint8_t       i_version;              /*!< version_number */
  int             b_current_next;         /*!< current_next_indicator */
  uint8_t       i_number;               /*!< section_number */
  uint8_t       i_last_number;          /*!< last_section_number */

  /* non-specific section data */
  /* the content is table-specific */
  uint8_t *     p_data;                 /*!< complete section */
  uint8_t *     p_payload_start;        /*!< payload start */
  uint8_t *     p_payload_end;          /*!< payload end */

  /* used if b_syntax_indicator is true */
  uint32_t      i_crc;                  /*!< CRC_32 */

  /* list handling */
  struct dvbpsi_psi_section_s *         p_next;         /*!< next element of
                                                             the list */

};


/*****************************************************************************
 * dvbpsi_NewPSISection
 *****************************************************************************/
/*!
 * \fn dvbpsi_psi_section_t * dvbpsi_NewPSISection(int i_max_size)
 * \brief Creation of a new dvbpsi_psi_section_t structure.
 * \param i_max_size max size in bytes of the section
 * \return a pointer to the new PSI section structure.
 */
dvbpsi_psi_section_t * dvbpsi_NewPSISection(int i_max_size);


/*****************************************************************************
 * dvbpsi_DeletePSISections
 *****************************************************************************/
/*!
 * \fn void dvbpsi_DeletePSISections(dvbpsi_psi_section_t * p_section)
 * \brief Destruction of a dvbpsi_psi_section_t structure.
 * \param p_section pointer to the first PSI section structure
 * \return nothing.
 */
void dvbpsi_DeletePSISections(dvbpsi_psi_section_t * p_section);


/*****************************************************************************
 * dvbpsi_ValidPSISection
 *****************************************************************************/
/*!
 * \fn int dvbpsi_ValidPSISection(dvbpsi_psi_section_t* p_section)
 * \brief Validity check of a PSI section.
 * \param p_section pointer to the PSI section structure
 * \return boolean value (0 if the section is not valid).
 *
 * Check the CRC_32 if the section has b_syntax_indicator set.
 */
int dvbpsi_ValidPSISection(dvbpsi_psi_section_t* p_section);


/*****************************************************************************
 * dvbpsi_BuildPSISection
 *****************************************************************************/
/*!
 * \fn void dvbpsi_BuildPSISection(dvbpsi_psi_section_t* p_section)
 * \brief Build a valid section based on the information in the structure.
 * \param p_section pointer to the PSI section structure
 * \return nothing.
 */
void dvbpsi_BuildPSISection(dvbpsi_psi_section_t* p_section);


#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of psi.h"
#endif

