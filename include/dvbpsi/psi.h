/*****************************************************************************
 * psi.h
 * (c)2001-2002 VideoLAN
 * $Id: psi.h 88 2004-02-24 14:31:18Z sam $
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


#define UV_PROGRAM_MAX_NUM			64

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

/*Í¨µÀ×´Ì¬£ºÍ¨µÀ1-8ÊÇ·ñ·ÖÎöÍê³ÉÖ¸Ê¾±êÖ¾£¬±êÖ¾0Î´·ÖÎö£¬1Îª·ÖÎöÍê³É¡£MSBÎª8Í¨µÀ£¬LSBÎª1Í¨µÀ¡£
Í¨µÀ±í×´Ì¬£ºÏàÓ¦±íÊÇ·ñ·ÖÎöÍê³É±êÖ¾£¬ºÍMIBµÄ±í·ÖÎöSECTION½ÚµãÖ±½Ó¶ÔÓ¦£¬PAT£¬CAT£¬NIT¶ÔÓ¦1¸ö½Úµã£¬
SDT¶ÔÓ¦5¸ö½Úµã£¬PMT¶ÔÓ¦32¸ö½Úµã£¨Ã¿¸ö½Úµã¶ÔÓ¦Ò»¸ösection£©£¬
½ÚµãSECTION·ÖÎöÍê³É¶ÔÓ¦Î»ÖÃ1£¬
Î´Íê³ÉÖÃ0£»5BÖÐµÄ×î¸ß×Ö½Ú±íÊ¾PAT(bit7)£¬CAT(bit6)£¬NIT(bit5)£¬SDT£¨bit4-bit0£©;
µÍ4×Ö½ÚÎªPMT£¨MIB½Úµã1¶ÔÓ¦×îµÍÎ»£¬MIB½Úµã32¶ÔÓ¦×î¸ßÎ»£©¡£
*/
typedef struct {
	uint8_t ch_s; 				// MSB is ch8
	uint8_t tbl_s[8][1 + (UV_PROGRAM_MAX_NUM / 8)];  
} __attribute__((packed)) uv_cha_si_stat;

typedef struct _uv_param{
	//--------------------------------------------------------------
	// ²ÎÊýÖ¸Õë
	void *p1;
	void *p2;
	void *p3;
	void *p4;
	unsigned int pcnt;
  //--------------------------------------------------------------
  // Õë¶ÔÍ¨µÀcha£¬±ítbl_typeµÄ section£¬
  // ¶ÔÓÚPMT·ÖÎö»¹ÒªÖ¸¶¨¾ßÌåµÄPMT´ÎÐòtbl_which_pmt
	UV_SI_OPS_TYPE type;														//  ³õÊ¼»¯Îª½âÎö²ã£¨EUV_SECTION»òÕßEUV_DEFAULT£©
  	uint8_t *sec[UV_MAX_SI_SECTION_CNT];//  ·ÖÎöÊ±£¬ÈôÖ»µÃµ½section£¬ÔòÒ»¶¨Òª½«Ö¸ÕëÖ¸ÏòÓÐÐ§ÄÚ´æ,´æ´¢section¾²Ì¬±äÁ¿Ö¸Õë
	unsigned short sec_len[UV_MAX_SI_SECTION_CNT];  //  ³õÊ¼»¯Îª0£¬Ã¿¸ösection³¤¶È£¬psi·ÖÎö¿ªÊ¼Ê±³õÊ¼»¯Ê±Ó¦Îª0
	unsigned char cur_cnt; 												  // section¸öÊý£¬¬psi·ÖÎö¿ªÊ¼Ê± ³õÊ¼»¯Îª0
	unsigned char cha; 															//  Í¨µÀ£¬±íÊ¾0 - 7
	UV_SI_TBL_TYPE tbl_type;  											//  Ö¸Ã÷·ÖÎöµÄtable ÀàÐÍ0Îªpid£¬1Îªsection
	unsigned char tbl_which_pmt;  									//  Èç¹ûÊÇpmt»¹Òª°´ÐòÖ¸Ã÷ÊÇÄÄÒ»¸öPMT
	//--------------------------------------------------------------
	// È«¾ÖÍ¨µÀ×´Ì¬ºÍÍ¨µÀ±íÐÅÏ¢
	uv_cha_si_stat *cur_stat; //ËùÓÐÍ¨µÀ×´Ì¬
} uv_param;

//#pragma pack(pop)

uv_param sg_si_param;


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
  int           b_current_next;         /*!< current_next_indicator */
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

