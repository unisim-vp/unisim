/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_UTIL_DEBUG_DWARF_ENCODING_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_ENCODING_HH__

#include <inttypes.h>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

// Children determination encoding
const uint8_t DW_CHILDREN_no = 0;
const uint8_t DW_CHILDREN_yes = 1;

// Tag encoding
// DWARF2
const uint16_t DW_TAG_array_type = 0x01;
const uint16_t DW_TAG_class_type = 0x02;
const uint16_t DW_TAG_entry_point = 0x03;
const uint16_t DW_TAG_enumeration_type = 0x04;
const uint16_t DW_TAG_formal_parameter = 0x05;
const uint16_t DW_TAG_imported_declaration = 0x08;
const uint16_t DW_TAG_label = 0x0a;
const uint16_t DW_TAG_lexical_block = 0x0b;
const uint16_t DW_TAG_member = 0x0d;
const uint16_t DW_TAG_pointer_type = 0x0f;
const uint16_t DW_TAG_reference_type = 0x10;
const uint16_t DW_TAG_compile_unit = 0x11;
const uint16_t DW_TAG_string_type = 0x12;
const uint16_t DW_TAG_structure_type = 0x13;
const uint16_t DW_TAG_subroutine_type = 0x15;
const uint16_t DW_TAG_typedef = 0x16;
const uint16_t DW_TAG_union_type = 0x17;
const uint16_t DW_TAG_unspecified_parameters = 0x18;
const uint16_t DW_TAG_variant = 0x19;
const uint16_t DW_TAG_common_block = 0x1a;
const uint16_t DW_TAG_common_inclusion = 0x1b;
const uint16_t DW_TAG_inheritance = 0x1c;
const uint16_t DW_TAG_inlined_subroutine = 0x1d;
const uint16_t DW_TAG_module = 0x1e;
const uint16_t DW_TAG_ptr_to_member_type = 0x1f;
const uint16_t DW_TAG_set_type = 0x20;
const uint16_t DW_TAG_subrange_type = 0x21;
const uint16_t DW_TAG_with_stmt = 0x22;
const uint16_t DW_TAG_access_declaration = 0x23;
const uint16_t DW_TAG_base_type = 0x24;
const uint16_t DW_TAG_catch_block = 0x25;
const uint16_t DW_TAG_const_type = 0x26;
const uint16_t DW_TAG_constant = 0x27;
const uint16_t DW_TAG_enumerator = 0x28;
const uint16_t DW_TAG_file_type = 0x29;
const uint16_t DW_TAG_friend = 0x2a;
const uint16_t DW_TAG_namelist = 0x2b;
const uint16_t DW_TAG_namelist_item = 0x2c;
const uint16_t DW_TAG_packed_type = 0x2d;
const uint16_t DW_TAG_subprogram = 0x2e;
const uint16_t DW_TAG_template_type_param = 0x2f;
const uint16_t DW_TAG_template_value_param = 0x30;
const uint16_t DW_TAG_thrown_type = 0x31;
const uint16_t DW_TAG_try_block = 0x32;
const uint16_t DW_TAG_variant_part = 0x33;
const uint16_t DW_TAG_variable = 0x34;
const uint16_t DW_TAG_volatile_type = 0x35;
const uint16_t DW_TAG_lo_user = 0x4080;
const uint16_t DW_TAG_hi_user = 0xffff;

