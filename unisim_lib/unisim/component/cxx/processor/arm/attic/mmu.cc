  /****************/
  /* MMU handling */
  /****************/
	
/** Check access permission and domain bits.
 *
 * @param is_read true if the access is a read
 * @param ap the access permission bits
 * @param domain the domain being used
 * 
 * @return true if correct, false otherwise
 */
bool 
CPU::
CheckAccessPermission(bool is_read,
                      uint32_t ap,
                      uint32_t domain)
{
  if ( ap == 0x00UL )
    {
      return false;
    }

  else if ( ap == 0x01UL )
    {
      uint32_t mode = cpsr.Get(M);
      if ( mode == USER_MODE )
        {
          logger << DebugError
                 << "Accessing privileged data under USER mode: "
                 << std::endl
                 << "- is_read = " << is_read << std::endl
                 << "- ap  = " << ap << std::endl
                 << "- domain = " << domain << std::endl
                 << EndDebugError;
          return false;
        }
      else
        {
          if ( verbose & 0x010 )
            logger << DebugInfo
                   << "Accessing privileged data under privileged mode."
                   << EndDebugInfo;
        }
    }

  else if ( ap == 0x02UL )
    {
      uint32_t mode = cpsr.Get(M);
      if ( mode == USER_MODE )
        {
          if ( !is_read )
			{
              logger << DebugError
                     << "Accessing privileged data under USER mode: "
                     << std::endl
                     << "- is_read = " << is_read << std::endl
                     << "- ap  = " << ap << std::endl
                     << "- domain = " << domain << std::endl
                     << EndDebugError;
              return false;
			}
          else
			{
              if ( verbose & 0x010 )
                logger << DebugInfo
                       << "Accessing privileged data under user mode"
                       << " (read-only)"
                       << EndDebugInfo;
			}
        }
      else
        {
          if ( verbose & 0x010 )
			{
              logger << DebugInfo
                     << "Accessing privileged data under privileged mode."
                     << EndDebugInfo;
			}
        }
    }	

  else if ( ap == 0x03UL )
    {
      if ( verbose & 0x010 )
        logger << DebugInfo
               << "Accessing non protected data."
               << EndDebugInfo;
    }

  // all the checks succeded
  return true;
}

/** Non intrusive translate address from VA to MVA and physical address.
 *
 * @param va the virtual address to handle
 * @param mva the generated modified virtual address
 * @param pa the generated physicial address
 * @return true on success, false on error
 */
