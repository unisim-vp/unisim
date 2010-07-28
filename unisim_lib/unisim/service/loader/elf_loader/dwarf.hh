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

#ifndef __UNISIM_SERVICE_LOADER_ELF_LOADER_DWARF_H__
#define __UNISIM_SERVICE_LOADER_ELF_LOADER_DWARF_H__

#include <vector>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/stmt.hh>
#include <iosfwd>
#include <string>
#include <inttypes.h>
#include <map>
#include <list>
#include <unisim/util/debug/register.hh>

namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

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


const unsigned int DW_CLASS_ADDRESS = 1;                  // DWARF_Address (machine address)
const unsigned int DW_CLASS_BLOCK = 2;                    // DWARF_Block (raw data block)
const unsigned int DW_CLASS_EXPRESSION = 3;               // DWARF_Expression (DWARF Expression)
const unsigned int DW_CLASS_UNSIGNED_CONSTANT = 4;        // DWARF_UnsignedConstant (unsigned integer up to 64-bit)
const unsigned int DW_CLASS_SIGNED_CONSTANT = 5;          // DWARF_SignedConstant (signed integer up to 64-bit)
const unsigned int DW_CLASS_UNSIGNED_LEB128_CONSTANT = 6; // DWARF_UnsignedLEB128Constant (variable length unsigned integer)
const unsigned int DW_CLASS_SIGNED_LEB128_CONSTANT = 7;   // DWARF_SignedLEB128Constant (variable length signed integer)
const unsigned int DW_CLASS_FLAG = 8;                     // DWARF_Flag (boolean)
const unsigned int DW_CLASS_LINEPTR = 9;                  // DWARF_LinePtr (reference to a statement program)
const unsigned int DW_CLASS_LOCLISTPTR = 10;              // DWARF_LocListPtr
const unsigned int DW_CLASS_MACPTR = 11;                  // DWARF_MacPtr
const unsigned int DW_CLASS_RANGELISTPTR = 12;            // DWARF_RangeListPtr
const unsigned int DW_CLASS_REFERENCE = 13;               // DWARF_Reference (reference to a DWARF_DIE)
const unsigned int DW_CLASS_STRING = 14;                  // DWARF_String (C string)

typedef enum
{
	FMT_DWARF32, // DWARF version 2 or 3
	FMT_DWARF64  // DWARF version 3 only
} DWARF_Format;

using unisim::util::endian::endian_type;
using unisim::util::debug::Statement;

class DWARF_LEB128;
class DWARF_Filename;
template <class MEMORY_ADDR> class DWARF_StatementProgram;
template <class MEMORY_ADDR> class DWARF_StatementVM;
template <class MEMORY_ADDR> class DWARF_AttributeValue;
template <class MEMORY_ADDR> class DWARF_Address;
template <class MEMORY_ADDR> class DWARF_Block;
template <class MEMORY_ADDR> class DWARF_UnsignedConstant;
template <class MEMORY_ADDR> class DWARF_SignedConstant;
template <class MEMORY_ADDR> class DWARF_UnsignedLEB128Constant;
template <class MEMORY_ADDR> class DWARF_SignedLEB128Constant;
template <class MEMORY_ADDR> class DWARF_Flag;
template <class MEMORY_ADDR> class DWARF_LinePtr;
template <class MEMORY_ADDR> class DWARF_LocListPtr;
template <class MEMORY_ADDR> class DWARF_MacPtr;
template <class MEMORY_ADDR> class DWARF_RangeListPtr;
template <class MEMORY_ADDR> class DWARF_Reference;
template <class MEMORY_ADDR> class DWARF_String;
template <class MEMORY_ADDR> class DWARF_Expression;
template <class MEMORY_ADDR> class DWARF_Attribute;
template <class MEMORY_ADDR> class DWARF_DIE;
template <class MEMORY_ADDR> class DWARF_CompilationUnit;
class DWARF_Abbrev;
class DWARF_AbbrevAttribute;
template <class MEMORY_ADDR> class DWARF_ExpressionVM;
template <class MEMORY_ADDR> class DWARF_CallFrameProgram;
template <class MEMORY_ADDR> class DWARF_CIE;
template <class MEMORY_ADDR> class DWARF_FDE;
template <class MEMORY_ADDR> class Rule;
template <class MEMORY_ADDR> class CFARule;
template <class MEMORY_ADDR> class OffsetRule;
template <class MEMORY_ADDR> class ValOffsetRule;
template <class MEMORY_ADDR> class RegisterRule;
template <class MEMORY_ADDR> class ExpressionRule;
template <class MEMORY_ADDR> class ValExpressionRule;
template <class MEMORY_ADDR> class RuleMatrixRow;
template <class MEMORY_ADDR> class DWARF_CallFrameVM;
template <class MEMORY_ADDR> class DWARF_RangeListEntry;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntry;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryDefine;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryUndef;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryStartFile;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryEndFile;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryVendorExtension;
template <class MEMORY_ADDR> class DWARF_AddressRangeDescriptor;
template <class MEMORY_ADDR> class DWARF_AddressRanges;
template <class MEMORY_ADDR> class DWARF_Pub;
template <class MEMORY_ADDR> class DWARF_Pubs;
template <class MEMORY_ADDR> class DWARF_LocListEntry;

template <class MEMORY_ADDR> class DWARF_Handler;

class DWARF_LEB128
{
public:
	DWARF_LEB128();
	DWARF_LEB128(const DWARF_LEB128& leb128);
	~DWARF_LEB128();
	