// Attribute name encoding
const uint16_t DW_AT_sibling              = 0x01; // DWARF2
const uint16_t DW_AT_location             = 0x02; // DWARF2
const uint16_t DW_AT_name                 = 0x03; // DWARF2
const uint16_t DW_AT_ordering             = 0x09; // DWARF2
const uint16_t DW_AT_byte_size            = 0x0b; // DWARF2
const uint16_t DW_AT_bit_offset           = 0x0c; // DWARF2
const uint16_t DW_AT_bit_size             = 0x0d; // DWARF2
const uint16_t DW_AT_stmt_list            = 0x10; // DWARF2
const uint16_t DW_AT_low_pc               = 0x11; // DWARF2
const uint16_t DW_AT_high_pc              = 0x12; // DWARF2
const uint16_t DW_AT_language             = 0x13; // DWARF2
const uint16_t DW_AT_discr                = 0x15; // DWARF2
const uint16_t DW_AT_discr_value          = 0x16; // DWARF2
const uint16_t DW_AT_visibility           = 0x17; // DWARF2
const uint16_t DW_AT_import               = 0x18; // DWARF2
const uint16_t DW_AT_string_length        = 0x19; // DWARF2
const uint16_t DW_AT_common_reference     = 0x1a; // DWARF2
const uint16_t DW_AT_comp_dir             = 0x1b; // DWARF2
const uint16_t DW_AT_const_value          = 0x1c; // DWARF2
const uint16_t DW_AT_containing_type      = 0x1d; // DWARF2
const uint16_t DW_AT_default_value        = 0x1e; // DWARF2
const uint16_t DW_AT_inline               = 0x20; // DWARF2
const uint16_t DW_AT_is_optional          = 0x21; // DWARF2
const uint16_t DW_AT_lower_bound          = 0x22; // DWARF2
const uint16_t DW_AT_producer             = 0x25; // DWARF2
const uint16_t DW_AT_prototyped           = 0x27; // DWARF2
const uint16_t DW_AT_return_addr          = 0x2a; // DWARF2
const uint16_t DW_AT_start_scope          = 0x2c; // DWARF2
const uint16_t DW_AT_stride_size          = 0x2e; // DWARF2
const uint16_t DW_AT_upper_bound          = 0x2f; // DWARF2
const uint16_t DW_AT_abstract_origin      = 0x31; // DWARF2
const uint16_t DW_AT_accessibility        = 0x32; // DWARF2
const uint16_t DW_AT_address_class        = 0x33; // DWARF2
const uint16_t DW_AT_artificial           = 0x34; // DWARF2
const uint16_t DW_AT_base_types           = 0x35; // DWARF2
const uint16_t DW_AT_calling_convention   = 0x36; // DWARF2
const uint16_t DW_AT_count                = 0x37; // DWARF2
const uint16_t DW_AT_data_member_location = 0x38; // DWARF2
const uint16_t DW_AT_decl_column          = 0x39; // DWARF2
const uint16_t DW_AT_decl_file            = 0x3a; // DWARF2
const uint16_t DW_AT_decl_line            = 0x3b; // DWARF2
const uint16_t DW_AT_declaration          = 0x3c; // DWARF2
const uint16_t DW_AT_discr_list           = 0x3d; // DWARF2
const uint16_t DW_AT_encoding             = 0x3e; // DWARF2
const uint16_t DW_AT_external             = 0x3f; // DWARF2
const uint16_t DW_AT_frame_base           = 0x40; // DWARF2
const uint16_t DW_AT_friend               = 0x41; // DWARF2
const uint16_t DW_AT_identifier_case      = 0x42; // DWARF2
const uint16_t DW_AT_macro_info           = 0x43; // DWARF2
const uint16_t DW_AT_namelist_item        = 0x44; // DWARF2
const uint16_t DW_AT_priority             = 0x45; // DWARF2
const uint16_t DW_AT_segment              = 0x46; // DWARF2
const uint16_t DW_AT_specification        = 0x47; // DWARF2
const uint16_t DW_AT_static_link          = 0x48; // DWARF2
const uint16_t DW_AT_type                 = 0x49; // DWARF2
const uint16_t DW_AT_use_location         = 0x4a; // DWARF2
const uint16_t DW_AT_variable_parameter   = 0x4b; // DWARF2
const uint16_t DW_AT_virtuality           = 0x4c; // DWARF2
const uint16_t DW_AT_vtable_elem_location = 0x4d; // DWARF2
const uint16_t DW_AT_allocated            = 0x4e; // DWARF3
const uint16_t DW_AT_associated           = 0x4f; // DWARF3
const uint16_t DW_AT_data_location        = 0x50; // DWARF3
const uint16_t DW_AT_byte_stride          = 0x51; // DWARF3
const uint16_t DW_AT_entry_pc             = 0x52; // DWARF3
const uint16_t DW_AT_use_UTF8             = 0x53; // DWARF3
const uint16_t DW_AT_extension            = 0x54; // DWARF3
const uint16_t DW_AT_ranges               = 0x55; // DWARF3
const uint16_t DW_AT_trampoline           = 0x56; // DWARF3
const uint16_t DW_AT_call_column          = 0x57; // DWARF3
const uint16_t DW_AT_call_file            = 0x58; // DWARF3
const uint16_t DW_AT_call_line            = 0x59; // DWARF3
const uint16_t DW_AT_description          = 0x5a; // DWARF3
const uint16_t DW_AT_binary_scale         = 0x5b; // DWARF3
const uint16_t DW_AT_decimal_scale        = 0x5c; // DWARF3
const uint16_t DW_AT_small                = 0x5d; // DWARF3
const uint16_t DW_AT_decimal_sign         = 0x5e; // DWARF3
const uint16_t DW_AT_digit_count          = 0x5f; // DWARF3
const uint16_t DW_AT_picture_string       = 0x60; // DWARF3
const uint16_t DW_AT_mutable              = 0x61; // DWARF3
const uint16_t DW_AT_threads_scaled       = 0x62; // DWARF3
const uint16_t DW_AT_explicit             = 0x63; // DWARF3
const uint16_t DW_AT_object_pointer       = 0x64; // DWARF3
const uint16_t DW_AT_endianity            = 0x65; // DWARF3
const uint16_t DW_AT_elemental            = 0x66; // DWARF3
const uint16_t DW_AT_pure                 = 0x67; // DWARF3
const uint16_t DW_AT_recursive            = 0x68; // DWARF3
const uint16_t DW_AT_lo_user              = 0x2000;
const uint16_t DW_AT_hi_user              = 0x3fff;

