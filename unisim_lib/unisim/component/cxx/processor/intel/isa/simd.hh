/*
 *  Copyright (c) 2007-2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

// /* ADDSUBPD -- Packed Single- or Double-FP Add/Subtract */
// op addsubpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addsubpd_vdq_wdq.disasm = { _sink << "addsubpd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
//  
// op addsubps_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0xd0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addsubps_vdq_wdq.disasm = { _sink << "addsubps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* BLENDP -- Blend Packed Single- or Double-Precision Floating-Point Values */
// op blendps_vdq_wdq_ib( 0x0f[8]:> <:0x3a[8]:> <:0x0c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// blendps_vdq_wdq_ib.disasm = { _sink << "blendps " << DisasmI( imm ) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op blendpd_vdq_wdq_ib( 0x0f[8]:> <:0x3a[8]:> <:0x0d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// blendpd_vdq_wdq_ib.disasm = { _sink << "blendpd " << DisasmI( imm ) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* BLENDVPD -- Variable Blend Packed Double Precision Floating-Point Values */
// op blendvps_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x14[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// blendvps_vdq_wdq.disasm = { _sink << "blendvps %xmm0," << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op blendvpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x15[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// blendvpd_vdq_wdq.disasm = { _sink << "blendvpd %xmm0," << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* CMPP -- Compare Packed Single- or Double-Precision Floating-Point Values */
// op cmpps_vdq_wdq_ib( 0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmpps_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmpps " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn );
//   else           _sink << "cmp" << op[imm&7] << "ps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn );
// };
// 
// op cmppd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmppd_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmppd " << DisasmI( imm ) << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn );
//   else           _sink << "cmp" << op[imm&7] << "pd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn );
// };
// 
// /* CMPSD -- Compare Scalar Single- Double-Precision Floating-Point Values */
// op cmpss_vdq_wdq_ib( 0xf3[8]:> <:0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmpss_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmpss " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn );
//   else           _sink << "cmp" << op[imm&7] << "ss " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn );
// };
// 
// op cmpsd_vdq_wdq_ib( 0xf2[8]:> <:0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmpsd_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmpsd " << DisasmI( imm ) << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn );
//   else           _sink << "cmp" << op[imm&7] << "sd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn );
// };
// 
// /* COMISD -- Compare Scalar Ordered Single- or Double-Precision Floating-Point Values and Set EFLAGS */
// op comisd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// comisd_vdq_wdq.disasm = { _sink << "comisd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op comiss_vdq_wdq( 0x0f[8]:> <:0x2f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// comiss_vdq_wdq.disasm = { _sink << "comiss " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };

/*********************/
/* SIMD CVT Functions*/
/*********************/

template <class ARCH, unsigned DOPSZ, unsigned SOPSZ>
struct CvtI2F : public Operation<ARCH>
{
  CvtI2F( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {}
  RMOp<ARCH> rm; uint8_t gn;
  char const* mnemonic() const { switch (DOPSZ) { case 32: return "cvtdq2ps"; case 64: return "cvtdq2pd"; } return "cvt<bad>"; }
  void disasm( std::ostream& sink ) const { sink << mnemonic() << ' ' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,DOPSZ>::f fdst_type;
    typedef typename TypeFor<ARCH,SOPSZ>::u usrc_type;
    for (unsigned idx = 0, end = 128/DOPSZ; idx < end; ++idx)
      {
        usrc_type src = arch.vmm_read( SSE(), rm, idx, usrc_type() );
        fdst_type dst = fdst_type(src);
        arch.vmm_write( SSE(), gn, idx, dst );
      }
  }
};

template <class ARCH, unsigned DOPSZ, class SOP>
struct CvtsI2F : public Operation<ARCH>
{
  CvtsI2F( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {}
  RMOp<ARCH> rm; uint8_t gn;
  char const* mnemonic() const { switch (DOPSZ) { case 32: return "cvtsi2ss"; case 64: return "cvtsi2sd"; } return "cvt<bad>"; }
  void disasm( std::ostream& sink ) const { sink << mnemonic() << ' ' << DisasmE( SOP(), rm ) << ',' << DisasmV( SSE(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,DOPSZ>::f fdst_type;
    typename TypeFor<ARCH,SOP::SIZE>::u src = arch.rmread( SOP(), rm );
    fdst_type dst = fdst_type(src);
    arch.vmm_write( SSE(), gn, 0, dst );
  }
};

template <class ARCH, unsigned DOPSZ, unsigned SOPSZ>
struct CvtsF2F : public Operation<ARCH>
{
  CvtsF2F( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const
  {
    sink << "cvts" << SizeID<SOPSZ>::fid() << "2s" << SizeID<DOPSZ>::fid() << " " << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,SOPSZ>::f fsrc_type;
    typedef typename TypeFor<ARCH,DOPSZ>::f fdst_type;

    fsrc_type src = arch.vmm_read( SSE(), rm, 0, fsrc_type() );
    //FPToFP( res, op, cpu, cpu.FPSCR );
    fdst_type dst = fdst_type(src);
    
    arch.vmm_write( SSE(), gn, 0, dst );
  }
};

template <class ARCH, unsigned SOPSZ, class DOP>
struct CvttF2I : public Operation<ARCH>
{
  CvttF2I( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {}
  RMOp<ARCH> rm; uint8_t gn;
  char const* mnemonic() const { switch (SOPSZ) { case 32: return "cvttss2si"; case 64: return "cvttsd2si"; } return "cvt<bad>"; }
  void disasm( std::ostream& sink ) const { sink << mnemonic() << ' ' << DisasmW( SSE(), rm ) << ',' << DisasmG( DOP(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,DOP::SIZE>::u udst_type;
    typedef typename TypeFor<ARCH,SOPSZ>::f fsrc_type;
    fsrc_type src = arch.vmm_read( SSE(), rm, 0, fsrc_type() );
    udst_type dst = udst_type(src);
    arch.regwrite( DOP(), gn, dst );
  }
};

template <class ARCH> struct DC<ARCH,CVT> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, simdF2() & opcode( "\x0f\x5a" ) & RM() ))

    return new CvtsF2F<ARCH,32,64>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simdF3() & opcode( "\x0f\x5a" ) & RM() ))

    return new CvtsF2F<ARCH,64,32>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simdF3() & opcode( "\x0f\xe6" ) & RM() ))

    return new CvtI2F<ARCH,64,32>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simd__() & opcode( "\x0f\x5b" ) & RM() ))

    return new CvtI2F<ARCH,32,32>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simdF2() & opcode( "\x0f\x2a" ) & RM() ))
    {
      if (ic.f3())
        {
          if (ic.opsize() == 32) return new CvtsI2F<ARCH,32,GOd>( _.opbase(), _.rmop(), _.greg() );
          if (ic.opsize() == 64) return new CvtsI2F<ARCH,32,GOq>( _.opbase(), _.rmop(), _.greg() );
          return 0;
        }
      if (ic.f2())
        {
          if (ic.opsize() == 32) return new CvtsI2F<ARCH,64,GOd>( _.opbase(), _.rmop(), _.greg() );
          if (ic.opsize() == 64) return new CvtsI2F<ARCH,64,GOq>( _.opbase(), _.rmop(), _.greg() );
          return 0;
        }
    }
  
  if (auto _ = match( ic, opcode( "\x0f\x2c" ) & RM() ))
    {
      if (ic.f3())
        {
          if (ic.opsize() == 32) return new CvttF2I<ARCH,32,GOd>( _.opbase(), _.rmop(), _.greg() );
          if (ic.opsize() == 64) return new CvttF2I<ARCH,32,GOq>( _.opbase(), _.rmop(), _.greg() );
          return 0;
        }
      if (ic.f2())
        {
          if (ic.opsize() == 32) return new CvttF2I<ARCH,64,GOd>( _.opbase(), _.rmop(), _.greg() );
          if (ic.opsize() == 64) return new CvttF2I<ARCH,64,GOq>( _.opbase(), _.rmop(), _.greg() );
          return 0;
        }
    }
  return 0;
}};

template <class ARCH>
struct LSMxcsr : public Operation<ARCH>
{
  LSMxcsr( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, bool _st, bool _vex ) : Operation<ARCH>( opbase ), rm( _rm ), st(_st), vex(_vex) {} RMOp<ARCH> rm; bool st, vex;
  void disasm( std::ostream& sink ) const { sink << (vex ? "v" : "") << (st ? "st" : "ld") << "mxcsr " << DisasmE( GOd(), rm ); }
  void execute( ARCH& arch ) const
  {
    if (st) arch.rmwrite( GOd(), rm, arch.mxcsr );
    else    arch.mxcsr = arch.rmread( GOd(), rm );
  }
};

template <class ARCH> struct DC<ARCH,MXCSR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, vex( "\x0f\xae" ) & RM_mem() ))
    {
      bool vex = ic.vex();
      if (vex and (ic.vlen() != 128 or _.vreg())) return 0;
      unsigned rmopc = _.greg();
      if ((rmopc|1) != 3) return 0;
      return new LSMxcsr<ARCH>( _.opbase(), _.rmop(), rmopc&1, vex );
    }
      
  return 0;
}};

/* CVTDQ2PS -- Convert Packed Dword Integers to Packed Single- or Double-Precision FP Values */
// op cvtdq2ps_vdq_wdq( 0x0f[8]:> <:0x5b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtdq2ps_vdq_wdq.disasm = { _sink << "cvtdq2ps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// op cvtdq2pd_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0xe6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtdq2pd_vdq_wdq.disasm = { _sink << "cvtdq2pd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTSI2SD -- Convert Dword Integer to Scalar Double-Precision FP Value */
// op cvtsi2sd_vdq_ed( 0xf2[8]:> <:0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtsi2sd_vdq_ed.disasm = { _sink << "cvtsi2sd " << DisasmEd( rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTSI2SS -- Convert Dword Integer to Scalar Single-Precision FP Value */
// op cvtsi2ss_vdq_ed( 0xf3[8]:> <:0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtsi2ss_vdq_ed.disasm = { _sink << "cvtsi2ss " << DisasmEd( rmop ) << ',' << DisasmV( SSE(), gn ); };