bool
CPU::
NonIntrusiveTranslateVA(bool is_read, 
                        uint32_t va, 
                        uint32_t &mva, 
                        uint32_t &pa,
                        uint32_t &cacheable,
                        uint32_t &bufferable)
{
  bool status = true;

  // transform the VA into MVA
  if ( (va & cp15.FCSE_PID_MASK) == 0 )
		mva = cp15.GetFCSE_PID() + va;
  else
		mva = va;
  if ( verbose & 0x020 )
    {
      logger << DebugWarning
             << "Non intrusive access" << std::endl
             << " - va = 0x" << std::hex << va 
             << std::endl
             << " - mva = 0x" << mva << std::dec
             << EndDebugWarning;
    }
  // Build first the address of the Translation Table Base
  uint32_t ttb_addr = 
		cp15.GetTTB() | ((mva & 0xfff00000UL) >> 18);
  // Get the first level descriptor
  // Two TLB are available, the lockdown TLB and the regular TLB
  // If the address is not found on the TLBs then perform a page walk
  // Check first the lockdown TLB and afterwards the regular TLB
  bool found_first_level = false;
  uint32_t first_level = 0;
  bool found_second_level = false;
  uint32_t second_level = 0;
  uint32_t way = 0;
  if ( ltlb.GetWay(ttb_addr, &way) )
    {
      first_level = ltlb.GetData(way);
      found_first_level = true;
    }

  if ( !found_first_level )
    {
      if ( verbose & 0x020 )
        {
          logger << DebugWarning
                 << " - not found in the lookdown TLB"
                 << EndDebugWarning;
        }
      // the entry was not found in the lockdown tlb
      // check the main tlb
      uint32_t tag = tlb.GetTag(ttb_addr);
      uint32_t set = tlb.GetSet(ttb_addr);
      if ( tlb.GetWay(tag, set, &way) )
        {
          first_level = tlb.GetData(set, way);
          found_first_level = true;
        }
    }

  if ( !found_first_level )
    {
      if ( verbose & 0x020 )
        {
          logger << DebugWarning
                 << " - not found in the main TLB"
                 << EndDebugWarning;
        }
      uint8_t first_level_data[4];
      // oops, we are forced to perform a page walk
      status = ExternalReadMemory(ttb_addr,
                                  first_level_data, 4);
      if ( !status ) 
        {
          logger << DebugInfo
                 << "Could not read first level descriptor from memory"
                 << " during non intrusive access:" << std::endl
                 << " - ttb address = 0x" << std::hex << ttb_addr
                 << std::dec
                 << EndDebugInfo;
          return false;
        }
      memcpy(&first_level, first_level_data, 4);
      first_level = LittleEndian2Host(first_level);
    }

  // NOTE: we let the found_first_level to false to signal that the
  //   entry was found in the memory table
  // now we have to check that the first level entry obtained is valid
  if ( (first_level & 0x03) == 0 )
    {
      // this is an invalid entry, provoke a data abort
      logger << DebugError
             << "Address translation for 0x"
             << std::hex << va << std::dec
             << " was not found (or returned a fault page)" << std::endl
             << " - ttb address = 0x" << std::hex << ttb_addr << std::dec
             << std::endl
             << " - first level fetched = 0x" << std::hex
             << first_level << std::dec
             << EndDebugError;
      return false;
    }

  // check wether is a section page or a second access is required
  if ( (first_level & 0x03UL) == 0 )
    {
      // fault page
      logger << DebugError
             << "First level descriptor fault when making a non intrusive"
             << " access:" << std::endl
             << " - ttb address = 0x" << std::hex << ttb_addr << std::dec
             << std::endl
             << " - first level fetched = 0x" << std::hex
             << first_level << std::dec
             << EndDebugError;
      return false;
    }

  else if ( (first_level & 0x03UL) == 0x01UL ) // coarse page
    {
      // a second level descriptor fetch is initiated
      // build the address of the 2nd level descriptor
      uint32_t coarse_addr =
        (first_level & 0xfffffc00UL) |
        ((mva & 0x000ff000UL) >> 10);
      if ( verbose & 0x020 )
        {
          logger << DebugInfo
                 << "Non intrusive access to coarse page descriptor" << std::endl
                 << " - first level entry   = 0x" << std::hex
                 << first_level << std::endl
                 << " - mva                 = 0x" << mva << std::endl
                 << " - coarse page address = 0x" << coarse_addr << std::dec
                 << EndDebugInfo;
        }
      uint32_t second_way = 0;
      if ( ltlb.GetWay(coarse_addr, &second_way) )
        {
          second_level = ltlb.GetData(second_way);
          found_second_level = true;
        }
      if ( !found_second_level )
        {
          if ( verbose & 0x020 )
			{
              logger << DebugInfo
                     << "Non intrusive access, coarse page descriptor not found"
                     << " in the lockdown TLB"
                     << EndDebugInfo;
			}
          // the entry was not faound in the lockdown tlb
          // check the main tlb
          uint32_t second_tag = tlb.GetTag(coarse_addr);
          uint32_t second_set = tlb.GetSet(coarse_addr);
          if ( tlb.GetWay(second_tag, second_set, &second_way) )
			{
              second_level = tlb.GetData(second_set, second_way);
              found_second_level = true;
			}
        }
      if ( !found_second_level )
        {
          if ( verbose & 0x020 )
			{
              logger << DebugInfo
                     << "Non intrusive access, coarse page descriptor not found"
                     << " in the main TLB"
                     << EndDebugInfo;
			}
          uint32_t second_level_data[4];
          // oops, we are forced to perform a page walk
          status = ExternalReadMemory(coarse_addr,
                                      second_level_data, 4);
          if ( !status ) 
			{
              logger << DebugInfo
                     << "Could not read coarse page descriptor from memory"
                     << " during non intrusive access:" << std::endl
                     << " - address = 0x" << std::hex << coarse_addr
                     << std::dec
                     << EndDebugInfo;
              return false;
			}
          memcpy(&second_level, second_level_data, 4);
          second_level = LittleEndian2Host(second_level);
        }
      // NOTE: we let the found_second_level to false to signal that the entry
      //   was found in the memory table
      // now we can check that the second level entry obtained is valid
      if ( (second_level & 0x03UL) == 0 )
        {
          // this is an invalid entry
          logger << DebugError
                 << "Address translation for 0x" << std::hex << va << std::dec
                 << " was not found during non intrusive access" << std::endl
                 << " - mva = 0x" << std::hex << mva << std::endl
                 << " - first_level = 0x" << first_level << std::endl
                 << " - coarse page address = 0x" << coarse_addr << std::endl
                 << " - second_level = 0x" << second_level << std::dec
                 << EndDebugError;
          return false;
        }

      // check whether is a large, small or tiny page
      if ( (second_level & 0x03UL) == 0 )
        {
          // fault page
          logger << DebugError
                 << "Found faulty translation second level entry"
                 << "for address 0x" << std::hex << va << std::dec
                 << " was not found during non intrusive access" << std::endl
                 << " - mva = 0x" << std::hex << mva << std::endl
                 << " - first_level = 0x" << first_level << std::endl
                 << " - coarse page address = 0x" << coarse_addr << std::endl
                 << " - second_level = 0x" << second_level << std::dec
                 << EndDebugError;
          return false;
        }

      else if ( (second_level & 0x03UL) == 1 )
        {
          // large page
          pa = (second_level & 0xffff0000UL) |
            (mva & 0x0000ffffUL);
          assert("TODO:Large page descriptor" == 0);
        }

      else if ( (second_level & 0x03UL) == 2 )
        {
          // small page
          pa = (second_level & 0xfffff000UL) |
            (mva & 0x00000fffUL);
          uint32_t ap[4];
          ap[0] = (second_level >> 4) & 0x03UL;
          ap[1] = (second_level >> 6) & 0x03UL;
          ap[2] = (second_level >> 8) & 0x03UL;
          ap[3] = (second_level >> 10) & 0x03UL;
          uint32_t ap_used = 0;
          switch ( ((mva & 0x0c00UL) >> 10) & 0x03UL )
			{
            case 0x00UL: ap_used = ap[0]; break;
            case 0x01UL: ap_used = ap[1]; break;
            case 0x02UL: ap_used = ap[2]; break;
            case 0x03UL: ap_used = ap[3]; break;
			}
          uint32_t domain = (first_level >> 5) & 0x0fUL;
          uint32_t c = (second_level >> 3) & 0x01UL;
          cacheable = c;
          uint32_t b = (second_level >> 2) & 0x01UL;
          bufferable = b;
          if ( verbose & 0x020 )
            logger << DebugInfo
                   << "Address translated using a small page descriptor"
                   << " (non intrusive access):"
                   << std::endl
                   << " - current pc = 0x" << std::hex << cur_instruction_address
                   << std::endl
                   << " - va  = 0x" << va << std::endl
                   << " - mva = 0x" << mva << std::endl
                   << " - pa  = 0x" << pa << std::dec << std::endl
                   << " - ap[0-3] = " << ap[0]
                   << " " << ap[1] << " " << ap[2] << " " << ap[3] << std::endl
                   << " - ap  = " << ap_used << std::endl
                   << " - domain = " << domain << std::endl
                   << " - c   = " << c << std::endl
                   << " - b   = " << b
                   << EndDebugInfo;
        }
    }

  else if ( (first_level & 0x02UL) == 0x02UL ) // section page
    {
      // the descriptor is a section descriptor
      pa = (first_level & 0xfff00000UL) |
        (mva & 0x000fffffUL);
      uint32_t ap = (first_level >> 10) & 0x03UL;
      uint32_t domain = (first_level >> 5) & 0x0fUL;
      uint32_t c = (first_level >> 3) & 0x01UL;
      cacheable = c;
      uint32_t b = (first_level >> 2) & 0x01UL;
      bufferable = b;
      if ( verbose & 0x020 )
        logger << DebugInfo
               << "Address translated using a section descriptor:" << std::endl
               << "- current pc = 0x" << std::hex << cur_instruction_address
               << std::dec << std::endl
               << "- va  = 0x" << std::hex << va << std::endl
               << "- mva = 0x" << mva << std::endl
               << "- pa  = 0x" << pa << std::dec << std::endl
               << "- ap  = " << ap << std::endl
               << "- domain = " << domain << std::endl
               << "- c   = " << c << std::endl
               << "- b   = " << b
               << EndDebugInfo;
    }

  return true;
}

