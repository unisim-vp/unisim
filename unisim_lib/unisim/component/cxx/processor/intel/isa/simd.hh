// /* ADDP -- Add Packed Single- or Double-Precision Floating-Point Values */
// op addps_vdq_wdq( 0x0f[8]:> <:0x58[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addps_vdq_wdq.disasm = { _sink << "addps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op addpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x58[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addpd_vdq_wdq.disasm = { _sink << "addpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* ADDSD -- Add Scalar Single- or Double-Precision Floating-Point Values */
// op addss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x58[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addss_vdq_wdq.disasm = { _sink << "addss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op addsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x58[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addsd_vdq_wdq.disasm = { _sink << "addsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* ADDSUBPD -- Packed Single- or Double-FP Add/Subtract */
// op addsubpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addsubpd_vdq_wdq.disasm = { _sink << "addsubpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
//  
// op addsubps_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0xd0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// addsubps_vdq_wdq.disasm = { _sink << "addsubps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* ANDPD -- Bitwise Logical AND of Packed Single- or Double-Precision Floating-Point Values */
// op andps_vdq_wdq( 0x0f[8]:> <:0x54[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// andps_vdq_wdq.disasm = { _sink << "andps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op andpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x54[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// andpd_vdq_wdq.disasm = { _sink << "andpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* ANDNP -- Bitwise Logical AND NOT of Packed Single- or Double-Precision Floating-Point Values */
// op andnps_vdq_wdq( 0x0f[8]:> <:0x55[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// andnps_vdq_wdq.disasm = { _sink << "andnps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op andnpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x55[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// andnpd_vdq_wdq.disasm = { _sink << "andnpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* BLENDP -- Blend Packed Single- or Double-Precision Floating-Point Values */
// op blendps_vdq_wdq_ib( 0x0f[8]:> <:0x3a[8]:> <:0x0c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// blendps_vdq_wdq_ib.disasm = { _sink << "blendps " << DisasmI( imm ) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op blendpd_vdq_wdq_ib( 0x0f[8]:> <:0x3a[8]:> <:0x0d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// blendpd_vdq_wdq_ib.disasm = { _sink << "blendpd " << DisasmI( imm ) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* BLENDVPD -- Variable Blend Packed Double Precision Floating-Point Values */
// op blendvps_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x14[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// blendvps_vdq_wdq.disasm = { _sink << "blendvps %xmm0," << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op blendvpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x15[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// blendvpd_vdq_wdq.disasm = { _sink << "blendvpd %xmm0," << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CMPP -- Compare Packed Single- or Double-Precision Floating-Point Values */
// op cmpps_vdq_wdq_ib( 0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmpps_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmpps " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn );
//   else           _sink << "cmp" << op[imm&7] << "ps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn );
// };
// 
// op cmppd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmppd_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmppd " << DisasmI( imm ) << DisasmWdq( rmop ) << ',' << DisasmVdq( gn );
//   else           _sink << "cmp" << op[imm&7] << "pd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn );
// };
// 
// /* CMPSD -- Compare Scalar Single- Double-Precision Floating-Point Values */
// op cmpss_vdq_wdq_ib( 0xf3[8]:> <:0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmpss_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmpss " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn );
//   else           _sink << "cmp" << op[imm&7] << "ss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn );
// };
// 
// op cmpsd_vdq_wdq_ib( 0xf2[8]:> <:0x0f[8]:> <:0xc2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// cmpsd_vdq_wdq_ib.disasm = {
//   char const* op[] = {"eq", "lt", "le", "unord", "neq","nlt","nle","ord"};
//   if (imm >= 8)  _sink << "cmpsd " << DisasmI( imm ) << DisasmWdq( rmop ) << ',' << DisasmVdq( gn );
//   else           _sink << "cmp" << op[imm&7] << "sd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn );
// };
// 
// /* COMISD -- Compare Scalar Ordered Single- or Double-Precision Floating-Point Values and Set EFLAGS */
// op comisd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// comisd_vdq_wdq.disasm = { _sink << "comisd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op comiss_vdq_wdq( 0x0f[8]:> <:0x2f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// comiss_vdq_wdq.disasm = { _sink << "comiss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTDQ2PS -- Convert Packed Dword Integers to Packed Single- or Double-Precision FP Values */
// op cvtdq2ps_vdq_wdq( 0x0f[8]:> <:0x5b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtdq2ps_vdq_wdq.disasm = { _sink << "cvtdq2ps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op cvtdq2pd_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0xe6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtdq2pd_vdq_wdq.disasm = { _sink << "cvtdq2pd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTPD2DQ -- Convert Packed Double-Precision FP Values to Packed Dword Integers */
// op cvtpd2dq_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0xe6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtpd2dq_vdq_wdq.disasm = { _sink << "cvtpd2dq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTPD2PI -- Convert Packed Double-Precision FP Values to Packed Dword Integers */
// op cvtpd2pi_pq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtpd2pi_pq_wdq.disasm = { _sink << "cvtpd2pi " << DisasmWdq( rmop ) << ',' << DisasmPq( gn ); };
// 
// /* CVTPD2PS -- Convert Packed Double-Precision FP Values to Packed Single-Precision FP Values */
// op cvtpd2ps_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtpd2ps_vdq_wdq.disasm = { _sink << "cvtpd2ps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTPI2P -- Convert Packed Dword Integers to Packed Single- or Double-Precision FP Values */
// op cvtpi2ps_vdq_qq( 0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtpi2ps_vdq_qq.disasm = { _sink << "cvtpi2ps " << DisasmQq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op cvtpi2pd_vdq_qq( 0x66[8]:> <:0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtpi2pd_vdq_qq.disasm = { _sink << "cvtpi2pd " << DisasmQq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTPS2DQ -- Convert Packed Single-Precision FP Values to Packed Dword Integers */
// 
// op cvtps2dq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtps2dq_vdq_wdq.disasm = { _sink << "cvtps2dq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTPS2PD -- Convert Packed Single-Precision FP Values to Packed Double-Precision FP Values */
// op cvtps2pd_vdq_wdq( 0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtps2pd_vdq_wdq.disasm = { _sink << "cvtps2pd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTPS2PI -- Convert Packed Single-Precision FP Values to Packed Dword Integers */
// op cvtps2pi_pq_wdq( 0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtps2pi_pq_wdq.disasm = { _sink << "cvtps2pi " << DisasmWdq( rmop ) << ',' << DisasmPq( gn ); };
// 
// /* CVTSD2SI -- Convert Scalar Double-Precision FP Value to Integer */
// op cvtsd2si_gd_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtsd2si_gd_wdq.disasm = { _sink << "cvtsd2si " << DisasmWdq( rmop ) << ',' << DisasmGd( gn ); };
// 
// /* CVTSD2SS -- Convert Scalar Double-Precision FP Value to Scalar Single-Precision FP Value */
// op cvtsd2ss_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtsd2ss_vdq_wdq.disasm = { _sink << "cvtsd2ss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTSI2SD -- Convert Dword Integer to Scalar Double-Precision FP Value */
// op cvtsi2sd_vdq_ed( 0xf2[8]:> <:0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtsi2sd_vdq_ed.disasm = { _sink << "cvtsi2sd " << DisasmEd( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTSI2SS -- Convert Dword Integer to Scalar Single-Precision FP Value */
// op cvtsi2ss_vdq_ed( 0xf3[8]:> <:0x0f[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtsi2ss_vdq_ed.disasm = { _sink << "cvtsi2ss " << DisasmEd( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTSS2SD -- Convert Scalar Single-Precision FP Value to Scalar Double-Precision FP Value */
// op cvtss2sd_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtss2sd_vdq_wdq.disasm = { _sink << "cvtss2sd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTSS2SI -- Convert Scalar Single-Precision FP Value to Dword Integer */
// op cvtss2si_gd_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x2d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvtss2si_gd_wdq.disasm = { _sink << "cvtss2si " << DisasmWdq( rmop ) << ',' << DisasmGd( gn ); };
// 
// /* CVTTPD2DQ -- Convert with Truncation Packed Double-Precision FP Values to Packed Dword Integers */
// op cvttpd2dq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvttpd2dq_vdq_wdq.disasm = { _sink << "cvttpd2dq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTTPD2PI -- Convert with Truncation Packed Double-Precision FP Values to Packed Dword Integers */
// op cvttpd2pi_pq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvttpd2pi_pq_wdq.disasm = { _sink << "cvttpd2pi " << DisasmWdq( rmop ) << ',' << DisasmPq( gn ); };
// 
// /* CVTTPS2DQ -- Convert with Truncation Packed Single-Precision FP Values to Packed Dword Integers */
// 
// op cvttps2dq_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvttps2dq_vdq_wdq.disasm = { _sink << "cvttps2dq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* CVTTPS2PI -- Convert with Truncation Packed Single-Precision FP Values to Packed Dword Integers */
// op cvttps2pi_pq_wdq( 0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvttps2pi_pq_wdq.disasm = { _sink << "cvttps2pi " << DisasmWdq( rmop ) << ',' << DisasmPq( gn ); };
// 
// /* CVTTSD2SI -- Convert with Truncation Scalar Double-Precision FP Value to Integer */
// op cvttsd2si_gd_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvttsd2si_gd_wdq.disasm = { _sink << "cvttsd2si " << DisasmWdq( rmop ) << ',' << DisasmGd( gn ); };
// 
// /* CVTTSS2SI -- Convert with Truncation Scalar Single-Precision FP Value to Dword Integer */
// op cvttss2si_gd_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x2c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// cvttss2si_gd_wdq.disasm = { _sink << "cvttss2si " << DisasmWdq( rmop ) << ',' << DisasmGd( gn ); };
// 
// /* DIVPD -- Divide Packed Floating-Point Values */
// op divps_vdq_wdq( 0x0f[8]:> <:0x5e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// divps_vdq_wdq.disasm = { _sink << "divps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op divpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// divpd_vdq_wdq.disasm = { _sink << "divpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op divss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// divss_vdq_wdq.disasm = { _sink << "divss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op divsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// divsd_vdq_wdq.disasm = { _sink << "divsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* DPPD -- Dot Product of Packed Double Precision Floating-Point Values */
// op dpps_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x40[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// dpps_vdq_wdq_ib.disasm = { _sink << "dpps " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op dppd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x41[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// dppd_vdq_wdq_ib.disasm = { _sink << "dppd " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* EMMS -- Empty MMX Technology State */
// op emms( 0x0f[8]:> <:0x77[8] );
// 
// emms.disasm = { _sink << "emms"; };
// 
// /* EXTRACTPS -- Extract Packed Single Precision Floating-Point Value */
// op extractps_rd_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x17[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// extractps_rd_wdq_ib.disasm = { _sink << "extractps " << DisasmI(imm) << ',' << DisasmVdq( gn ) << ',' << DisasmEd( rmop ); };
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
// haddps_vdq_wdq.disasm = { _sink << "haddps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op haddpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x7c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// haddpd_vdq_wdq.disasm = { _sink << "haddpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* HSUBPD -- Packed Single- or Double-FP Horizontal Substract */
// op hsubps_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x7d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// hsubps_vdq_wdq.disasm = { _sink << "hsubps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op hsubpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x7d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// hsubpd_vdq_wdq.disasm = { _sink << "hsubpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* INSERTPS -- Insert Packed Single Precision Floating-Point Value */
// op insertps_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x21[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// insertps_vdq_wdq_ib.disasm = { _sink << "insertps " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* LDDQU -- Load Unaligned Integer 128 Bits */
// op lddqu_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0xf0[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// lddqu_vdq_wdq.disasm = { _sink << "lddqu " << DisasmM( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MASKMOVDQU -- Store Selected Bytes of Double Quadword */
// op maskmovdqu_vdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0xf7[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// maskmovdqu_vdq_vdq.disasm = { _sink << "maskmovdqu " << DisasmVdq( rm ) << ',' << DisasmVdq( gn ); };
// 
// /* MASKMOVQ */
// op maskmovq_pq_nq( 0x0f[8]:> <:0xf7[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// maskmovq_pq_nq.disasm = { _sink << "maskmovq " << DisasmPq( rm ) << ',' << DisasmPq( gn ); };
// 
// /* MAXPS -- Return Maximum Packed Single-Precision Floating-Point Values */
// op maxps_vdq_wdq( 0x0f[8]:> <:0x5f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// maxps_vdq_wdq.disasm = { _sink << "maxps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MAXPD -- Return Maximum Packed Double-Precision Floating-Point Values */
// op maxpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// maxpd_vdq_wdq.disasm = { _sink << "maxpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MAXSS -- Return Maximum Scalar Single-Precision Floating-Point Values */
// op maxss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// maxss_vdq_wdq.disasm = { _sink << "maxss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MAXSD -- Return Maximum Scalar Double-Precision Floating-Point Values */
// op maxsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// maxsd_vdq_wdq.disasm = { _sink << "maxsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MINPS -- Return Minimum Packed Single-Precision Floating-Point Values */
// op minps_vdq_wdq( 0x0f[8]:> <:0x5d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// minps_vdq_wdq.disasm = { _sink << "minps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MINPD -- Return Minimum Packed Double-Precision Floating-Point Values */
// op minpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// minpd_vdq_wdq.disasm = { _sink << "minpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MINSS -- Return Minimum Scalar Single-Precision Floating-Point Values */
// op minss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// minss_vdq_wdq.disasm = { _sink << "minss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MINSD -- Return Minimum Scalar Double-Precision Floating-Point Values */
// op minsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// minsd_vdq_wdq.disasm = { _sink << "minsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MOVAP */
// op movaps_vdq_wdq( 0x0f[8]:> <:0x28[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movaps_vdq_wdq.disasm = { _sink << "movaps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movaps_wdq_vdq( 0x0f[8]:> <:0x29[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movaps_wdq_vdq.disasm = { _sink << "movaps " << DisasmVdq( gn ) << ',' << DisasmWdq( rmop ); };
// 
// op movapd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x28[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movapd_vdq_wdq.disasm = { _sink << "movapd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movapd_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x29[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movapd_wdq_vdq.disasm = { _sink << "movapd " << DisasmVdq( gn ) << ',' << DisasmWdq( rmop ); };
//