/* CVTSD2SS -- Convert Scalar Double-Precision FP Value to Scalar Single-Precision FP Value */
// op cvtsd2ss_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
/* CVTPD2DQ -- Convert Packed Double-Precision FP Values to Packed Dword Integers */
// op cvtpd2dq_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0xe6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpd2dq_vdq_wdq.disasm = { _sink << "cvtpd2dq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPD2PI -- Convert Packed Double-Precision FP Values to Packed Dword Integers */
// op cvtpd2pi_pq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpd2pi_pq_wdq.disasm = { _sink << "cvtpd2pi " << DisasmW( SSE(), rmop ) << ',' << DisasmPq( gn ); };
/* CVTPD2PS -- Convert Packed Double-Precision FP Values to Packed Single-Precision FP Values */
// op cvtpd2ps_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpd2ps_vdq_wdq.disasm = { _sink << "cvtpd2ps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPI2P -- Convert Packed Dword Integers to Packed Single- or Double-Precision FP Values */
// op cvtpi2ps_vdq_qq( 0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpi2ps_vdq_qq.disasm = { _sink << "cvtpi2ps " << DisasmQq( rmop ) << ',' << DisasmV( SSE(), gn ); };
// op cvtpi2pd_vdq_qq( 0x66[8]:> <:0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtpi2pd_vdq_qq.disasm = { _sink << "cvtpi2pd " << DisasmQq( rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPS2DQ -- Convert Packed Single-Precision FP Values to Packed Dword Integers */
// op cvtps2dq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtps2dq_vdq_wdq.disasm = { _sink << "cvtps2dq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPS2PD -- Convert Packed Single-Precision FP Values to Packed Double-Precision FP Values */
// op cvtps2pd_vdq_wdq( 0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtps2pd_vdq_wdq.disasm = { _sink << "cvtps2pd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTPS2PI -- Convert Packed Single-Precision FP Values to Packed Dword Integers */
// op cvtps2pi_pq_wdq( 0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtps2pi_pq_wdq.disasm = { _sink << "cvtps2pi " << DisasmW( SSE(), rmop ) << ',' << DisasmPq( gn ); };
/* CVTSD2SI -- Convert Scalar Double-Precision FP Value to Integer */
// op cvtsd2si_gd_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtsd2si_gd_wdq.disasm = { _sink << "cvtsd2si " << DisasmW( SSE(), rmop ) << ',' << DisasmGd( gn ); };
// cvtsd2ss_vdq_wdq.disasm = { _sink << "cvtsd2ss " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTSS2SD -- Convert Scalar Single-Precision FP Value to Scalar Double-Precision FP Value */
// op cvtss2sd_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtss2sd_vdq_wdq.disasm = { _sink << "cvtss2sd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTSS2SI -- Convert Scalar Single-Precision FP Value to Dword Integer */
// op cvtss2si_gd_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvtss2si_gd_wdq.disasm = { _sink << "cvtss2si " << DisasmW( SSE(), rmop ) << ',' << DisasmGd( gn ); };
/* CVTTPD2DQ -- Convert with Truncation Packed Double-Precision FP Values to Packed Dword Integers */
// op cvttpd2dq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttpd2dq_vdq_wdq.disasm = { _sink << "cvttpd2dq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTTPD2PI -- Convert with Truncation Packed Double-Precision FP Values to Packed Dword Integers */
// op cvttpd2pi_pq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttpd2pi_pq_wdq.disasm = { _sink << "cvttpd2pi " << DisasmW( SSE(), rmop ) << ',' << DisasmPq( gn ); };
/* CVTTPS2DQ -- Convert with Truncation Packed Single-Precision FP Values to Packed Dword Integers */
// op cvttps2dq_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttps2dq_vdq_wdq.disasm = { _sink << "cvttps2dq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
/* CVTTPS2PI -- Convert with Truncation Packed Single-Precision FP Values to Packed Dword Integers */
// op cvttps2pi_pq_wdq( 0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttps2pi_pq_wdq.disasm = { _sink << "cvttps2pi " << DisasmW( SSE(), rmop ) << ',' << DisasmPq( gn ); };
/* CVTTSD2SI -- Convert with Truncation Scalar Double-Precision FP Value to Integer */
// op cvttsd2si_gd_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttsd2si_gd_wdq.disasm = { _sink << "cvttsd2si " << DisasmW( SSE(), rmop ) << ',' << DisasmGd( gn ); };
/* CVTTSS2SI -- Convert with Truncation Scalar Single-Precision FP Value to Dword Integer */
// op cvttss2si_gd_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// cvttss2si_gd_wdq.disasm = { _sink << "cvttss2si " << DisasmW( SSE(), rmop ) << ',' << DisasmGd( gn ); };


// /* DPPD -- Dot Product of Packed Double Precision Floating-Point Values */
// op dpps_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x40[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// dpps_vdq_wdq_ib.disasm = { _sink << "dpps " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op dppd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x41[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// dppd_vdq_wdq_ib.disasm = { _sink << "dppd " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* EMMS -- Empty MMX Technology State */
// op emms( 0x0f[8]:> <:0x77[8] );
// 
// emms.disasm = { _sink << "emms"; };
// 
// /* EXTRACTPS -- Extract Packed Single Precision Floating-Point Value */
// op extractps_rd_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x17[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// extractps_rd_wdq_ib.disasm = { _sink << "extractps " << DisasmI(imm) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEd( rmop ); };
// 
// /* FXSAVE/FXRSTOR -- Save/Restore x87 FPU, MMX Technology, and SSE State */
// op fxrstor( 0x0f[8]:> <:0xae[8]:> <:?[2]:1[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// fxrstor.disasm = { _sink << "fxrstor " << DisasmM( rmop ); };
// 
// op fxsave( 0x0f[8]:> <:0xae[8]:> <:?[2]:0[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// fxsave.disasm = { _sink << "fxsave " << DisasmM( rmop ); };
// 
// /* HADDPD -- Packed Single- or Double-FP Horizontal Add */
// op haddps_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x7c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// haddps_vdq_wdq.disasm = { _sink << "haddps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op haddpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x7c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// haddpd_vdq_wdq.disasm = { _sink << "haddpd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* HSUBPD -- Packed Single- or Double-FP Horizontal Substract */
// op hsubps_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x7d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// hsubps_vdq_wdq.disasm = { _sink << "hsubps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op hsubpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x7d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// hsubpd_vdq_wdq.disasm = { _sink << "hsubpd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* INSERTPS -- Insert Packed Single Precision Floating-Point Value */
// op insertps_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x21[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// insertps_vdq_wdq_ib.disasm = { _sink << "insertps " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* LDDQU -- Load Unaligned Integer 128 Bits */
// op lddqu_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0xf0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// lddqu_vdq_wdq.disasm = { _sink << "lddqu " << DisasmM( rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MASKMOVDQU -- Store Selected Bytes of Double Quadword */
// op maskmovdqu_vdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0xf7[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// maskmovdqu_vdq_vdq.disasm = { _sink << "maskmovdqu " << DisasmV( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MASKMOVQ */
// op maskmovq_pq_nq( 0x0f[8]:> <:0xf7[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// maskmovq_pq_nq.disasm = { _sink << "maskmovq " << DisasmPq( rm ) << ',' << DisasmPq( gn ); };
// 
// /* MAXPS -- Return Maximum Packed Single-Precision Floating-Point Values */
// op maxps_vdq_wdq( 0x0f[8]:> <:0x5f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// maxps_vdq_wdq.disasm = { _sink << "maxps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MAXPD -- Return Maximum Packed Double-Precision Floating-Point Values */
// op maxpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// maxpd_vdq_wdq.disasm = { _sink << "maxpd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MAXSS -- Return Maximum Scalar Single-Precision Floating-Point Values */
// op maxss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// maxss_vdq_wdq.disasm = { _sink << "maxss " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MAXSD -- Return Maximum Scalar Double-Precision Floating-Point Values */
// op maxsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// maxsd_vdq_wdq.disasm = { _sink << "maxsd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MINPS -- Return Minimum Packed Single-Precision Floating-Point Values */
// op minps_vdq_wdq( 0x0f[8]:> <:0x5d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// minps_vdq_wdq.disasm = { _sink << "minps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MINPD -- Return Minimum Packed Double-Precision Floating-Point Values */
// op minpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// minpd_vdq_wdq.disasm = { _sink << "minpd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MINSS -- Return Minimum Scalar Single-Precision Floating-Point Values */
// op minss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// minss_vdq_wdq.disasm = { _sink << "minss " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MINSD -- Return Minimum Scalar Double-Precision Floating-Point Values */
// op minsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// minsd_vdq_wdq.disasm = { _sink << "minsd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MOVD/MOVQ -- Move Doubleword/Move Quadword */
template <class ARCH, class GOP>
struct MovEP : public Operation<ARCH>
{
  MovEP( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "mov" << (GOP::SIZE==64?'q':'d') << ' ' << DisasmPq( gn ) << ',' << DisasmE( GOP(), rmop ); }
};

template <class ARCH, class GOP>
struct MovPE : public Operation<ARCH>
{
  MovPE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "mov" << (GOP::SIZE==64?'q':'d') << ' ' << DisasmE( GOP(), rmop ) << ',' << DisasmPq( gn ); }
};

template <class ARCH, class VR, class GOP>
struct MovEV : public Operation<ARCH>
{
  MovEV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << (VR::vex()?"v":"") << "mov" << SizeID<GOP::SIZE>::iid() << ' ' << DisasmV( SSE(), gn ) << ',' << DisasmE( GOP(), rmop ); }
  void execute( ARCH& arch ) const
  {
    arch.rmwrite( GOP(), rmop, arch.vmm_read( VR(), gn, 0, typename TypeFor<ARCH,GOP::SIZE>::u() ) );
  }
};

template <class ARCH, class VR, class GOP>
struct MovVE : public Operation<ARCH>
{
  MovVE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << (VR::vex()?"v":"") << "mov" << SizeID<GOP::SIZE>::iid() << ' ' << DisasmE( GOP(), rmop ) << ',' << DisasmV( SSE(), gn ); }
  void execute( ARCH& arch ) const
  {
    for (unsigned idx = 0, end = VR::size() / GOP::SIZE; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, typename TypeFor<ARCH,GOP::SIZE>::u(0) );
    arch.vmm_write( VR(), gn, 0, arch.rmread( GOP(), rmop ) );
  }
};

template <class ARCH> struct DC<ARCH,MOVGV> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  bool vex_p = ic.vex(), quad_p = (ic.opsize() == 64);
  if (not vex_p and not quad_p) return getMOVGV<SSE,GOd>( ic );
  if (not vex_p and     quad_p) return getMOVGV<SSE,GOq>( ic );
  if (    vex_p and (ic.vlen() != 128 or ic.vreg())) return 0;
  if (    vex_p and not quad_p) return getMOVGV<XMM,GOd>( ic );
  if (    vex_p and     quad_p) return getMOVGV<XMM,GOq>( ic );
  return 0;
}
template <typename VR, typename GOP> Operation<ARCH>* getMOVGV( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, simd__() & opcode( "\x0f\x6e" ) & RM() ))
  
    return ic.vex() ? 0 : new MovPE<ARCH,GOP>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simd__() & opcode( "\x0f\x7e" ) & RM() ))
  
    return ic.vex() ? 0 : new MovEP<ARCH,GOP>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x6e" ) & RM() ))
  
    return new MovVE<ARCH,VR,GOP>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, vex( "\x66\x0f\x7e" ) & RM() ))
  
    return new MovEV<ARCH,VR,GOP>( _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}
};

template <class ARCH, bool ALIGNED>
struct MovdqVW : public Operation<ARCH>
{
  MovdqVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "movdq" << (ALIGNED?"a ":"u ") << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( SSE(), gn, 0,  arch.vmm_read( SSE(), rmop, 0, typename ARCH::u64_t() ) );
    arch.vmm_write( SSE(), gn, 1,  arch.vmm_read( SSE(), rmop, 1, typename ARCH::u64_t() ) );
  }
};

template <class ARCH, bool ALIGNED>
struct MovdqWV : public Operation<ARCH>
{
  MovdqWV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "movdq" << (ALIGNED?"a ":"u ") << DisasmV( SSE(), gn ) << ',' << DisasmW( SSE(), rmop ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( SSE(), rmop, 0, arch.vmm_read( SSE(), gn, 0, typename ARCH::u64_t() ) );
    arch.vmm_write( SSE(), rmop, 1, arch.vmm_read( SSE(), gn, 1, typename ARCH::u64_t() ) );
  }
};

/* MOVDQA/MOVDQU -- Move Aligned Unaligned Double Quadword */
template <class ARCH> struct DC<ARCH,MOVDQ> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, simdF3() & opcode( "\x0f\x6f" ) & RM() ))
    
    return new MovdqVW<ARCH,false>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simdF3() & opcode( "\x0f\x7f" ) & RM() ))
    
    return new MovdqWV<ARCH,false>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simd66() & opcode( "\x0f\x6f" ) & RM() ))
    
    return new MovdqVW<ARCH,true>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simd66() & opcode( "\x0f\x7f" ) & RM() ))
    
    return new MovdqWV<ARCH,true>( _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}};