/** Translate address from MVA to physical address.
 *
 * @param mva the generated modified virtual address
 * @param pa the generated physicial address
 * @return true on success, false on error
 */
bool 
CPU::
TranslateMVA(uint32_t mva, uint32_t &pa)
{
  // Build first the address of the Translation Table Base
  uint32_t ttb_addr = 
		cp15.GetTTB() | ((mva & 0xfff00000UL) >> 18);
  if ( verbose & 0x010 )
    {
      logger << DebugInfo
             << "Translating modified virtual address:" << std::endl
             << " - mva = 0x" << mva << std::endl
             << " - ttb = 0x" << cp15.GetTTB() << std::dec
             << EndDebugInfo;
    }

  uint32_t first_level = 0;
  GetTLBEntry(ttb_addr, first_level);

  // check wether is a section page or a second access is required
  if ( first_level & 0x02UL )
    {
      // the descriptor is a section descriptor
      pa = (first_level & 0xfff00000UL) |
        (mva & 0x000fffffUL);
    }
  else
    {
      // the page is not a section a second access to the tlb is required
      assert("Translation fault (TODO: second level access)" == 0);
    }

  return true;
}

/** Get the TLB descriptor pointed by the given Translation Table Base address.
 *
 * This method check the logical TLB for the given Translation Table address, by
 *   accessing first the lockdown TLB, then the main TLB or otherwise the memory
 *   TLB.
 *
 * @param ttb_addr the address on the Translation Table Base to look for
 * @param descriptor the found descriptor
 */
void
CPU::
GetTLBEntry(uint32_t ttb_addr,
            uint32_t &descriptor)
{
  if ( unlikely(verbose & 0x010) )
    {
      logger << DebugInfo
             << "Looking for TLB descriptor entry with Translation Table address"
             << " 0x" << std::hex << ttb_addr << std::dec
             << EndDebugInfo;
    }

  // Get the first level descriptor
  // Two TLB are available, the lockdown TLB and the regular TLB
  // If the address is not found on the TLBs then perform a page walk
  // Check first the lockdown TLB and afterwards the regular TLB
  bool found = false;
  uint32_t way = 0;
  ltlb.read_accesses++;
  if ( ltlb.GetWay(ttb_addr, &way) )
    {
      if ( ltlb.GetValid(way) )
        {
          ltlb.read_hits++;
          descriptor = ltlb.GetData(way);
          found = true;
        }
    }
  if ( unlikely(ltlb.power_estimator_import != 0) )
		ltlb.power_estimator_import->ReportReadAccess();

  if ( !found)
    {
      if ( unlikely(verbose & 0x010) )
        {
          logger << DebugInfo
                 << "TLB descriptor not found in the lockdown TLB,"
                 << " checking main TLB."
                 << EndDebugInfo;
        }
      // the entry was not found in the lockdown tlb
      // check the main tlb
      uint32_t tag = tlb.GetTag(ttb_addr);
      uint32_t set = tlb.GetSet(ttb_addr);
      tlb.read_accesses++;
      if ( tlb.GetWay(tag, set, &way) )
        {
          if ( tlb.GetValid(set, way) )
			{
              tlb.read_hits++;
              descriptor = tlb.GetData(set, way);
              found = true;
			}
        }
      if ( unlikely(tlb.power_estimator_import != 0) )
        tlb.power_estimator_import->ReportReadAccess();
    }
  if ( !found )
    {
      if ( verbose & 0x010 )
        {
          logger << DebugInfo
                 << "TLB descriptor not found in the main TLB, "
                 << "reading it from memory."
                 << EndDebugInfo;
        }
      uint8_t descriptor_data[4];
      // oops, we are forced to perform a page walk
      PrRead(ttb_addr, descriptor_data, 4);
      memcpy(&descriptor, descriptor_data, 4);
      descriptor = LittleEndian2Host(descriptor);
	
      // ok, we have the entry add it to the main tlb
      uint32_t tag = tlb.GetTag(ttb_addr);
      uint32_t set = tlb.GetSet(ttb_addr);
      uint32_t way = tlb.GetNewWay(set);
      if ( verbose & 0x010 )
        {
          logger << DebugInfo
                 << "Filling main tlb with fetched descriptor:"
                 << " - descriptor address = 0x" 
                 << std::hex << ttb_addr << std::endl
                 << " - descriptor value   = 0x"
                 << descriptor << std::endl
                 << " - tag = 0x" << tag << std::dec << std::endl
                 << " - set = " << set << std::endl
                 << " - way = " << way
                 << EndDebugInfo;
        }
      tlb.write_accesses++;
      tlb.SetEntry(tag, set, way, descriptor, 1);
    }
}

