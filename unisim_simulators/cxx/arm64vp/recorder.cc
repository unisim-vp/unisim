/*
 *  Copyright (c) 2019-2024,
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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <architecture.hh>
#include <fcntl.h>
#include <sys/stat.h>

AArch64::Recorder::Operand::Value::Value()
  : valid(0)
  , values()
{
  values[0] = 0xdeadbeef;
  values[1] = 0xdeadbeef;
}

void AArch64::Recorder::Operand::Value::Output(std::ostream& sink, unsigned length)
{
  for (unsigned idx = 0; idx < length; ++idx)
  {
    if (idx) sink << ' ';
    sink << std::hex << values[idx];
  }
}

AArch64::Recorder::Operand::Operand(unsigned _type, unsigned _storage_type, unsigned _reg, unsigned length)
  : type(_type)
  , storage_type(_storage_type)
  , reg(_reg)
  , id(-1)
  , save_id(-1)
  , values(length)
{
}

void AArch64::Recorder::Operand::Begin(unsigned length)
{
  values.resize(length);
}

void AArch64::Recorder::Operand::Cancel()
{
  if (values.size()) values.pop_back();
}

void AArch64::Recorder::Operand::Merge(Operand const& o)
{
  for (Values::const_iterator itr = o.values.begin(); itr != o.values.end(); ++itr)
    values.push_back(*itr);
}

bool AArch64::Recorder::Operand::Match(unsigned _storage_type) const
{
  return (storage_type == _storage_type);
}

bool AArch64::Recorder::Operand::Match(unsigned _storage_type, unsigned _reg) const
{
  return (storage_type == _storage_type) and (((storage_type != GPR) and (storage_type != VR)) or (reg == _reg));
}

bool AArch64::Recorder::Operand::Match(Operand const& o) const
{
  return Match(o.storage_type, o.reg);
}

void AArch64::Recorder::Operand::ValueRead(uint64_t value_lo, uint64_t value_hi)
{
  Value& value = values.back();
  if (not (value.valid & 1)) value.values[0] = value_lo;
  if (not (value.valid & 2)) value.values[1] = value_hi;
  value.valid = 3;
  type |= INPUT;
}

void AArch64::Recorder::Operand::ValueRead(uint64_t _value)
{
  Value& value = values.back();
  if (not (value.valid & 1)) value.values[0] = _value;
  value.valid = 1;
  type |= INPUT;
}

void AArch64::Recorder::Operand::ValueWrite()
{
  type |= OUTPUT;
}

void AArch64::Recorder::Operand::Output(std::ostream& sink, unsigned idx)
{
  values[idx].Output(sink, storage_type == VR ? 2 : 1);
}

AArch64::Recorder::Record::Record(const std::string& _name, const std::string& _opname, const std::string& _disasm)
  : name(_name)
  , opname(_opname)
  , disasm(_disasm)
  , operands()
  , length(0)
{
}

std::string AArch64::Recorder::Record::Instruction() const
{
  std::istringstream sstr(disasm);
  std::string insn;
  sstr >> insn;
  return insn;
}

void AArch64::Recorder::Record::Begin()
{
  ++length;
  for (Operands::iterator itr = operands.begin(); itr != operands.end(); ++itr)
    itr->Begin(length);
}

void AArch64::Recorder::Record::Cancel()
{
  for (Operands::iterator itr = operands.begin(); itr != operands.end(); ++itr)
    itr->Cancel();
  --length;
}

void AArch64::Recorder::Record::Merge(Record const& o)
{
  for (Operands::iterator itr = operands.begin(); itr != operands.end(); ++itr)
    {
      Operand& operand = *itr;
      for (Operands::const_iterator itr2 = o.operands.begin(); itr2 != o.operands.end(); ++itr2)
        {
          Operand const& operand2 = *itr2;
          if (operand.Match(operand2))
            {
              operand.Merge(operand2);
            }
        }
    }
}

AArch64::Recorder::Operand& AArch64::Recorder::Record::GetOperand(unsigned storage_type, unsigned reg)
{
  for (Operands::iterator itr = operands.begin(); itr != operands.end(); ++itr)
    if (itr->Match(storage_type, reg))
      return *itr;
  operands.emplace_back(Operand(Operand::INPUT, storage_type, reg, length));
  return operands.back();
}

bool AArch64::Recorder::Record::ParseNumber(const std::string &s, std::size_t& pos, unsigned& value)
{
  int state = 0;
  value = 0;
  while (pos < s.length())
    {
      char c = s[pos];

      switch(state)
        {
          case 0:
            if ((c >= '0') and (c <= '9'))
              {
                value = c - '0';
                state = 1;
                ++pos;
                break;
              }
            return false;
          case 1:
            if ((c >= '0') and (c <= '9'))
              {
                value = (value * 10) + (c - '0');
                ++pos;
                break;
              }
            return true;
        }
    }
  return state != 0;
}

bool AArch64::Recorder::Record::ParseHexNumber(const std::string &s, std::size_t& pos, std::string& r)
{
  r.clear();
  int state = 0;
  std::size_t _pos = pos;
  while (_pos < s.length())
    {
      char c = s[_pos];

      switch(state)
        {
          case 0:
            if (c == '0')
              {
                state = 1;
                r.append(1, c);
                ++_pos;
                break;
              }
            return false;
          case 1:
            if ((c == 'x') or (c == 'X'))
              {
                state = 2;
                r.append(1, c);
                ++_pos;
                break;
              }
            return false;
          case 2:
            if (((c >= '0') and (c <= '9')) or ((c >= 'a') and (c <= 'f')) or ((c >= 'A') and (c <= 'F')))
              {
                state = 3;
                r.append(1, c);
                ++_pos;
                break;
              }
            return false;
          case 3:
            if (((c >= '0') and (c <= '9')) or ((c >= 'a') and (c <= 'f')) or ((c >= 'A') and (c <= 'F')))
              {
                r.append(1, c);
                ++_pos;
                break;
              }
            pos = _pos;
            return true;
        }
    }
  if (state == 3)
    {
      pos = _pos;
      return true;
    }
  return false;
}

void AArch64::Recorder::Record::Output(std::ostream& pattern_file, std::ostream& input_file)
{
  std::string insn(Instruction());
  if (insn.length() < 1) return;
  bool float_output = (insn[0] == 'f') and (insn.compare(0, 5, "fcvtz") != 0); // mnemonic starts with 'f' and do not start with 'fcvtz'
  bool float_input = (insn[0] == 'f') or (((insn[0] == 's') or (insn[0] == 'u')) and (insn.compare(1, 4, "cvtf") == 0)); // mnemonic starts with 'f' or starts with either scvtf or ucvtf

  int last_id = -1;
  for (Operands::iterator itr = operands.begin(); itr != operands.end(); ++itr)
    {
      Operand& operand = *itr;
      if (operand.Match(Operand::NZCV_IN, 0) and (operand.type & Operand::INPUT))
        {
          operand.id = ++last_id;
          pattern_file << "msr NZCV, %nzcv_in" << std::endl;
        }
      if (operand.Match(Operand::FPCR_IN, 0) and (operand.type & Operand::INPUT))
        {
          operand.id = ++last_id;
          pattern_file << "msr FPCR, %fpcr_in" << std::endl;
        }
      if (operand.Match(Operand::FPSR_IN, 0) and (operand.type & Operand::INPUT))
        {
          operand.id = ++last_id;
          pattern_file << "msr FPSR, %fpsr_in" << std::endl;
        }
      if (operand.Match(Operand::SP))
        {
          operand.save_id = ++last_id;
          pattern_file << "mov %tmp_gpr, sp" << std::endl;
          if (operand.type & Operand::INPUT)
            {
              operand.id = ++last_id;
              pattern_file << "mov sp, %src_" << ((operand.type & Operand::OUTPUT) ? "dst_" : "") << "nzgpr" << std::endl;
            }
        }
    }

  std::size_t pos = 0;
  while (pos < disasm.length())
    {
      std::string hex;
      if (ParseHexNumber(disasm, pos, hex))
        {
          pattern_file << hex;
          continue;
        }

      char c = disasm[pos];

      unsigned reg = 0;
      if ((c == 'x') or (c == 'w'))
        {
          std::size_t pos2 = pos + 1;
          if (ParseNumber(disasm, pos2, reg) and (reg != 31))
            {
              Operand& operand = GetOperand(Operand::GPR, reg);
              pattern_file << '%';
              ++last_id;
              if (operand.id < 0)
                {
                  operand.id = last_id;
                  if (operand.type & Operand::INPUT) pattern_file << "src_";
                  if (operand.type & Operand::OUTPUT) pattern_file << "dst_";
                  pattern_file << ((c == 'x') ? "nzgpr" : "nzgpr32");
                }
              else
                {
                  pattern_file << operand.id << '/' << c;
                }
              pos = pos2;
              continue;
            }
        }
      if ((c == 'q') or (c == 'd') or (c == 's') or (c == 'h') or (c == 'b'))
        {
          std::size_t pos2 = pos + 1;
          if (ParseNumber(disasm, pos2, reg))
            {
              Operand& operand = GetOperand(Operand::VR, reg);
              pattern_file << '%';
              ++last_id;
              if (operand.id < 0)
                {
                  operand.id = last_id;
                  if (operand.type & Operand::INPUT) pattern_file << "src_";
                  if (operand.type & Operand::OUTPUT) pattern_file << "dst_";
                  pattern_file << "vr_" << ((((operand.type & Operand::OUTPUT) and float_output) or ((operand.type & Operand::INPUT) and float_input)) ? 'f' : 'i') << c;
                }
              else
                {
                  pattern_file << operand.id << '/' << c;
                }
              pos = pos2;
              continue;
            }
        }
      if (c == 'v')
        {
          std::size_t pos2 = pos + 1;
          if (ParseNumber(disasm, pos2, reg))
            {
              Operand& operand = GetOperand(Operand::VR, reg);
              if (pos2 < disasm.length())
                {
                  char dot = disasm[pos2];
                  if (dot == '.')
                    {
                      ++pos2;
                      char element = disasm[pos2];
                      if ((element == 'b') or (element == 'h') or (element == 's') or (element == 'd'))
                        {
                          ++pos2;
                          pattern_file << '%';
                          ++last_id;
                          if (operand.id < 0)
                            {
                              operand.id = last_id;
                              if (operand.type & Operand::INPUT) pattern_file << "src_";
                              if (operand.type & Operand::OUTPUT) pattern_file << "dst_";
                              pattern_file << "vr." << element;
                            }
                          else
                            {
                              pattern_file << operand.id;
                            }
                          pos = pos2;
                          continue;
                        }
                      unsigned elements = 0;
                      if (ParseNumber(disasm, pos2, elements))
                        {
                          if (pos2 < disasm.length())
                            {
                              element = disasm[pos2];
                              if ((element == 'b') or (element == 'h') or (element == 's') or (element == 'd'))
                                {
                                  ++pos2;
                                  pattern_file << '%';
                                  ++last_id;
                                  if (operand.id < 0)
                                    {
                                      operand.id = last_id;
                                      if (operand.type & Operand::INPUT) pattern_file << "src_";
                                      if (operand.type & Operand::OUTPUT) pattern_file << "dst_";
                                      pattern_file << "vr_" << ((((operand.type & Operand::OUTPUT) and float_output) or ((operand.type & Operand::INPUT) and float_input)) ? 'f' : 'i') << elements << element;
                                    }
                                  else
                                    {
                                      pattern_file << operand.id << '/' << elements << element;
                                    }
                                  pos = pos2;
                                  continue;
                                }
                            }
                        }
                    }
                }
            }
        }
      pattern_file << c;
      ++pos;
    }
  pattern_file << std::endl;

  for (Operands::iterator itr = operands.begin(); itr != operands.end(); ++itr)
    {
      Operand& operand = *itr;
      
      if (operand.Match(Operand::NZCV_OUT, 0) and (operand.type & Operand::OUTPUT))
        {
          operand.id = ++last_id;
          pattern_file << "mrs %nzcv_out, NZCV" << std::endl;
        }
      if (operand.Match(Operand::FPSR_OUT, 0) and (operand.type & Operand::OUTPUT))
        {
          operand.id = ++last_id;
          pattern_file << std::endl << "mrs %fpsr_out, FPSR" << std::endl;
        }
      if (operand.Match(Operand::SP, 0))
        {
          if (operand.type & Operand::OUTPUT)
            {
              if (operand.id < 0)
                {
                  operand.id = ++last_id;
                  pattern_file << "mov %dst_nzgpr, sp" << std::endl;
                }
              else
                {
                  pattern_file << "mov %" << operand.id << ", sp" << std::endl;
                }
            }
          ++last_id;
          pattern_file << "mov sp, %" << operand.save_id << std::endl;
        }
    }

  typedef std::vector<Operand *> OperandsById;
  OperandsById operands_by_id(last_id + 1);
  bool has_inputs = false;
  bool has_outputs = false;

  for (Operands::iterator itr = operands.begin(); itr != operands.end(); ++itr)
    {
      Operand& operand = *itr;
      if (operand.id >= 0)
        {
          operands_by_id[operand.id] = &operand;
          if (operand.type & Operand::INPUT) has_inputs = true;
          if (operand.type & Operand::OUTPUT) has_outputs = true;
        }
    }

  if (not has_outputs)
    {
      std::cerr << "WARNING! " << disasm << " has no outputs" << std::endl;
    }

  if (has_inputs)
    {
      typedef std::set<std::string> InputLines;
      InputLines input_lines;
      for (unsigned idx = 0; idx < length; ++idx)
        {
          std::ostringstream sstr;
          bool first = true;
          for (OperandsById::iterator itr = operands_by_id.begin(); itr != operands_by_id.end(); ++itr)
            {
              if (*itr)
                {
                  Operand& operand = **itr;
                  if (operand.type & Operand::INPUT)
                    {
                      if (first) first = false; else sstr << ' ';
                      operand.Output(sstr, idx);
                    }
                }
            }
          input_lines.insert(sstr.str());
        }
      
      for (InputLines::iterator itr = input_lines.begin(); itr != input_lines.end(); ++itr)
        {
          const std::string& input_line = *itr;
          input_file << input_line << std::endl;
        }
    }
}

void AArch64::Recorder::Record::ValueRead(unsigned storage_type, unsigned reg, uint64_t value_lo, uint64_t value_hi)
{
  Operands::iterator itr;
  for (itr = operands.begin(); itr != operands.end(); ++itr)
    if (itr->Match(storage_type, reg))
      break;
  Operand& operand = (itr == operands.end()) ? (operands.emplace_back(Operand(Operand::INPUT, storage_type, reg, length)), operands.back()) : (*itr);
  operand.ValueRead(value_lo, value_hi);
}

void AArch64::Recorder::Record::ValueRead(unsigned storage_type, unsigned reg, uint64_t value)
{
  Operands::iterator itr;
  for (itr = operands.begin(); itr != operands.end(); ++itr)
    if (itr->Match(storage_type, reg))
      break;
  Operand& operand = (itr == operands.end()) ? (operands.emplace_back(Operand(Operand::INPUT, storage_type, reg, length)), operands.back()) : (*itr);
  operand.ValueRead(value);
}

void AArch64::Recorder::Record::ValueRead(unsigned storage_type, uint64_t value)
{
  Operands::iterator itr;
  for (itr = operands.begin(); itr != operands.end(); ++itr)
    if (itr->Match(storage_type, 0))
      break;
  Operand& operand = (itr == operands.end()) ? (operands.emplace_back(Operand(Operand::INPUT, storage_type, 0, length)), operands.back()) : (*itr);
  operand.ValueRead(value);
}

void AArch64::Recorder::Record::ValueWrite(unsigned storage_type, unsigned reg)
{
  Operands::iterator itr;
  for (itr = operands.begin(); itr != operands.end(); ++itr)
    if (itr->Match(storage_type, reg))
      break;
  Operand& operand = (itr == operands.end()) ? (operands.emplace_back(Operand(Operand::OUTPUT, storage_type, reg, length)), operands.back()) : (*itr);
  operand.ValueWrite();
}

void AArch64::Recorder::Record::ValueWrite(unsigned storage_type)
{
  Operands::iterator itr;
  for (itr = operands.begin(); itr != operands.end(); ++itr)
    if (itr->Match(storage_type, 0))
      break;
  Operand& operand = (itr == operands.end()) ? (operands.emplace_back(Operand(Operand::OUTPUT, storage_type, 0, length)), operands.back()) : (*itr);
  operand.ValueWrite();
}

AArch64::Recorder::Recorder(char const* name, unisim::kernel::Object* parent)
  : unisim::kernel::Object(name, parent, "Instruction and data stream recorder that generates instruction unit tests at assembly level")
  , enable(false)
  , param_enable("enable", this, enable, "Enable/Disable recording")
  , output_dir("unit_tests")
  , param_output_dir("output-dir", this, output_dir, "Output directory")
  , arch("armv8.2-a+fp16")
  , param_arch("arch", this, arch, "Architecture level for generated assembler")
  , records()
  , record(0)
  , xr_regex("^x[0-9]+$")
  , wr_regex("^w[0-9]+$")
  , vr_regex("^v[0-9]+")
  , vr_scalar_regex("^([bhsdq])[0-9]+")
  , include_regex("^tbx$|^tbl$")
  , exclude_regex("^nop$|^adrp?.*$|^ld.*$|^st.*$|^cas.*$|^swp.*$|^msr.*$|^mrs.*$|^b$|^b_cond$|^bl.*$|^br.*$|^cbn?z.*$|^tb.*$|^e?ret$|^prfm.*$|^hvc$|^svc$")
{
}

AArch64::Recorder::~Recorder()
{
  Output();
  
  for (Records::iterator itr = records.begin(); itr != records.end(); ++itr)
    {
      Record *record = (*itr).second;
      delete record;
    }
}

std::string AArch64::Recorder::RegisterAnonymizer(std::string s)
{
  std::string r = std::regex_replace(s, xr_regex, "x");
  r = std::regex_replace(r, wr_regex, "w");
  r = std::regex_replace(r, vr_regex, "v");
  r = std::regex_replace(r, vr_scalar_regex, "$1");
  return r;
}

std::string AArch64::Recorder::MakeRecordName(std::string disasm)
{
  std::string res;
  bool sep = false;
  std::string word;
  for (std::size_t pos = 0; pos < disasm.length(); ++pos)
    {
      char c = disasm[pos];
      if (((c >= 'a') and (c <= 'z')) or
          ((c >= 'A') and (c <= 'Z')) or
          ((c >= '0') and (c <= '9')) or
          (c == '-') or
          (c == '.'))
        {
          word.append(1, c);
          sep = false;
        }
      else if (c == ';')
        {
          break;
        }
      else if (not sep)
        {
          if (pos and word.length()) res += RegisterAnonymizer(word);
          word.clear();
          res.append(1, '_');
          sep = true;
        }
    }
  if (disasm.length() and word.length()) res += RegisterAnonymizer(word);
  while (res.length() && (res.back() == '_')) res.pop_back();
  return res;
}

void AArch64::Recorder::mkdir_p(const std::string& dirname, mode_t mode)
{
  std::string path;
  std::string subpath;
  std::size_t pos = 0;
  while (pos < dirname.length())
    {
      bool flush = false;
      char c = dirname[pos];
      if (++pos == dirname.length())
        flush = true;
      if (c == '/')
        flush = true;
      else
        subpath.append(1, c);

      if (flush)
        {
          path += subpath;
          mkdir(path.c_str(), mode);
          path.append(1, '/');
          subpath.clear();
        }
    }
}

void AArch64::Recorder::Begin(Operation *op)
{
  if (not enable) return;

  std::string opname = op->GetName();
  
  if (std::regex_match(opname, include_regex) or not std::regex_match(opname, exclude_regex))
  {
    DisasmState ds;
    std::ostringstream sstr;
    op->disasm( ds, sstr );
    std::string disasm = sstr.str();

    Records::iterator itr = records.find(disasm);
    record = (itr != records.end()) ? (*itr).second : (*records.emplace(disasm, new Record(MakeRecordName(disasm), opname, disasm)).first).second;
    
    record->Begin();
  }
  else
  {
    record = 0;
  }
}

void AArch64::Recorder::Cancel()
{
  if (not enable) return;

  if (record)
  {
    record->Cancel();
    record = 0;
  }
}

void AArch64::Recorder::Output()
{
  if (not records.size()) return;
  if (not output_dir.length()) return;
  
  typedef std::map<std::string, Record *> RecordsPerName;
  RecordsPerName records_per_name;
  for (Records::iterator itr = records.begin(); itr != records.end();)
  {
    Record *record = (*itr).second;
    RecordsPerName::iterator itr2 = records_per_name.find(record->name);
    if (itr2 != records_per_name.end())
    {
      Record *record2 = (*itr2).second;
      record2->Merge(*record);
      Records::iterator next_itr(itr);
      ++next_itr;
      records.erase(itr);
      delete record;
      itr = next_itr;
    }
    else
    {
      records_per_name.emplace(record->name, record);
      ++itr;
    }
  }

  std::string pattern_dir_name = output_dir + "/patterns";
  mkdir_p(pattern_dir_name.c_str(), 0700);

  for (RecordsPerName::iterator itr = records_per_name.begin(); itr != records_per_name.end(); ++itr)
  {
    Record& record = *(*itr).second;
    std::string insn = record.Instruction();
    std::string record_input_dir_name = output_dir + "/inputs/" + arch + '/' + insn + '/' + record.name;
    
    mkdir_p(record_input_dir_name.c_str(), 0700);
    std::ofstream input_file((record_input_dir_name + "/input.txt").c_str());
    std::ofstream pattern_file((pattern_dir_name + '/' + record.name + ".pat").c_str());
    
    record.Output(pattern_file, input_file);
  }

  std::ofstream testplan_file((output_dir + "/testplan.sh").c_str());
  testplan_file << "#!/bin/bash" << std::endl;
  testplan_file << "NUM_RANDOM_UNIT_TESTS_PER_INSN=0" << std::endl;
  testplan_file << "MAX_FUNCTIONS=1" << std::endl;

  testplan_file << "function gen" << std::endl;
  testplan_file << "{" << std::endl;
  testplan_file << "\tlocal ARCH=\"$1\"" << std::endl;
  testplan_file << "\tshift" << std::endl;
  testplan_file << "\tgenerate ${ARCH} ${NUM_RANDOM_UNIT_TESTS_PER_INSN} ${MAX_FUNCTIONS} \"$@\"" << std::endl;
  testplan_file << "}" << std::endl;

  testplan_file << "function testplan" << std::endl;
  testplan_file << "{" << std::endl;
  for (RecordsPerName::iterator itr = records_per_name.begin(); itr != records_per_name.end(); ++itr)
  {
    Record& record = *(*itr).second;
    std::string insn = record.Instruction();
    testplan_file << "\t# " << insn << " (" << record.opname << ")" << std::endl;
    testplan_file << "\tgen " << arch << " '" << record.name << ".pat' '" << insn << '/' << record.name << "' " << insn << std::endl;
  }
  testplan_file << "}" << std::endl;
}

void AArch64::Recorder::VRValueRead(unsigned reg, uint64_t value_lo, uint64_t value_hi)
{
  if (record) record->ValueRead(Operand::VR, reg, value_lo, value_hi);
}

void AArch64::Recorder::GPRValueRead(unsigned reg, uint64_t value)
{
  if (record) record->ValueRead(Operand::GPR, reg, value);
}

void AArch64::Recorder::SPValueRead(unsigned reg, uint64_t value)
{
  if (record) record->ValueRead(Operand::SP, value);
}

void AArch64::Recorder::NZCVValueRead(uint64_t value)
{
  if (record) record->ValueRead(Operand::NZCV_IN, value);
}

void AArch64::Recorder::FPCRValueRead(uint64_t value)
{
  if (record) record->ValueRead(Operand::FPCR_IN, value);
}

void AArch64::Recorder::FPSRValueRead(uint64_t value)
{
  if (record) record->ValueRead(Operand::FPSR_IN, value);
}

void AArch64::Recorder::GPRValueWrite(unsigned reg)
{
  if (record) record->ValueWrite(Operand::GPR, reg);
}

void AArch64::Recorder::SPValueWrite()
{
  if (record) record->ValueWrite(Operand::SP);
}

void AArch64::Recorder::VRValueWrite(unsigned reg)
{
  if (record) record->ValueWrite(Operand::VR, reg);
}

void AArch64::Recorder::NZCVValueWrite()
{
  if (record) record->ValueWrite(Operand::NZCV_OUT);
}

void AArch64::Recorder::FPSRValueWrite()
{
  if (record) record->ValueWrite(Operand::FPSR_OUT);
}

void AArch64::Recorder::report(AArch64& cpu, ReportAccess report_access, unsigned reg, bool write)
{
  static bool reentrant = false;
  if (reentrant) return;
  if (not enable) return;
  reentrant = true;

  struct RecorderTV {};

  if (report_access == report_simd_access)
    {
      VRValueRead( reg, cpu.untaint(RecorderTV(), cpu.GetVU64(reg, 0)), cpu.untaint(RecorderTV(), cpu.GetVU64(reg, 1)) );
    }
  else if (report_access == report_gsr_access)
    {
      if (reg == 31)
        SPValueRead( reg, cpu.untaint(RecorderTV(), cpu.GetGSR(reg)) );
      else
        GPRValueRead( reg, cpu.untaint(RecorderTV(), cpu.GetGSR(reg)) );
    }
  else if (report_access == report_gzr_access)
    {
      if (reg != 31)
        GPRValueRead( reg, cpu.untaint(RecorderTV(), cpu.GetGZR(reg)) );
    }
  else if (report_access == report_nzcv_access)
    {
      NZCVValueRead( uint64_t(cpu.untaint(RecorderTV(), cpu.nzcv) << 28) );
    }

  if (write)
    {
      if (report_access == report_simd_access)
        {
          VRValueWrite( reg );
        }
      else if (report_access == report_gsr_access)
        {
          if (reg == 31)
            SPValueWrite();
          else
            GPRValueWrite( reg);
        }
      else if (report_access == report_gzr_access)
        {
          if (reg != 31)
            GPRValueWrite( reg );
        }
      else if (report_access == report_nzcv_access)
        {
          NZCVValueWrite();
        }
    }
  reentrant = false;
}