// /* MOVAP */
// op movaps_vdq_wdq( 0x0f[8]:> <:0x28[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movaps_vdq_wdq.disasm = { _sink << "movaps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// op movaps_wdq_vdq( 0x0f[8]:> <:0x29[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movaps_wdq_vdq.disasm = { _sink << "movaps " << DisasmV( SSE(), gn ) << ',' << DisasmW( SSE(), rmop ); };
// op movapd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x28[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movapd_vdq_wdq.disasm = { _sink << "movapd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// op movapd_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x29[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movapd_wdq_vdq.disasm = { _sink << "movapd " << DisasmV( SSE(), gn ) << ',' << DisasmW( SSE(), rmop ); };
// op movups_vdq_wdq( 0x0f[8]:> <:0x10[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movups_vdq_wdq.disasm = { _sink << "movups " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// op movups_wdq_vdq( 0x0f[8]:> <:0x11[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movups_wdq_vdq.disasm = { _sink << "movups " << DisasmV( SSE(), gn ) << ',' << DisasmW( SSE(), rmop ); };
// op movupd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x10[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movupd_vdq_wdq.disasm = { _sink << "movupd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// op movupd_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x11[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movupd_wdq_vdq.disasm = { _sink << "movupd " << DisasmV( SSE(), gn ) << ',' << DisasmW( SSE(), rmop ); };

template <class VR, unsigned OPSIZE, bool ALIGNED>
struct MovfpMnemo
{
  struct S
  {
    friend std::ostream& operator << ( std::ostream& sink, S const& )
    {
      sink << (VR::vex() ? "v" : "") << "mov" << (ALIGNED ? 'a' : 'u')
           << 'p' << (OPSIZE == 32 ? "s" : OPSIZE == 64 ? "d" : "<bad>");
      return sink;
    }
  };
  S mnemonic() const { return S(); }
};

template <class ARCH, class VR, unsigned OPSIZE, bool ALIGNED>
struct MovfpVW : public Operation<ARCH>, MovfpMnemo<VR,OPSIZE,ALIGNED>
{
  MovfpVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << this->mnemonic() << ' '<< DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    for (unsigned idx = 0, end = VR::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx,  arch.vmm_read( VR(), rm, idx, u_type() ) );
  }
};

template <class ARCH, class VR, unsigned OPSIZE, bool ALIGNED>
struct MovfpWV : public Operation<ARCH>, MovfpMnemo<VR,OPSIZE,ALIGNED>
{
  MovfpWV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << this->mnemonic() << DisasmV( VR(), gn ) << ',' << DisasmW( VR(), rm ); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    for (unsigned idx = 0, end = VR::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( VR(), rm, idx,  arch.vmm_read( VR(), gn, idx, u_type() ) );
  }
};

/* TODO: should move real floating point value */
template <class ARCH> struct DC<ARCH,MOVFP> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, vex( "\x0f\x28" ) & RM() ))
    {
      if (not ic.vex()) return new MovfpVW<ARCH,SSE,32,true>( _.opbase(), _.rmop(), _.greg() );
      if (_.vreg()) return 0;
      if (ic.vlen() == 128) return new MovfpVW<ARCH,XMM,32,true>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vlen() == 256) return new MovfpVW<ARCH,YMM,32,true>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, vex( "\x0f\x29" ) & RM() ))
    {
      if (not ic.vex()) return new MovfpWV<ARCH,SSE,32,true>( _.opbase(), _.rmop(), _.greg() );
      if (_.vreg()) return 0;
      if (ic.vlen() == 128) return new MovfpWV<ARCH,XMM,32,true>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vlen() == 256) return new MovfpWV<ARCH,YMM,32,true>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }

  if (auto _ = match( ic, vex( "\x66\x0f\x28" ) & RM() ))
    {
      if (not ic.vex()) return new MovfpVW<ARCH,SSE,64,true>( _.opbase(), _.rmop(), _.greg() );
      if (_.vreg()) return 0;
      if (ic.vlen() == 128) return new MovfpVW<ARCH,XMM,64,true>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vlen() == 256) return new MovfpVW<ARCH,YMM,64,true>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, vex( "\x66\x0f\x29" ) & RM() ))
    {
      if (not ic.vex()) return new MovfpWV<ARCH,SSE,64,true>( _.opbase(), _.rmop(), _.greg() );
      if (_.vreg()) return 0;
      if (ic.vlen() == 128) return new MovfpWV<ARCH,XMM,64,true>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vlen() == 256) return new MovfpWV<ARCH,YMM,64,true>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, vex( "\x0f\x10" ) & RM() ))
    {
      if (not ic.vex()) return new MovfpVW<ARCH,SSE,32,false>( _.opbase(), _.rmop(), _.greg() );
      if (_.vreg()) return 0;
      if (ic.vlen() == 128) return new MovfpVW<ARCH,XMM,32,false>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vlen() == 256) return new MovfpVW<ARCH,YMM,32,false>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, vex( "\x0f\x11" ) & RM() ))
    {
      if (not ic.vex()) return new MovfpWV<ARCH,SSE,32,false>( _.opbase(), _.rmop(), _.greg() );
      if (_.vreg()) return 0;
      if (ic.vlen() == 128) return new MovfpWV<ARCH,XMM,32,false>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vlen() == 256) return new MovfpWV<ARCH,YMM,32,false>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }

  if (auto _ = match( ic, vex( "\x66\x0f\x10" ) & RM() ))
    {
      if (not ic.vex()) return new MovfpVW<ARCH,SSE,64,false>( _.opbase(), _.rmop(), _.greg() );
      if (_.vreg()) return 0;
      if (ic.vlen() == 128) return new MovfpVW<ARCH,XMM,64,false>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vlen() == 256) return new MovfpVW<ARCH,YMM,64,false>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }

  if (auto _ = match( ic, vex( "\x66\x0f\x11" ) & RM() ))
    {
      if (not ic.vex()) return new MovfpWV<ARCH,SSE,64,false>( _.opbase(), _.rmop(), _.greg() );
      if (_.vreg()) return 0;
      if (ic.vlen() == 128) return new MovfpWV<ARCH,XMM,64,false>( _.opbase(), _.rmop(), _.greg() );
      if (ic.vlen() == 256) return new MovfpWV<ARCH,YMM,64,false>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }

  return 0;
}};


// /* MOVDQ2Q -- Move Quadword from XMM to MMX Technology Register */
// op movdq2q_pq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x2d[8]:> <:0b11[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM] );
// 
// movdq2q_pq_vdq.disasm = { _sink << "movdq2q " << DisasmV( SSE(), rm ) << ',' << DisasmPq( gn ); };
// 
// /* MOVHLPS -- Move Packed Single-Precision Floating-Point Values High to Low */
// op movhlps_vdq_vdq( 0x0f[8]:> <:0x12[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movhlps_vdq_vdq.disasm = { _sink << "movhlps " << DisasmV( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MOVHPD -- Move High Packed Double-Precision Floating-Point Value */
// op movhpd_vdq_mdq( 0x66[8]:> <:0x0f[8]:> <:0x16[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movhpd_vdq_mdq.disasm = { _sink << "movhpd " << DisasmM( rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op movhpd_mdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x17[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movhpd_mdq_vdq.disasm = { _sink << "movhpd " << DisasmV( SSE(), gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVHPS -- Move High Packed Single-Precision Floating-Point Value */
// op movhps_vdq_mdq( 0x0f[8]:> <:0x16[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movhps_vdq_mdq.disasm = { _sink << "movhps " << DisasmM( rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op movhps_mdq_vdq( 0x0f[8]:> <:0x17[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movhps_mdq_vdq.disasm = { _sink << "movhps " << DisasmV( SSE(), gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVLHPS -- Move Packed Single-Precision Floating-Point Values Low to High */
// op movlhps_vdq_wdq( 0x0f[8]:> <:0x16[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movlhps_vdq_wdq.disasm = { _sink << "movlhps " << DisasmV( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MOVLPD -- Move Low Packed Double-Precision Floating-Point Value */
// op movlpd_vdq_mdq( 0x66[8]:> <:0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movlpd_vdq_mdq.disasm = { _sink << "movlpd " << DisasmM( rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op movlpd_mdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x13[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movlpd_mdq_vdq.disasm = { _sink << "movlpd " << DisasmV( SSE(), gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVLPS -- Move Low Packed Single-Precision Floating-Point Value */
// op movlps_vdq_mdq( 0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movlps_vdq_mdq.disasm = { _sink << "movlps " << DisasmM( rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op movlps_mdq_vdq( 0x0f[8]:> <:0x13[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movlps_mdq_vdq.disasm = { _sink << "movlps " << DisasmV( SSE(), gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVMSKPD -- Extract Packed Single- or Double-Precision Floating-Point Sign Mask */
// op movmskpd_gd_vdq( 0x66[8]:> <:0x0f[8]:> <:0x50[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movmskpd_gd_vdq.disasm = { _sink << "movmskpd " << DisasmV( SSE(), rm ) << ',' << DisasmGd( gn ); };
// 
// op movmskps_gd_vdq( 0x0f[8]:> <:0x50[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movmskps_gd_vdq.disasm = { _sink << "movmskps " << DisasmV( SSE(), rm ) << ',' << DisasmGd( gn ); };
// 
// /* MOVNTDQA  --  Load Double Quadword Non-Temporal Aligned Hint */
// op movntdqa_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntdqa_vdq_wdq.disasm = { _sink << "movntdqa " << DisasmM( rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MOVNTDQ -- Store Double Quadword Using Non-Temporal Hint */
// op movntdq_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0xe7[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntdq_wdq_vdq.disasm = { _sink << "movntdq " << DisasmV( SSE(), gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVNTPD -- Store Packed Single- or Double-Precision Floating-Point Values Using Non-Temporal Hint */
// op movntpd_mdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x2b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntpd_mdq_vdq.disasm = { _sink << "movntpd " << DisasmV( SSE(), gn ) << ',' << DisasmM( rmop ); };
// 
// op movntps_mdq_vdq( 0x0f[8]:> <:0x2b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntps_mdq_vdq.disasm = { _sink << "movntps " << DisasmV( SSE(), gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVNTQ -- Store Double Quadword Using Non-Temporal Hint */
// op movntq_mq_pq( 0x0f[8]:> <:0xe7[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntq_mq_pq.disasm = { _sink << "movntq " << DisasmPq( gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVQ -- Move Quadword (MMX) */
// op movq_pq_mq( 0x0f[8]:> <:0x6f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movq_pq_mq.disasm = { _sink << "movq " << DisasmM( rmop ) << ',' << DisasmPq( gn ); };
// 
// op movq_mq_pq( 0x0f[8]:> <:0x7f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movq_mq_pq.disasm = { _sink << "movq " << DisasmPq( gn ) << ',' << DisasmM( rmop ); };
// 
// op movq_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x7e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movq_vdq_wdq.disasm = { _sink << "movq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op movq_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0xd6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movq_wdq_vdq.disasm = { _sink << "movq " << DisasmV( SSE(), gn ) << ',' << DisasmW( SSE(), rmop ); };
// 
// /* MOVQ2DQ -- Move Quadword from MMX Technology to XMM Register */
// op movq2dq_vdq_pq( 0xf3[8]:> <:0x0f[8]:> <:0xd6[8]:> <:0b11[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM] );
// 
// movq2dq_vdq_pq.disasm = { _sink << "movq2dq " << DisasmV( SSE(), gn ) << ',' << DisasmPq( rm ); };
//

// /* MOVSS -- Move Scalar Single-Precision Floating-Point Values */
// /* MOVSD -- Move Scalar Double-Precision Floating-Point Value */

template <class VR, unsigned OPSIZE>
struct LSMnemo
{
  typedef LSMnemo<VR,OPSIZE> this_type;
  friend std::ostream& operator << ( std::ostream& sink, this_type const& ) { sink << (VR::vex() ? "v" : "") << "movs" << SizeID<OPSIZE>::fid(); return sink; };
  this_type const& mnemonic() const { return *this; }
};