/** Translate address from VA to MVA and physical address.
 *
 * @param va the virtual address to handle
 * @param mva the generated modified virtual address
 * @param pa the generated physicial address
 * @return true on success, false on error
 */
bool
CPU::
TranslateVA(bool is_read, 
            uint32_t va, 
            uint32_t &mva, 
            uint32_t &pa,
            uint32_t &cacheable,
            uint32_t &bufferable)
{
  // transform the VA into MVA
  if ( (va & cp15.FCSE_PID_MASK) == 0 )
		mva = cp15.GetFCSE_PID() + va;
  else
		mva = va;
  // Build first the address of the Translation Table Base
  uint32_t ttb_addr = 
		cp15.GetTTB() | ((mva & 0xfff00000UL) >> 18);
  if ( verbose & 0x010 )
    {
      logger << DebugInfo
             << "Translating virtual address:" << std::endl
             << " - va = 0x" << std::hex << va << std::endl
             << " - mva = 0x" << mva << std::endl
             << " - ttb = 0x" << cp15.GetTTB() << std::dec
             << EndDebugInfo;
    }

  uint32_t first_level = 0;
  GetTLBEntry(ttb_addr, first_level);

  // check wether is a section page or a second access is required
  if ( (first_level & 0x03UL) == 0 )
    {
      // fault page
      // this is an invalid entry, provoke a data abort
      logger << DebugError
             << "Address translation for 0x"
             << std::hex << va << std::dec
             << " was not found (or returned a fault page)" << std::endl
             << " - ttb address = 0x" << std::hex << ttb_addr << std::dec
             << std::endl
             << " - first level fetched = 0x" << std::hex
             << first_level << std::endl
             << " - pc = 0x" << cur_instruction_address << std::dec
             << EndDebugError;
      return false;
    }

  else if ( (first_level & 0x03UL) == 0x01UL ) // Coarse page
    {
      // a second level descriptor fetch is initiated
      // build the address of the 2nd level descriptor
      uint32_t coarse_addr =
        (first_level & 0xfffffc00UL) |
        ((mva & 0x000ff000UL) >> 10);
      if ( verbose & 0x010)
        {
          logger << DebugInfo
                 << "First level TLB entry was a coarse page descriptor,"
                 << " fetching second level entry:" << std::endl
                 << " - first level entry   = 0x" << std::hex
                 << first_level << std::endl
                 << " - mva                 = 0x" << mva << std::endl
                 << " - coarse page address = 0x"
                 << coarse_addr << std::dec
                 << EndDebugInfo;
        }

      uint32_t second_level = 0;
      GetTLBEntry(coarse_addr, second_level);

      // check whether is a large, small or tiny page
      if ( (second_level & 0x03UL) == 0 )
        {
          // fault page
          assert("Translation fault (TODO: handle second level descriptor"
                 " fault)" == 0);
        }

      else if ( (second_level & 0x03UL) == 1 )
        {
          // large page
          pa = (second_level & 0xffff0000UL) |
            (mva & 0x0000ffffUL);
          assert("TODO: Large page descriptor" == 0);
        }

      else if ( (second_level & 0x03UL) == 2 )
        {
          // small page
          pa = (second_level & 0xfffff000UL ) |
            (mva & 0x00000fffUL);
          uint32_t ap[4];
          ap[0] = (second_level >> 4) & 0x03UL;
          ap[1] = (second_level >> 6) & 0x03UL;
          ap[2] = (second_level >> 8) & 0x03UL;
          ap[3] = (second_level >> 10) & 0x03UL;
          uint32_t ap_used = 0;
          switch ( ((mva & 0x0c00UL) >> 10) & 0x03UL )
			{
            case 0x00UL: ap_used = ap[0]; break;
            case 0x01UL: ap_used = ap[1]; break;
            case 0x02UL: ap_used = ap[2]; break;
            case 0x03UL: ap_used = ap[3]; break;
			}
          uint32_t domain = (first_level >> 5) & 0x0fUL;
          uint32_t c = (second_level >> 3) & 0x01UL;
          cacheable = c;
          uint32_t b = (second_level >> 2) & 0x01UL;
          bufferable = b;
          if ( verbose & 0x010 )
            logger << DebugInfo
                   << "Address translated using a small page descriptor:"
                   << std::endl
                   << " - current pc = 0x" << std::hex << cur_instruction_address
                   << std::endl
                   << " - va  = 0x" << va << std::endl
                   << " - mva = 0x" << mva << std::endl
                   << " - pa  = 0x" << pa << std::dec << std::endl
                   << " - ap[0-3] = " << ap[0]
                   << " " << ap[1] << " " << ap[2] << " " << ap[3] << std::endl
                   << " - ap  = " << ap_used << std::endl
                   << " - domain = " << domain << std::endl
                   << " - c   = " << c << std::endl
                   << " - b   = " << b
                   << EndDebugInfo;

          // check access permissions
          if ( unlikely(!CheckAccessPermission(is_read,
                                               ap_used, domain)) )
			{
              logger << DebugError
                     << "Check access permission and domain"
                     << std::endl
                     << " - current pc = 0x" << std::hex << cur_instruction_address
                     << " - va  = 0x" << va << std::endl
                     << " - mva = 0x" << mva << std::endl
                     << " - pa  = 0x" << pa << std::dec << std::endl
                     << " - ap[0-3] = " << ap[0]
                     << " " << ap[1] << " " << ap[2] << " " << ap[3] << std::endl
                     << " - ap  = " << ap_used << std::endl
                     << " - domain = " << domain << std::endl
                     << " - c   = " << c << std::endl
                     << " - b   = " << b
                     << EndDebugError;
              assert("TODO: check access permission and domain" == 0);
			}

        }

      else // 3 tiny page
        {
          // tiny page
          assert("TODO: Tiny page descriptor" == 0);
        }
    }
	
  else if ( (first_level & 0x03UL) == 0x02UL ) // Section page
    {
      // the descriptor is a section descriptor
      pa = (first_level & 0xfff00000UL) |
        (mva & 0x000fffffUL);
      uint32_t ap = (first_level >> 10) & 0x03UL;
      uint32_t domain = (first_level >> 5) & 0x0fUL;
      uint32_t c = (first_level >> 3) & 0x01UL;
      cacheable = c;
      uint32_t b = (first_level >> 2) & 0x01UL;
      bufferable = b;
      if ( verbose & 0x010 )
        logger << DebugInfo
               << "Address translated using a section descriptor:" << std::endl
               << "- current pc = 0x" << std::hex << cur_instruction_address
               << std::dec << std::endl
               << "- va  = 0x" << std::hex << va << std::endl
               << "- mva = 0x" << mva << std::endl
               << "- pa  = 0x" << pa << std::dec << std::endl
               << "- ap  = " << ap << std::endl
               << "- domain = " << domain << std::endl
               << "- c   = " << c << std::endl
               << "- b   = " << b
               << EndDebugInfo;

      // check the access permissions - domain couple
      //  if ap == 3 then no need to check anything else,
      //    read/write access are allowed
      if ( unlikely(!CheckAccessPermission(is_read,
                                           ap, domain)) )
        {
          logger << DebugError
                 << "Check access permission and domain"
                 << std::endl
                 << " - current pc = 0x" << std::hex << cur_instruction_address
                 << " - va  = 0x" << va << std::endl
                 << " - mva = 0x" << mva << std::endl
                 << " - pa  = 0x" << pa << std::dec << std::endl
                 << " - ap  = " << ap << std::endl
                 << " - domain = " << domain << std::endl
                 << " - c   = " << c << std::endl
                 << " - b   = " << b
                 << EndDebugError;
          assert("TODO: check access permission and domain" == 0);
        }
    }

  else // 0x03UL fine page table
    {
      assert("Translation fault (TODO: fine page table)" == 0);
    }

  return true;
}

  
/** Performs a prefetch access.
 *
 * @param memop the memory operation containing the prefetch access
 */
