// TODO: cpuid should be handled be the architecture
template <class ARCH>
struct CpuID : public Operation<ARCH>
{
  CpuID( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "cpuid"; }
  void execute( ARCH& arch ) const
  {
    switch (mkint( arch.regread32( 0 ) )) {
    case 0: {
      arch.template regwrite<32>( 0, u32_t( 1 ) );
  
      char const* name = "GenuineIntel";
      { uint32_t word = 0;
        int idx = 12;
        while (--idx >= 0) {
          word = (word << 8) | name[idx];
          if (idx % 4) continue;
          arch.template regwrite<32>( 3 - (idx/4), u32_t( word ) );
          word = 0;
        }
      }
    } break;
    case 1: {
      uint32_t const eax =
        (6  << 0 /* stepping id */) |
        (0  << 4 /* model */) |
        (15 << 8 /* family */) |
        (0  << 12 /* processor type */) |
        (0  << 16 /* extended model */) |
        (0  << 20 /* extended family */);
      arch.template regwrite<32>( 0, u32_t( eax ) );
    
      uint32_t const ebx =
        (0 <<  0 /* Brand index */) |
        (4 <<  8 /* Cache line size (/ 64bits) */) |
        (1 << 16 /* Maximum number of addressable IDs for logical processors in this physical package* */) |
        (0 << 24 /* Initial APIC ID */);
      arch.template regwrite<32>( 3, u32_t( ebx ) );
    
      uint32_t const ecx =
        (0 << 0x00 /* Streaming SIMD Extensions 3 (SSE3) */) |
        (0 << 0x01 /* PCLMULQDQ Available */) |
        (0 << 0x02 /* 64-bit DS Area */) |
        (0 << 0x03 /* MONITOR/MWAIT */) |
        (0 << 0x04 /* CPL Qualified Debug Store */) |
        (0 << 0x05 /* Virtual Machine Extensions */) |
        (0 << 0x06 /* Safer Mode Extensions */) |
        (0 << 0x07 /* Enhanced Intel SpeedStep® technology */) |
        (0 << 0x08 /* Thermal Monitor 2 */) |
        (0 << 0x09 /* Supplemental Streaming SIMD Extensions 3 (SSSE3) */) |
        (0 << 0x0a /* L1 Context ID */) |
        (0 << 0x0b /* Reserved */) |
        (0 << 0x0c /* FMA */) |
        (0 << 0x0d /* CMPXCHG16B Available */) |
        (0 << 0x0e /* xTPR Update Control */) |
        (0 << 0x0f /* Perfmon and Debug Capability */) |
        (0 << 0x10 /* Reserved */) |
        (0 << 0x11 /* Process-context identifiers */) |
        (0 << 0x12 /* DCA */) |
        (0 << 0x13 /* SSE4.1 */) |
        (0 << 0x14 /* SSE4.2 */) |
        (0 << 0x15 /* x2APIC */) |
        (1 << 0x16 /* MOVBE Available */) |
        (1 << 0x17 /* POPCNT Available */) |
        (0 << 0x18 /* TSC-Deadline */) |
        (0 << 0x19 /* AESNI */) |
        (0 << 0x1a /* XSAVE */) |
        (0 << 0x1b /* OSXSAVE */) |
        (0 << 0x1c /* AVX */) |
        (1 << 0x1d /* F16C */) |
        (1 << 0x1e /* RDRAND Available */) |
        (1 << 0x1f /* Is virtual machine */);
      arch.template regwrite<32>( 1, u32_t( ecx ) );
    
      uint32_t const edx =
        (1 << 0x00 /* Floating Point Unit On-Chip */) |
        (0 << 0x01 /* Virtual 8086 Mode Enhancements */) |
        (0 << 0x02 /* Debugging Extensions */) |
        (0 << 0x03 /* Page Size Extension */) |
        (0 << 0x04 /* Time Stamp Counter */) |
        (0 << 0x05 /* Model Specific Registers RDMSR and WRMSR Instructions */) |
        (0 << 0x06 /* Physical Address Extension */) |
        (0 << 0x07 /* Machine Check Exception */) |
        (0 << 0x08 /* CMPXCHG8B Available */) |
        (0 << 0x09 /* APIC On-Chip */) |
        (0 << 0x0a /* Reserved */) |
        (0 << 0x0b /* SYSENTER and SYSEXIT Instructions */) |
        (0 << 0x0c /* Memory Type Range Registers */) |
        (0 << 0x0d /* Page Global Bit */) |
        (0 << 0x0e /* Machine Check ARCHitecture */) |
        (1 << 0x0f /* Conditional Move Instructions */) |
        (0 << 0x10 /* Page Attribute Table */) |
        (0 << 0x11 /* 36-Bit Page Size Extension */) |
        (0 << 0x12 /* Processor Serial Number */) |
        (0 << 0x13 /* CLFLUSH Instruction */) |
        (0 << 0x14 /* Reserved */) |
        (0 << 0x15 /* Debug Store */) |
        (0 << 0x16 /* Thermal Monitor and Software Controlled Clock Facilities */) |
        (0 << 0x17 /* Intel MMX Technology */) |
        (0 << 0x18 /* FXSAVE and FXRSTOR Instructions */) |
        (0 << 0x19 /* SSE */) |
        (0 << 0x1a /* SSE2 */) |
        (0 << 0x1b /* Self Snoop */) |
        (0 << 0x1c /* Max APIC IDs reserved field is Valid */) |
        (0 << 0x1d /* Thermal Monitor */) |
        (0 << 0x1e /* Resrved */) |
        (0 << 0x1f /* Pending Break Enable */);
      arch.template regwrite<32>( 2, u32_t( edx ) );
    
    } break;
    case 2: {
      arch.template regwrite<32>( 0, u32_t( 0 ) );
      arch.template regwrite<32>( 3, u32_t( 0 ) );
      arch.template regwrite<32>( 1, u32_t( 0 ) );
      arch.template regwrite<32>( 2, u32_t( 0 ) );
    } break;
    case 4: {
      // Small cache config
      switch (mkint( arch.regread32( 1 ) )) { // %ecx holds requested cache id
      case 0: { // L1 D-CACHE
        arch.template regwrite<32>( 0, u32_t( (1 << 26) | (0 << 14) | (1 << 8) | (1 << 5) | (1 << 0) ) ); // 0x4000121
        arch.template regwrite<32>( 3, u32_t( (0 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x1c0003f
        arch.template regwrite<32>( 1, u32_t( (0 << 22) | (0x03f << 0) ) ); // 0x000003f
        arch.template regwrite<32>( 2, u32_t( 0x0000001 ) ); // 0x0000001
      } break;
      case 1: { // L1 I-CACHE
        arch.template regwrite<32>( 0, u32_t( (1 << 26) | (0 << 14) | (1 << 8) | (1 << 5) | (2 << 0) ) ); // 0x4000122
        arch.template regwrite<32>( 3, u32_t( (0 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x1c0003f
        arch.template regwrite<32>( 1, u32_t( (0 << 22) | (0x03f << 0) ) ); // 0x000003f
        arch.template regwrite<32>( 2, u32_t( 0x0000001 ) ); // 0x0000001
      } break;
      case 2: { // L2 U-CACHE
        arch.template regwrite<32>( 0, u32_t( (1 << 26) | (1 << 14) | (1 << 8) | (2 << 5) | (3 << 0) ) ); // 0x4000143
        arch.template regwrite<32>( 3, u32_t( (1 << 26) | (3 << 22) | (0 << 12) | (0x3f << 0) ) ); // 0x5c0003f
        arch.template regwrite<32>( 1, u32_t( (0 << 22) | (0xfff << 0) ) ); // 0x0000fff
        arch.template regwrite<32>( 2, u32_t( 0x0000001 ) ); // 0x0000001
      } break;
      case 3: { // TERMINATING NULL ENTRY
        // 0, 0, 0, 0
        arch.template regwrite<32>( 0, u32_t( 0 ) );
        arch.template regwrite<32>( 3, u32_t( 0 ) );
        arch.template regwrite<32>( 1, u32_t( 0 ) );
        arch.template regwrite<32>( 2, u32_t( 0 ) );
      } break;
      }
    } break;
  
    case 0x80000000: {
      arch.template regwrite<32>( 0, u32_t( 0x80000001 ) );
      arch.template regwrite<32>( 3, u32_t( 0 ) );
      arch.template regwrite<32>( 1, u32_t( 0 ) );
      arch.template regwrite<32>( 2, u32_t( 0 ) );
    } break;
    case 0x80000001: {
      arch.template regwrite<32>( 0, u32_t( 0 ) );
      arch.template regwrite<32>( 3, u32_t( 0 ) );
      arch.template regwrite<32>( 1, u32_t( 0 ) );
      arch.template regwrite<32>( 2, u32_t( 0 ) );
    } break;
    default:
      std::cerr << "Unknown cmd for cpuid, " << std::hex
                << "%eax=0x" << mkint( arch.regread32( 0 ) ) << ", "
                << "%eip=0x" << this->address << "\n";
      throw "not implemented";
      break;
    }
  }
};

template <class ARCH> struct DC<ARCH,CPUID> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.lock_f0) return 0;
  
  if (auto _ = match( ic, opcode( "\x0f\xa2" ) )) return new CpuID<ARCH>( _.opbase() );
  
  return 0;
}};

template <class ARCH>
struct RdTSC : public Operation<ARCH>
{
  RdTSC( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  
  void disasm( std::ostream& sink ) const { sink << "rdtsc"; }
  
  void execute( ARCH& arch ) const
  {
    u64_t tsc = arch.tscread();
    arch.template regwrite<32>( 0, u32_t( tsc >> 0 ) );
    arch.template regwrite<32>( 2, u32_t( tsc >> 32 ) );
  }
};

template <class ARCH> struct DC<ARCH,RDTSC> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.lock_f0) return 0;
  
  if (auto _ = match( ic, opcode( "\x0f\x31" ) )) return new RdTSC<ARCH>( _.opbase() );
  
  return 0;
}};

// 
// op wrmsr( 0x0f[8]:> <:0x30[8] );
// 
// wrmsr.disasm = { _sink << "wrmsr"; };
// 
// 
// 
// op rdtscp( 0x0f[8]:> <:0x01[8]:> <:0xf9[8] );
// 
// rdtscp.disasm = { _sink << "rdtscp"; };
// 
// op rdmsr( 0x0f[8]:> <:0x32[8] );
// 
// rdmsr.disasm = { _sink << "rdmsr"; };
// 
// op rdpmc( 0x0f[8]:> <:0x33[8] );
// 
// rdpmc.disasm = { _sink << "rdpmc"; };
// 
// op hlt( 0xf4[8] );
// 
// hlt.disasm = { _sink << "hlt"; };
// 
// hlt.execute = { arch.stop(); };
// 
// op in_al_ib( 0xe4[8]:> <:port[8] );
// 
// in_al_ib.disasm = { _sink << "in " << DisasmI( port ) << ',' << DisasmRb( 0 ); };
// 
// op in_ax_ib( 0x66[8]:> <:0xe5[8]:> <:port[8] );
// 
// in_ax_ib.disasm = { _sink << "in " << DisasmI( port ) << ',' << DisasmRw( 0 ); };
// 
// op in_eax_ib( 0xe5[8]:> <:port[8] );
// 
// in_eax_ib.disasm = { _sink << "in " << DisasmI( port ) << ',' << DisasmRd( 0 ); };
// 
// op out_ib_al( 0xe6[8]:> <:port[8] );
// 
// out_ib_al.disasm = { _sink << "out " << DisasmRb( 0 ) << ',' << DisasmI( port ); };
// 
// op out_ib_ax( 0x66[8]:> <:0xe7[8]:> <:port[8] );
// 
// out_ib_ax.disasm = { _sink << "out " << DisasmRw( 0 ) << ',' << DisasmI( port ); };
// 
// op out_ib_eax( 0xe7[8]:> <:port[8] );
// 
// out_ib_eax.disasm = { _sink << "out " << DisasmRd( 0 ) << ',' << DisasmI( port ); };
// 
// op in_al_dx( 0xec[8] );
// 
// in_al_dx.disasm = { _sink << "in (" << DisasmRw( 2 ) << ")," << DisasmRb( 0 ); };
// 
// op in_ax_dx( 0x66[8]:> <:0xed[8] );
// 
// in_ax_dx.disasm = { _sink << "in (" << DisasmRw( 2 ) << ")," << DisasmRw( 0 ); };
// 
// op in_eax_dx( 0xed[8] );
// 
// in_eax_dx.disasm = { _sink << "in (" << DisasmRw( 2 ) << ")," << DisasmRd( 0 ); };
// 
// op out_dx_al( 0xee[8] );
// 
// out_dx_al.disasm = { _sink << "out " << DisasmRb( 0 ) << ",(" << DisasmRw( 2 ) << ")"; };
// 
// op out_dx_ax( 0x66[8]:> <:0xef[8] );
// 
// out_dx_ax.disasm = { _sink << "out " << DisasmRw( 0 ) << ",(" << DisasmRw( 2 ) << ")"; };
// 
// op out_dx_eax( 0xef[8] );
// 
// out_dx_eax.disasm = { _sink << "out " << DisasmRd( 0 ) << ",(" << DisasmRw( 2 ) << ")"; };
// 
// op clc( 0xf8[8] );
// 
// clc.disasm = { _sink << "clc"; };
// 
// op stc( 0xf9[8] );
// 
// stc.disasm = { _sink << "stc"; };
// 
// op cli( 0xfa[8] );
// 
// cli.disasm = { _sink << "cli"; };
// 
// op sti( 0xfb[8] );
// 
// sti.disasm = { _sink << "sti"; };
// 
// op pause( 0xf3[8]:> <:0x90[8] );
// 
// pause.disasm = { _sink << "pause"; };
// 
// op rsm( 0x0f[8]:> <:0xaa[8] );
// 
// rsm.disasm = { _sink << "rsm"; };
// 
// op ud1( 0x0f[8]:> <:0xb9[8]:> <:*modrm[ModRM] );
// 
// ud1.disasm = { _sink << "ud1"; };
// 
// op ud2( 0x0f[8]:> <:0x0b[8] );
// 
// ud2.disasm = { _sink << "ud2"; };
// 
// op rdrand_rw( 0x66[8]:> <:0x0f[8]:> <:0xc7[8]:> <:0b11[2]:110[3]:reg[3] );
// 
// rdrand_rw.disasm = { _sink << "rdrand " << DisasmRw( reg ); };
// 
// op rdrand_rd( 0x0f[8]:> <:0xc7[8]:> <:0b11[2]:0b110[3]:reg[3] );
// 
// rdrand_rd.disasm = { _sink << "rdrand " << DisasmRd( reg ); };
// 
// op rdseed_rw( 0x66[8]:> <:0x0f[8]:> <:0xc7[8]:> <:0b11[2]:111[3]:reg[3] );
// 
// rdseed_rw.disasm = { _sink << "rdseed " << DisasmRw( reg ); };
// 
// op rdseed_rd( 0x0f[8]:> <:0xc7[8]:> <:0b11[2]:0b111[3]:reg[3] );
// 
// rdseed_rd.disasm = { _sink << "rdseed " << DisasmRd( reg ); };
// 
// op icebp( 0xf1[8] );
// 
// icebp.disasm = { _sink << "icebp"; };
// 
// op xabort( 0xc6[8]:> <:0xf8[8]:> <:imm[8] );
// 
// xabort.disasm = { _sink << "xabort " << DisasmI( imm ); };
// 
// op xbegin_jd( 0xc7[8]:> <:0xf8[8]:> <:offset[32] );
// 
// xbegin_jd.disasm = { _sink << "xbegin 0x" << std::hex << (this->addr + this->size + offset); };
// 
// op xend( 0x0f[8]:> <:0x01[8]:> <:0xd5[8] );
// 
// xend.disasm = { _sink << "xend"; };
// 
// op xtest( 0x0f[8]:> <:0x01[8]:> <:0xd6[8] );
// 
// xtest.disasm = { _sink << "xtest"; };
// 
// op invd( 0x0f[8]:> <:0x08[8] );
// 
// invd.disasm = { _sink << "invd"; };
// 
// op wbinvd( 0x0f[8]:> <:0x09[8] );
// 
// wbinvd.disasm = { _sink << "wbinvd"; };
// 
// op syscall( 0x0f[8]:> <:0x05[8] );
// 
// syscall.disasm = { _sink << "syscall"; };
// 
// op sysret( 0x0f[8]:> <:0x07[8] );
// 
// sysret.disasm = { _sink << "sysret"; };
// 
// op sysenter( 0x0f[8]:> <:0x34[8] );
// 
// sysenter.disasm = { _sink << "sysenter"; };
// 
// op sysexit( 0x0f[8]:> <:0x35[8] );
// 
// sysexit.disasm = { _sink << "sysexit"; };
// 
// op clts( 0x0f[8]:> <:0x06[8] );
// 
// clts.disasm = { _sink << "clts"; };
// 
// op lar_gd_ew( 0x0f[8]:> <:0x02[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// lar_gd_ew.disasm = { _sink << "lar " << DisasmEw( modrm, segment ) << ',' << DisasmRd( gn ); };
// 
// op lsl_gd_ew( 0x0f[8]:> <:0x03[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// lsl_gd_ew.disasm = { _sink << "lsl " << DisasmEw( modrm, segment ) << ',' << DisasmRd( gn ); };
// 
// op mov_rd_cd( 0x0f[8]:> <:0x20[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// mov_rd_cd.disasm = { _sink << "mov " << DisasmCd( gn ) << ',' << DisasmRd( rm ); };
// 
// op mov_rd_dd( 0x0f[8]:> <:0x21[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// mov_rd_dd.disasm = { _sink << "mov " << DisasmDd( gn ) << ',' << DisasmRd( rm ); };
// 
// op mov_cd_rd( 0x0f[8]:> <:0x22[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// mov_cd_rd.disasm = { _sink << "mov " << DisasmRd( rm ) << ',' << DisasmCd( gn ); };
// 
// op mov_dd_rd( 0x0f[8]:> <:0x23[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// mov_dd_rd.disasm = { _sink << "mov " << DisasmRd( rm ) << ',' << DisasmDd( gn ); };
// 
// op sgdt_ms( 0x0f[8]:> <:0x01[8]:> <:?[2]:0b000[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sgdt_ms.disasm = { _sink << "sgdtl " << DisasmM( modrm, segment ); };
// 
// op vmcall( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b000[3]:0b001[3]:> rewind <:*modrm[ModRM] );
// 
// vmcall.disasm = { _sink << "vmcall"; };
// 
// op vmlaunch( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b000[3]:0b010[3]:> rewind <:*modrm[ModRM] );
// 
// vmlaunch.disasm = { _sink << "vmlaunch "; };
// 
// op vmresume( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b000[3]:0b011[3]:> rewind <:*modrm[ModRM] );
// 
// vmresume.disasm = { _sink << "vmresume "; };
// 
// op vmxoff( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b000[3]:0b100[3]:> rewind <:*modrm[ModRM] );
// 
// vmxoff.disasm = { _sink << "vmxoff "; };
// 
// op sidt_ms( 0x0f[8]:> <:0x01[8]:> <:?[2]:0b001[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sidt_ms.disasm = { _sink << "sidtl " << DisasmM( modrm, segment ); };
// 
// op monitor( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b001[3]:0b000[3]:> rewind <:*modrm[ModRM] );
// 
// monitor.disasm = { _sink << "monitor "; };
// 
// op mwait( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b001[3]:0b001[3]:> rewind <:*modrm[ModRM] );
// 
// mwait.disasm = { _sink << "mwait "; };
// 
// op clac( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b001[3]:0b010[3]:> rewind <:*modrm[ModRM] );
// 
// clac.disasm = { _sink << "clac "; };
// 
// op stac( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b001[3]:0b011[3]:> rewind <:*modrm[ModRM] );
// 
// stac.disasm = { _sink << "stac "; };
// 
// op lgdt_ms( 0x0f[8]:> <:0x01[8]:> <:?[2]:0b010[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// lgdt_ms.disasm = { _sink << "lgdtl " << DisasmM( modrm, segment ); };
// 
// op xgetbv( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b010[3]:0b000[3]:> rewind <:*modrm[ModRM] );
// 
// xgetbv.disasm = { _sink << "xgetbv "; };
// 
// op xsetbv( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b010[3]:0b001[3]:> rewind <:*modrm[ModRM] );
// 
// xsetbv.disasm = { _sink << "xsetbv "; };
// 
// op vmfunc( 0x0f[8]:> <:0x01[8]:> <:0b11[2]:0b010[3]:0b100[3]:> rewind <:*modrm[ModRM] );
// 
// vmfunc.disasm = { _sink << "vmfunc "; };
// 
// op lidt_ms( 0x0f[8]:> <:0x01[8]:> <:?[2]:0b011[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// lidt_ms.disasm = { _sink << "lidtl " << DisasmM( modrm, segment ); };
// 
// op smsw_ed( 0x0f[8]:> <:0x01[8]:> <:?[2]:0b100[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// smsw_ed.disasm = { _sink << "smsw " << DisasmEd( modrm, segment ); };
// 
// op lmsw_ed( 0x0f[8]:> <:0x01[8]:> <:?[2]:0b110[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// lmsw_ed.disasm = { _sink << "lmsw " << DisasmEd( modrm, segment ); };
// 
// op invlpg_mb( 0x0f[8]:> <:0x01[8]:> <:?[2]:0b111[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// invlpg_mb.disasm = { _sink << "invlpg " << DisasmM( modrm, segment ); };
// 
// op prefetch( 0x0f[8]:> <:0x0d[8]:> <:?[2]:0b000[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// prefetch.disasm = { _sink << "prefetch " << DisasmM( modrm, segment ); };
// 
// op prefetchw( 0x0f[8]:> <:0x0d[8]:> <:?[2]:0b001[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// prefetchw.disasm = { _sink << "prefetchw " << DisasmM( modrm, segment ); };
// 
// op prefetchwt1( 0x0f[8]:> <:0x0d[8]:> <:?[2]:0b010[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// prefetchwt1.disasm = { _sink << "prefetchwt1 " << DisasmM( modrm, segment ); };
// 
// op sldt( 0x0f[8]:> <:0x0[8]:> <:?[2]:0b000[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sldt.disasm = { _sink << "sldt " << DisasmEd( modrm, segment ); };
// 
// op str( 0x0f[8]:> <:0x0[8]:> <:?[2]:0b001[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// str.disasm = { _sink << "str " << DisasmEd( modrm, segment ); };
// 
// op lldt( 0x0f[8]:> <:0x0[8]:> <:?[2]:0b010[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// lldt.disasm = { _sink << "lldt " << DisasmEw( modrm, segment ); };
// 
// op ltr( 0x0f[8]:> <:0x0[8]:> <:?[2]:0b011[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// ltr.disasm = { _sink << "ltr " << DisasmEw( modrm, segment ); };
// 
// op verr( 0x0f[8]:> <:0x0[8]:> <:?[2]:0b100[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// verr.disasm = { _sink << "verr " << DisasmEw( modrm, segment ); };
// 
// op verw( 0x0f[8]:> <:0x0[8]:> <:?[2]:0b101[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// verw.disasm = { _sink << "verw " << DisasmEw( modrm, segment ); };
// 
// op ldmxcsr( 0x0f[8]:> <:0xae[8]:> <:?[2]:0b010[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// ldmxcsr.disasm = { _sink << "ldmxcsr " << DisasmM( modrm, segment ); };
// 
// op stmxcsr( 0x0f[8]:> <:0xae[8]:> <:?[2]:0b011[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// stmxcsr.disasm = { _sink << "stmxcsr " << DisasmM( modrm, segment ); };
// 
// op xsave( 0x0f[8]:> <:0xae[8]:> <:?[2]:0b100[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// xsave.disasm = { _sink << "xsave " << DisasmM( modrm, segment ); };
// 
// op xrstor( 0x0f[8]:> <:0xae[8]:> <:?[2]:0b101[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// xrstor.disasm = { _sink << "xrstor " << DisasmM( modrm, segment ); };
// 
// op xsaveopt( 0x0f[8]:> <:0xae[8]:> <:?[2]:0b110[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// xsaveopt.disasm = { _sink << "xsaveopt " << DisasmM( modrm, segment ); };
// 
// op clflush( 0x0f[8]:> <:0xae[8]:> <:?[2]:0b111[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// clflush.disasm = { _sink << "clflush " << DisasmM( modrm, segment ); };
// 
// op vmptrld( 0x0f[8]:> <:0xc7[8]:> <:?[2]:0b110[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// vmptrld.disasm = { _sink << "vmptrld " << DisasmM( modrm, segment ); };
// 
// op vmclear( 0x66[8]:> <:0x0f[8]:> <:0xc7[8]:> <:?[2]:0b110[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// vmclear.disasm = { _sink << "vmclear " << DisasmM( modrm, segment ); };
// 
// op vmxon( 0xf3[8]:> <:0x0f[8]:> <:0xc7[8]:> <:?[2]:0b110[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// vmxon.disasm = { _sink << "vmxon " << DisasmM( modrm, segment ); };
// 
// op vmptrst1( 0x0f[8]:> <:0xc7[8]:> <:?[2]:0b111[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// vmptrst1.disasm = { _sink << "vmptrst " << DisasmM( modrm, segment ); };
// 
// op vmptrst2( 0xf3[8]:> <:0x0f[8]:> <:0xc7[8]:> <:?[2]:0b111[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// vmptrst2.disasm = { _sink << "vmptrst " << DisasmM( modrm, segment ); };
// 
// op lfence( 0x0f[8]:> <:0xae[8]:> <:0xe8[8] );
// 
// lfence.disasm = { _sink << "lfence"; };
// 
// op mfence( 0x0f[8]:> <:0xae[8]:> <:0xf0[8] );
// 
// mfence.disasm = { _sink << "mfence"; };
// 
// op sfence( 0x0f[8]:> <:0xae[8]:> <:0xf8[8] );
// 
// sfence.disasm = { _sink << "sfence"; };
// 