	unsigned int GetByteSize() const;
	unsigned int GetBitLength() const;
	template <typename T> bool Fit(const T *t = 0) const;
	template <typename T> operator T() const;
	DWARF_LEB128& operator = (const uint8_t *leb128);
	DWARF_LEB128& operator = (const DWARF_LEB128& leb128);
	bool operator == (const DWARF_LEB128& leb128) const;
	
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	bool IsZero() const;
	
	std::string to_hex(bool is_signed) const;
	std::string to_string(bool is_signed) const;
private:
	const uint8_t *leb128;
};

std::ostream& operator << (std::ostream& os, const DWARF_Filename& dw_filename);

class DWARF_Filename
{
public:
	DWARF_Filename();
	DWARF_Filename(const DWARF_Filename& dw_filename);
	~DWARF_Filename();
	const char *GetFilename() const;
	const DWARF_LEB128& GetDirectoryIndex() const;
	const DWARF_LEB128& GetLastModificationTime() const;
	const DWARF_LEB128& GetByteLength() const;
	
	DWARF_Filename& operator = (const DWARF_Filename& dw_filename);
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	
	friend std::ostream& operator << (std::ostream& os, const DWARF_Filename& dw_filename);
private:
	const char *filename;
	DWARF_LEB128 directory_index;         // unsigned, directory index of the directory in which file was found
	DWARF_LEB128 last_modification_time;  // unsigned, time of last modification for the file
	DWARF_LEB128 byte_length;             // unsigned, length in bytes of the file
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog);

template <class MEMORY_ADDR>
class DWARF_StatementProgram
{
public:
	DWARF_StatementProgram(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_StatementProgram();
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog);
	uint64_t GetOffset() const;
	endian_type GetEndianness() const;
private:
	friend class DWARF_StatementVM<MEMORY_ADDR>;
	
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	
	uint64_t offset;
	
	uint64_t unit_length;                 // The size in bytes of the statement information for this compilation unit
	                                      // (not including the unit_length field itself).

	uint16_t version;                     // Version identifier for the statement information format.
	
	uint64_t header_length;               // The number of bytes following the header_length field to the beginning of the first
	                                      // byte of the statement program itself.

	uint8_t minimum_instruction_length;   // The size in bytes of the smallest target machine instruction. Statement program opcodes
	                                      // that alter the address register first multiply their operands by this value.

	uint8_t default_is_stmt;              // The initial value of the is_stmt register.
	                                      // A simple code generator that emits machine instructions in the order implied by the source
	                                      // program would set this to "true", and every entry in the matrix would represent a
	                                      // statement boundary. A pipeline scheduling code generator would set this to "false" and
	                                      // emit a specific statement program opcode for each instruction that represented a statement
	                                      // boundary.

	int8_t line_base;                     // This parameter affects the meaning of the special opcodes. See below.

	uint8_t line_range;                   // This parameter affects the meaning of the special opcodes. See below.
	
	uint8_t opcode_base;                  // The number assigned to the first special opcode.

	std::vector<uint8_t> standard_opcode_lengths;   // This array specifies the number of LEB128 operands for each of the standard opcodes. The
	                                                // first element of the array corresponds to the opcode whose value is 1, and the last element
	                                                // corresponds to the opcode whose value is opcode_base - 1. By increasing
	                                                // opcode_base, and adding elements to this array, new standard opcodes can be added,
	                                                // while allowing consumers who do not know about these new opcodes to be able to skip
	                                                // them.
	
	std::vector<const char *> include_directories; // The sequence contains an entry for each path that was searched for included source files in
	                                               // this compilation. (The paths include those directories specified explicitly by the user for
	                                               // the compiler to search and those the compiler searches without explicit direction). Each
	                                               // path entry is either a full path name or is relative to the current directory of the compilation.
	                                               // The current directory of the compilation is understood to be the first entry and is not
	                                               // explicitly represented. Each entry is a null-terminated string containing a full path name.
	                                               // The last entry is followed by a single null byte.

	std::vector<DWARF_Filename> filenames;        // The sequence contains an entry for each source file that contributed to the statement
	                                              // information for this compilation unit or is used in other contexts, such as in a declaration
	                                              // coordinate or a macro file inclusion. Each entry has a null-terminated string containing the
	                                              // file name, an unsigned LEB128 number representing the directory index of the directory in
	                                              // which the file was found, an unsigned LEB128 number representing the time of last
	                                              // modification for the file and an unsigned LEB128 number representing the length in bytes
	                                              // of the file. A compiler may choose to emit LEB128(0) for the time and length fields to
	                                              // indicate that this information is not available. The last entry is followed by a single null
	                                              // byte.

	uint32_t program_length;
	const uint8_t *program;                       // the program itself
};

template <class MEMORY_ADDR>
class DWARF_StatementVM
{
public:
	DWARF_StatementVM();
	~DWARF_StatementVM();

	bool Run(const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog, std::ostream *os, std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *> *matrix);
private:
	// Machine state
	MEMORY_ADDR address;   // The program-counter value corresponding to a machine instruction generated by the compiler.
	unsigned int file;     // An unsigned integer indicating the identity of the source file corresponding to a machine instruction.
	unsigned int line;     // An unsigned integer indicating a source line number. Lines are numbered beginning at 1. The compiler may emit the value 0 in cases where an instruction cannot be attributed to any source line.
	unsigned int column;   // An unsigned integer indicating a column number within a source line. Columns are numbered beginning at 1. The value 0 is reserved to indicate that a statement begins at the ‘‘left edge’’ of the line.
	bool is_stmt;          // A boolean indicating that the current instruction is the beginning of a statement.
	bool basic_block;      // A boolean indicating that the current instruction is the beginning of a basic block.
	bool end_sequence;     // A boolean indicating that the current address is that of the first byte after the end of a sequence of target machine instructions.
	bool prologue_end;     // A boolean indicating that the current address is one (of possibly many) where execution should be suspended for an entry breakpoint of a function.
	bool prologue_begin;   // A boolean indicating that the current address is one (of possibly many) where execution should be suspended for an exit breakpoint of a function.
	unsigned int isa;      // An unsigned integer whose value encodes the applicable instruction set architecture for the current instruction.