template <class ARCH, class VR, unsigned OPSIZE>
struct LSFPS : public Operation<ARCH>
{
  LSFPS( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, unsigned _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {}
  RMOp<ARCH> rm; unsigned gn;
};

template <class ARCH, class VR, unsigned OPSIZE>
struct StoreFPS : public LSFPS<ARCH,VR,OPSIZE>, public LSMnemo<VR,OPSIZE>
{
  typedef LSFPS<ARCH,VR,OPSIZE> LSBASE; using LSBASE::LSFPS; using LSBASE::rm; using LSBASE::gn;
  void disasm( std::ostream& sink ) const { sink << this->mnemonic() << ' ' << DisasmV( VR(), gn ) << ',' << DisasmW( VR(), rm ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( VR(), rm, 0, arch.vmm_read( VR(), gn, 0, typename TypeFor<ARCH,OPSIZE>::f() ) );
  }
};

template <class ARCH, class VR, unsigned OPSIZE>
struct LoadFPS : public LSFPS<ARCH,VR,OPSIZE>, public LSMnemo<VR,OPSIZE>
{
  typedef LSFPS<ARCH,VR,OPSIZE> LSBASE; using LSBASE::LSFPS; using LSBASE::rm; using LSBASE::gn;
  void disasm( std::ostream& sink ) const { sink << this->mnemonic() << ' ' << DisasmW( VR(), rm ) << ',' << DisasmV( VR(), gn ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( VR(), gn, 0, arch.vmm_read( VR(), rm, 0, typename TypeFor<ARCH,OPSIZE>::f() ) );
    for (unsigned idx = 1, end = VR::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, typename TypeFor<ARCH,OPSIZE>::f(0) );
  }
};

template <class ARCH, unsigned OPSIZE>
struct MergeFPS : public Operation<ARCH>, public LSMnemo<XMM,OPSIZE>
{
  MergeFPS(OpBase<ARCH> const& opbase, unsigned _dst, unsigned _src1, unsigned _src2)
    : Operation<ARCH>(opbase), dst(_dst), src1(_src1), src2(_src2)
  {}
  void disasm( std::ostream& sink ) const { sink << this->mnemonic() << ' ' << DisasmV( XMM(), src1 ) << ',' << DisasmV( XMM(), src2 ) << ',' << DisasmV( XMM(), dst ); }
  void execute( ARCH& arch ) const
  {
    arch.vmm_write( XMM(), dst, 0, arch.vmm_read( XMM(), src2, 0, typename TypeFor<ARCH,OPSIZE>::f() ) );
    for (unsigned idx = 1, end = XMM::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( XMM(), dst, idx, arch.vmm_read( XMM(), src1, idx, typename TypeFor<ARCH,OPSIZE>::f() ) );
  }
  uint8_t dst, src1, src2;
};


template <class ARCH> struct DC<ARCH,VMOVSD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "\xf3\x0f\x10" ) & RM() ))
    
    return newMovs<32>( ic, false, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\xf3\x0f\x11" ) & RM() ))
    
    return newMovs<32>( ic,  true, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\xf2\x0f\x10" ) & RM() ))
    
    return newMovs<64>( ic, false, _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, vex( "\xf2\x0f\x11" ) & RM() ))
    
    return newMovs<64>( ic,  true, _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}
template <unsigned OPSIZE>
Operation<ARCH>* newMovs( InputCode<ARCH> const& ic, bool store, OpBase<ARCH> const& opbase, MOp<ARCH> const* rm, unsigned gn )
{
  bool is_mem; { RMOp<ARCH> _rm(rm); is_mem = _rm.is_memory_operand(); _rm.release(); }

  if (is_mem ? store : not ic.vex())
    {
      // Store
      if (not ic.vex()) return new StoreFPS<ARCH,SSE,OPSIZE>( opbase, rm, gn );
      else              return new StoreFPS<ARCH,XMM,OPSIZE>( opbase, rm, gn );
    }

  if (is_mem)
    {
      // Load
      if (not ic.vex()) return new LoadFPS<ARCH,SSE,OPSIZE>( opbase, rm, gn );
      else              return new LoadFPS<ARCH,XMM,OPSIZE>( opbase, rm, gn );
    }

  // Merger (AVX only)
  unsigned dst = gn, src1 = ic.vreg(), src2 = unsigned(uintptr_t(rm));
  if (store) std::swap(dst,src2);
  return new MergeFPS<ARCH,OPSIZE>( opbase, dst, src1, src2 );
}
};

// /* MOVSHDUP -- Move Packed Single-FP High and Duplicate */
// op movshdup_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x16[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movshdup_vdq_wdq.disasm = { _sink << "movshdup " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MOVSLDUP -- Move Packed Single-FP Low and Duplicate */
// op movsldup_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movsldup_vdq_wdq.disasm = { _sink << "movsldup " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MPSADBW -- Compute Multiple Packed Sums of Absolute Difference */
// op mpsadbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x42[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// mpsadbw_vdq_wdq.disasm = { _sink << "mpsadbw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
//
template <class VR, unsigned OPSZ, bool PACKED>
struct VMnemo
{
  struct ToPrint
  {
    ToPrint( char const* _base ) : base(_base) {} char const* base;
    friend std::ostream& operator << (std::ostream& sink, ToPrint const& tp)
    {
      sink << (VR::vex() ? "v" : "") << tp.base << (PACKED ? "p" : "s") << (OPSZ == 32 ? "s" : "d");
      return sink;
    }
  };
  ToPrint mnemonic( char const* base ) const { return ToPrint( base ); }
};

template <class ARCH, class VR, unsigned OPSZ, bool PACKED>
struct MulVVW : public Operation<ARCH>, VMnemo<VR,OPSZ,PACKED>
{
  MulVVW( OpBase<ARCH> const& opbase, uint8_t _gn, uint8_t _vn, MOp<ARCH> const* _rm )
    : Operation<ARCH>( opbase ), rm( _rm ), vn( _vn ), gn( _gn )
  {}
  void disasm( std::ostream& sink ) const
  {
    sink << this->mnemonic( "mul" ) << ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSZ>::f f_type;
    for (unsigned idx = 0, end = PACKED ? VR::size()/OPSZ : 1; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, arch.vmm_read( VR(), vn, idx, f_type() ) * arch.vmm_read( VR(), rm, idx, f_type() ) );
  }
  RMOp<ARCH> rm; uint8_t vn, gn;
};

/* MUL -- Multiply Scalar or Packed Single- or Double-Precision Floating-Point Values */
template <class ARCH> struct DC<ARCH,VMUL> {
template <class VR> Operation<ARCH>*
newMulVVW( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, unsigned greg, unsigned vreg, MOp<ARCH> const* _rm )
{
  if (match( ic, simd__() )) return new MulVVW<ARCH,VR,32, true>( opbase, greg, vreg, _rm );
  if (match( ic, simd66() )) return new MulVVW<ARCH,VR,64, true>( opbase, greg, vreg, _rm );
  if (match( ic, simdF3() )) return new MulVVW<ARCH,VR,32,false>( opbase, greg, vreg, _rm );
  if (match( ic, simdF2() )) return new MulVVW<ARCH,VR,64,false>( opbase, greg, vreg, _rm );
  return 0;
}
Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, vex( "*\x0f\x59" ) & RM() ))
    {
      unsigned greg = _.greg();
      if (not ic.vex())     return newMulVVW<SSE>( ic, _.opbase(), greg, greg, _.rmop() );
      unsigned vreg = ic.vreg();
      if (ic.vlen() == 128) return newMulVVW<XMM>( ic, _.opbase(), greg, vreg, _.rmop() );
      if (ic.vlen() == 256) return newMulVVW<YMM>( ic, _.opbase(), greg, vreg, _.rmop() );
    }
  
  return 0;
}};

template <class ARCH, class VR, unsigned OPSZ, bool PACKED>
struct DivVVW : public Operation<ARCH>, VMnemo<VR,OPSZ,PACKED>
{
  DivVVW( OpBase<ARCH> const& opbase, uint8_t _gn, uint8_t _vn, MOp<ARCH> const* _rm )
    : Operation<ARCH>( opbase ), rm( _rm ), vn( _vn ), gn( _gn )
  {}
  void disasm( std::ostream& sink ) const
  {
    sink << this->mnemonic( "div" ) << ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSZ>::f f_type;
    for (unsigned idx = 0, end = PACKED ? VR::size()/OPSZ : 1; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, arch.vmm_read( VR(), gn, idx, f_type() ) / arch.vmm_read( VR(), rm, idx, f_type() ) );
  }
  RMOp<ARCH> rm; uint8_t vn, gn;
};

/* DIV -- Divide Scalar or Packed Single- or Double-Precision Floating-Point Values */
template <class ARCH> struct DC<ARCH,VDIV> {
template <class VR> Operation<ARCH>*
newDivVVW( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, unsigned greg, unsigned vreg, MOp<ARCH> const* _rm )
{
  if (match( ic, simd__() )) return new DivVVW<ARCH,VR,32, true>( opbase, greg, vreg, _rm );
  if (match( ic, simd66() )) return new DivVVW<ARCH,VR,64, true>( opbase, greg, vreg, _rm );
  if (match( ic, simdF3() )) return new DivVVW<ARCH,VR,32,false>( opbase, greg, vreg, _rm );
  if (match( ic, simdF2() )) return new DivVVW<ARCH,VR,64,false>( opbase, greg, vreg, _rm );
  return 0;
}
Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "*\x0f\x5e" ) & RM() ))
    {
      unsigned greg = _.greg();
      if (not ic.vex())     return newDivVVW<SSE>( ic, _.opbase(), greg, greg, _.rmop() );
      unsigned vreg = ic.vreg();
      if (ic.vlen() == 128) return newDivVVW<XMM>( ic, _.opbase(), greg, vreg, _.rmop() );
      if (ic.vlen() == 256) return newDivVVW<YMM>( ic, _.opbase(), greg, vreg, _.rmop() );
    }
  
  return 0;
}};

template <class ARCH, class VR, unsigned OPSZ, bool PACKED>
struct SubVVW : public Operation<ARCH>, VMnemo<VR,OPSZ,PACKED>
{
  SubVVW( OpBase<ARCH> const& opbase, uint8_t _gn, uint8_t _vn, MOp<ARCH> const* _rm )
    : Operation<ARCH>( opbase ), rm( _rm ), vn( _vn ), gn( _gn )
  {}
  void disasm( std::ostream& sink ) const
  {
    sink << this->mnemonic( "sub" ) << ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSZ>::f f_type;
    for (unsigned idx = 0, end = PACKED ? VR::size()/OPSZ : 1; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, arch.vmm_read( VR(), vn, idx, f_type() ) - arch.vmm_read( VR(), rm, idx, f_type() ) );
  }
  RMOp<ARCH> rm; uint8_t vn, gn;
};

/* SUB -- Substract Scalar or Packed Single- or Double-Precision Floating-Point Values */
template <class ARCH> struct DC<ARCH,VSUB> {
template <class VR> Operation<ARCH>*
newSubVVW( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, unsigned greg, unsigned vreg, MOp<ARCH> const* _rm )
{
  if (match( ic, simd__() )) return new SubVVW<ARCH,VR,32, true>( opbase, greg, vreg, _rm );
  if (match( ic, simd66() )) return new SubVVW<ARCH,VR,64, true>( opbase, greg, vreg, _rm );
  if (match( ic, simdF3() )) return new SubVVW<ARCH,VR,32,false>( opbase, greg, vreg, _rm );
  if (match( ic, simdF2() )) return new SubVVW<ARCH,VR,64,false>( opbase, greg, vreg, _rm );
  return 0;
}
Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "*\x0f\x5c" ) & RM() ))
    {
      unsigned greg = _.greg();
      if (not ic.vex())     return newSubVVW<SSE>( ic, _.opbase(), greg, greg, _.rmop() );
      unsigned vreg = ic.vreg();
      if (ic.vlen() == 128) return newSubVVW<XMM>( ic, _.opbase(), greg, vreg, _.rmop() );
      if (ic.vlen() == 256) return newSubVVW<YMM>( ic, _.opbase(), greg, vreg, _.rmop() );
    }
  
  return 0;
}};

