/* define heap and stack size in Local Data SRAM */
__HEAP_SIZE            = 16384;
__STACK_SIZE           = 49148;

/* Define memory size and base address for code in flash */
FLASH_SIZE      = 2560K;
FLASH_BASE_ADDR = 0x1000000;

/* Define memory size and base address for data in system SRAM */
SRAM_SIZE      =  128K;    
SRAM_BASE_ADDR = 0x40000000;
  
/* Define memory size and base address for shared data in system SRAM */
SHARE_SIZE     = 17K;
SHARE_BASE_ADDR = 0x40060000;

/* Define Core Local Instruction SRAM Allocation */  
LOCAL_IMEM_SIZE = 16K;
/* Define Core Local Instruction SRAM Base Address */
LOCAL_IMEM_BASE_ADDR = 0x50000000;

/* Define Core Local Data SRAM Allocation */
LOCAL_DMEM_SIZE = 64K;
/* Define Core Local Data SRAM Base Address */
LOCAL_DMEM_BASE_ADDR = 0x50800000;

/* Define Command line Base Address */
CMDLINE_BASE_ADDR = 0x40064400;
/* Define Command line Size */
CMDLINE_SIZE = 1024;

/* Define EBI MEM Base Address */
EBI_MEM_BASE_ADDR = 0x20000000;
/* Define EBI MEM size */
EBI_MEM_SIZE = 64M;

/* Define RAM disk size */
RAMDISK_SIZE = 64M;

INCLUDE ldscripts/small/common/mpc57xx.ld