	std::vector<DWARF_Filename> filenames; // filenames (filenames in statement program prologue + those added by extended opcode DW_LNE_define_file)

	bool IsAbsolutePath(const char *filename) const;
	void AddRow(const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog, std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>& matrix);
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AttributeValue<MEMORY_ADDR>& dw_value);

template <class MEMORY_ADDR>
class DWARF_AttributeValue
{
public:
	DWARF_AttributeValue(unsigned int dw_class);
	virtual ~DWARF_AttributeValue();
	unsigned int GetClass() const;
	const char *GetClassName() const;
	virtual std::string to_string() const = 0;
	virtual uint64_t to_int() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_AttributeValue& dw_value);
private:
	unsigned int dw_class;
};

template <class MEMORY_ADDR>
class DWARF_Address : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Address(MEMORY_ADDR addr);
	~DWARF_Address();
	MEMORY_ADDR GetValue() const;
	virtual std::string to_string() const;
	virtual uint64_t to_int() const;
private:
	MEMORY_ADDR value;
};

template <class MEMORY_ADDR>
class DWARF_Block : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Block(uint64_t length, const uint8_t *value);
	~DWARF_Block();
	uint64_t GetLength() const;
	const uint8_t *GetValue() const;
	virtual std::string to_string() const;
private:
	uint64_t length;
	const uint8_t *value;
};

template <class MEMORY_ADDR>
class DWARF_UnsignedConstant : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_UnsignedConstant(uint64_t value);
	~DWARF_UnsignedConstant();
	uint64_t GetValue() const;
	virtual std::string to_string() const;
	virtual uint64_t to_int() const;
private:
	uint64_t value;
};

template <class MEMORY_ADDR>
class DWARF_SignedConstant : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_SignedConstant(int64_t value);
	~DWARF_SignedConstant();
	int64_t GetValue() const;
	virtual std::string to_string() const;
	virtual uint64_t to_int() const;
private:
	int64_t value;
};

template <class MEMORY_ADDR>
class DWARF_UnsignedLEB128Constant : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_UnsignedLEB128Constant(const DWARF_LEB128& leb128);
	~DWARF_UnsignedLEB128Constant();
	const DWARF_LEB128& GetValue() const;
	virtual std::string to_string() const;
	virtual uint64_t to_int() const;
private:
	DWARF_LEB128 value;
};

template <class MEMORY_ADDR>
class DWARF_SignedLEB128Constant : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_SignedLEB128Constant(const DWARF_LEB128& leb128);
	~DWARF_SignedLEB128Constant();
	const DWARF_LEB128& GetValue() const;
	virtual std::string to_string() const;
	virtual uint64_t to_int() const;
private:
	DWARF_LEB128 value;
};

template <class MEMORY_ADDR>
class DWARF_Flag : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Flag(bool value);
	~DWARF_Flag();
	bool GetValue() const;
	virtual std::string to_string() const;
	virtual uint64_t to_int() const;
private:
	bool value;
};

template <class MEMORY_ADDR>
class DWARF_LinePtr : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_LinePtr(uint64_t debug_line_offset);
	~DWARF_LinePtr();
	const DWARF_StatementProgram<MEMORY_ADDR> *GetValue() const;
	uint64_t GetDebugLineOffset() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	uint64_t debug_line_offset;
	const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog;
};

template <class MEMORY_ADDR>
class DWARF_LocListPtr : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_LocListPtr(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_loc_offset);
	~DWARF_LocListPtr();
	const DWARF_LocListEntry<MEMORY_ADDR> *GetValue() const;
	uint64_t GetDebugLocOffset() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	uint64_t debug_loc_offset;
	const DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry;
};

template <class MEMORY_ADDR>
class DWARF_MacPtr : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_MacPtr(uint64_t debug_macinfo_offset);
	~DWARF_MacPtr();
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *GetValue() const;
	uint64_t GetDebugMacInfoOffset() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	uint64_t debug_macinfo_offset;
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry;
};

template <class MEMORY_ADDR>
class DWARF_RangeListPtr : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_RangeListPtr(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_ranges_offset);
	~DWARF_RangeListPtr();
	uint64_t GetDebugRangesOffset() const;
	const DWARF_RangeListEntry<MEMORY_ADDR> *GetValue() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	uint64_t debug_ranges_offset;
	const DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry;
};

template <class MEMORY_ADDR>
class DWARF_Reference : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Reference(uint64_t debug_info_offset);
	~DWARF_Reference();
	const DWARF_DIE<MEMORY_ADDR> *GetValue() const;
	uint64_t GetDebugInfoOffset() const;
	virtual std::string to_string() const;
	virtual void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
private:
	uint64_t debug_info_offset;
	const DWARF_DIE<MEMORY_ADDR> *dw_die; // Available once resolved
};

template <class MEMORY_ADDR>
class DWARF_String : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_String(uint64_t debug_strp_offset);
	DWARF_String(const char *value);
	~DWARF_String();
	unsigned int GetLength() const;
	const char *GetValue() const;
	virtual std::string to_string() const;