template <class ARCH, class VR, unsigned OPSZ, bool PACKED>
struct AddVVW : public Operation<ARCH>, VMnemo<VR,OPSZ,PACKED>
{
  AddVVW( OpBase<ARCH> const& opbase, uint8_t _gn, uint8_t _vn, MOp<ARCH> const* _rm )
    : Operation<ARCH>( opbase ), rm( _rm ), vn( _vn ), gn( _gn )
  {}
  void disasm( std::ostream& sink ) const
  {
    sink << this->mnemonic( "add" ) << ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSZ>::f f_type;
    for (unsigned idx = 0, end = PACKED ? VR::size()/OPSZ : 1; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, arch.vmm_read( VR(), vn, idx, f_type() ) + arch.vmm_read( VR(), rm, idx, f_type() ) );
  }
  RMOp<ARCH> rm; uint8_t vn, gn;
};

/* ADD -- Add Scalar or Packed Single- or Double-Precision Floating-Point Values */
template <class ARCH> struct DC<ARCH,VADD> {
template <class VR> Operation<ARCH>*
newAddVVW( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, unsigned greg, unsigned vreg, MOp<ARCH> const* _rm )
{
  if (match( ic, simd__() )) return new AddVVW<ARCH,VR,32, true>( opbase, greg, vreg, _rm );
  if (match( ic, simd66() )) return new AddVVW<ARCH,VR,64, true>( opbase, greg, vreg, _rm );
  if (match( ic, simdF3() )) return new AddVVW<ARCH,VR,32,false>( opbase, greg, vreg, _rm );
  if (match( ic, simdF2() )) return new AddVVW<ARCH,VR,64,false>( opbase, greg, vreg, _rm );
  return 0;
}
Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "*\x0f\x58" ) & RM() ))
    {
      unsigned greg = _.greg();
      if (not ic.vex())     return newAddVVW<SSE>( ic, _.opbase(), greg, greg, _.rmop() );
      unsigned vreg = ic.vreg();
      if (ic.vlen() == 128) return newAddVVW<XMM>( ic, _.opbase(), greg, vreg, _.rmop() );
      if (ic.vlen() == 256) return newAddVVW<YMM>( ic, _.opbase(), greg, vreg, _.rmop() );
    }
  
  return 0;
}};


// /* PABSB/PABSW/PABSD -- Packed Absolute Value */
// op pabsb_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x1c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsb_pq_qq.disasm = { _sink << "pabsb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pabsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x1d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsw_pq_qq.disasm = { _sink << "pabsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pabsd_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x1e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsd_pq_qq.disasm = { _sink << "pabsd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pabsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x1c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsb_vdq_wdq.disasm = { _sink << "pabsb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pabsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x1d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsw_vdq_wdq.disasm = { _sink << "pabsw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pabsd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x1e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsd_vdq_wdq.disasm = { _sink << "pabsd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PACKSSWB/PACKSSDW -- Pack with Signed Saturation */
// op packsswb_pq_qq( 0x0f[8]:> <:0x63[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packsswb_pq_qq.disasm = { _sink << "packsswb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op packssdw_pq_qq( 0x0f[8]:> <:0x6b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packssdw_pq_qq.disasm = { _sink << "packssdw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op packsswb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x63[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packsswb_vdq_wdq.disasm = { _sink << "packsswb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op packssdw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x6b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packssdw_vdq_wdq.disasm = { _sink << "packssdw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PACKUSWB/PACKUSDW -- Pack with Unsigned Saturation */
// op packuswb_pq_qq( 0x0f[8]:> <:0x67[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packuswb_pq_qq.disasm = { _sink << "packuswb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op packuswb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x67[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packuswb_vdq_wdq.disasm = { _sink << "packuswb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op packusdw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x2b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packusdw_vdq_wdq.disasm = { _sink << "packusdw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PADDB/PADDW/PADDD/PADDQ -- Add Packed Integers */
// op paddb_pq_qq( 0x0f[8]:> <:0xfc[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddb_pq_qq.disasm = { _sink << "paddb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op paddw_pq_qq( 0x0f[8]:> <:0xfd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddw_pq_qq.disasm = { _sink << "paddw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op paddd_pq_qq( 0x0f[8]:> <:0xfe[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddd_pq_qq.disasm = { _sink << "paddd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op paddq_pq_qq( 0x0f[8]:> <:0xd4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddq_pq_qq.disasm = { _sink << "paddq " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op paddb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfc[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddb_vdq_wdq.disasm = { _sink << "paddb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddw_vdq_wdq.disasm = { _sink << "paddw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfe[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddd_vdq_wdq.disasm = { _sink << "paddd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddq_vdq_wdq.disasm = { _sink << "paddq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PADDSB/PADDSW -- Add Packed Signed Integers with Signed Saturation */
// op paddsb_pq_qq( 0x0f[8]:> <:0xec[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddsb_pq_qq.disasm = { _sink << "paddsb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op paddsw_pq_qq( 0x0f[8]:> <:0xed[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddsw_pq_qq.disasm = { _sink << "paddsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op paddsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xec[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddsb_vdq_wdq.disasm = { _sink << "paddsb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xed[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddsw_vdq_wdq.disasm = { _sink << "paddsw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PADDUSB/PADDUSW -- Add Packed Unsigned Integers with Unsigned Saturation */
// op paddusb_pq_qq( 0x0f[8]:> <:0xdc[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddusb_pq_qq.disasm = { _sink << "paddusb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op paddusw_pq_qq( 0x0f[8]:> <:0xdd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddusw_pq_qq.disasm = { _sink << "paddusw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op paddusb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xdc[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddusb_vdq_wdq.disasm = { _sink << "paddusb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op paddusw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xdd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddusw_vdq_wdq.disasm = { _sink << "paddusw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PALIGNR -- Packed Align Right */
// op palignr_pq_qq_ib( 0x0f[8]:> <:0x3a[8]:> <:0x0f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// palignr_pq_qq_ib.disasm = { _sink << "palignr " << DisasmI(imm) << ',' << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op palignr_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// palignr_vdq_wdq_ib.disasm = { _sink << "palignr " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PAVG -- Average Packed Integers */
// op pavgb_pq_qq( 0x0f[8]:> <:0xe0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pavgb_pq_qq.disasm = { _sink << "pavgb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pavgw_pq_qq( 0x0f[8]:> <:0xe3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pavgw_pq_qq.disasm = { _sink << "pavgw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pavgb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pavgb_vdq_wdq.disasm = { _sink << "pavgb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pavgw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pavgw_vdq_wdq.disasm = { _sink << "pavgw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PBLENDVB -- Variable Blend Packed Bytes */
// op pblenvb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x10[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pblenvb_vdq_wdq.disasm = { _sink << "pblenvb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PBLENDW -- Blend Packed Words */
// op pblendw_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pblendw_vdq_wdq_ib.disasm = { _sink << "pblendw " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PCLMULQDQ - Carry-Less Multiplication Quadword */
// op pclmulqdq_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x44[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pclmulqdq_vdq_wdq_ib.disasm = { _sink << "pclmulqdq " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PCMPESTRM -- Packed Compare Explicit Length Strings, Return Mask */
// op pcmpestrm_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x60[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pcmpestrm_vdq_wdq_ib.disasm = { _sink << "pcmpestrm " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PCMPESTRI -- Packed Compare Explicit Length Strings, Return Index */
// op pcmpestri_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x61[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pcmpestri_vdq_wdq_ib.disasm = { _sink << "pcmpestri " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PCMPISTRM -- Packed Compare Implicit Length Strings, Return Mask */
// op pcmpistrm_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x62[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pcmpistrm_vdq_wdq_ib.disasm = { _sink << "pcmpistrm " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PCMPISTRI -- Packed Compare Implicit Length Strings, Return Index */
// op pcmpistri_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x63[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pcmpistri_vdq_wdq_ib.disasm = { _sink << "pcmpistri " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 

/* PCMPEQB/PCMPEQW/PCMPEQD/PCMEQQ -- Compare Packed Data for Equal */
template <class ARCH, unsigned OPSIZE>
struct PCmpEqVW : public Operation<ARCH>
{
  PCmpEqVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const {
    sink << "pcmpeq" << ("bwdq"[SB<OPSIZE/8>::begin]) << ' ' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    
    for (unsigned sub = 0; sub < (128/OPSIZE); ++sub)
      {
        u_type msk = u_type(arch.vmm_read( SSE(), gn, sub, u_type() ) != arch.vmm_read( SSE(), rmop, sub, u_type() )) - u_type(1);
        arch.vmm_write( SSE(), gn, sub, msk );
      }
  }
};

template <class ARCH> struct DC<ARCH,PCMPEQ> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, simd66() & opcode( "\x0f\x74" ) & RM() ))
    
    return new PCmpEqVW<ARCH,8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simd66() & opcode( "\x0f\x75" ) & RM() ))
    
    return new PCmpEqVW<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simd66() & opcode( "\x0f\x76" ) & RM() ))
    
    return new PCmpEqVW<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, simd66() & opcode( "\x0f\x38\x29" ) & RM() ))
    
    return new PCmpEqVW<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}};