void 
CPU::
PerformPrefetchAccess(unisim::component::cxx::processor::arm::MemoryOp
                      *memop)
{
  uint32_t va = memop->GetAddress();
  uint32_t mva = va;
  uint32_t pa = mva;
  uint32_t cacheable = 1;
  uint32_t bufferable = 1;

  if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
           << "Performing prefetch with memop values:" << std::endl
           << " - addr = 0x" << std::hex << va << std::dec 
           << EndDebugInfo;

  if ( likely(cp15.IsMMUEnabled()) )
		if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
      {
        if ( unlikely(verbose & 0x02) )
          logger << DebugInfo
                 << "Prefetch not performed because there is no translation"
                 << " for the requested address (0x" << std::hex
                 << va << std::dec << ")"
                 << EndDebugInfo;
        // no translation found for the given address, do nothing
        return;
      }

  if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize() && cacheable) )
    {
      dcache.prefetch_accesses++;
      uint32_t cache_tag = dcache.GetTag(mva);
      uint32_t cache_set = dcache.GetSet(mva);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) )
			{
              // the access is a hit, nothing needs to be done
              cache_hit = true;
              if ( unlikely(verbose & 0x02) )
                logger << DebugInfo
                       << "Cache prefetch hit"
                       << EndDebugInfo;
			}
        }
      // if the access was a miss, data needs to be fetched from main
      //   memory and placed into the cache
      if ( unlikely(!cache_hit) )
        {
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "Cache prefetch miss"
                   << EndDebugInfo;

          // get a way to replace
          cache_way = dcache.GetNewWay(cache_set);
          // get the valid and dirty bits from the way to replace
          uint32_t cache_valid = dcache.GetValid(cache_set,
                                                 cache_way);
          uint32_t cache_dirty = dcache.GetDirty(cache_set,
                                                 cache_way);
          if ( (cache_valid != 0) && (cache_dirty != 0) )
			{
              // the cache line to replace is valid and dirty so it needs
              //   to be sent to the main memory
              uint8_t *rep_cache_data = 0;
              uint32_t mva_cache_address =
                dcache.GetBaseAddress(cache_set,
                                      cache_way);
              uint32_t pa_cache_address = mva_cache_address;
              // translate the address
              if ( likely(cp15.IsMMUEnabled()) )
                TranslateMVA(mva_cache_address,
                             pa_cache_address);
              if ( unlikely(verbose & 0x02) )
                logger << DebugInfo
                       << "Replace cache line with:" << std::endl
                       << " - mva = 0x" << std::hex << mva_cache_address
                       << std::endl
                       << " - pa = 0x" << pa_cache_address << std::dec
                       << EndDebugInfo;
              dcache.GetData(cache_set, cache_way,
                             &rep_cache_data);
              PrWrite(pa_cache_address, rep_cache_data,
                      dcache.LINE_SIZE);
			}
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            dcache.GetBaseAddressFromAddress(pa);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "Fetching cache line with:" << std::endl
                   << " - pa (base_address) = 0x" << std::hex << cache_address
                   << std::endl
                   << " - set = " << std::dec << cache_set << std::endl
                   << " - way = " << cache_way
                   << EndDebugInfo;
          uint32_t cache_line_size = dcache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          dcache.SetTag(cache_set, cache_way, cache_tag);
          dcache.SetValid(cache_set, cache_way, 1);
          dcache.SetDirty(cache_set, cache_way, 0);
        }
      else
        {
          dcache.prefetch_hits++;
        }
      if ( unlikely(dcache.power_estimator_import != 0) )
        dcache.power_estimator_import->ReportReadAccess();
    }
  else
    {
      /* it is just a cache prefetch, ignore the request if there is 
       * no cache */
    }
  /* CHECK: should we report a memory access for a prefetch???? */
}