// /* MOVD/MOVQ -- Move Doubleword/Move Quadword */
template <class ARCH, class GOP>
struct MovEP : public Operation<ARCH>
{
  MovEP( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "mov" << (GOP::OPSIZE==64?'q':'d') << ' ' << DisasmPq( gn ) << ',' << DisasmE( GOP(), rmop ); }
};

template <class ARCH, class GOP>
struct MovEV : public Operation<ARCH>
{
  MovEV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "mov" << (GOP::OPSIZE==64?'q':'d') << ' ' << DisasmVdq( gn ) << ',' << DisasmE( GOP(), rmop ); }
  void execute( ARCH& arch ) const { arch.rmwrite( GOP(), rmop, arch.template xmm_uread<GOP::OPSIZE>( gn, 0 ) ); }
};

template <class ARCH, class GOP>
struct MovPE : public Operation<ARCH>
{
  MovPE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "mov" << (GOP::OPSIZE==64?'q':'d') << ' ' << DisasmE( GOP(), rmop ) << ',' << DisasmPq( gn ); }
};

template <class ARCH, class GOP>
struct MovVE : public Operation<ARCH>
{
  MovVE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "mov" << (GOP::OPSIZE==64?'q':'d') << ' ' << DisasmE( GOP(), rmop ) << ',' << DisasmVdq( gn ); }
  void execute( ARCH& arch ) const
  {
    arch.template xmm_uwrite<64>( gn, 0, typename ARCH::u64_t(0) );
    arch.template xmm_uwrite<64>( gn, 1, typename ARCH::u64_t(0) );
    arch.template xmm_uwrite<GOP::OPSIZE>( gn, 0, arch.rmread( GOP(), rmop ) );
  }
};