private:
	uint64_t debug_strp_offset;
	const char *value;
};

template <class MEMORY_ADDR>
class DWARF_Expression : public DWARF_AttributeValue<MEMORY_ADDR>
{
public:
	DWARF_Expression(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t length, const uint8_t *value);
	DWARF_Expression(const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp, uint64_t length, const uint8_t *value);
	~DWARF_Expression();
	uint64_t GetLength() const;
	const uint8_t *GetValue() const;
	virtual std::string to_string() const;
private:
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp;
	uint64_t length;
	const uint8_t *value;
};


template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Attribute<MEMORY_ADDR>& dw_attribute);

template <class MEMORY_ADDR>
class DWARF_Attribute
{
public:
	DWARF_Attribute(const DWARF_DIE<MEMORY_ADDR> *dw_die, DWARF_AbbrevAttribute *dw_abbrev_attribute, DWARF_AttributeValue<MEMORY_ADDR> *dw_value);
	~DWARF_Attribute();
	const DWARF_AbbrevAttribute *GetAbbrevAttribute() const;
	const DWARF_AttributeValue<MEMORY_ADDR> *GetValue() const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_Attribute& dw_attribute);
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	std::ostream& to_XML(std::ostream& os);
private:
	const DWARF_DIE<MEMORY_ADDR> *dw_die;
	DWARF_AbbrevAttribute *dw_abbrev_attribute;
	DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_DIE<MEMORY_ADDR>& dw_die);

template <class MEMORY_ADDR>
class DWARF_DIE // Debug Info Entry
{
public:
	DWARF_DIE(DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, DWARF_DIE<MEMORY_ADDR> *dw_parent_die = 0);
	virtual ~DWARF_DIE();
	const DWARF_CompilationUnit<MEMORY_ADDR> *GetCompilationUnit() const;
	const DWARF_DIE<MEMORY_ADDR> *GetRootDIE() const;
	const DWARF_DIE<MEMORY_ADDR> *GetParentDIE() const;
	const DWARF_Abbrev *GetAbbrev() const;
	const std::vector<DWARF_DIE<MEMORY_ADDR> *>& GetChildren() const;
	const std::vector<DWARF_Attribute<MEMORY_ADDR> *>& GetAttributes() const;
	const DWARF_Attribute<MEMORY_ADDR> *GetAttribute(uint16_t dw_at) const;
	uint16_t GetTag() const;
	uint64_t GetOffset() const;
	void Add(DWARF_Attribute<MEMORY_ADDR> *dw_attribute);
	void Add(DWARF_DIE<MEMORY_ADDR> *child);
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	bool IsNull() const;
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	std::ostream& to_XML(std::ostream& os);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_DIE<MEMORY_ADDR>& dw_die);
private:
	DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	DWARF_DIE<MEMORY_ADDR> *dw_parent_die;
	uint64_t offset;
	const DWARF_Abbrev *abbrev;
	std::vector<DWARF_DIE<MEMORY_ADDR> *> children;
	std::vector<DWARF_Attribute<MEMORY_ADDR> *> attributes;
};


template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CompilationUnit<MEMORY_ADDR>& dw_cu);

template <class MEMORY_ADDR>
class DWARF_CompilationUnit
{
public:
	DWARF_CompilationUnit(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_CompilationUnit();

	const DWARF_Abbrev *FindAbbrev(const DWARF_LEB128& dw_abbrev_code) const;
	DWARF_Handler<MEMORY_ADDR> *GetHandler() const;
	const char *GetString(uint64_t debug_str_offset) const;
	endian_type GetEndianness() const;
	DWARF_Format GetFormat() const;
	uint8_t GetAddressSize() const;
	uint16_t GetVersion() const;
	uint8_t GetOffsetSize() const;
	uint64_t GetOffset() const;
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	void Register(DWARF_DIE<MEMORY_ADDR> *dw_die);
	std::ostream& to_XML(std::ostream& os);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_CompilationUnit& dw_cu);
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_Format dw_fmt;
	
	uint64_t offset;
	
	uint64_t unit_length;                 // The length of the .debug_info contribution for that compilation unit
	                                      // (not including the unit_length field itself).

	uint16_t version;                     // Version identifier for the DWARF information.
	
	uint64_t debug_abbrev_offset;         // Offset into the .debug_abbrev section. This offset associates the
	                                      // compilation unit with a particular set of debugging information entry abbreviations.
	
	uint8_t address_size;                 // unsigned integer representing the size in bytes of an address on the target
	                                      // architecture. If the system uses segmented addressing, this value represents the size of the
	                                      // offset portion of an address.
	
	std::vector<DWARF_DIE<MEMORY_ADDR> *> debug_info_entries;
};

std::ostream& operator << (std::ostream& os, const DWARF_Abbrev& dw_abbrev);

class DWARF_Abbrev
{
public:
	DWARF_Abbrev(endian_type endianness);
	~DWARF_Abbrev();
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	friend std::ostream& operator << (std::ostream& os, const DWARF_Abbrev& dw_abbrev);
	bool IsNull() const;
	const DWARF_LEB128& GetAbbrevCode() const;
	const DWARF_LEB128& GetTag() const;
	uint64_t GetOffset() const;
	const std::vector<DWARF_AbbrevAttribute *>& GetAbbrevAttributes() const;
	bool HasChildren() const;
private:
	endian_type endianness;
	uint64_t offset;
	DWARF_LEB128 abbrev_code;
	DWARF_LEB128 dw_tag;
	uint8_t dw_children;
	std::vector<DWARF_AbbrevAttribute *> dw_abbrev_attributes;
};


