target remote :7224
set architecture powerpc:vle
load noot.elf
symbol-file boot.elf
break main
break *0x10
break *0x60
