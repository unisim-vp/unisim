/*****************************************************************************
 
  CONFIG_GENERATOR.h -- definition of the generator.
 
  Original Author: Khaled Rahmouni, Schneider Electric

  Version : June 18, 2008
 
 *****************************************************************************/
#ifndef __UNISIM_COMPONENT_TLM2_COM_ARGA_CONFIG_GENERATOR_HH__
#define __UNISIM_COMPONENT_TLM2_COM_ARGA_CONFIG_GENERATOR_HH__



/*
 **************** DEFINITION DES CONSTANTES ***********************************
 */

#define ARGA_SQRT_2         1.41421356237   		/* Racine carree de 2                                */
#define PI                  3.141592653589793  /* Constante pi                                      */

/* RESOLUTIONS ***************************************************************/

#define ARGA_ADC_FULLSCALE  59134.0          /* CAN 16 bits, 59134 points                         */


/* FREQUENCES ****************************************************************/

#define ARGA_FREQ_SAMPLING  1800.0           /* 1800 Hz                                           */


/* TENSIONS DE REFERENCE *****************************************************/
#define ARGA_VREF_I     1.2         /* Entrees CAN courant voies 0, 1, 2, 3                      */


#define ARGA_VREF_U     1.0         /* Entrees CAN tension voies 0, 1, 2 et courant GF           */


/////////////////////////////////////////////////////////
/////												/////
/////               CURRENT PARAMETERS              /////
/////												/////
/////////////////////////////////////////////////////////


#define C1_TYPE 1 // 1 : sinus ; 2 : cosinus ; 3 : constant
#define C1_FREQUENCY 50 // Hz
#define C1_DEPHASAGE 0 //degree



#define C2_TYPE 1 // 1 : sinus ; 2 : cosinus ; 3 : constant
#define C2_FREQUENCY 50 // Hz
#define C2_DEPHASAGE 120 //degree



#define C3_TYPE 1 // 1 : sinus ; 2 : cosinus ; 3 : constant
#define C3_FREQUENCY 50 // Hz
#define C3_DEPHASAGE 240 //degree


#define coeff_capteur 17.9



#endif