std::ostream& operator << (std::ostream& os, const DWARF_AbbrevAttribute& dw_abbrev_attribute);

class DWARF_AbbrevAttribute
{
public:
	DWARF_AbbrevAttribute();
	~DWARF_AbbrevAttribute();
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	bool IsNull() const;
	const char *GetName() const;
	const DWARF_LEB128& GetTag() const;
	const DWARF_LEB128& GetForm() const;
	friend std::ostream& operator << (std::ostream& os, const DWARF_AbbrevAttribute& dw_abbrev_attribute);
private:
	DWARF_LEB128 dw_at;
	DWARF_LEB128 dw_form;
};

const unsigned int DW_LOC_REGISTER = 1;
const unsigned int DW_LOC_MEMORY = 2;

template <class MEMORY_ADDR>
class DWARF_LocationPiece
{
public:
	DWARF_LocationPiece(unsigned int dw_loc_type);
	~DWARF_LocationPiece();
	unsigned int GetType() const;
private:
	unsigned int dw_loc_type;
};

template <class MEMORY_ADDR>
class DWARF_MemoryLocationPiece : public DWARF_LocationPiece<MEMORY_ADDR>
{
public:
	DWARF_MemoryLocationPiece(MEMORY_ADDR dw_addr, unsigned int dw_bit_offset, unsigned int dw_bit_size);
	~DWARF_MemoryLocationPiece();
	MEMORY_ADDR GetAddress() const;
	unsigned int GetBitOffset() const;
	unsigned int GetBitSize() const;
private:
	MEMORY_ADDR dw_addr;
	unsigned int dw_bit_offset;
	unsigned int dw_bit_size;
};

template <class MEMORY_ADDR>
class DWARF_RegisterLocationPiece : public DWARF_LocationPiece<MEMORY_ADDR>
{
public:
	DWARF_RegisterLocationPiece(unisim::util::debug::Register *dw_reg, unsigned int dw_bit_offset, unsigned int dw_bit_size);
	~DWARF_RegisterLocationPiece();
	unsigned int GetRegisterNumber() const;
	unsigned int GetBitOffset() const;
	unsigned int GetBitSize() const;
private:
	unisim::util::debug::Register *dw_reg;
	unsigned int dw_bit_offset;
	unsigned int dw_bit_size;
};

template <class MEMORY_ADDR>
class DWARF_Location
{
public:
	DWARF_Location();
	~DWARF_Location();
	void Add(DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece);
	const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& GetLocationPieces() const;
private:
	std::vector<DWARF_LocationPiece<MEMORY_ADDR> *> dw_location_pieces;
};

template <class MEMORY_ADDR>
class DWARF_ExpressionVM
{
public:
	DWARF_ExpressionVM(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu);
	DWARF_ExpressionVM(const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp);
	~DWARF_ExpressionVM();
	
	bool Disasm(std::ostream& os, const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	bool Execute(const DWARF_Expression<MEMORY_ADDR> *dw_expr, DWARF_Location<MEMORY_ADDR> *dw_location);
private:
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp;
	unisim::util::debug::Register *registers[32];

	bool Run(const DWARF_Expression<MEMORY_ADDR> *dw_expr, std::ostream *os, DWARF_Location<MEMORY_ADDR> *dw_location);

};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog);

template <class MEMORY_ADDR>
class DWARF_CallFrameProgram
{
public:
	DWARF_CallFrameProgram(DWARF_Handler<MEMORY_ADDR> *dw_handler, uint64_t length, const uint8_t *program);
	~DWARF_CallFrameProgram();
	endian_type GetEndianness() const;
	uint8_t GetAddressSize() const;
	uint64_t GetLength() const;
	const uint8_t *GetProgram() const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog);
private:
	friend class DWARF_CallFrameVM<MEMORY_ADDR>;

	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	uint64_t length;
	const uint8_t *program;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CIE<MEMORY_ADDR>& dw_cie);

template <class MEMORY_ADDR>
class DWARF_CIE
{
public:
	DWARF_CIE(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_CIE();
	
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_CIE<MEMORY_ADDR>& dw_cie);
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_Format dw_fmt;
	uint64_t offset;
	
	uint64_t length;          // length not including field 'length'
	
	uint64_t cie_id;          // 32-bit all 1's for 32-bit DWARF, 64-bit all 1's for 64-bit DWARF
	
	uint8_t version;          // Independent of DWARF version number. DWARF v2/version=1; DWARF v3/version=3
	
	const char *augmentation; // UTF-8 string
	
	DWARF_LEB128 code_alignment_factor; // unsigned
	
	DWARF_LEB128 data_alignment_factor; // signed
	
	uint8_t dw2_return_address_register;
	DWARF_LEB128 dw3_return_address_register; // unsigned
	
	DWARF_CallFrameProgram<MEMORY_ADDR> *dw_initial_call_frame_prog;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_FDE<MEMORY_ADDR>& dw_fde);

template <class MEMORY_ADDR>
class DWARF_FDE
{
public:
	DWARF_FDE(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_FDE();
	
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_FDE<MEMORY_ADDR>& dw_fde);
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_Format dw_fmt;
	uint64_t offset;

	uint64_t length;               // length not including field 'length'
	
	uint64_t cie_pointer;          // A constant offset into the .debug_frame section that denotes the CIE that is associated with this FDE.

	MEMORY_ADDR initial_location;  // An addressing-unit sized constant indicating the address of the first location associated with this table entry.

	MEMORY_ADDR address_range;     // An addressing unit sized constant indicating the number of bytes of program instructions described by this entry.

