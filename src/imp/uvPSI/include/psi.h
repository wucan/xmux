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

/*通道状态：通道1-8是否分析完成指示标志，标志0未分析，1为分析完成。MSB为8通道，LSB为1通道。
通道表状态：相应表是否分析完成标志，和MIB的表分析SECTION节点直接对应，PAT，CAT，NIT对应1个节点，
SDT对应5个节点，PMT对应32个节点（每个节点对应一个section），
节点SECTION分析完成对应位置1，
未完成置0；5B中的最高字节表示PAT(bit7)，CAT(bit6)，NIT(bit5)，SDT（bit4-bit0）;
低4字节为PMT（MIB节点1对应最低位，MIB节点32对应最高位）。
*/
typedef struct {
	uint8_t ch_s; 				// MSB is ch8
	uint8_t tbl_s[8][5];  
} uv_cha_si_stat __attribute__((packed));

typedef struct _uv_param{
	//--------------------------------------------------------------
	// 参数指针
	void *p1;
	void *p2;
	void *p3;
	void *p4;
	unsigned int pcnt;
  //--------------------------------------------------------------
  // 针对通道cha，表tbl_type的 section，
  // 对于PMT分析还要指定具体的PMT次序tbl_which_pmt
	UV_SI_OPS_TYPE type;														//  初始化为解析层（EUV_SECTION或者EUV_DEFAULT）
  	uint8_t *sec[UV_MAX_SI_SECTION_CNT];//  分析时，若只得到section，则一定要将指针指向有效内存,存储section静态变量指针
	unsigned short sec_len[UV_MAX_SI_SECTION_CNT];  //  初始化为0，每个section长度，psi分析开始时初始化时应为0
	unsigned char cur_cnt; 												  // section个数，琾si分析开始时 初始化为0
	unsigned char cha; 															//  通道，表示0 - 7
	UV_SI_TBL_TYPE tbl_type;  											//  指明分析的table 类型0为pid，1为section
	unsigned char tbl_which_pmt;  									//  如果是pmt还要按序指明是哪一个PMT
	//--------------------------------------------------------------
	// 全局通道状态和通道表信息
	uv_cha_si_stat *cur_stat; //所有通道状态
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