const uint16_t DW_FORM_addr = 0x01;
const uint16_t DW_FORM_block2 = 0x03;
const uint16_t DW_FORM_block4 = 0x04;
const uint16_t DW_FORM_data2 = 0x05;
const uint16_t DW_FORM_data4 = 0x06;
const uint16_t DW_FORM_data8 = 0x07;
const uint16_t DW_FORM_string = 0x08;
const uint16_t DW_FORM_block = 0x09;
const uint16_t DW_FORM_block1 = 0x0a;
const uint16_t DW_FORM_data1 = 0x0b;
const uint16_t DW_FORM_flag = 0x0c;
const uint16_t DW_FORM_sdata = 0x0d;
const uint16_t DW_FORM_strp = 0x0e;
const uint16_t DW_FORM_udata = 0x0f;
const uint16_t DW_FORM_ref_addr = 0x10;
const uint16_t DW_FORM_ref1 = 0x11;
const uint16_t DW_FORM_ref2 = 0x12;
const uint16_t DW_FORM_ref4 = 0x13;
const uint16_t DW_FORM_ref8 = 0x14;
const uint16_t DW_FORM_ref_udata = 0x15;
const uint16_t DW_FORM_indirect = 0x16;

const uint8_t DW_OP_addr = 0x03;
const uint8_t DW_OP_deref = 0x06;
const uint8_t DW_OP_const1u = 0x08;
const uint8_t DW_OP_const1s = 0x09;
const uint8_t DW_OP_const2u = 0x0a;
const uint8_t DW_OP_const2s = 0x0b;
const uint8_t DW_OP_const4u = 0x0c;
const uint8_t DW_OP_const4s = 0x0d;
const uint8_t DW_OP_const8u = 0x0e;
const uint8_t DW_OP_const8s = 0x0f;
const uint8_t DW_OP_constu = 0x10;
const uint8_t DW_OP_consts = 0x11;
const uint8_t DW_OP_dup = 0x12;
const uint8_t DW_OP_drop = 0x13;
const uint8_t DW_OP_over = 0x14;
const uint8_t DW_OP_pick = 0x15;
const uint8_t DW_OP_swap = 0x16;
const uint8_t DW_OP_rot = 0x17;
const uint8_t DW_OP_xderef = 0x18;
const uint8_t DW_OP_abs = 0x19;
const uint8_t DW_OP_and = 0x1a;
const uint8_t DW_OP_div = 0x1b;
const uint8_t DW_OP_minus = 0x1c;
const uint8_t DW_OP_mod = 0x1d;
const uint8_t DW_OP_mul = 0x1e;
const uint8_t DW_OP_neg = 0x1f;
const uint8_t DW_OP_not = 0x20;
const uint8_t DW_OP_or = 0x21;
const uint8_t DW_OP_plus = 0x22;
const uint8_t DW_OP_plus_uconst = 0x23;
const uint8_t DW_OP_shl = 0x24;
const uint8_t DW_OP_shr = 0x25;
const uint8_t DW_OP_shra = 0x26;
const uint8_t DW_OP_xor = 0x27;
const uint8_t DW_OP_skip = 0x2f;
const uint8_t DW_OP_bra = 0x28;
const uint8_t DW_OP_eq = 0x29;
const uint8_t DW_OP_ge = 0x2a;
const uint8_t DW_OP_gt = 0x2b;
const uint8_t DW_OP_le = 0x2c;
const uint8_t DW_OP_lt = 0x2d;
const uint8_t DW_OP_ne = 0x2e;
const uint8_t DW_OP_lit0 = 0x30;
const uint8_t DW_OP_lit1 = 0x31;
const uint8_t DW_OP_lit2 = 0x32;
const uint8_t DW_OP_lit3 = 0x33;
const uint8_t DW_OP_lit4 = 0x34;
const uint8_t DW_OP_lit5 = 0x35;
const uint8_t DW_OP_lit6 = 0x36;
const uint8_t DW_OP_lit7 = 0x37;
const uint8_t DW_OP_lit8 = 0x38;
const uint8_t DW_OP_lit9 = 0x39;
const uint8_t DW_OP_lit10 = 0x3a;
const uint8_t DW_OP_lit11 = 0x3b;
const uint8_t DW_OP_lit12 = 0x3c;
const uint8_t DW_OP_lit13 = 0x3d;
const uint8_t DW_OP_lit14 = 0x3e;
const uint8_t DW_OP_lit15 = 0x3f;
const uint8_t DW_OP_lit16 = 0x40;
const uint8_t DW_OP_lit17 = 0x41;
const uint8_t DW_OP_lit18 = 0x42;
const uint8_t DW_OP_lit19 = 0x43;
const uint8_t DW_OP_lit20 = 0x44;
const uint8_t DW_OP_lit21 = 0x45;
const uint8_t DW_OP_lit22 = 0x46;
const uint8_t DW_OP_lit23 = 0x47;
const uint8_t DW_OP_lit24 = 0x48;
const uint8_t DW_OP_lit25 = 0x49;
const uint8_t DW_OP_lit26 = 0x4a;
const uint8_t DW_OP_lit27 = 0x4b;
const uint8_t DW_OP_lit28 = 0x4c;
const uint8_t DW_OP_lit29 = 0x4d;
const uint8_t DW_OP_lit30 = 0x4e;
const uint8_t DW_OP_lit31 = 0x4f;
const uint8_t DW_OP_reg0 = 0x50;
const uint8_t DW_OP_reg1 = 0x51;
const uint8_t DW_OP_reg2 = 0x52;
const uint8_t DW_OP_reg3 = 0x53;
const uint8_t DW_OP_reg4 = 0x54;
const uint8_t DW_OP_reg5 = 0x55;
const uint8_t DW_OP_reg6 = 0x56;
const uint8_t DW_OP_reg7 = 0x57;
const uint8_t DW_OP_reg8 = 0x58;
const uint8_t DW_OP_reg9 = 0x59;
const uint8_t DW_OP_reg10 = 0x5a;
const uint8_t DW_OP_reg11 = 0x5b;
const uint8_t DW_OP_reg12 = 0x5c;
const uint8_t DW_OP_reg13 = 0x5d;
const uint8_t DW_OP_reg14 = 0x5e;
const uint8_t DW_OP_reg15 = 0x5f;
const uint8_t DW_OP_reg16 = 0x60;
const uint8_t DW_OP_reg17 = 0x61;
const uint8_t DW_OP_reg18 = 0x62;
const uint8_t DW_OP_reg19 = 0x63;
const uint8_t DW_OP_reg20 = 0x64;
const uint8_t DW_OP_reg21 = 0x65;
const uint8_t DW_OP_reg22 = 0x66;
const uint8_t DW_OP_reg23 = 0x67;
const uint8_t DW_OP_reg24 = 0x68;
const uint8_t DW_OP_reg25 = 0x69;
const uint8_t DW_OP_reg26 = 0x6a;
const uint8_t DW_OP_reg27 = 0x6b;
const uint8_t DW_OP_reg28 = 0x6c;
const uint8_t DW_OP_reg29 = 0x6d;
const uint8_t DW_OP_reg30 = 0x6e;
const uint8_t DW_OP_reg31 = 0x6f;
const uint8_t DW_OP_breg0 = 0x70;
const uint8_t DW_OP_breg1 = 0x71;
const uint8_t DW_OP_breg2 = 0x72;
const uint8_t DW_OP_breg3 = 0x73;
const uint8_t DW_OP_breg4 = 0x74;
const uint8_t DW_OP_breg5 = 0x75;
const uint8_t DW_OP_breg6 = 0x76;
const uint8_t DW_OP_breg7 = 0x77;
const uint8_t DW_OP_breg8 = 0x78;
const uint8_t DW_OP_breg9 = 0x79;
const uint8_t DW_OP_breg10 = 0x7a;
const uint8_t DW_OP_breg11 = 0x7b;
const uint8_t DW_OP_breg12 = 0x7c;
const uint8_t DW_OP_breg13 = 0x7d;
const uint8_t DW_OP_breg14 = 0x7e;
const uint8_t DW_OP_breg15 = 0x7f;
const uint8_t DW_OP_breg16 = 0x80;
const uint8_t DW_OP_breg17 = 0x81;
const uint8_t DW_OP_breg18 = 0x82;
const uint8_t DW_OP_breg19 = 0x83;
const uint8_t DW_OP_breg20 = 0x84;
const uint8_t DW_OP_breg21 = 0x85;
const uint8_t DW_OP_breg22 = 0x86;
const uint8_t DW_OP_breg23 = 0x87;
const uint8_t DW_OP_breg24 = 0x88;
const uint8_t DW_OP_breg25 = 0x89;
const uint8_t DW_OP_breg26 = 0x8a;
const uint8_t DW_OP_breg27 = 0x8b;
const uint8_t DW_OP_breg28 = 0x8c;
const uint8_t DW_OP_breg29 = 0x8d;
const uint8_t DW_OP_breg30 = 0x8e;
const uint8_t DW_OP_breg31 = 0x8f;
const uint8_t DW_OP_regx = 0x90;
const uint8_t DW_OP_fbreg = 0x91;
const uint8_t DW_OP_bregx = 0x92;
const uint8_t DW_OP_piece = 0x93;
const uint8_t DW_OP_deref_size = 0x94;
const uint8_t DW_OP_xderef_size = 0x95;
const uint8_t DW_OP_nop = 0x96;
const uint8_t DW_OP_push_object_address = 0x97;
const uint8_t DW_OP_call2 = 0x98;
const uint8_t DW_OP_call4 = 0x99;
const uint8_t DW_OP_call_ref = 0x9a;
const uint8_t DW_OP_form_tls_address = 0x9b;
const uint8_t DW_OP_call_frame_cfa = 0x9c;
const uint8_t DW_OP_bit_piece = 0x9d;
const uint8_t DW_OP_lo_user = 0xe0;
const uint8_t DW_OP_hi_user = 0xff;