template <class ARCH> struct DC<ARCH,MOVD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, sse__() & opcode( "\x0f\x6e" ) & RM() ))
    {
      if (ic.opsize() == 64) return new MovPE<ARCH,GOq>( _.opbase(), _.rmop(), _.greg() );
      else                   return new MovPE<ARCH,GOd>( _.opbase(), _.rmop(), _.greg() );
    }

  if (auto _ = match( ic, sse__() & opcode( "\x0f\x7e" ) & RM() ))
    {
      if (ic.opsize() == 64) return new MovEP<ARCH,GOq>( _.opbase(), _.rmop(), _.greg() );
      else                   return new MovEP<ARCH,GOd>( _.opbase(), _.rmop(), _.greg() );
    }

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x6e" ) & RM() ))
    {
      if (ic.opsize() == 64) return new MovVE<ARCH,GOq>( _.opbase(), _.rmop(), _.greg() );
      else                   return new MovVE<ARCH,GOd>( _.opbase(), _.rmop(), _.greg() );
    }

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x7e" ) & RM() ))
    {
      if (ic.opsize() == 64) return new MovEV<ARCH,GOq>( _.opbase(), _.rmop(), _.greg() );
      else                   return new MovEV<ARCH,GOd>( _.opbase(), _.rmop(), _.greg() );
    }

  return 0;
}};


// /* MOVDDUP -- Move One Double-FP and Duplicate */
// 
// op movddup_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movddup_vdq_wdq.disasm = { _sink << "movddup " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 

// /* MOVDQA -- Move Aligned Double Quadword */
// 
// op movdqa_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x6f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movdqa_vdq_wdq.disasm = { _sink << "movdqa " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movdqa_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x7f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movdqa_wdq_vdq.disasm = { _sink << "movdqa " << DisasmVdq( gn ) << ',' << DisasmWdq( rmop ); };

/* MOVDQU -- Move Unaligned Double Quadword */
template <class ARCH, bool ALIGNED>
struct MovdqVW : public Operation<ARCH>
{
  MovdqVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "movdq" << (ALIGNED?"a ":"u ") << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); }
  void execute( ARCH& arch ) const
  {
    arch.template xmm_uwrite<64>( gn, 0,  arch.template xmm_uread<64>( rmop, 0 ) );
    arch.template xmm_uwrite<64>( gn, 1,  arch.template xmm_uread<64>( rmop, 1 ) );
  }
};

template <class ARCH, bool ALIGNED>
struct MovdqWV : public Operation<ARCH>
{
  MovdqWV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "movdq" << (ALIGNED?"a ":"u ") << DisasmVdq( gn ) << ',' << DisasmWdq( rmop ); }
  void execute( ARCH& arch ) const
  {
    arch.template xmm_uwrite<64>( rmop, 0,  arch.template xmm_uread<64>( gn, 0 ) );
    arch.template xmm_uwrite<64>( rmop, 1,  arch.template xmm_uread<64>( gn, 1 ) );
  }
};

template <class ARCH> struct DC<ARCH,MOVDQ> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, sseF3() & opcode( "\x0f\x6f" ) & RM() ))
    
    return new MovdqVW<ARCH,false>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, sseF3() & opcode( "\x0f\x7f" ) & RM() ))
    
    return new MovdqWV<ARCH,false>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, sse66() & opcode( "\x0f\x6f" ) & RM() ))
    
    return new MovdqVW<ARCH,true>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, sse66() & opcode( "\x0f\x7f" ) & RM() ))
    
    return new MovdqWV<ARCH,true>( _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}};

