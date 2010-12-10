/*****************************************************************************
 * dvbpsi_private.h: main private header
 *----------------------------------------------------------------------------
 * (c)2001-2002 VideoLAN
 * $Id: dvbpsi_private.h,v 1.2 2002/01/22 20:30:16 bozo Exp $
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
 *----------------------------------------------------------------------------
 *
 *****************************************************************************/

#ifndef _DVBPSI_DVBPSI_PRIVATE_H_
#define _DVBPSI_DVBPSI_PRIVATE_H_


/*****************************************************************************
 * Error management
 *****************************************************************************/
#define DVBPSI_ERROR(src, str)                                          \
        fprintf(stderr, "[TSA] error (" src "): " str "\n");

#ifdef WIN32
	#define DVBPSI_ERROR_ARG(src, str, x)                                \
        fprintf(stderr, "[TSA] error (" src "): " str "\n", x);
#else
	#define DVBPSI_ERROR_ARG(src, str, x...)                                \
        fprintf(stderr, "[TSA] error (" src "): " str "\n", x);
#endif




#ifdef DEBUG
#  define DVBPSI_DEBUG(src, str)                                        \
          fprintf(stderr, "[TSA] error (" src "): " str "\n");
#  define DVBPSI_DEBUG_ARG(src, str, x...)                              \
          fprintf(stderr, "[TSA[] error (" src "): " str "\n", x);
#else
#  define DVBPSI_DEBUG(src, str)

	#ifdef WIN32
		#  define DVBPSI_DEBUG_ARG(src, str, x)
	#else
		#  define DVBPSI_DEBUG_ARG(src, str, x...)                              
        
	#endif

#endif


#else
#error "Multiple inclusions of dvbpsi_private.h"
#endif