	DWARF_CallFrameProgram<MEMORY_ADDR> *dw_call_frame_prog;
};


const unsigned int DW_RULE_CFA = 0;
const unsigned int DW_RULE_UNDEFINED = 1;
const unsigned int DW_RULE_SAME_VALUE = 2;
const unsigned int DW_RULE_OFFSET = 3;
const unsigned int DW_RULE_VAL_OFFSET = 4;
const unsigned int DW_RULE_REGISTER = 5;
const unsigned int DW_RULE_EXPRESSION = 6;
const unsigned int DW_RULE_VAL_EXPRESSION = 7;
const unsigned int DW_RULE_ARCHITECTURAL = 8;

template <class MEMORY_ADDR>
class Rule
{
public:
	Rule(unsigned int dw_rule_type);
	virtual ~Rule();
	unsigned int GetType() const;
private:
	unsigned int dw_rule_type;
};

template <class MEMORY_ADDR>
class CFARule : public Rule<MEMORY_ADDR>
{
public:
	CFARule(const DWARF_CIE<MEMORY_ADDR> *dw_cie);
	~CFARule();
	void SetRegister(const DWARF_LEB128& dw_reg_num);
	void SetFactoredOffset(const DWARF_LEB128& dw_offset_sf);
	void SetOffset(const DWARF_LEB128& dw_offset);
	const DWARF_LEB128& GetRegisterNumber() const;
	int64_t GetOffset() const;
private:
	DWARF_LEB128 dw_reg_num;
	int64_t dw_offset_sf;
};

template <class MEMORY_ADDR>
class OffsetRule : public Rule<MEMORY_ADDR>
{
public:
	OffsetRule(const Rule<MEMORY_ADDR> *dw_cfa_rule, const DWARF_LEB128& dw_offset);
	virtual ~OffsetRule();
	MEMORY_ADDR GetValue() const;
private:
	const Rule<MEMORY_ADDR> *dw_cfa_rule;
	DWARF_LEB128 dw_offset; // unsigned
};

template <class MEMORY_ADDR>
class ValOffsetRule : public Rule<MEMORY_ADDR>
{
public:
	ValOffsetRule(const Rule<MEMORY_ADDR> *dw_cfa_rule, const DWARF_LEB128& dw_offset);
	virtual ~ValOffsetRule();
	MEMORY_ADDR GetValue() const;
private:
	const Rule<MEMORY_ADDR> *dw_cfa_rule;
	DWARF_LEB128 dw_offset; // unsigned
};

template <class MEMORY_ADDR>
class RegisterRule : public Rule<MEMORY_ADDR>
{
public:
	RegisterRule(const DWARF_LEB128& dw_reg_num);
	virtual ~RegisterRule();
	const DWARF_LEB128& GetValue() const;
private:
	DWARF_LEB128 dw_reg_num;
};

template <class MEMORY_ADDR>
class ExpressionRule : public Rule<MEMORY_ADDR>
{
public:
	ExpressionRule(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	virtual ~ExpressionRule();
	MEMORY_ADDR GetValue() const;
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
class ValExpressionRule : public Rule<MEMORY_ADDR>
{
public:
	ValExpressionRule(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	virtual ~ValExpressionRule();
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
class RuleMatrixRow : public Rule<MEMORY_ADDR>
{
public:
private:
	CFARule<MEMORY_ADDR> *cfa_rule;
	std::vector<RegisterRule<MEMORY_ADDR> *> reg_rules;
};

template <class MEMORY_ADDR>
class DWARF_CallFrameVM
{
public:
	DWARF_CallFrameVM();
	~DWARF_CallFrameVM();
	bool Disasm(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog);
	bool Execute(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>& rule_matrix);
private:
	bool Run(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::ostream *os, std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *> *rule_matrix);
	
};


template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RangeListEntry<MEMORY_ADDR>& dw_range_list_entry);

template <class MEMORY_ADDR>
class DWARF_RangeListEntry
{
public:
	DWARF_RangeListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu);
	~DWARF_RangeListEntry();
	bool IsBaseAddressSelection() const;
	bool IsEndOfList() const;
	MEMORY_ADDR GetBegin() const;
	MEMORY_ADDR GetEnd() const;
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	const DWARF_RangeListEntry<MEMORY_ADDR> *GetNext() const;
	uint64_t GetOffset() const;
	std::ostream& to_XML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_RangeListEntry<MEMORY_ADDR>& dw_range_list_entry);
private:
	friend class DWARF_Handler<MEMORY_ADDR>;
	
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	DWARF_RangeListEntry<MEMORY_ADDR> *next;
	uint64_t offset; // offset within .debug_range section
	MEMORY_ADDR begin;
	MEMORY_ADDR end;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_MacInfoListEntry<MEMORY_ADDR>& dw_macinfo_list_entry);

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntry
{
public:
	DWARF_MacInfoListEntry(uint8_t dw_mac_info_type);
	virtual ~DWARF_MacInfoListEntry();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset) = 0;
	uint8_t GetType() const;
	uint64_t GetOffset() const;
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *GetNext() const;
	virtual std::string to_string() const = 0;
	virtual std::ostream& to_XML(std::ostream& os) const = 0;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_MacInfoListEntry<MEMORY_ADDR>& dw_macinfo_list_entry);