const uint8_t DW_ATE_address = 0x01;          // DWARF v2
const uint8_t DW_ATE_boolean = 0x02;          // DWARF v2
const uint8_t DW_ATE_complex_float = 0x03;    // DWARF v2
const uint8_t DW_ATE_float = 0x04;            // DWARF v2
const uint8_t DW_ATE_signed = 0x05;           // DWARF v2
const uint8_t DW_ATE_signed_char = 0x06;      // DWARF v2
const uint8_t DW_ATE_unsigned = 0x07;         // DWARF v2
const uint8_t DW_ATE_unsigned_char = 0x08;    // DWARF v2
const uint8_t DW_ATE_imaginary_float = 0x09;  // DWARF v3
const uint8_t DW_ATE_packed_decimal = 0x0a;   // DWARF v3
const uint8_t DW_ATE_numeric_string = 0x0b;   // DWARF v3
const uint8_t DW_ATE_edited = 0x0c;           // DWARF v3
const uint8_t DW_ATE_signed_fixed = 0x0d;     // DWARF v3
const uint8_t DW_ATE_unsigned_fixed = 0x0e;   // DWARF v3
const uint8_t DW_ATE_decimal_float = 0x0f;    // DWARF v3
const uint8_t DW_ATE_lo_user = 0x80;
const uint8_t DW_ATE_hi_user = 0xff;