// op pcmpeqb_pq_qq( 0x0f[8]:> <:0x74[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqb_pq_qq.disasm = { _sink << "pcmpeqb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pcmpeqw_pq_qq( 0x0f[8]:> <:0x75[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqw_pq_qq.disasm = { _sink << "pcmpeqw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pcmpeqd_pq_qq( 0x0f[8]:> <:0x76[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqd_pq_qq.disasm = { _sink << "pcmpeqd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pcmpeqb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x74[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqb_vdq_wdq.disasm = { _sink << "pcmpeqb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pcmpeqw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x75[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqw_vdq_wdq.disasm = { _sink << "pcmpeqw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pcmpeqd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x76[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqd_vdq_wdq.disasm = { _sink << "pcmpeqd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pcmpeqq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x29[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqq_vdq_wdq.disasm = { _sink << "pcmpeqq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PCMPGTB/PCMPGTW/PCMPGTD/PCMPGTQ -- Compare Packed Signed Integers for Greater Than */
// op pcmpgtb_pq_qq( 0x0f[8]:> <:0x64[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtb_pq_qq.disasm = { _sink << "pcmpgtb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pcmpgtw_pq_qq( 0x0f[8]:> <:0x65[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtw_pq_qq.disasm = { _sink << "pcmpgtw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pcmpgtd_pq_qq( 0x0f[8]:> <:0x66[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtd_pq_qq.disasm = { _sink << "pcmpgtd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pcmpgtb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x64[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtb_vdq_wdq.disasm = { _sink << "pcmpgtb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pcmpgtw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x65[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtw_vdq_wdq.disasm = { _sink << "pcmpgtw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pcmpgtd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x66[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtd_vdq_wdq.disasm = { _sink << "pcmpgtd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pcmpgtq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x37[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtq_vdq_wdq.disasm = { _sink << "pcmpgtq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PEXTRB/PEXTRW/PEXTRD/PEXTRQ -- Extract Byte/Dword/Qword */
// op pextrw_rd_pq_ib( 0x0f[8]:> <:0xc5[8]:> <:0b11[2]:gn[3]:rm[3]:> <:imm[8] );
// 
// pextrw_rd_pq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmPq( rm ) << ',' << DisasmGd( gn ); };
// 
// op pextrw_rd_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0xc5[8]:> <:0b11[2]:gn[3]:rm[3]:> <:imm[8] );
// 
// pextrw_rd_vdq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmV( SSE(), rm ) << ',' << DisasmGd( gn ); };
// 
// op pextrb_eb_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x14[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pextrb_eb_vdq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEb( rmop ); };
// 
// op pextrw_ew_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x15[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pextrw_ew_vdq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEw( rmop ); };
// 
// op pextrd_ed_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x16[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pextrd_ed_vdq_ib.disasm = { _sink << "pextrd " << DisasmI( imm ) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEd( rmop ); };
// 
// // op pextrq_eb_vdq_ib( 0x66[8]:> <:0x48[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x16[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// // pextrq_eb_vdq_ib.disasm = { _sink << "pextrq " << DisasmI( imm ) << ',' << DisasmV( SSE(), gn ) << ',' << DisasmEq( rmop ); };
// 
// /* PHADDW/PHADDD -- Packed Horizontal Add */
// op phaddw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x01[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddw_pq_qq.disasm = { _sink << "phaddw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op phaddd_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x02[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddd_pq_qq.disasm = { _sink << "phaddd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op phaddw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x01[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddw_vdq_wdq.disasm = { _sink << "phaddw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op phaddd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x02[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddd_vdq_wdq.disasm = { _sink << "phaddd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PHADDSW -- Packed Horizontal Add and Saturate */
// op phaddsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x03[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddsw_pq_qq.disasm = { _sink << "phaddsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op phaddsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x03[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddsw_vdq_wdq.disasm = { _sink << "phaddsw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PHMINPOSUW -- Packed Horizontal Word Minimum */
// op phminposuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x41[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phminposuw_vdq_wdq.disasm = { _sink << "phminposuw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// 
// /* PHSUBW/PHSUBD -- Packed Horizontal Subtract */
// op phsubw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x05[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubw_pq_qq.disasm = { _sink << "phsubw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op phsubd_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x06[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubd_pq_qq.disasm = { _sink << "phsubd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op phsubw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x05[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubw_vdq_wdq.disasm = { _sink << "phsubw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op phsubd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x06[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubd_vdq_wdq.disasm = { _sink << "phsubd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PHSUBSW -- Packed Horizontal Subtract and Saturate */
// op phsubsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x07[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubsw_pq_qq.disasm = { _sink << "phsubsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op phsubsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x07[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubsw_vdq_wdq.disasm = { _sink << "phsubsw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PINSRB/PINSRW/PINSRD/PINSRQ -- Insert Byte/Word/Dword/Qword */
// op pinsrw_pq_ed_ib( 0x0f[8]:> <:0xc4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pinsrw_pq_ed_ib.disasm = { _sink << "pinsrw " << DisasmI( imm ) << ',' << DisasmEd( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pinsrb_vdq_ed_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x20[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pinsrb_vdq_ed_ib.disasm = { _sink << "pinsrb " << DisasmI( imm ) << ',' << DisasmEd( rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pinsrw_vdq_ed_ib( 0x66[8]:> <:0x0f[8]:> <:0xc4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pinsrw_vdq_ed_ib.disasm = { _sink << "pinsrw " << DisasmI( imm ) << ',' << DisasmEd( rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pinsrd_vdq_ed_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x22[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pinsrd_vdq_ed_ib.disasm = { _sink << "pinsrd " << DisasmI( imm ) << ',' << DisasmEd( rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMADDUBSW -- PINSRB/PINSRD/PINSRQ -- Insert Byte/Dword/Qword */
// op pmaddubsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x04[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddubsw_pq_qq.disasm = { _sink << "pmaddubsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmaddubsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x04[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddubsw_vdq_wdq.disasm = { _sink << "pmaddubsw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMADDWD -- Multiply and Add Packed Integers */
// op pmaddwd_pq_qq( 0x0f[8]:> <:0xf5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddwd_pq_qq.disasm = { _sink << "pmaddwd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmaddwd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddwd_vdq_wdq.disasm = { _sink << "pmaddwd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMAXS -- Maximum of Packed Signed Byte/Word/DWord/QWord Integers */
// op pmaxsw_pq_qq( 0x0f[8]:> <:0xee[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxsw_pq_qq.disasm = { _sink << "pmaxsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmaxsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxsb_vdq_wdq.disasm = { _sink << "pmaxsb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmaxsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xee[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxsw_vdq_wdq.disasm = { _sink << "pmaxsw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmaxsd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxsd_vdq_wdq.disasm = { _sink << "pmaxsd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMAXU -- Maximum of Packed Unsigned Byte/Word/DWord Integers */
// op pmaxub_pq_qq( 0x0f[8]:> <:0xde[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxub_pq_qq.disasm = { _sink << "pmaxub " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmaxub_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xde[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxub_vdq_wdq.disasm = { _sink << "pmaxub " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmaxuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxuw_vdq_wdq.disasm = { _sink << "pmaxuw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmaxud_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxud_vdq_wdq.disasm = { _sink << "pmaxud " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMINS -- Minimum of Packed Signed Byte/Word/DWord Integers */
// op pminsw_pq_qq( 0x0f[8]:> <:0xea[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminsw_pq_qq.disasm = { _sink << "pminsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pminsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x38[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminsb_vdq_wdq.disasm = { _sink << "pminsb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pminsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xea[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminsw_vdq_wdq.disasm = { _sink << "pminsw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pminsd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x39[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminsd_vdq_wdq.disasm = { _sink << "pminsd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMINU -- Minimum of Packed Unsigned Byte/Word/DWord Integers */
// op pminub_pq_qq( 0x0f[8]:> <:0xda[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminub_pq_qq.disasm = { _sink << "pminub " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pminub_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xda[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminub_vdq_wdq.disasm = { _sink << "pminub " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pminuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminuw_vdq_wdq.disasm = { _sink << "pminuw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pminud_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminud_vdq_wdq.disasm = { _sink << "pminud " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 

// /* PMOVMSKB -- Move Byte Mask */
template <class ARCH>
struct PMovMskBRP : public Operation<ARCH>
{
  PMovMskBRP( OpBase<ARCH> const& opbase, uint8_t _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} uint8_t rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pmovmskb " << DisasmPq( rm ) << ',' << DisasmG( GOd(), gn ); }
};
 
template <class ARCH>
struct PMovMskBRV : public Operation<ARCH>
{
  PMovMskBRV( OpBase<ARCH> const& opbase, uint8_t _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} uint8_t rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pmovmskb " << DisasmV( SSE(), rm ) << ',' << DisasmG( GOd(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;
    typedef typename ARCH::u8_t  u8_t;
    
    u32_t res = u32_t(0);
    
    for (unsigned sub = 0; sub < 16; ++sub)
      res |= u32_t(arch.vmm_read( SSE(), rm, sub, u8_t() ) >> 7) << sub;
    
    arch.regwrite( GOd(), gn, res );
  }
};

template <class ARCH> struct DC<ARCH,PMOVMSKB> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, simd__() & opcode( "\x0f\xd7" ) & RM_reg() ))
    
    return new PMovMskBRP<ARCH>( _.opbase(), _.ereg(), _.greg() );
  
  if (auto _ = match( ic, simd66() & opcode( "\x0f\xd7" ) & RM_reg() ))

    return new PMovMskBRV<ARCH>( _.opbase(), _.ereg(), _.greg() );
  
  return 0;
}};

// 
// /* PMOVSX -- Packed Move with Sign Extend */
// op pmovsxbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x20[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxbw_vdq_wdq.disasm = { _sink << "pmovsxbw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxbd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x21[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxbd_vdq_wdq.disasm = { _sink << "pmovsxbd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxbq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x22[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxbq_vdq_wdq.disasm = { _sink << "pmovsxbq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxwd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x23[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxwd_vdq_wdq.disasm = { _sink << "pmovsxwd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxwq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x24[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxwq_vdq_wdq.disasm = { _sink << "pmovsxwq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovsxdq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x25[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxdq_vdq_wdq.disasm = { _sink << "pmovsxdq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMOVZX -- Packed Move with Sign Extend */
// op pmovzxbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x30[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxbw_vdq_wdq.disasm = { _sink << "pmovzxbw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxbd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x31[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxbd_vdq_wdq.disasm = { _sink << "pmovzxbd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxbq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x32[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxbq_vdq_wdq.disasm = { _sink << "pmovzxbq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxwd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x33[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxwd_vdq_wdq.disasm = { _sink << "pmovzxwd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxwq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x34[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxwq_vdq_wdq.disasm = { _sink << "pmovzxwq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pmovzxdq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x35[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxdq_vdq_wdq.disasm = { _sink << "pmovzxdq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULDQ -- Multiply Packed Signed Dword Integers */
// op pmuldq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x28[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmuldq_vdq_wdq.disasm = { _sink << "pmuldq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULHRSW -- Packed Multiply High with Round and Scale */
// op pmulhrsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x0b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhrsw_pq_qq.disasm = { _sink << "pmulhrsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmulhrsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x0b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhrsw_vdq_wdq.disasm = { _sink << "pmulhrsw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULHUW -- Multiply Packed Unsigned Integers and Store High Result */
// op pmulhuw_pq_qq( 0x0f[8]:> <:0xe4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhuw_pq_qq.disasm = { _sink << "pmulhuw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmulhuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhuw_vdq_wdq.disasm = { _sink << "pmulhuw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULHW -- Multiply Packed Signed Integers and Store High Result */
// op pmulhw_pq_qq( 0x0f[8]:> <:0xe5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhw_pq_qq.disasm = { _sink << "pmulhw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmulhw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhw_vdq_wdq.disasm = { _sink << "pmulhw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULLD -- Multiply Packed Signed Dword Integers and Store Low Result */
// op pmulld_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x40[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulld_vdq_wdq.disasm = { _sink << "pmulld " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULLW -- Multiply Packed Signed Integers and Store Low Result */
// op pmullw_pq_qq( 0x0f[8]:> <:0xd5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmullw_pq_qq.disasm = { _sink << "pmullw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmullw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmullw_vdq_wdq.disasm = { _sink << "pmullw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PMULUDQ -- Multiply Packed Unsigned Doubleword Integers */
// op pmuludq_pq_qq( 0x0f[8]:> <:0xf4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmuludq_pq_qq.disasm = { _sink << "pmuludq " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmuludq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmuludq_vdq_wdq.disasm = { _sink << "pmuludq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSADBW -- Compute Sum of Absolute Differences */
// op psadbw_pq_qq( 0x0f[8]:> <:0xf6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psadbw_pq_qq.disasm = { _sink << "psadbw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psadbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psadbw_vdq_wdq.disasm = { _sink << "psadbw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSHUFB -- Packed Shuffle Bytes */
// op pshufb_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x00[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pshufb_pq_qq.disasm = { _sink << "pshufb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pshufb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x00[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pshufb_vdq_wdq.disasm = { _sink << "pshufb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSHUFD -- Shuffle Packed Doublewords */
// op pshufd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pshufd_vdq_wdq_ib.disasm = { _sink << "pshufd " << DisasmI(imm) << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
//

template <class ARCH>
struct Pshufd : public Operation<ARCH>
{
  Pshufd( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, uint8_t _oo ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn), oo(_oo) {} RMOp<ARCH> rm; uint8_t gn; uint8_t oo;
  void disasm( std::ostream& sink ) const { sink << "pshufd " << DisasmI(oo) << ',' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;
    
    for (unsigned idx = 0; idx < 4; ++idx)
      {
        unsigned part = (oo >> 2*idx) % 4;
        arch.vmm_write( SSE(), gn, idx, arch.vmm_read( SSE(), rm, part, u32_t() ) );
      }
  }
};

template <class ARCH> struct DC<ARCH,PSHUFD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, simd66() & opcode( "\x0f\x70" ) & RM() & Imm<8>() ))
    
      return new Pshufd<ARCH>( _.opbase(), _.rmop(), _.greg(), _.i(int8_t()) );

  return 0;
}};