protected:
	friend class DWARF_Handler<MEMORY_ADDR>;
	
	DWARF_MacInfoListEntry<MEMORY_ADDR> *next;
	uint64_t offset; // offset within .debug_macinfo section_name
	uint8_t dw_mac_info_type;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryDefine : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryDefine();
	virtual ~DWARF_MacInfoListEntryDefine();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	const DWARF_LEB128& GetLineNo() const;
	const char *GetPreprocessorSymbolName() const;
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
private:
	DWARF_LEB128 lineno;
	const char *preprocessor_symbol_name;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryUndef : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryUndef();
	virtual ~DWARF_MacInfoListEntryUndef();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	const DWARF_LEB128& GetLineNo() const;
	const char *GetPreprocessorSymbolName() const;
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
private:
	DWARF_LEB128 lineno;
	const char *preprocessor_symbol_name;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryStartFile : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryStartFile();
	virtual ~DWARF_MacInfoListEntryStartFile();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	const DWARF_LEB128& GetLineNo() const;
	const DWARF_LEB128& GetFileIndex() const;
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
private:
	DWARF_LEB128 lineno;
	DWARF_LEB128 file_idx;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryEndFile : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryEndFile();
	virtual ~DWARF_MacInfoListEntryEndFile();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryVendorExtension : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryVendorExtension();
	virtual ~DWARF_MacInfoListEntryVendorExtension();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	const DWARF_LEB128& GetConstant() const;
	const char *GetString() const;
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
private:
	DWARF_LEB128 vendor_ext_constant;
	const char *vendor_ext_string;
};

template <class MEMORY_ADDR>
class DWARF_MacInfoListEntryNull : public DWARF_MacInfoListEntry<MEMORY_ADDR>
{
public:
	DWARF_MacInfoListEntryNull();
	virtual ~DWARF_MacInfoListEntryNull();
	virtual int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	virtual std::string to_string() const;
	virtual std::ostream& to_XML(std::ostream& os) const;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AddressRangeDescriptor<MEMORY_ADDR>& dw_addr_range_desc);

template <class MEMORY_ADDR>
class DWARF_AddressRangeDescriptor
{
public:
	DWARF_AddressRangeDescriptor(const DWARF_AddressRanges<MEMORY_ADDR> *dw_aranges);
	~DWARF_AddressRangeDescriptor();
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	MEMORY_ADDR GetAddress() const;
	MEMORY_ADDR GetLength() const;
	bool IsNull() const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_AddressRangeDescriptor<MEMORY_ADDR>& dw_addr_range_desc);
private:
	const DWARF_AddressRanges<MEMORY_ADDR> *dw_aranges;
	MEMORY_ADDR addr;
	MEMORY_ADDR length;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AddressRanges<MEMORY_ADDR>& dw_aranges);

template <class MEMORY_ADDR>
class DWARF_AddressRanges
{
public:
	DWARF_AddressRanges(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_AddressRanges();
	endian_type GetEndianness() const;
	uint8_t GetSegmentSize() const;
	uint8_t GetAddressSize() const;
	const std::vector<DWARF_AddressRangeDescriptor<MEMORY_ADDR> *>& GetDescriptors() const;
	const DWARF_CompilationUnit<MEMORY_ADDR> *GetCompilationUnit() const;
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_AddressRanges<MEMORY_ADDR>& dw_aranges);
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	
	uint64_t unit_length;        // The length of the entries for that set, not including the length field itself (see Section 7.2.2).
	
	uint16_t version;            // A version number. This number is specific to the address lookup table and
	                             // is independent of the DWARF version number.
						
	uint64_t debug_info_offset;  // The offset from the beginning of the .debug_info section of the compilation unit header
	                             // referenced by the set.
						
	uint8_t address_size;        // The size of an address in bytes on the target architecture. For segmented addressing, this is
	                             // the size of the offset portion of the address.

	uint8_t segment_size;        // The size of a segment descriptor in bytes on the target architecture. If the target system uses a
	                             // flat address space, this value is 0.
	
	std::vector<DWARF_AddressRangeDescriptor<MEMORY_ADDR> *> dw_addr_range_descriptors;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pub<MEMORY_ADDR>& dw_pub);

template <class MEMORY_ADDR>
class DWARF_Pub
{
public:
	DWARF_Pub(const DWARF_Pubs<MEMORY_ADDR> *dw_pubs);
	~DWARF_Pub();
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	const DWARF_DIE<MEMORY_ADDR> *GetDIE() const;
	bool IsNull() const;
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_Pub<MEMORY_ADDR>& dw_pub);
private:
	const DWARF_Pubs<MEMORY_ADDR> *dw_pubs;
	const DWARF_DIE<MEMORY_ADDR> *dw_die;
	uint64_t debug_info_offset;
	const char *name;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pubs<MEMORY_ADDR>& dw_pubs);

template <class MEMORY_ADDR>
class DWARF_Pubs
{
public:
	DWARF_Pubs(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_Pubs();
	uint64_t GetDebugInfoOffset() const;
	endian_type GetEndianness() const;
	DWARF_Format GetFormat() const;
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_Pubs<MEMORY_ADDR>& dw_pubs);
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_Format dw_fmt;
	uint64_t offset;
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	
	uint64_t unit_length;        // The length of the entries for that set, not including the length field itself (see Section 7.2.2).

	uint16_t version;            // A version number. This number is specific to the name lookup table and is
	                             // independent of the DWARF version number.

	uint64_t debug_info_offset;  // The offset from the beginning of the .debug_info section of the compilation unit header
	                             // referenced by the set.

	uint64_t debug_info_length;  // The size in bytes of the contents of the .debug_info section generated to represent that
	                             // compilation unit.

	std::vector<DWARF_Pub<MEMORY_ADDR> *> dw_pubs;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_LocListEntry<MEMORY_ADDR>& dw_loc_list_entry);