const uint8_t DW_DS_unsigned = 0x01;           // DWARF v3
const uint8_t DW_DS_leading_overpunch = 0x02;  // DWARF v3
const uint8_t DW_DS_trailing_overpunch = 0x03; // DWARF v3
const uint8_t DW_DS_leading_separate = 0x04;   // DWARF v3
const uint8_t DW_DS_trailing_separate = 0x05;  // DWARF v3

const uint8_t DW_END_default = 0x00;
const uint8_t DW_END_big = 0x01;
const uint8_t DW_END_little = 0x02;
const uint8_t DW_END_lo_user = 0x40;
const uint8_t DW_END_hi_user = 0xff;

const uint8_t DW_ACCESS_public = 0x01;
const uint8_t DW_ACCESS_protected = 0x02;
const uint8_t DW_ACCESS_private = 0x03;

const uint8_t DW_VIS_local = 0x01;
const uint8_t DW_VIS_exported = 0x02;
const uint8_t DW_VIS_qualified = 0x03;

const uint8_t DW_VIRTUALITY_none = 0x00;
const uint8_t DW_VIRTUALITY_virtual = 0x01;
const uint8_t DW_VIRTUALITY_pure_virtual = 0x02;

const uint16_t DW_LANG_C89 = 0x0001;
const uint16_t DW_LANG_C = 0x0002;
const uint16_t DW_LANG_Ada83 = 0x0003;
const uint16_t DW_LANG_C_plus_plus = 0x0004;
const uint16_t DW_LANG_Cobol74 = 0x0005;
const uint16_t DW_LANG_Cobol85 = 0x0006;
const uint16_t DW_LANG_Fortran77 = 0x0007;
const uint16_t DW_LANG_Fortran90 = 0x0008;
const uint16_t DW_LANG_Pascal83 = 0x0009;
const uint16_t DW_LANG_Modula2 = 0x000a;
const uint16_t DW_LANG_Java = 0x000b;
const uint16_t DW_LANG_C99 = 0x000c;
const uint16_t DW_LANG_Ada95 = 0x000d;
const uint16_t DW_LANG_Fortran95 = 0x000e;
const uint16_t DW_LANG_PLI = 0x000f;
const uint16_t DW_LANG_ObjC = 0x0010;
const uint16_t DW_LANG_ObjC_plus_plus = 0x0011;
const uint16_t DW_LANG_UPC = 0x0012;
const uint16_t DW_LANG_D = 0x0013;
const uint16_t DW_LANG_lo_user = 0x8000;
const uint16_t DW_LANG_hi_user = 0xffff;
const uint16_t DW_LANG_Mips_Assembler = 0x8001;
const uint16_t DW_LANG_Upc = 0x8765;