/** Performs a write access.
 * @param memop the memory operation containing the write access
 */
void 
CPU::
PerformWriteAccess(unisim::component::cxx::processor::arm::MemoryOp
                   *memop)
{
  uint32_t va = memop->GetAddress();
  uint32_t mva = va;
  uint32_t pa = va;
  uint32_t size = memop->GetSize();
  uint8_t val8 = 0;
  uint16_t val16 = 0;
  uint32_t val32 = 0;
  uint8_t data8, data16[2], data32[4];
  uint8_t *data = 0;
  uint32_t cacheable = 1;
  uint32_t bufferable = 1;

  if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
           << "Performing write with memop values:" << std::endl
           << " - addr = 0x" << std::hex << va << std::dec << std::endl
           << "- size = " << size
           << EndDebugInfo;

  data8 = 0;
  memset(data16, 0, sizeof(uint8_t) * 2);
  memset(data32, 0, sizeof(uint8_t) * 4);
  // fix the write address depending on the request size and endianess
  //   and prepare the data to write
  switch(size)
    {
    case 1:
      va = va ^ munged_address_mask8;
      val8 = (uint8_t)memop->GetWriteValue();
      memcpy(&data8, &val8, 1);
      data = &data8;
      break;
    case 2:
      va = va ^ munged_address_mask16;
      val16 = (uint16_t)memop->GetWriteValue();
      val16 = Host2Target(GetEndianness(), val16);
      memcpy(data16, &val16, 2);
      data = data16;
      break;
    case 4:
      val32 = memop->GetWriteValue();
      val32 = Host2Target(GetEndianness(), val32);
      memcpy(data32, &val32, 4);
      data = data32;
      break;
    default: // should never happen
      break;
    }

  if ( likely(cp15.IsMMUEnabled()) )
		if ( !TranslateVA(false, va, mva, pa, cacheable, bufferable) )
      {
        logger << DebugError
               << "Could not translate address when performing write:"
               << std::endl
               << " - va = 0x" << std::hex << va << std::dec << std::endl
               << " - size = " << size << std::endl
               << " - pc = 0x" << std::hex << cur_instruction_address << std::dec << std::endl
               << " - instruction counter = " << instruction_counter
               << EndDebugError;
        unisim::kernel::service::Simulator::Instance()->Stop(this, __LINE__);
      }

  if ( unlikely(verbose & 0x02) )
    {
      logger << DebugInfo
             << "Writing into 0x" << std::hex << pa
             << " with data = "
             << (unsigned int)data[0]
             << " " << (unsigned int)data[1]
             << " " << (unsigned int)data[2]
             << " " << (unsigned int)data[3]
             << std::dec;
      if ( cp15.IsDCacheEnabled() && dcache.GetSize() && cacheable )
        logger << std::endl
               << " - line cacheable"
               << EndDebugInfo;
      else
        logger << EndDebugInfo;
    }

  if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize() && cacheable) )
    {
      dcache.write_accesses++;
      uint32_t cache_tag = dcache.GetTag(mva);
      uint32_t cache_set = dcache.GetSet(mva);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) )
			{
              // the access is a hit
              cache_hit = true;
			}
          else
            if ( unlikely(verbose & 0x02) )
              logger << DebugInfo
                     << "Way found but not valid (way = "
                     << cache_way << ")"
                     << EndDebugInfo;
        }
      if ( likely(cache_hit) )
        {
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "Cache hit (set = "
                   << cache_set << ", way = "
                   << cache_way << ")"
                   << EndDebugInfo;
          dcache.write_hits++;
          uint32_t cache_index = dcache.GetIndex(mva);
          dcache.SetData(cache_set, cache_way, cache_index,
                         size, data);
          dcache.SetDirty(cache_set, cache_way, 1);
          // if the access is not bufferable the data has to be written to the
          //   memory system like a write through cache
          if ( bufferable == 0 )
			{
              if ( unlikely(verbose & 0x02) )
                logger << DebugInfo
                       << "Seding to memory"
                       << EndDebugInfo;
              PrWrite(pa, data, size);
			}
        }
      else
        {
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "Cache miss"
                   << EndDebugInfo;
          if ( bufferable )
			{
              if ( unlikely(verbose & 0x02) )
                logger << DebugInfo
                       << "Bufferable"
                       << EndDebugInfo;
              // the cache act as a write back cache, so the line is fetched
              //   into cache and modified in the cache but not sent into
              //   memory
              // get a way to replace
              cache_way = dcache.GetNewWay(cache_set);
              // get the valid and dirty bits from the way to replace
              uint8_t cache_valid = dcache.GetValid(cache_set,
                                                    cache_way);
              uint8_t cache_dirty = dcache.GetDirty(cache_set,
                                                    cache_way);

              if ( (cache_valid != 0) && (cache_dirty != 0) )
                {
                  // the cache line to replace is valid and dirty so it needs
                  //   to be sent to the main memory
                  uint8_t *rep_cache_data = 0;
                  uint32_t mva_cache_address =
                    dcache.GetBaseAddress(cache_set, cache_way);
                  dcache.GetData(cache_set, cache_way, &rep_cache_data);
                  uint32_t pa_cache_address = mva_cache_address;
                  // translate the address
                  if ( likely(cp15.IsMMUEnabled()) )
                    TranslateMVA(mva_cache_address,
                                 pa_cache_address);
                  if ( unlikely(verbose & 0x02) )
                    {
                      uint32_t rep_tag = dcache.GetTag(cache_set, cache_way);
                      logger << DebugInfo
                             << "Evicting cache line:" << std::endl
                             << " - mva = 0x" << std::hex << mva_cache_address << std::endl
                             << " - pa  = 0x" << pa_cache_address << std::endl
                             << " - tag = 0x" << rep_tag << std::endl
                             << " - set = " << std::dec << cache_set << std::endl
                             << " - way = " << cache_way << std::hex << std::endl
                             << " - data =";
                      for ( unsigned int i = 0; i < dcache.LINE_SIZE; i++ )
                        logger << " " << (unsigned int)rep_cache_data[i];
                      logger << std::dec
                             << EndDebugInfo;
                    }
                  PrWrite(pa_cache_address, rep_cache_data, 
                          dcache.LINE_SIZE);
                }
              // the new data can be requested
              uint8_t *cache_data = 0;
              uint8_t prev_data[64];
              uint32_t cache_address =
                dcache.GetBaseAddressFromAddress(pa);
              // when getting the data we get the pointer to the cache line
              //   containing the data, so no need to write the cache
              //   afterwards
              uint32_t cache_line_size = dcache.GetData(cache_set,
                                                        cache_way, &cache_data);
              PrRead(cache_address, cache_data,
                     cache_line_size);
              memcpy(prev_data, cache_data, cache_line_size);
              dcache.SetTag(cache_set, cache_way, cache_tag);
              dcache.SetValid(cache_set, cache_way, 1);
              uint32_t cache_index = dcache.GetIndex(mva);
              dcache.SetData(cache_set, cache_way, cache_index,
                             size, data);
              dcache.SetDirty(cache_set, cache_way, 1);
              if ( unlikely(verbose & 0x02) )
                {
                  cache_line_size = dcache.GetData(cache_set,
                                                   cache_way, &cache_data);
                  logger << DebugInfo
                         << "Written line:" << std::endl
                         << " - mva = 0x" << std::hex << mva << std::dec << std::endl
                         << " - pa  = 0x" << std::hex << pa << std::dec << std::endl
                         << " - tag = 0x" << std::hex << cache_tag << std::dec << std::endl
                         << " - set = " << cache_set << std::endl
                         << " - way = " << cache_way << std::endl
                         << " - index = " << cache_index << std::endl
                         << " - size = " << size << std::endl
                         << " - valid = " << (uint32_t)dcache.GetValid(cache_set, cache_way) << std::endl
                         << " - dirty = " << (uint32_t)dcache.GetDirty(cache_set, cache_way) << std::endl
                         << " - data =" << std::hex;
                  for ( unsigned int i = 0; i < size; i++ )
                    logger << " " << (unsigned int)data[i];
                  logger << std::endl
                         << " - pre line =";
                  for ( unsigned int i = 0; i < cache_line_size; i++ )
                    logger << " " << (unsigned int)prev_data[i];
                  logger << std::endl
                         << " - new line =";
                  for ( unsigned int i = 0; i < cache_line_size; i++ )
                    logger << " " << (unsigned int)cache_data[i];
                  logger << std::dec
                         << EndDebugInfo;
                }
			}
          else
            PrWrite(pa, data, size);
        }

      if ( unlikely(dcache.power_estimator_import != 0) )
        dcache.power_estimator_import->ReportWriteAccess();
    }
  else // there is no data cache
    {
      if ( unlikely(verbose & 0x02) )
        logger << DebugInfo
               << "No cache, sending to memory"
               << EndDebugInfo;
      // there is no data cache, so just send the request to the
      //   memory interface
      PrWrite(pa, data, size);
    }

  /* report read memory access if necessary */
  if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
      memory_access_reporting_import->ReportMemoryAccess(
                                                         unisim::util::debug::MAT_WRITE,
                                                         unisim::util::debug::MT_DATA,
                                                         memop->GetAddress(), size);
}