template <class MEMORY_ADDR>
class DWARF_LocListEntry
{
public:
	DWARF_LocListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu);
	~DWARF_LocListEntry();
	MEMORY_ADDR GetBeginAddressOffset() const;
	MEMORY_ADDR GetEndAddressOffset() const;
	bool IsBaseAddressSelection() const;
	bool IsEndOfList() const;
	uint64_t GetOffset() const;
	const DWARF_LocListEntry<MEMORY_ADDR> *GetNext() const;
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	std::ostream& to_XML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_LocListEntry<MEMORY_ADDR>& dw_loc_list_entry);
private:
	friend class DWARF_Handler<MEMORY_ADDR>;
	uint64_t offset;
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	DWARF_LocListEntry<MEMORY_ADDR> *next;
	MEMORY_ADDR begin_addr_offset;
	MEMORY_ADDR end_addr_offset;
	DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
class DWARF_Handler
{
public:
	DWARF_Handler(endian_type endianness, uint8_t address_size);
	~DWARF_Handler();
	void Handle(const char *section_name, uint8_t *section, uint64_t section_size);
	void Initialize();
	void to_XML(std::ostream& os);
	
	void Register(DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog);
	void Register(DWARF_DIE<MEMORY_ADDR> *dw_die);
	void Register(DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry);
	void Register(DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry);
	void Register(DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry);
	
	const DWARF_StatementProgram<MEMORY_ADDR> *FindStatementProgram(uint64_t debug_line_offset);
	const DWARF_DIE<MEMORY_ADDR> *FindDIE(uint64_t debug_info_offset) const;
	const DWARF_RangeListEntry<MEMORY_ADDR> *FindRangeListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_ranges_offset);
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *FindMacInfoListEntry(uint64_t debug_macinfo_offset);
	const DWARF_CompilationUnit<MEMORY_ADDR> *FindCompilationUnit(uint64_t debug_info_offset) const;
	const DWARF_LocListEntry<MEMORY_ADDR> *FindLocListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_loc_offset);
	
	endian_type GetEndianness() const;
	uint8_t GetAddressSize() const;
	const DWARF_Abbrev *FindAbbrev(uint64_t debug_abbrev_offset, const DWARF_LEB128& dw_abbrev_code) const;
	const char *GetString(uint64_t debug_str_offset) const;

	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(MEMORY_ADDR addr) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const;
private:
	endian_type endianness;
	uint8_t address_size;
	
	// Raw data
	uint8_t *debug_line_section;     // .debug_line section (raw data)
	uint64_t debug_line_section_size;
	uint8_t *debug_info_section;     // .debug_info section (raw data)
	uint64_t debug_info_section_size;
	uint8_t *debug_abbrev_section;   // .debug_abbrev section (raw data)
	uint64_t debug_abbrev_section_size;
	uint8_t *debug_aranges_section;  // .debug_aranges section (raw data)
	uint64_t debug_aranges_section_size;
	uint8_t *debug_pubnames_section; // .debug_pubnames section (raw data)
	uint64_t debug_pubnames_section_size;
	uint8_t *debug_pubtypes_section; // .debug_pubtypes section (raw data)
	uint64_t debug_pubtypes_section_size;
	uint8_t *debug_macinfo_section;  // .debug_macinfo section (raw data)
	uint64_t debug_macinfo_section_size;
	uint8_t *debug_frame_section;    // .debug_frame section (raw data)
	uint64_t debug_frame_section_size;
	uint8_t *debug_str_section;      // .debug_str section (raw data)
	uint64_t debug_str_section_size;
	uint8_t *debug_loc_section;      // .debug_loc section (raw data)
	uint64_t debug_loc_section_size;
	uint8_t *debug_ranges_section;   // .debug_ranges section (raw data)
	uint64_t debug_ranges_section_size;
	
	std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *> dw_stmt_progs;   // statement programs from section .debug_line indexed by .debug_line section offset
	std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *> stmt_matrix;               // Result of running dw_stmt_progs on dw_stmt_vms
	std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *> dw_cus;           // compilation units contributions to section .debug_info indexed by .debug_info section offset
	std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *> dw_dies;                      // debug info entries in section .debug_info indexed by .debug_info section offset
	std::map<uint64_t, DWARF_Abbrev *> dw_abbrevs;                             // from section .debug_abbrev indexed by .debug_abbrev section offset
	std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *> dw_cies;                      // from section .debug_frame indexed by .debug_frame section offset
	std::vector<DWARF_FDE<MEMORY_ADDR> *> dw_fdes;                             // from section .debug_frame
	std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *> dw_range_list;     // range list entries in section .debug_ranges indexed by .debug_ranges section offset
	std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *> dw_macinfo_list; // macinfo list entries in section .debug_macinfo indexed by .debug_macinfo section offset
	std::vector<DWARF_AddressRanges<MEMORY_ADDR> *> dw_aranges;                // from section .debug_frame
	std::vector<DWARF_Pubs<MEMORY_ADDR> *> dw_pubnames;                        // from section .debug_pubnames
	std::vector<DWARF_Pubs<MEMORY_ADDR> *> dw_pubtypes;                        // from section .debug_pubtypes
	std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> * > dw_loc_list;        // location lists in section .debug_loc indexed by .debug_loc section offset
	
	void DumpStatementMatrix();
	bool IsAbsolutePath(const char *filename) const;

};

const char *DWARF_GetTagName(uint16_t dw_tag);
const char *DWARF_GetATName(uint16_t dw_at);
const char *DWARF_GetFORMName(uint16_t dw_form);
const char *DWARF_GetCLASSName(unsigned int dw_class);
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

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
