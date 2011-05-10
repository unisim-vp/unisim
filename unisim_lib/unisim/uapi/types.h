/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : UNISIM C API                                                **  
** filename  : types.h                                                     ** 
** version   : 1                                                           ** 
** date      : 4/5/2011                                                    ** 
**                                                                         ** 
***************************************************************************** 
**                                                                         ** 
** Copyright (c) 2011, Commissariat a l'Energie Atomique (CEA)             ** 
** All rights reserved.                                                    ** 
**                                                                         ** 
***************************************************************************** 
 
VERSION HISTORY: 
---------------- 

Version     : 1
Date        : 4/5/2011
Revised by  : Daniel Gracia Perez
Description : Original version.
              * Defined simulator type.
			  * Defined variable type.

*/

#ifndef __UNISIM__UAPI__TYPES_H__
#define __UNISIM__UAPI__TYPES_H__

extern "C"
{
	typedef struct _UniSimulator *UniSimulator;

	typedef struct _UniVariable *UniVariable;
}

#endif // __UNISIM__UAPI__TYPES_H__
