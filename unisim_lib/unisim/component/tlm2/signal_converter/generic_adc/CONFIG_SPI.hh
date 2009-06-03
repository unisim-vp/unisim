/*****************************************************************************
 
  CONFIG_ARGA_SPI.h -- definition of the cmd .
 
  Original Author: Khaled Rahmouni, Schneider Electric

  Version : June 18, 2008

 *****************************************************************************/
#ifndef __UNISIM_COMPONENT_TLM2_COM_ARGA_CONFIG_ARGA_SPI_HH__
#define __UNISIM_COMPONENT_TLM2_COM_ARGA_CONFIG_ARGA_SPI_HH__


// Operations possibles                                   

#define    ARGA_OP_WRITE 						 0         /*   Ecriture                                               */
#define    ARGA_OP_READ  						 1         /*   Lecture                                                */



// definition of the command code
#define    ARGA_CMD_RAM               0x00U  /*   Memoire RAM                                             */
#define    ARGA_CMD_STATUS0           0x01U  /*   Registre d'etat des interruptions, premiere partie      */
#define    ARGA_CMD_MASK0             0x02U  /*   Registre de masquage des interruptions, premiere partie */
#define    ARGA_CMD_STATUS1           0x03U  /*   Registre d'etat des interruptions, seconde partie       */
#define    ARGA_CMD_MASK1             0x04U  /*   Registre de masquage des interruptions, seconde partie  */
#define    ARGA_CMD_OTP_RD            0x05U  /*   Memoire OTP-ROM (lecture)                               */
#define    ARGA_CMD_RC_POS0           0x06U  /*   Position des roues codeuses, premiere partie            */
#define    ARGA_CMD_RC_POS1           0x07U  /*   Position des roues codeuses, seconde partie             */
#define    ARGA_CMD_EEP_PRERD         0x08U  /*   Memoire EEPROM (pre-lecture)                            */
#define    ARGA_CMD_EEP_RD            0x09U  /*   Memoire EEPROM (lecture rapide)                         */
#define    ARGA_CMD_EEP_WR            0x0AU  /*   Memoire EEPROM (ecriture)                               */
#define    ARGA_CMD_OUT_CTRL0         0x0BU  /*   Sorties "dysfonctionnelles"                             */
#define    ARGA_CMD_OUT_CTRL1         0x0CU  /*   Sorties "actionneur"                                    */
#define    ARGA_CMD_OUT_CTRL2         0x0DU  /*   Sorties "fonctionnelles"                                */
#define    ARGA_CMD_LOAD_LR           0x0EU  /*   Precharge LR                                            */
#define    ARGA_CMD_MEMTH             0x0FU  /*   Memoire thermique                                       */
#define    ARGA_CMD_BACKUP            0x10U  /*   Registre de sauvegarde                                  */
#define    ARGA_CMD_TEST_KEY          0x11U  /*   Registre de clef d'entree en mode test                  */
#define    ARGA_CMD_TEST_MODE         0x12U  /*   Registre de mode test                                   */
#define    ARGA_CMD_ADC_VALUES        0x13U  /*   Echantillons bruts de convertisseur 16 bits             */
#define    ARGA_CMD_ADC_CTRL          0x14U  /*   Controle/commande de convertisseur 16 bits              */
#define    ARGA_CMD_ADC_8BITS         0x15U  /*   Controle/commande/echantillons de convertisseur 8 bits  */
#define    ARGA_CMD_OTP_WR            0x16U  /*   Memoire OTP-ROM (ecriture)                              */
#define    ARGA_CMD_OTP_DUM           0x17U  /*   Memoire OTP-ROM (ecriture simulee)                      */
#define    ARGA_CMD_INIT0             0x18U  /*   Initiation complete                                     */
#define    ARGA_CMD_INIT1             0x19U  /*   Initiation partielle                                    */
#define    ARGA_CMD_RAZ_WDT           0x1AU  /*   Muselage du chien de garde                              */
#define    ARGA_CMD_MCROM             0x1BU  /*   Memoire ROM microcode                                   */
#define    ARGA_CMD_THROM             0x1CU  /*   Memoire ROM seuils et temporisations                    */
#define    ARGA_CMD_NOP               0x3FU  /*   Aucun domaine                                           */


 // RAM MAPPING                                              

#define    ARGA_RAM_I_V_VALUE0        0x9FU  /*   Variable i_v_value[0]                                   */
#define    ARGA_RAM_I_V_VALUE1        0xBFU  /*   Variable i_v_value[1]                                   */
#define    ARGA_RAM_I_V_VALUE2        0xDFU  /*   Variable i_v_value[2]                                   */
#define    ARGA_RAM_I_V_VALUE3        0xFFU  /*   Variable i_v_value[3]                                   */
                              
#define    ARGA_RAM_U2RMS0            0x41U  /*   Variable u2rms[0]                                       */
#define    ARGA_RAM_U2RMS1            0x51U  /*   Variable u2rms[1]                                       */
#define    ARGA_RAM_U2RMS2            0x61U  /*   Variable u2rms[2]                                       */
                              
#define    ARGA_RAM_I2RMS0            0x40U  /*   Variable i2rms[0]                                       */
#define    ARGA_RAM_I2RMS1            0x50U  /*   Variable i2rms[1]                                       */
#define    ARGA_RAM_I2RMS2            0x60U  /*   Variable i2rms[2]                                       */
#define    ARGA_RAM_I2RMS3            0x70U  /*   Variable i2rms[3]                                       */
#define    ARGA_RAM_I2RMS4            0x71U  /*   Variable i2rms[4]                                       */
                              
#define    ARGA_RAM_PROT_U2RMS0       0x41U  /*   Variable prot_u2rms[0]                                  */
#define    ARGA_RAM_PROT_U2RMS1       0x51U  /*   Variable prot_u2rms[1]                                  */
#define    ARGA_RAM_PROT_U2RMS2       0x61U  /*   Variable prot_u2rms[2]                                  */
                             
#define    ARGA_RAM_PROT_I2RMS0       0x44U  /*   Variable prot_i2rms[0]                                  */
#define    ARGA_RAM_PROT_I2RMS1       0x54U  /*   Variable prot_i2rms[1]                                  */
#define    ARGA_RAM_PROT_I2RMS2       0x64U  /*   Variable prot_i2rms[2]                                  */
#define    ARGA_RAM_PROT_I2RMS3       0x74U  /*   Variable prot_i2rms[3]                                  */
#define    ARGA_RAM_PROT_I2RMS4       0x75U  /*   Variable prot_i2rms[4]                                  */
                              
#define    ARGA_RAM_PA0               0x43U  /*   Variable pa[0]                                          */
#define    ARGA_RAM_PA1               0x53U  /*   Variable pa[1]                                          */
#define    ARGA_RAM_PA2               0x63U  /*   Variable pa[2]                                          */
                              
#define    ARGA_RAM_PROT_PA0          0x46U  /*   Variable prot_pa[0]                                     */
#define    ARGA_RAM_PROT_PA1          0x56U  /*   Variable prot_pa[1]                                     */
#define    ARGA_RAM_PROT_PA2          0x66U  /*   Variable prot_pa[2]                                     */
                              
#define    ARGA_RAM_ALPHAOFF          0x20U  /*   Constante ALPHAOFF                                      */
#define    ARGA_RAM_TRAINCTMAXVIGI    0x22U  /*   Constante TRAINCTMAXVIGI                                */
#define    ARGA_RAM_FS                0x2BU  /*   Constante FS                                            */
#define    ARGA_RAM_TRAINCTMAX_GF     0x31U  /*   Constante TRAINCTMAX_GF                                 */


#endif