/** Performs a read access.
 * @param memop the memory operation containing the read access
 */
void 
CPU::
PerformReadAccess(unisim::component::cxx::processor::arm::MemoryOp
                  *memop)
{
  uint32_t size = memop->GetSize();
  uint32_t va = memop->GetAddress() & ~(uint32_t)(size - 1);
  uint32_t cacheable = 1;
  uint32_t bufferable = 1;
  uint8_t data32[4];
  uint8_t *data;

  if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
           << "Performing read with memop values:" << std::endl
           << " - addr = 0x" << std::hex << va << std::dec << std::endl
           << "- size = " << size
           << EndDebugInfo;

  // fix the read address depending on the request size and endianess
  switch (size)
    {
		case 1: va = va ^ munged_address_mask8; break;
		case 2: va = va ^ munged_address_mask16; break;
		case 4: // nothing to do
		default: // nothing to do
      break;
    }
  uint32_t mva = va;
  uint32_t pa = mva;

  if ( likely(cp15.IsMMUEnabled()) )
		if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
      {
        logger << DebugError
               << "Could not translate address when performing read:"
               << std::endl
               << " - va = 0x" << std::hex << va << std::dec << std::endl
               << " - size = " << size << std::endl
               << " - pc = 0x" << std::hex << cur_instruction_address << std::dec << std::endl
               << " - instruction counter = " << instruction_counter
               << EndDebugError;
        unisim::kernel::service::Simulator::Instance()->Stop(this, __LINE__);
      }

  if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
           << "Read address translated:" << std::endl
           << " - va = 0x" << std::hex << va << std::endl
           << " - mva = 0x" << mva << std::endl
           << " - pa = 0x" << pa << std::dec << std::endl
           << " - cacheable = " << cacheable << std::endl
           << " - bufferable = " << bufferable
           << EndDebugInfo;

  if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize() && cacheable) )
    {
      dcache.read_accesses++;
      uint32_t cache_tag = dcache.GetTag(mva);
      uint32_t cache_set = dcache.GetSet(mva);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) )
			{
              // the access is a hit, nothing needs to be done
              cache_hit = true;
              if ( unlikely(verbose & 0x02) )
                logger << DebugInfo
                       << "Cache read hit"
                       << EndDebugInfo;
			}
        }
      // if the access was a miss, data needs to be fetched from main
      //   memory and placed into the cache
      if ( unlikely(!cache_hit) )
        {
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "Cache read miss"
                   << EndDebugInfo;
          // get a way to replace
          cache_way = dcache.GetNewWay(cache_set);
          // get the valid and dirty bits from the way to replace
          uint8_t cache_valid = dcache.GetValid(cache_set,
                                                cache_way);
          uint8_t cache_dirty = dcache.GetDirty(cache_set,
                                                cache_way);

          if ( (cache_valid != 0) && (cache_dirty != 0) )
			{
              // the cache line to replace is valid and dirty so it needs
              //   to be sent to the main memory
              uint8_t *rep_cache_data = 0;
              uint32_t mva_cache_address =
                dcache.GetBaseAddress(cache_set, cache_way);
              uint32_t pa_cache_address = mva_cache_address;
              // translate the address
              if ( likely(cp15.IsMMUEnabled()) )
                TranslateMVA(mva_cache_address,
                             pa_cache_address);
              if ( unlikely(verbose & 0x02) )
                logger << DebugInfo
                       << "Replacing cache line with:" << std::endl
                       << " - mva = 0x" << std::hex << mva_cache_address 
                       << std::endl
                       << " - pa = 0x" << pa_cache_address << std::dec
                       << EndDebugInfo;
              dcache.GetData(cache_set, cache_way, &rep_cache_data);
              PrWrite(pa_cache_address, rep_cache_data,
                      dcache.LINE_SIZE);
			}
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            dcache.GetBaseAddressFromAddress(pa);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "Fetching cache line with:" << std::endl
                   << " - pa (base_address) = 0x" << std::hex << cache_address
                   << std::endl
                   << " - set = " << std::dec << cache_set << std::endl
                   << " - way = " << cache_way
                   << EndDebugInfo;
          uint32_t cache_line_size = dcache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          dcache.SetTag(cache_set, cache_way, cache_tag);
          dcache.SetValid(cache_set, cache_way, 1);
          dcache.SetDirty(cache_set, cache_way, 0);
        }
      else
        {
          // cache hit
          dcache.read_hits++;
        }

      // at this point the data is in the cache, we can read it from the
      //   cache
      uint32_t cache_index = dcache.GetIndex(mva);
      (void)dcache.GetData(cache_set, cache_way, cache_index,
                           size, &data);
    }
  else // there is no data cache
    {
      if ( unlikely(verbose & 0x02) )
        logger << DebugInfo
               << "Fetching data from memory with:" << std::endl
               << " - pa = 0x" << std::hex << pa << std::dec << std::endl
               << " - size = " << size
               << EndDebugInfo;
      // just read the data from the memory system
      PrRead(pa, data32, size);
      data = data32;
    }

  // fix the data depending on its size
  uint32_t value = 0;
  if (size == 1)
    {
      uint8_t val8 = *data;
      if (memop->IsSigned())
        value = (int32_t)(int8_t)val8;
      else
        value = val8;
    }
  else if (size == 2)
    {
      uint16_t val16 = 0;
      memcpy(&val16, data, 2);
      val16 = Target2Host(GetEndianness(), val16);
      if (memop->IsSigned())
        value = (int32_t)(int16_t)val16;
      else
        value = val16;
    }
  else if (size == 4)
    {
      uint32_t val32;
      uint32_t val32_l, val32_r;
      uint32_t align;

      memcpy(&val32, data, 4);
      val32 = Target2Host(GetEndianness(), val32);
      // we need to check alignment
      align = memop->GetAddress() & (uint32_t)0x03;
      if (align != 0)
        {
          val32_l = (val32 << (align*8)) &
            ((~((uint32_t)0)) << (align*8));
          val32_r = (val32 >> ((4 - align) * 8)) &
            ((~((uint32_t)0)) >> ((4 - align) * 8));
          val32 = val32_l + val32_r;
        }
      value = val32;
    }

  if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
           << "Finished performing read access, setting register with:" 
           << std::endl
           << " - register = " << memop->GetTargetReg() << std::endl
           << " - value = 0x" << std::hex << value << std::dec
           << EndDebugInfo;

  //if ( likely(memop->GetType() == MemoryOp::READ) )
  if (cpsr.Get(M) != USER_MODE)
		SetGPR(memop->GetTargetReg(), value);
  else
		SetGPR_usr(memop->GetTargetReg(), value);

  if ( likely(dcache.GetSize()) )
		if ( unlikely(dcache.power_estimator_import != 0) )
      dcache.power_estimator_import->ReportReadAccess();

  /* report read memory access if necessary */
  if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
      memory_access_reporting_import->ReportMemoryAccess(
                                                         unisim::util::debug::MAT_READ,
                                                         unisim::util::debug::MT_DATA,
                                                         memop->GetAddress(), size);
}