const uint8_t DW_ID_case_sensitive = 0x00;
const uint8_t DW_ID_up_case = 0x01;
const uint8_t DW_ID_down_case = 0x02;
const uint8_t DW_ID_case_insensitive = 0x03;

const uint8_t DW_CC_normal = 0x01;
const uint8_t DW_CC_program = 0x02;
const uint8_t DW_CC_nocall = 0x03;
const uint8_t DW_CC_lo_user = 0x40;
const uint8_t DW_CC_hi_user = 0xff;

const uint8_t DW_INL_not_inlined = 0x00;
const uint8_t DW_INL_inlined = 0x01;
const uint8_t DW_INL_declared_not_inlined = 0x02;
const uint8_t DW_INL_declared_inlined = 0x03;

const uint8_t DW_ORD_row_major = 0x00;
const uint8_t DW_ORD_col_major = 0x01;

const uint8_t DW_DSC_label = 0x00;
const uint8_t DW_DSC_range = 0x01;

const uint8_t DW_LNS_copy = 0x01;               // DWARF v2
const uint8_t DW_LNS_advance_pc = 0x02;         // DWARF v2
const uint8_t DW_LNS_advance_line = 0x03;       // DWARF v2
const uint8_t DW_LNS_set_file = 0x04;           // DWARF v2
const uint8_t DW_LNS_set_column = 0x05;         // DWARF v2
const uint8_t DW_LNS_negate_stmt = 0x06;        // DWARF v2
const uint8_t DW_LNS_set_basic_block = 0x07;    // DWARF v2
const uint8_t DW_LNS_const_add_pc = 0x08;       // DWARF v2
const uint8_t DW_LNS_fixed_advance_pc = 0x09;   // DWARF v2
const uint8_t DW_LNS_set_prologue_end = 0x0a;   // DWARF v3
const uint8_t DW_LNS_set_epilogue_begin = 0x0b; // DWARF v3
const uint8_t DW_LNS_set_isa = 0x0c;            // DWARF v3