// /* PSHUFHW -- Shuffle Packed High Words */
// op pshufhw_vdq_wdq_ib( 0xf3[8]:> <:0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pshufhw_vdq_wdq_ib.disasm = { _sink << "pshufhw " << DisasmI(imm) << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSHUFLW -- Shuffle Packed Low Words */
// op pshuflw_vdq_wdq_ib( 0xf2[8]:> <:0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pshuflw_vdq_wdq_ib.disasm = { _sink << "pshuflw " << DisasmI(imm) << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSHUFW -- Shuffle Packed Words */
// op pshufw_pq_qq( 0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pshufw_pq_qq.disasm = { _sink << "pshufw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// /* PSIGNB/PSIGNW/PSIGND -- Packed SIGN */
// op psignb_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x08[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignb_pq_qq.disasm = { _sink << "psignb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psignw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x09[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignw_pq_qq.disasm = { _sink << "psignw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psignd_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x0a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignd_pq_qq.disasm = { _sink << "psignd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psignb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x08[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignb_vdq_wdq.disasm = { _sink << "psignb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psignw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x09[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignw_vdq_wdq.disasm = { _sink << "psignw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psignd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x0a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignd_vdq_wdq.disasm = { _sink << "psignd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSLLDQ -- Shift Double Quadword Left Logical */
// op pslldq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:7[3]:rm[3]:> <:imm[8] );
// 
// pslldq_vdq_ib.disasm = { _sink << "pslldq " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// /* PSLLW/PSLLD/PSLLQ -- Shift Packed Data Left Logical */
// op psllw_pq_qq( 0x0f[8]:> <:0xf1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psllw_pq_qq.disasm = { _sink << "psllw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psllw_pq_ib( 0x0f[8]:> <:0x71[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllw_pq_ib.disasm = { _sink << "psllw " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op pslld_pq_qq( 0x0f[8]:> <:0xf2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pslld_pq_qq.disasm = { _sink << "pslld " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pslld_pq_ib( 0x0f[8]:> <:0x72[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// pslld_pq_ib.disasm = { _sink << "pslld " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psllq_pq_qq( 0x0f[8]:> <:0xf3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psllq_pq_qq.disasm = { _sink << "psllq " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psllq_pq_ib( 0x0f[8]:> <:0x73[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllq_pq_ib.disasm = { _sink << "psllq " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psllw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psllw_vdq_wdq.disasm = { _sink << "psllw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psllw_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x71[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllw_vdq_ib.disasm = { _sink << "psllw " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op pslld_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pslld_vdq_wdq.disasm = { _sink << "pslld " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op pslld_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x72[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// pslld_vdq_ib.disasm = { _sink << "pslld " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op psllq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psllq_vdq_wdq.disasm = { _sink << "psllq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psllq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllq_vdq_ib.disasm = { _sink << "psllq " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// /* PSRAW/PSRAD -- Shift Packed Data Right Arithmetic */
// op psraw_pq_qq( 0x0f[8]:> <:0xe1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psraw_pq_qq.disasm = { _sink << "psraw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psraw_pq_ib( 0x0f[8]:> <:0x71[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psraw_pq_ib.disasm = { _sink << "psraw " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psrad_pq_qq( 0x0f[8]:> <:0xe2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrad_pq_qq.disasm = { _sink << "psrad " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psrad_pq_ib( 0x0f[8]:> <:0x72[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psrad_pq_ib.disasm = { _sink << "psrad " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psraw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psraw_vdq_wdq.disasm = { _sink << "psraw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psraw_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x71[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psraw_vdq_ib.disasm = { _sink << "psraw " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op psrad_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrad_vdq_wdq.disasm = { _sink << "psrad " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psrad_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x72[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psrad_vdq_ib.disasm = { _sink << "psrad " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// /* PSRLDQ -- Shift Double Quadword Right Logical */
// op psrldq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:3[3]:rm[3]:> <:imm[8] );
// 
// psrldq_vdq_ib.disasm = { _sink << "psrldq " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// /* PSRLW/PSRLD/PSRLQ -- Shift Packed Data Right Logical */
// op psrlw_pq_qq( 0x0f[8]:> <:0xd1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrlw_pq_qq.disasm = { _sink << "psrlw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psrlw_pq_ib( 0x0f[8]:> <:0x71[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlw_pq_ib.disasm = { _sink << "psrlw " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psrld_pq_qq( 0x0f[8]:> <:0xd2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrld_pq_qq.disasm = { _sink << "psrld " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psrld_pq_ib( 0x0f[8]:> <:0x72[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrld_pq_ib.disasm = { _sink << "psrld " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psrlq_pq_qq( 0x0f[8]:> <:0xd3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrlq_pq_qq.disasm = { _sink << "psrlq " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psrlq_pq_ib( 0x0f[8]:> <:0x73[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlq_pq_ib.disasm = { _sink << "psrlq " << DisasmI(imm) << ',' << DisasmPq( rm ); };
// 
// op psrlw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd1[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrlw_vdq_wdq.disasm = { _sink << "psrlw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psrlw_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x71[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlw_vdq_ib.disasm = { _sink << "psrlw " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op psrld_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrld_vdq_wdq.disasm = { _sink << "psrld " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psrld_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x72[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrld_vdq_ib.disasm = { _sink << "psrld " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// op psrlq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrlq_vdq_wdq.disasm = { _sink << "psrlq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psrlq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlq_vdq_ib.disasm = { _sink << "psrlq " << DisasmI(imm) << ',' << DisasmV( SSE(), rm ); };
// 
// /* PSUBB/PSUBW/PSUBD/PSUBQ -- Subtract Packed Integers */
// op psubb_pq_qq( 0x0f[8]:> <:0xf8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubb_pq_qq.disasm = { _sink << "psubb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psubw_pq_qq( 0x0f[8]:> <:0xf9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubw_pq_qq.disasm = { _sink << "psubw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psubd_pq_qq( 0x0f[8]:> <:0xfa[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubd_pq_qq.disasm = { _sink << "psubd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psubq_pq_qq( 0x0f[8]:> <:0xfb[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubq_pq_qq.disasm = { _sink << "psubq " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psubb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubb_vdq_wdq.disasm = { _sink << "psubb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubw_vdq_wdq.disasm = { _sink << "psubw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfa[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubd_vdq_wdq.disasm = { _sink << "psubd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfb[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubq_vdq_wdq.disasm = { _sink << "psubq " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSUBSB/PSUBSW -- Subtract Packed Signed Integers with Signed Saturation */
// op psubsb_pq_qq( 0x0f[8]:> <:0xe8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubsb_pq_qq.disasm = { _sink << "psubsb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psubsw_pq_qq( 0x0f[8]:> <:0xe9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubsw_pq_qq.disasm = { _sink << "psubsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psubsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubsb_vdq_wdq.disasm = { _sink << "psubsb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubsw_vdq_wdq.disasm = { _sink << "psubsw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PSUBUSB/PSUBUSW -- Subtract Packed Unsigned Integers with Unsigned Saturation */
// op psubusb_pq_qq( 0x0f[8]:> <:0xd8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubusb_pq_qq.disasm = { _sink << "psubusb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psubusw_pq_qq( 0x0f[8]:> <:0xd9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubusw_pq_qq.disasm = { _sink << "psubusw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psubusb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd8[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubusb_vdq_wdq.disasm = { _sink << "psubusb " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// op psubusw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubusw_vdq_wdq.disasm = { _sink << "psubusw " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* PTEST -- Logical Compare */
// op ptest_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x17[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// ptest_vdq_wdq.disasm = { _sink << "ptest " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 

// op punpckhbw_pq_qq( 0x0f[8]:> <:0x68[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpckhbw_pq_qq.disasm = { _sink << "punpckhbw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// op punpckhwd_pq_qq( 0x0f[8]:> <:0x69[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpckhwd_pq_qq.disasm = { _sink << "punpckhwd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// op punpckhdq_pq_qq( 0x0f[8]:> <:0x6a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpckhdq_pq_qq.disasm = { _sink << "punpckhdq " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// op punpcklbw_pq_qq( 0x0f[8]:> <:0x60[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpcklbw_pq_qq.disasm = { _sink << "punpcklbw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// op punpcklwd_pq_qq( 0x0f[8]:> <:0x61[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpcklwd_pq_qq.disasm = { _sink << "punpcklwd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// op punpckldq_pq_qq( 0x0f[8]:> <:0x62[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// punpckldq_pq_qq.disasm = { _sink << "punpckldq " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };

template <class ARCH, unsigned OPSIZE, bool HI>
struct PunpckV : public Operation<ARCH>
{
  PunpckV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "punpck" << (HI?'h':'l') << SizeID<OPSIZE>::iid() << SizeID<2*OPSIZE>::iid() << ' ' << DisasmW(SSE(),rm) << ',' << DisasmV(SSE(),gn); }
  void execute( ARCH& arch ) const
  {
    unsigned const COUNT = 64/OPSIZE;
    unsigned const RMSK = HI ? 0 : COUNT - 1;
    unsigned const HALF = HI ? COUNT : 0;

    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    
    u_type res[2*COUNT];
    
    for (unsigned chunk = 0; chunk < COUNT; ++chunk)
      {
        unsigned idx = chunk & RMSK;
        res[2*idx+0] = arch.vmm_read( SSE(), gn, HALF+idx, u_type() );
        res[2*idx+1] = arch.vmm_read( SSE(), rm, HALF+idx, u_type() );
      }
    for (unsigned idx = 0; idx < 2*COUNT; ++idx)
      arch.vmm_write( SSE(), gn, idx, res[idx] );
  }
};

/* PUNPCKHBW/PUNPCKHWD/PUNPCKHDQ/PUNPCKHQDQ/PUNPCKLBW/PUNPCKLWD/PUNPCKLDQ/PUNPCKLQDQ -- Unpack Data */
template <class ARCH> struct DC<ARCH,PUNPCK> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, simd66() & opcode( "\x0f\x68" ) & RM() ))

    return new PunpckV<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd66() & opcode( "\x0f\x69" ) & RM() ))

    return new PunpckV<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd66() & opcode( "\x0f\x6a" ) & RM() ))

    return new PunpckV<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd66() & opcode( "\x0f\x6d" ) & RM() ))

    return new PunpckV<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd66() & opcode( "\x0f\x60" ) & RM() ))

    return new PunpckV<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd66() & opcode( "\x0f\x61" ) & RM() ))

    return new PunpckV<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd66() & opcode( "\x0f\x62" ) & RM() ))

    return new PunpckV<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd66() & opcode( "\x0f\x6c" ) & RM() ))

    return new PunpckV<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );

  return 0;
}};

struct VAND {}; struct VANDN {}; struct VOR {}; struct VXOR {};

/* TODO: this operation will always blow the vector typing, but is this avoidable ? */
template <class ARCH, class VR, class MANIP, unsigned OPSIZE>
struct PBitManipVVW : public Operation<ARCH>
{
  typedef typename TypeFor<ARCH,OPSIZE>::u valtype;
  valtype eval (  VXOR const&, valtype const& src1, valtype const& src2 ) const { return src1 ^ src2; }
  valtype eval (  VAND const&, valtype const& src1, valtype const& src2 ) const { return src1 & src2; }
  valtype eval ( VANDN const&, valtype const& src1, valtype const& src2 ) const { return src1 &~ src2; }
  valtype eval (   VOR const&, valtype const& src1, valtype const& src2 ) const { return src1 | src2; }
  
  PBitManipVVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, unsigned _vn, unsigned _gn, char const* _mn ) : Operation<ARCH>(opbase), rm(_rm), mn(_mn), vn(_vn), gn(_gn) {}
  RMOp<ARCH> rm; char const* mn; uint8_t vn, gn; 
  void disasm( std::ostream& sink ) const
  {
    sink << mn << ' ' << DisasmW( VR(), rm );
    if (VR::vex()) sink << ',' << DisasmV( VR(), vn );
    sink << ',' << DisasmV( VR(), gn );
  }
  void execute( ARCH& arch ) const
  {
    for (unsigned idx = 0, end = VR::size()/OPSIZE; idx < end; ++idx)
      arch.vmm_write( VR(), gn, idx, eval( MANIP(), arch.vmm_read( VR(), vn, idx, valtype() ), arch.vmm_read( VR(), rm, idx, valtype() ) ) );
  }
};

template <class ARCH>
struct PAndnPQ : public Operation<ARCH>
{
  PAndnPQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pandn " << DisasmQq( rm ) << ',' << DisasmPq( gn ); }
};

template <class ARCH>
struct PAndPQ : public Operation<ARCH>
{
  PAndPQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pand " << DisasmQq( rm ) << ',' << DisasmPq( gn ); }
};

template <class ARCH>
struct POrPQ : public Operation<ARCH>
{
  POrPQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "por " << DisasmQq( rm ) << ',' << DisasmPq( gn ); }
};

template <class ARCH>
struct PXorPQ : public Operation<ARCH>
{
  PXorPQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pxor " << DisasmQq( rm ) << ',' << DisasmPq( gn ); }
};

/**** Packed Bit Manipulations ****/
/* PAND -- Bitwise Logical AND */
/* ANDPD -- Bitwise Logical AND of Packed Double-Precision Floating-Point Values */
/* ANDPS -- Bitwise Logical AND of Packed Single-Precision Floating-Point Values */
/* PANDN -- Bitwise Logical AND NOT */
/* ANDNPD -- Bitwise Logical AND NOT of Packed Double-Precision Floating-Point Values */
/* ANDNPS -- Bitwise Logical AND NOT of Packed Single-Precision Floating-Point Values */
/* POR -- Bitwise Logical OR */
/* ORPD -- Bitwise Logical OR of Double-Precision Floating-Point Values */
/* ORPS -- Bitwise Logical OR of Single-Precision Floating-Point Values */
/* PXOR -- Logical Exclusive OR */
/* XORPD -- Bitwise Logical XOR for Double-Precision Floating-Point Values */
/* XORPS -- Bitwise Logical XOR for Single-Precision Floating-Point Values */
template <class ARCH> struct DC<ARCH,PBM> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  /* MMX */
  
  if (auto _ = match( ic, simd__() & opcode( "\x0f\xeb" ) & RM() ))

    return new POrPQ<ARCH>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd__() & opcode( "\x0f\xdb" ) & RM() ))

    return new PAndPQ<ARCH>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd__() & opcode( "\x0f\xdb" ) & RM() ))

    return new PAndnPQ<ARCH>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, simd__() & opcode( "\x0f\xef" ) & RM() ))

    return new PXorPQ<ARCH>( _.opbase(), _.rmop(), _.greg() );

  /* SSE/AVX Integer */
  
  if (auto _ = match( ic, vex( "\x66\x0f\xdb" ) & RM() ))
    
    return newPBitManipVVW<VAND,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vpand" : "pand" );

  if (auto _ = match( ic, vex( "\x66\x0f\xdf" ) & RM() ))

    return newPBitManipVVW<VANDN,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vpandn" : "pandn" );

  if (auto _ = match( ic, vex( "\x66\x0f\xeb" ) & RM() ))

    return newPBitManipVVW<VOR,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vpor" : "por" );

  if (auto _ = match( ic, vex( "\x66\x0f\xef" ) & RM() ))
    
    return newPBitManipVVW<VXOR,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vpxor" : "pxor" );

  /* SSE/AVX Floating Point */
  
  if (auto _ = match( ic, vex( "\x0f\x54" ) & RM() ))

    return newPBitManipVVW<VAND,32>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vandps" : "andps" );

  if (auto _ = match( ic, vex( "\x66\x0f\x54" ) & RM() ))

    return newPBitManipVVW<VAND,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vandpd" : "andpd" );

  if (auto _ = match( ic, vex( "\x0f\x55" ) & RM() ))

    return newPBitManipVVW<VANDN,32>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vandnps" : "andnps" );
  
  if (auto _ = match( ic, vex( "\x66\x0f\x55" ) & RM() ))

    return newPBitManipVVW<VANDN,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vandnpd" : "andnpd" );
  
  if (auto _ = match( ic, vex( "\x0f\x56" ) & RM() ))

    return newPBitManipVVW<VOR,32>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vorps" : "orps" );

  if (auto _ = match( ic, vex( "\x66\x0f\x56" ) & RM() ))

    return newPBitManipVVW<VOR,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vorpd" : "orpd" );

  if (auto _ = match( ic, vex( "\x0f\x57" ) & RM() ))

    return newPBitManipVVW<VXOR,32>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vxorps" : "xorps" );

  if (auto _ = match( ic, vex( "\x66\x0f\x57" ) & RM() ))

    return newPBitManipVVW<VXOR,64>( ic, _.opbase(), _.rmop(), _.greg(), ic.vex() ? "vxorpd" : "xorpd" );

  return 0;
}
template <class MANIP, unsigned OPSIZE>
Operation<ARCH>* newPBitManipVVW( InputCode<ARCH> const& ic, OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, char const* _mn )
{
  if (not ic.vex())     return new PBitManipVVW<ARCH,SSE,MANIP,OPSIZE>( opbase, _rm, _gn, _gn, _mn );
  unsigned vn = ic.vreg();
  if (ic.vlen() == 128) return new PBitManipVVW<ARCH,XMM,MANIP,OPSIZE>( opbase, _rm,  vn, _gn, _mn );
  if (ic.vlen() == 256) return new PBitManipVVW<ARCH,YMM,MANIP,OPSIZE>( opbase, _rm,  vn, _gn, _mn );
  return 0;
}
};

template <class ARCH, unsigned OPSZ>
struct Ucomis : public Operation<ARCH>
{
  Ucomis( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, bool _v ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn) {} RMOp<ARCH> rm; uint8_t gn; bool v;
  void disasm( std::ostream& sink ) const { sink << (v?"v":"") << "ucomis" << SizeID<OPSZ>::fid() << ' ' << DisasmW( SSE(), rm ) << ',' << DisasmV( SSE(), gn ); }
  
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSZ>::f f_type;
    typedef typename ARCH::bit_t bit_t;
    
    f_type a = arch.vmm_read( SSE(), gn, 0, f_type() );
    f_type b = arch.vmm_read( SSE(), rm, 0, f_type() );
    bit_t notle = not (a <= b);
    bit_t notge = not (a >= b);

    arch.flagwrite( ARCH::FLAG::ZF, notge == notle );
    arch.flagwrite( ARCH::FLAG::PF, notge and notle );
    arch.flagwrite( ARCH::FLAG::CF, notge );
    arch.flagwrite( ARCH::FLAG::OF, bit_t(false) );
    arch.flagwrite( ARCH::FLAG::SF, bit_t(false) );
    arch.flagwrite( ARCH::FLAG::AF, bit_t(false) );
  }
};

/* UCOMISD -- Unordered Compare Scalar Double-Precision Floating-Point Values and Set EFLAGS */
/* UCOMISS -- Unordered Compare Scalar Single-Precision Floating-Point Values and Set EFLAGS */
template <class ARCH> struct DC<ARCH,UCOMIS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, vex( "*\x0f\x2e" ) & RM() ))
    {
      if (ic.vex() and ic.vreg()) return 0;
      if (match( ic, simd__() )) return new Ucomis<ARCH,32>( _.opbase(), _.rmop(), _.greg(), ic.vex() );
      if (match( ic, simd66() )) return new Ucomis<ARCH,64>( _.opbase(), _.rmop(), _.greg(), ic.vex() );
    }

  return 0;
}};

// /* RCPPS -- Compute Reciprocals of Packed Single-Precision Floating-Point Values */
// op rcpps_vdq_wdq( 0x0f[8]:> <:0x53[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rcpps_vdq_wdq.disasm = { _sink << "rcpps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* RCPSS—Compute Reciprocal of Scalar Single-Precision Floating-Point Values */
// op rcpss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x53[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rcpss_vdq_wdq.disasm = { _sink << "rcpss " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* ROUNDPS -- Round Packed Single-Precision Floating-Point Values */
// op roundps_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x08[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundps_vdq_wdq_ib.disasm = { _sink << "roundps " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* ROUNDPD -- Round Packed Double-Precision Floating-Point Values */
// op roundpd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x09[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundpd_vdq_wdq_ib.disasm = { _sink << "roundpd " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* ROUNDSS -- Round Scalar Single-Precision Floating-Point Values */
// op roundss_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundss_vdq_wdq_ib.disasm = { _sink << "roundss " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* ROUNDSD -- Round Scalar Double-Precision Floating-Point Values */
// op roundsd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundsd_vdq_wdq_ib.disasm = { _sink << "roundsd " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* RSQRTPS -- Compute Reciprocals of Square Roots of Packed Single-Precision Floating-Point Values */
// op rsqrtps_vdq_wdq( 0x0f[8]:> <:0x52[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rsqrtps_vdq_wdq.disasm = { _sink << "rsqrtps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* RSQRTSS -- Compute Reciprocal of Square Root of Scalar Single-Precision Floating-Point Value */
// op rsqrtss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x52[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rsqrtss_vdq_wdq.disasm = { _sink << "rsqrtss " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SHUFPS -- Shuffle Packed Single-Precision Floating-Point Values */
// op shufps_vdq_wdq_ib( 0x0f[8]:> <:0xc6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// shufps_vdq_wdq_ib.disasm = { _sink << "shufps " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SHUFPD -- Shuffle Packed Double-Precision Floating-Point Values */
// op shufpd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0xc6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// shufpd_vdq_wdq_ib.disasm = { _sink << "shufpd " << DisasmI(imm) << ',' << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SQRTPS -- Compute Square Roots of Packed Single-Precision Floating-Point Values */
// op sqrtps_vdq_wdq( 0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtps_vdq_wdq.disasm = { _sink << "sqrtps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SQRTPD -- Compute Square Roots of Packed Double-Precision Floating-Point Values */
// op sqrtpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtpd_vdq_wdq.disasm = { _sink << "sqrtpd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SQRTSS -- Compute Square Roots of Packed Single-Precision Floating-Point Values */
// op sqrtss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtss_vdq_wdq.disasm = { _sink << "sqrtss " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SQRTSD -- Compute Square Roots of Packed Double-Precision Floating-Point Values */
// op sqrtsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtsd_vdq_wdq.disasm = { _sink << "sqrtsd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SUBPS -- Subtract Packed Single-Precision Floating-Point Values */
// op subps_vdq_wdq( 0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subps_vdq_wdq.disasm = { _sink << "subps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SUBPD -- Subtract Packed Double-Precision Floating-Point Values */
// op subpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subpd_vdq_wdq.disasm = { _sink << "subpd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SUBSS -- Subtract Scalar Single-Precision Floating-Point Values */
// op subss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subss_vdq_wdq.disasm = { _sink << "subss " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* SUBSD -- Subtract Scalar Double-Precision Floating-Point Values */
// op subsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subsd_vdq_wdq.disasm = { _sink << "subsd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// 
// /* UNPCKHPS -- Unpack and Interleave High Packed Single-Precision Floating-Point Values */
// op unpckhps_vdq_wdq( 0x0f[8]:> <:0x15[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// unpckhps_vdq_wdq.disasm = { _sink << "unpckhps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* UNPCKHPD -- Unpack and Interleave High Packed Double-Precision Floating-Point Values */
// op unpckhpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x15[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// unpckhpd_vdq_wdq.disasm = { _sink << "unpckhpd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* UNPCKLPS -- Unpack and Interleave Low Packed Single-Precision Floating-Point Values */
// op unpcklps_vdq_wdq( 0x0f[8]:> <:0x14[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// unpcklps_vdq_wdq.disasm = { _sink << "unpcklps " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* UNPCKLPD -- Unpack and Interleave Low Packed Double-Precision Floating-Point Values */
// op unpcklpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x14[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// unpcklpd_vdq_wdq.disasm = { _sink << "unpcklpd " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };
// 
// /* MOVDDUP -- Move One Double-FP and Duplicate */
// op movddup_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// movddup_vdq_wdq.disasm = { _sink << "movddup " << DisasmW( SSE(), rmop ) << ',' << DisasmV( SSE(), gn ); };