// /* MOVDQ2Q -- Move Quadword from XMM to MMX Technology Register */
// op movdq2q_pq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x2d[8]:> <:0b11[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM] );
// 
// movdq2q_pq_vdq.disasm = { _sink << "movdq2q " << DisasmVdq( rm ) << ',' << DisasmPq( gn ); };
// 
// /* MOVHLPS -- Move Packed Single-Precision Floating-Point Values High to Low */
// op movhlps_vdq_vdq( 0x0f[8]:> <:0x12[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movhlps_vdq_vdq.disasm = { _sink << "movhlps " << DisasmVdq( rm ) << ',' << DisasmVdq( gn ); };
// 
// /* MOVHPD -- Move High Packed Double-Precision Floating-Point Value */
// op movhpd_vdq_mdq( 0x66[8]:> <:0x0f[8]:> <:0x16[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movhpd_vdq_mdq.disasm = { _sink << "movhpd " << DisasmM( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movhpd_mdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x17[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movhpd_mdq_vdq.disasm = { _sink << "movhpd " << DisasmVdq( gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVHPS -- Move High Packed Single-Precision Floating-Point Value */
// op movhps_vdq_mdq( 0x0f[8]:> <:0x16[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movhps_vdq_mdq.disasm = { _sink << "movhps " << DisasmM( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movhps_mdq_vdq( 0x0f[8]:> <:0x17[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movhps_mdq_vdq.disasm = { _sink << "movhps " << DisasmVdq( gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVLHPS -- Move Packed Single-Precision Floating-Point Values Low to High */
// op movlhps_vdq_wdq( 0x0f[8]:> <:0x16[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movlhps_vdq_wdq.disasm = { _sink << "movlhps " << DisasmVdq( rm ) << ',' << DisasmVdq( gn ); };
// 
// /* MOVLPD -- Move Low Packed Double-Precision Floating-Point Value */
// op movlpd_vdq_mdq( 0x66[8]:> <:0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movlpd_vdq_mdq.disasm = { _sink << "movlpd " << DisasmM( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movlpd_mdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x13[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movlpd_mdq_vdq.disasm = { _sink << "movlpd " << DisasmVdq( gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVLPS -- Move Low Packed Single-Precision Floating-Point Value */
// op movlps_vdq_mdq( 0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movlps_vdq_mdq.disasm = { _sink << "movlps " << DisasmM( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movlps_mdq_vdq( 0x0f[8]:> <:0x13[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movlps_mdq_vdq.disasm = { _sink << "movlps " << DisasmVdq( gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVMSKPD -- Extract Packed Single- or Double-Precision Floating-Point Sign Mask */
// op movmskpd_gd_vdq( 0x66[8]:> <:0x0f[8]:> <:0x50[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movmskpd_gd_vdq.disasm = { _sink << "movmskpd " << DisasmVdq( rm ) << ',' << DisasmGd( gn ); };
// 
// op movmskps_gd_vdq( 0x0f[8]:> <:0x50[8]:> <:0b11[2]:gn[3]:rm[3] );
// 
// movmskps_gd_vdq.disasm = { _sink << "movmskps " << DisasmVdq( rm ) << ',' << DisasmGd( gn ); };
// 
// /* MOVNTDQA  --  Load Double Quadword Non-Temporal Aligned Hint */
// op movntdqa_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x2a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntdqa_vdq_wdq.disasm = { _sink << "movntdqa " << DisasmM( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MOVNTDQ -- Store Double Quadword Using Non-Temporal Hint */
// op movntdq_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0xe7[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntdq_wdq_vdq.disasm = { _sink << "movntdq " << DisasmVdq( gn ) << ',' << DisasmM( rmop ); };
// 
// /* MOVNTPD -- Store Packed Single- or Double-Precision Floating-Point Values Using Non-Temporal Hint */
// op movntpd_mdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x2b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntpd_mdq_vdq.disasm = { _sink << "movntpd " << DisasmVdq( gn ) << ',' << DisasmM( rmop ); };
// 
// op movntps_mdq_vdq( 0x0f[8]:> <:0x2b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movntps_mdq_vdq.disasm = { _sink << "movntps " << DisasmVdq( gn ) << ',' << DisasmM( rmop ); };
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
// movq_vdq_wdq.disasm = { _sink << "movq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movq_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0xd6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movq_wdq_vdq.disasm = { _sink << "movq " << DisasmVdq( gn ) << ',' << DisasmWdq( rmop ); };
// 
// /* MOVQ2DQ -- Move Quadword from MMX Technology to XMM Register */
// op movq2dq_vdq_pq( 0xf3[8]:> <:0x0f[8]:> <:0xd6[8]:> <:0b11[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM] );
// 
// movq2dq_vdq_pq.disasm = { _sink << "movq2dq " << DisasmVdq( gn ) << ',' << DisasmPq( rm ); };
// 
// /* MOVSD -- Move Scalar Double-Precision Floating-Point Value */
// op movsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x10[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movsd_vdq_wdq.disasm = { _sink << "movsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movsd_wdq_vdq( 0xf2[8]:> <:0x0f[8]:> <:0x11[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movsd_wdq_vdq.disasm = { _sink << "movsd " << DisasmVdq( gn ) << ',' << DisasmWdq( rmop ); };
// 
template <class ARCH>
struct Movsd : public Operation<ARCH>
{
  Movsd( OpBase<ARCH> const& opbase, MOp<ARCH> const* _dst, MOp<ARCH> const* _src ) : Operation<ARCH>( opbase ), dst( _dst ), src( _src ) {} RMOp<ARCH> dst; RMOp<ARCH> src;
  void disasm( std::ostream& sink ) const { sink << "movsd " << DisasmWdq( src ) << ',' << DisasmWdq( dst ); }
  void execute( ARCH& arch ) const
  {
    arch.template xmm_uwrite<64>( dst, 0,  arch.template xmm_uread<64>( src, 0 ) );
    if (not dst.is_memory_operand())
      arch.template xmm_uwrite<64>( dst, 1, typename ARCH::u64_t(0) );
  }
};

template <class ARCH> struct DC<ARCH,VMOVSD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, sseF2() & opcode( "\x0f\x10" ) & RM() ))
    
    return new Movsd<ARCH>( _.opbase(), make_rop<ARCH>( _.greg() ), _.rmop() );
  
  if (auto _ = match( ic, sseF2() & opcode( "\x0f\x11" ) & RM() ))
    
    return new Movsd<ARCH>( _.opbase(), _.rmop(), make_rop<ARCH>( _.greg() ) );
  
  return 0;
}};

// /* MOVSHDUP -- Move Packed Single-FP High and Duplicate */
// op movshdup_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x16[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movshdup_vdq_wdq.disasm = { _sink << "movshdup " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MOVSLDUP -- Move Packed Single-FP Low and Duplicate */
// op movsldup_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x12[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movsldup_vdq_wdq.disasm = { _sink << "movsldup " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MOVSS -- Move Scalar Single-Precision Floating-Point Values */
// op movss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x10[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movss_vdq_wdq.disasm = { _sink << "movss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movss_wdq_vdq( 0xf3[8]:> <:0x0f[8]:> <:0x11[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movss_wdq_vdq.disasm = { _sink << "movss " << DisasmVdq( gn ) << ',' << DisasmWdq( rmop ); };
// 
// /* MOVUP -- Move Unaligned Packed Single- or Double-Precision Floating-Point Values */
// op movups_vdq_wdq( 0x0f[8]:> <:0x10[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movups_vdq_wdq.disasm = { _sink << "movups " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movups_wdq_vdq( 0x0f[8]:> <:0x11[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movups_wdq_vdq.disasm = { _sink << "movups " << DisasmVdq( gn ) << ',' << DisasmWdq( rmop ); };
// 
// op movupd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x10[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movupd_vdq_wdq.disasm = { _sink << "movupd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op movupd_wdq_vdq( 0x66[8]:> <:0x0f[8]:> <:0x11[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// movupd_wdq_vdq.disasm = { _sink << "movupd " << DisasmVdq( gn ) << ',' << DisasmWdq( rmop ); };
// 
// /* MPSADBW -- Compute Multiple Packed Sums of Absolute Difference */
// op mpsadbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x42[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// mpsadbw_vdq_wdq.disasm = { _sink << "mpsadbw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MULP -- Multiply Packed Single- or Double-Precision Floating-Point Values */
// op mulps_vdq_wdq( 0x0f[8]:> <:0x59[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// mulps_vdq_wdq.disasm = { _sink << "mulps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op mulpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x59[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// mulpd_vdq_wdq.disasm = { _sink << "mulpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* MULS -- Multiply Scalar Single- or Double-Precision Floating-Point Values */
// op mulss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x59[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// mulss_vdq_wdq.disasm = { _sink << "mulss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op mulsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x59[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// mulsd_vdq_wdq.disasm = { _sink << "mulsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* ORP -- Bitwise Logical OR of Single- or Double-Precision Floating-Point Values */
// op orps_vdq_wdq( 0x0f[8]:> <:0x56[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// orps_vdq_wdq.disasm = { _sink << "orps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op orpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x56[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// orpd_vdq_wdq.disasm = { _sink << "orpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
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
// pabsb_vdq_wdq.disasm = { _sink << "pabsb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pabsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x1d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsw_vdq_wdq.disasm = { _sink << "pabsw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pabsd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x1e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pabsd_vdq_wdq.disasm = { _sink << "pabsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// packsswb_vdq_wdq.disasm = { _sink << "packsswb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op packssdw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x6b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packssdw_vdq_wdq.disasm = { _sink << "packssdw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PACKUSWB/PACKUSDW -- Pack with Unsigned Saturation */
// op packuswb_pq_qq( 0x0f[8]:> <:0x67[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packuswb_pq_qq.disasm = { _sink << "packuswb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op packuswb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x67[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packuswb_vdq_wdq.disasm = { _sink << "packuswb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op packusdw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x2b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// packusdw_vdq_wdq.disasm = { _sink << "packusdw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// paddb_vdq_wdq.disasm = { _sink << "paddb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op paddw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddw_vdq_wdq.disasm = { _sink << "paddw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op paddd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfe[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddd_vdq_wdq.disasm = { _sink << "paddd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op paddq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddq_vdq_wdq.disasm = { _sink << "paddq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// paddsb_vdq_wdq.disasm = { _sink << "paddsb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op paddsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xed[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddsw_vdq_wdq.disasm = { _sink << "paddsw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// paddusb_vdq_wdq.disasm = { _sink << "paddusb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op paddusw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xdd[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// paddusw_vdq_wdq.disasm = { _sink << "paddusw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PALIGNR -- Packed Align Right */
// op palignr_pq_qq_ib( 0x0f[8]:> <:0x3a[8]:> <:0x0f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// palignr_pq_qq_ib.disasm = { _sink << "palignr " << DisasmI(imm) << ',' << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op palignr_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// palignr_vdq_wdq_ib.disasm = { _sink << "palignr " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PAND -- Logical AND */
// op pand_pq_qq( 0x0f[8]:> <:0xdb[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pand_pq_qq.disasm = { _sink << "pand " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pand_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xdb[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pand_vdq_wdq.disasm = { _sink << "pand " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PANDN -- Logical AND NOT */
// op pandn_pq_qq( 0x0f[8]:> <:0xdf[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pandn_pq_qq.disasm = { _sink << "pandn " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pandn_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xdf[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pandn_vdq_wdq.disasm = { _sink << "pandn " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// pavgb_vdq_wdq.disasm = { _sink << "pavgb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pavgw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pavgw_vdq_wdq.disasm = { _sink << "pavgw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PBLENDVB -- Variable Blend Packed Bytes */
// op pblenvb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x10[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pblenvb_vdq_wdq.disasm = { _sink << "pblenvb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PBLENDW -- Blend Packed Words */
// op pblendw_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pblendw_vdq_wdq_ib.disasm = { _sink << "pblendw " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PCLMULQDQ - Carry-Less Multiplication Quadword */
// op pclmulqdq_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x44[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pclmulqdq_vdq_wdq_ib.disasm = { _sink << "pclmulqdq " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PCMPESTRM -- Packed Compare Explicit Length Strings, Return Mask */
// op pcmpestrm_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x60[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pcmpestrm_vdq_wdq_ib.disasm = { _sink << "pcmpestrm " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PCMPESTRI -- Packed Compare Explicit Length Strings, Return Index */
// op pcmpestri_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x61[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pcmpestri_vdq_wdq_ib.disasm = { _sink << "pcmpestri " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PCMPISTRM -- Packed Compare Implicit Length Strings, Return Mask */
// op pcmpistrm_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x62[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pcmpistrm_vdq_wdq_ib.disasm = { _sink << "pcmpistrm " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PCMPISTRI -- Packed Compare Implicit Length Strings, Return Index */
// op pcmpistri_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x63[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pcmpistri_vdq_wdq_ib.disasm = { _sink << "pcmpistri " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 

/* PCMPEQB/PCMPEQW/PCMPEQD/PCMEQQ -- Compare Packed Data for Equal */
template <class ARCH, unsigned OPSIZE>
struct PCmpEqVW : public Operation<ARCH>
{
  PCmpEqVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const {
    sink << "pcmpeq" << ("bwdq"[SB<OPSIZE/8>::begin]) << ' ' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    
    for (unsigned sub = 0; sub < (128/OPSIZE); ++sub)
      {
        u_type msk = u_type(arch.template xmm_uread<OPSIZE>( gn, sub ) != arch.template xmm_uread<OPSIZE>( rmop, sub )) - u_type(1);
        arch.template xmm_uwrite<OPSIZE>( gn, sub, msk );
      }
  }
};

template <class ARCH> struct DC<ARCH,PCMPEQ> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, sse66() & opcode( "\x0f\x74" ) & RM() ))
    
    return new PCmpEqVW<ARCH,8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, sse66() & opcode( "\x0f\x75" ) & RM() ))
    
    return new PCmpEqVW<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, sse66() & opcode( "\x0f\x76" ) & RM() ))
    
    return new PCmpEqVW<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, sse66() & opcode( "\x0f\x38\x29" ) & RM() ))
    
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
// pcmpeqb_vdq_wdq.disasm = { _sink << "pcmpeqb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pcmpeqw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x75[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqw_vdq_wdq.disasm = { _sink << "pcmpeqw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pcmpeqd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x76[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqd_vdq_wdq.disasm = { _sink << "pcmpeqd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pcmpeqq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x29[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpeqq_vdq_wdq.disasm = { _sink << "pcmpeqq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// pcmpgtb_vdq_wdq.disasm = { _sink << "pcmpgtb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pcmpgtw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x65[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtw_vdq_wdq.disasm = { _sink << "pcmpgtw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pcmpgtd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x66[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtd_vdq_wdq.disasm = { _sink << "pcmpgtd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pcmpgtq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x37[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pcmpgtq_vdq_wdq.disasm = { _sink << "pcmpgtq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PEXTRB/PEXTRW/PEXTRD/PEXTRQ -- Extract Byte/Dword/Qword */
// op pextrw_rd_pq_ib( 0x0f[8]:> <:0xc5[8]:> <:0b11[2]:gn[3]:rm[3]:> <:imm[8] );
// 
// pextrw_rd_pq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmPq( rm ) << ',' << DisasmGd( gn ); };
// 
// op pextrw_rd_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0xc5[8]:> <:0b11[2]:gn[3]:rm[3]:> <:imm[8] );
// 
// pextrw_rd_vdq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmVdq( rm ) << ',' << DisasmGd( gn ); };
// 
// op pextrb_eb_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x14[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pextrb_eb_vdq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmVdq( gn ) << ',' << DisasmEb( rmop ); };
// 
// op pextrw_ew_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x15[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pextrw_ew_vdq_ib.disasm = { _sink << "pextrw " << DisasmI( imm ) << ',' << DisasmVdq( gn ) << ',' << DisasmEw( rmop ); };
// 
// op pextrd_ed_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x16[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pextrd_ed_vdq_ib.disasm = { _sink << "pextrd " << DisasmI( imm ) << ',' << DisasmVdq( gn ) << ',' << DisasmEd( rmop ); };
// 
// // op pextrq_eb_vdq_ib( 0x66[8]:> <:0x48[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x16[8]:> <:?[2]:gn[3]:rm[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// // pextrq_eb_vdq_ib.disasm = { _sink << "pextrq " << DisasmI( imm ) << ',' << DisasmVdq( gn ) << ',' << DisasmEq( rmop ); };
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
// phaddw_vdq_wdq.disasm = { _sink << "phaddw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op phaddd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x02[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddd_vdq_wdq.disasm = { _sink << "phaddd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PHADDSW -- Packed Horizontal Add and Saturate */
// op phaddsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x03[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddsw_pq_qq.disasm = { _sink << "phaddsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op phaddsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x03[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phaddsw_vdq_wdq.disasm = { _sink << "phaddsw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PHMINPOSUW -- Packed Horizontal Word Minimum */
// op phminposuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x41[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phminposuw_vdq_wdq.disasm = { _sink << "phminposuw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// phsubw_vdq_wdq.disasm = { _sink << "phsubw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op phsubd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x06[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubd_vdq_wdq.disasm = { _sink << "phsubd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PHSUBSW -- Packed Horizontal Subtract and Saturate */
// op phsubsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x07[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubsw_pq_qq.disasm = { _sink << "phsubsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op phsubsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x07[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// phsubsw_vdq_wdq.disasm = { _sink << "phsubsw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PINSRB/PINSRW/PINSRD/PINSRQ -- Insert Byte/Word/Dword/Qword */
// op pinsrw_pq_ed_ib( 0x0f[8]:> <:0xc4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pinsrw_pq_ed_ib.disasm = { _sink << "pinsrw " << DisasmI( imm ) << ',' << DisasmEd( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pinsrb_vdq_ed_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x20[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pinsrb_vdq_ed_ib.disasm = { _sink << "pinsrb " << DisasmI( imm ) << ',' << DisasmEd( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pinsrw_vdq_ed_ib( 0x66[8]:> <:0x0f[8]:> <:0xc4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pinsrw_vdq_ed_ib.disasm = { _sink << "pinsrw " << DisasmI( imm ) << ',' << DisasmEd( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pinsrd_vdq_ed_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x22[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pinsrd_vdq_ed_ib.disasm = { _sink << "pinsrd " << DisasmI( imm ) << ',' << DisasmEd( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMADDUBSW -- PINSRB/PINSRD/PINSRQ -- Insert Byte/Dword/Qword */
// op pmaddubsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x04[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddubsw_pq_qq.disasm = { _sink << "pmaddubsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmaddubsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x04[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddubsw_vdq_wdq.disasm = { _sink << "pmaddubsw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMADDWD -- Multiply and Add Packed Integers */
// op pmaddwd_pq_qq( 0x0f[8]:> <:0xf5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddwd_pq_qq.disasm = { _sink << "pmaddwd " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmaddwd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaddwd_vdq_wdq.disasm = { _sink << "pmaddwd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMAXS -- Maximum of Packed Signed Byte/Word/DWord/QWord Integers */
// op pmaxsw_pq_qq( 0x0f[8]:> <:0xee[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxsw_pq_qq.disasm = { _sink << "pmaxsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmaxsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxsb_vdq_wdq.disasm = { _sink << "pmaxsb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmaxsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xee[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxsw_vdq_wdq.disasm = { _sink << "pmaxsw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmaxsd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3d[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxsd_vdq_wdq.disasm = { _sink << "pmaxsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMAXU -- Maximum of Packed Unsigned Byte/Word/DWord Integers */
// op pmaxub_pq_qq( 0x0f[8]:> <:0xde[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxub_pq_qq.disasm = { _sink << "pmaxub " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmaxub_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xde[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxub_vdq_wdq.disasm = { _sink << "pmaxub " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmaxuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxuw_vdq_wdq.disasm = { _sink << "pmaxuw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmaxud_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmaxud_vdq_wdq.disasm = { _sink << "pmaxud " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMINS -- Minimum of Packed Signed Byte/Word/DWord Integers */
// op pminsw_pq_qq( 0x0f[8]:> <:0xea[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminsw_pq_qq.disasm = { _sink << "pminsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pminsb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x38[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminsb_vdq_wdq.disasm = { _sink << "pminsb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pminsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xea[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminsw_vdq_wdq.disasm = { _sink << "pminsw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pminsd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x39[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminsd_vdq_wdq.disasm = { _sink << "pminsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMINU -- Minimum of Packed Unsigned Byte/Word/DWord Integers */
// op pminub_pq_qq( 0x0f[8]:> <:0xda[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminub_pq_qq.disasm = { _sink << "pminub " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pminub_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xda[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminub_vdq_wdq.disasm = { _sink << "pminub " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pminuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminuw_vdq_wdq.disasm = { _sink << "pminuw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pminud_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x3b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pminud_vdq_wdq.disasm = { _sink << "pminud " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 

// /* PMOVMSKB -- Move Byte Mask */
template <class ARCH>
struct PMovMskBRP : public Operation<ARCH>
{
  PMovMskBRP( OpBase<ARCH> const& opbase, uint8_t _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} uint8_t rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pmovmskb " << DisasmPq( rm ) << ',' << DisasmGd( gn ); }
};
 
template <class ARCH>
struct PMovMskBRV : public Operation<ARCH>
{
  PMovMskBRV( OpBase<ARCH> const& opbase, uint8_t _rm, uint8_t _gn ) : Operation<ARCH>( opbase ), rm( _rm ), gn( _gn ) {} uint8_t rm; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pmovmskb " << DisasmVdq( rm ) << ',' << DisasmGd( gn ); }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;
    
    u32_t res = u32_t(0);
    
    for (unsigned sub = 0; sub < 16; ++sub)
      res |= u32_t(arch.template xmm_uread<8>( rm, sub ) >> 7) << sub;
    
    arch.regwrite( GOd(), gn, res );
  }
};

template <class ARCH> struct DC<ARCH,PMOVMSKB> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, sse__() & opcode( "\x0f\xd7" ) & RM_reg() ))
    
    return new PMovMskBRP<ARCH>( _.opbase(), _.ereg(), _.greg() );
  
  if (auto _ = match( ic, sse66() & opcode( "\x0f\xd7" ) & RM_reg() ))

    return new PMovMskBRV<ARCH>( _.opbase(), _.ereg(), _.greg() );
  
  return 0;
}};

// 
// /* PMOVSX -- Packed Move with Sign Extend */
// op pmovsxbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x20[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxbw_vdq_wdq.disasm = { _sink << "pmovsxbw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovsxbd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x21[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxbd_vdq_wdq.disasm = { _sink << "pmovsxbd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovsxbq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x22[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxbq_vdq_wdq.disasm = { _sink << "pmovsxbq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovsxwd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x23[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxwd_vdq_wdq.disasm = { _sink << "pmovsxwd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovsxwq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x24[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxwq_vdq_wdq.disasm = { _sink << "pmovsxwq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovsxdq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x25[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovsxdq_vdq_wdq.disasm = { _sink << "pmovsxdq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMOVZX -- Packed Move with Sign Extend */
// op pmovzxbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x30[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxbw_vdq_wdq.disasm = { _sink << "pmovzxbw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovzxbd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x31[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxbd_vdq_wdq.disasm = { _sink << "pmovzxbd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovzxbq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x32[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxbq_vdq_wdq.disasm = { _sink << "pmovzxbq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovzxwd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x33[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxwd_vdq_wdq.disasm = { _sink << "pmovzxwd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovzxwq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x34[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxwq_vdq_wdq.disasm = { _sink << "pmovzxwq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pmovzxdq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x35[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmovzxdq_vdq_wdq.disasm = { _sink << "pmovzxdq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMULDQ -- Multiply Packed Signed Dword Integers */
// op pmuldq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x28[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmuldq_vdq_wdq.disasm = { _sink << "pmuldq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMULHRSW -- Packed Multiply High with Round and Scale */
// op pmulhrsw_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x0b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhrsw_pq_qq.disasm = { _sink << "pmulhrsw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmulhrsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x0b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhrsw_vdq_wdq.disasm = { _sink << "pmulhrsw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMULHUW -- Multiply Packed Unsigned Integers and Store High Result */
// op pmulhuw_pq_qq( 0x0f[8]:> <:0xe4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhuw_pq_qq.disasm = { _sink << "pmulhuw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmulhuw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhuw_vdq_wdq.disasm = { _sink << "pmulhuw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMULHW -- Multiply Packed Signed Integers and Store High Result */
// op pmulhw_pq_qq( 0x0f[8]:> <:0xe5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhw_pq_qq.disasm = { _sink << "pmulhw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmulhw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulhw_vdq_wdq.disasm = { _sink << "pmulhw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMULLD -- Multiply Packed Signed Dword Integers and Store Low Result */
// op pmulld_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x40[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmulld_vdq_wdq.disasm = { _sink << "pmulld " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMULLW -- Multiply Packed Signed Integers and Store Low Result */
// op pmullw_pq_qq( 0x0f[8]:> <:0xd5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmullw_pq_qq.disasm = { _sink << "pmullw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmullw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd5[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmullw_vdq_wdq.disasm = { _sink << "pmullw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PMULUDQ -- Multiply Packed Unsigned Doubleword Integers */
// op pmuludq_pq_qq( 0x0f[8]:> <:0xf4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmuludq_pq_qq.disasm = { _sink << "pmuludq " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pmuludq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf4[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pmuludq_vdq_wdq.disasm = { _sink << "pmuludq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* POR -- Bitwise Logical OR */
// op por_pq_qq( 0x0f[8]:> <:0xeb[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// por_pq_qq.disasm = { _sink << "por " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op por_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xeb[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// por_vdq_wdq.disasm = { _sink << "por " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PSADBW -- Compute Sum of Absolute Differences */
// op psadbw_pq_qq( 0x0f[8]:> <:0xf6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psadbw_pq_qq.disasm = { _sink << "psadbw " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op psadbw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psadbw_vdq_wdq.disasm = { _sink << "psadbw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PSHUFB -- Packed Shuffle Bytes */
// op pshufb_pq_qq( 0x0f[8]:> <:0x38[8]:> <:0x00[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pshufb_pq_qq.disasm = { _sink << "pshufb " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); };
// 
// op pshufb_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x00[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pshufb_vdq_wdq.disasm = { _sink << "pshufb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PSHUFD -- Shuffle Packed Doublewords */
// op pshufd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pshufd_vdq_wdq_ib.disasm = { _sink << "pshufd " << DisasmI(imm) << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
//

template <class ARCH>
struct Pshufd : public Operation<ARCH>
{
  Pshufd( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rm, uint8_t _gn, uint8_t _oo ) : Operation<ARCH>(opbase), rm(_rm), gn(_gn), oo(_oo) {} RMOp<ARCH> rm; uint8_t gn; uint8_t oo;
  void disasm( std::ostream& sink ) const { sink << "pshufd " << DisasmI(oo) << ',' << DisasmWdq( rm ) << ',' << DisasmVdq( gn ); }
  void execute( ARCH& arch ) const
  {
    for (unsigned idx = 0; idx < 4; ++idx)
      {
        unsigned part = (oo >> 2*idx) % 4;
        arch.template xmm_uwrite<32>( gn, idx, arch.template xmm_uread<32>( rm, part ) );
      }
  }
};

template <class ARCH> struct DC<ARCH,PSHUFD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x70" ) & RM() & Imm<8>() ))
    
      return new Pshufd<ARCH>( _.opbase(), _.rmop(), _.greg(), _.i(int8_t()) );

  return 0;
}};



// /* PSHUFHW -- Shuffle Packed High Words */
// op pshufhw_vdq_wdq_ib( 0xf3[8]:> <:0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pshufhw_vdq_wdq_ib.disasm = { _sink << "pshufhw " << DisasmI(imm) << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PSHUFLW -- Shuffle Packed Low Words */
// op pshuflw_vdq_wdq_ib( 0xf2[8]:> <:0x0f[8]:> <:0x70[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// pshuflw_vdq_wdq_ib.disasm = { _sink << "pshuflw " << DisasmI(imm) << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// psignb_vdq_wdq.disasm = { _sink << "psignb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psignw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x09[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignw_vdq_wdq.disasm = { _sink << "psignw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psignd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x0a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psignd_vdq_wdq.disasm = { _sink << "psignd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PSLLDQ -- Shift Double Quadword Left Logical */
// op pslldq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:7[3]:rm[3]:> <:imm[8] );
// 
// pslldq_vdq_ib.disasm = { _sink << "pslldq " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
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
// psllw_vdq_wdq.disasm = { _sink << "psllw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psllw_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x71[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllw_vdq_ib.disasm = { _sink << "psllw " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
// 
// op pslld_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// pslld_vdq_wdq.disasm = { _sink << "pslld " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op pslld_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x72[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// pslld_vdq_ib.disasm = { _sink << "pslld " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
// 
// op psllq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psllq_vdq_wdq.disasm = { _sink << "psllq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psllq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:6[3]:rm[3]:> <:imm[8] );
// 
// psllq_vdq_ib.disasm = { _sink << "psllq " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
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
// psraw_vdq_wdq.disasm = { _sink << "psraw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psraw_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x71[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psraw_vdq_ib.disasm = { _sink << "psraw " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
// 
// op psrad_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrad_vdq_wdq.disasm = { _sink << "psrad " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psrad_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x72[8]:> <:0b11[2]:4[3]:rm[3]:> <:imm[8] );
// 
// psrad_vdq_ib.disasm = { _sink << "psrad " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
// 
// /* PSRLDQ -- Shift Double Quadword Right Logical */
// op psrldq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:3[3]:rm[3]:> <:imm[8] );
// 
// psrldq_vdq_ib.disasm = { _sink << "psrldq " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
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
// psrlw_vdq_wdq.disasm = { _sink << "psrlw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psrlw_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x71[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlw_vdq_ib.disasm = { _sink << "psrlw " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
// 
// op psrld_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd2[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrld_vdq_wdq.disasm = { _sink << "psrld " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psrld_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x72[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrld_vdq_ib.disasm = { _sink << "psrld " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
// 
// op psrlq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd3[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psrlq_vdq_wdq.disasm = { _sink << "psrlq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psrlq_vdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x73[8]:> <:0b11[2]:2[3]:rm[3]:> <:imm[8] );
// 
// psrlq_vdq_ib.disasm = { _sink << "psrlq " << DisasmI(imm) << ',' << DisasmVdq( rm ); };
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
// psubb_vdq_wdq.disasm = { _sink << "psubb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psubw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xf9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubw_vdq_wdq.disasm = { _sink << "psubw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psubd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfa[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubd_vdq_wdq.disasm = { _sink << "psubd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psubq_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xfb[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubq_vdq_wdq.disasm = { _sink << "psubq " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// psubsb_vdq_wdq.disasm = { _sink << "psubsb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psubsw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xe9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubsw_vdq_wdq.disasm = { _sink << "psubsw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
// psubusb_vdq_wdq.disasm = { _sink << "psubusb " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// op psubusw_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0xd9[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// psubusw_vdq_wdq.disasm = { _sink << "psubusw " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* PTEST -- Logical Compare */
// op ptest_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x38[8]:> <:0x17[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// ptest_vdq_wdq.disasm = { _sink << "ptest " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
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
  PunpckV( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "punpck" << (HI?'h':'l') << SizeID<OPSIZE>::iid() << SizeID<2*OPSIZE>::iid() << ' ' << DisasmWdq(rmop) << ',' << DisasmVdq(gn); }
  void execute( ARCH& arch ) const
  {
    unsigned const COUNT = 64/OPSIZE;
    unsigned const RMSK = HI ? 0 : COUNT - 1;
    unsigned const HALF = HI ? COUNT : 0;
    
    for (unsigned chunk = 0; chunk < COUNT; ++chunk)
      {
        unsigned idx = chunk & RMSK;
        arch.template xmm_uwrite<OPSIZE>( gn, 2*idx+0, arch.template xmm_uread<OPSIZE>( gn, HALF+idx ) );
        arch.template xmm_uwrite<OPSIZE>( gn, 2*idx+1, arch.template xmm_uread<OPSIZE>( rmop, HALF+idx ) );
      }
  }
};

/* PUNPCKHBW/PUNPCKHWD/PUNPCKHDQ/PUNPCKHQDQ/PUNPCKLBW/PUNPCKLWD/PUNPCKLDQ/PUNPCKLQDQ -- Unpack Data */
template <class ARCH> struct DC<ARCH,PUNPCK> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x68" ) & RM() ))

    return new PunpckV<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x69" ) & RM() ))

    return new PunpckV<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x6a" ) & RM() ))

    return new PunpckV<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x6d" ) & RM() ))

    return new PunpckV<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x60" ) & RM() ))

    return new PunpckV<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x61" ) & RM() ))

    return new PunpckV<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x62" ) & RM() ))

    return new PunpckV<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, sse66() & opcode( "\x0f\x6c" ) & RM() ))

    return new PunpckV<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );

  return 0;
}};



template <class ARCH>
struct PXorPQ : public Operation<ARCH>
{
  PXorPQ( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pxor " << DisasmQq( rmop ) << ',' << DisasmPq( gn ); }
};

template <class ARCH>
struct PXorVW : public Operation<ARCH>
{
  PXorVW( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "pxor " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); }
  void execute( ARCH& arch ) const
  {
    arch.template xmm_uwrite<64>( gn, 0,  arch.template xmm_uread<64>( gn, 0 ) ^ arch.template xmm_uread<64>( rmop, 0 ) );
    arch.template xmm_uwrite<64>( gn, 1,  arch.template xmm_uread<64>( gn, 1 ) ^ arch.template xmm_uread<64>( rmop, 1 ) );
  }
};

/* PXOR -- Logical Exclusive OR */
template <class ARCH> struct DC<ARCH,PXOR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, sse__() & opcode( "\x0f\xef" ) & RM() ))

    return new PXorPQ<ARCH>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, sse66() & opcode( "\x0f\xef" ) & RM() ))

    return new PXorVW<ARCH>( _.opbase(), _.rmop(), _.greg() );

  return 0;
}};

// /* RCPPS -- Compute Reciprocals of Packed Single-Precision Floating-Point Values */
// op rcpps_vdq_wdq( 0x0f[8]:> <:0x53[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rcpps_vdq_wdq.disasm = { _sink << "rcpps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* RCPSS—Compute Reciprocal of Scalar Single-Precision Floating-Point Values */
// op rcpss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x53[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rcpss_vdq_wdq.disasm = { _sink << "rcpss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* ROUNDPS -- Round Packed Single-Precision Floating-Point Values */
// op roundps_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x08[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundps_vdq_wdq_ib.disasm = { _sink << "roundps " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* ROUNDPD -- Round Packed Double-Precision Floating-Point Values */
// op roundpd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x09[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundpd_vdq_wdq_ib.disasm = { _sink << "roundpd " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* ROUNDSS -- Round Scalar Single-Precision Floating-Point Values */
// op roundss_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0a[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundss_vdq_wdq_ib.disasm = { _sink << "roundss " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* ROUNDSD -- Round Scalar Double-Precision Floating-Point Values */
// op roundsd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0x3a[8]:> <:0x0b[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// roundsd_vdq_wdq_ib.disasm = { _sink << "roundsd " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* RSQRTPS -- Compute Reciprocals of Square Roots of Packed Single-Precision Floating-Point Values */
// op rsqrtps_vdq_wdq( 0x0f[8]:> <:0x52[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rsqrtps_vdq_wdq.disasm = { _sink << "rsqrtps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* RSQRTSS -- Compute Reciprocal of Square Root of Scalar Single-Precision Floating-Point Value */
// op rsqrtss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x52[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// rsqrtss_vdq_wdq.disasm = { _sink << "rsqrtss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SHUFPS -- Shuffle Packed Single-Precision Floating-Point Values */
// op shufps_vdq_wdq_ib( 0x0f[8]:> <:0xc6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// shufps_vdq_wdq_ib.disasm = { _sink << "shufps " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SHUFPD -- Shuffle Packed Double-Precision Floating-Point Values */
// op shufpd_vdq_wdq_ib( 0x66[8]:> <:0x0f[8]:> <:0xc6[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM]:> <:imm[8] );
// 
// shufpd_vdq_wdq_ib.disasm = { _sink << "shufpd " << DisasmI(imm) << ',' << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SQRTPS -- Compute Square Roots of Packed Single-Precision Floating-Point Values */
// op sqrtps_vdq_wdq( 0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtps_vdq_wdq.disasm = { _sink << "sqrtps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SQRTPD -- Compute Square Roots of Packed Double-Precision Floating-Point Values */
// op sqrtpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtpd_vdq_wdq.disasm = { _sink << "sqrtpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SQRTSS -- Compute Square Roots of Packed Single-Precision Floating-Point Values */
// op sqrtss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtss_vdq_wdq.disasm = { _sink << "sqrtss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SQRTSD -- Compute Square Roots of Packed Double-Precision Floating-Point Values */
// op sqrtsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x51[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// sqrtsd_vdq_wdq.disasm = { _sink << "sqrtsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SUBPS -- Subtract Packed Single-Precision Floating-Point Values */
// op subps_vdq_wdq( 0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subps_vdq_wdq.disasm = { _sink << "subps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SUBPD -- Subtract Packed Double-Precision Floating-Point Values */
// op subpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subpd_vdq_wdq.disasm = { _sink << "subpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SUBSS -- Subtract Scalar Single-Precision Floating-Point Values */
// op subss_vdq_wdq( 0xf3[8]:> <:0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subss_vdq_wdq.disasm = { _sink << "subss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* SUBSD -- Subtract Scalar Double-Precision Floating-Point Values */
// op subsd_vdq_wdq( 0xf2[8]:> <:0x0f[8]:> <:0x5c[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// subsd_vdq_wdq.disasm = { _sink << "subsd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// 
// /* UCOMISS -- Unordered Compare Scalar Single-Precision Floating-Point Values and Set EFLAGS */
// op ucomiss_vdq_wdq( 0x0f[8]:> <:0x2e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// ucomiss_vdq_wdq.disasm = { _sink << "ucomiss " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* UCOMISD -- Unordered Compare Scalar Double-Precision Floating-Point Values and Set EFLAGS */
// op ucomisd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x2e[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// ucomisd_vdq_wdq.disasm = { _sink << "ucomisd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* UNPCKHPS -- Unpack and Interleave High Packed Single-Precision Floating-Point Values */
// op unpckhps_vdq_wdq( 0x0f[8]:> <:0x15[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// unpckhps_vdq_wdq.disasm = { _sink << "unpckhps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* UNPCKHPD -- Unpack and Interleave High Packed Double-Precision Floating-Point Values */
// op unpckhpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x15[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// unpckhpd_vdq_wdq.disasm = { _sink << "unpckhpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* UNPCKLPS -- Unpack and Interleave Low Packed Single-Precision Floating-Point Values */
// op unpcklps_vdq_wdq( 0x0f[8]:> <:0x14[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// unpcklps_vdq_wdq.disasm = { _sink << "unpcklps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* UNPCKLPD -- Unpack and Interleave Low Packed Double-Precision Floating-Point Values */
// op unpcklpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x14[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// unpcklpd_vdq_wdq.disasm = { _sink << "unpcklpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* XORPD -- Bitwise Logical XOR for Double-Precision Floating-Point Values */
// op xorpd_vdq_wdq( 0x66[8]:> <:0x0f[8]:> <:0x57[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// xorpd_vdq_wdq.disasm = { _sink << "xorpd " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
// 
// /* XORPS -- Bitwise Logical XOR for Single-Precision Floating-Point Values */
// op xorps_vdq_wdq( 0x0f[8]:> <:0x57[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*modrm[ModRM] );
// 
// xorps_vdq_wdq.disasm = { _sink << "xorps " << DisasmWdq( rmop ) << ',' << DisasmVdq( gn ); };