const uint8_t DW_LNE_end_sequence = 0x01;
const uint8_t DW_LNE_set_address = 0x02;
const uint8_t DW_LNE_define_file = 0x03;

const uint8_t DW_MACINFO_define = 0x01;
const uint8_t DW_MACINFO_undef = 0x02;
const uint8_t DW_MACINFO_start_file = 0x03;
const uint8_t DW_MACINFO_end_file = 0x04;
const uint8_t DW_MACINFO_vendor_ext = 0xff;

const uint8_t DW_CFA_advance_loc = (0x1 << 6);
const uint8_t DW_CFA_offset = (0x2 << 6);
const uint8_t DW_CFA_restore = (0x3 << 6);
const uint8_t DW_CFA_nop = 0x00;
const uint8_t DW_CFA_set_loc = 0x01;
const uint8_t DW_CFA_advance_loc1 = 0x02;
const uint8_t DW_CFA_advance_loc2 = 0x03;
const uint8_t DW_CFA_advance_loc4 = 0x04;
const uint8_t DW_CFA_offset_extended = 0x05;
const uint8_t DW_CFA_restore_extended = 0x06;
const uint8_t DW_CFA_undefined = 0x07;
const uint8_t DW_CFA_same_value = 0x08;
const uint8_t DW_CFA_register = 0x09;
const uint8_t DW_CFA_remember_state = 0xa;
const uint8_t DW_CFA_restore_state = 0x0b;
const uint8_t DW_CFA_def_cfa = 0x0c;
const uint8_t DW_CFA_def_cfa_register = 0x0d;
const uint8_t DW_CFA_def_cfa_offset = 0x0e;
const uint8_t DW_CFA_def_cfa_expression = 0x0f;
const uint8_t DW_CFA_expression = 0x10;
const uint8_t DW_CFA_offset_extended_sf = 0x11;
const uint8_t DW_CFA_def_cfa_sf = 0x12;
const uint8_t DW_CFA_def_cfa_offset_sf = 0x13;
const uint8_t DW_CFA_val_offset = 0x14;
const uint8_t DW_CFA_val_offset_sf = 0x15;
const uint8_t DW_CFA_val_expression = 0x16;
const uint8_t DW_CFA_lo_user = 0x1c;
const uint8_t DW_CFA_hi_user = 0x3f;

const char *DWARF_GetTagName(uint16_t dw_tag);
const char *DWARF_GetATName(uint16_t dw_at);
const char *DWARF_GetFORMName(uint16_t dw_form);
const char *DWARF_GetATEName(uint8_t dw_ate);
const char *DWARF_GetLANGName(uint16_t dw_lang);
const char *DWARF_GetVIRTUALITYName(uint8_t dw_virtuality);
const char *DWARF_GetVISName(uint8_t dw_vis);
const char *DWARF_GetACCESSName(uint8_t dw_access);
const char *DWARF_GetENDName(uint8_t dw_end);
const char *DWARF_GetDSName(uint8_t dw_ds);
const char *DWARF_GetIDName(uint8_t dw_id);
const char *DWARF_GetCCName(uint8_t dw_cc);
const char *DWARF_GetINLName(uint8_t dw_inl);
const char *DWARF_GetORDName(uint8_t dw_ord);
const char *DWARF_GetDSCName(uint8_t dw_dsc);
const char *DWARF_GetCHILDRENName(uint8_t dw_children);

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif