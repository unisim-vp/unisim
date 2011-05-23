/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : UNISIM C API                                                **  
** filename  : types.h                                                     ** 
** version   : 1                                                           ** 
** date      : 12/5/2011                                                   ** 
**                                                                         ** 
***************************************************************************** 
**                                                                         ** 
** Copyright (c) 2011, Commissariat a l'Energie Atomique (CEA)             ** 
** All rights reserved.                                                    ** 
**                                                                         ** 
***************************************************************************** 
 
*/

#ifndef __UNISIM__UAPI__TYPES_H__
#define __UNISIM__UAPI__TYPES_H__

extern "C"
{
	typedef struct _UnisimSimulator *UnisimSimulator;

	typedef struct _UnisimVariable *UnisimVariable;

	typedef struct _UnisimExtendedAPI *UnisimExtendedAPI;
}

#endif // __UNISIM__UAPI__TYPES_H__
