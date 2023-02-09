#include <inttypes.h>

namespace unisim {
namespace util {
namespace trace {

  template <typename ADDRESS>
  struct SimpleLoc
  {
    typedef ADDRESS address_type;
    SimpleLoc(ADDRESS ea, ADDRESS ra) : entry_address(ea), return_address(ra) {}
    bool match( ADDRESS ea, ADDRESS ra ) const { return entry_address == ea and return_address == ra; }
    uintptr_t id() const { return (uintptr_t)this; }
    
    address_type entry_address;
    address_type return_address;
  };

  template <typename ADDRESS>
  struct CompactLoc
  {
    typedef ADDRESS address_type;
    CompactLoc(ADDRESS ea, ADDRESS ra) : entry_address(ea) {}
    bool match( ADDRESS ea, ADDRESS ra ) const { return entry_address == ea; }
    uintptr_t id() const { return (uintptr_t)this; }
    
    address_type entry_address;
  };
  
  template <typename LOCATION>
  struct CallStack
  {
    typedef LOCATION loc_type;
    typedef typename LOCATION::address_type address_type;

    CallStack() : current(new Site(0, 0, 0)), calls() {}

    struct Site
    {
      Site*    targets;
      Site*    siblings;
      loc_type loc;

      Site(Site* _siblings, address_type ea, address_type ra, Site* rec)
        : targets((Site*)((uintptr_t)rec | 1)), siblings(_siblings), loc(ea, ra)
      {}
    
      Site(Site* _siblings, address_type ea, address_type ra)
        : targets(0), siblings(_siblings), loc(ea, ra)
      {}
    
      Site* resolve_recursion()
      {
        // By convention, if the target pointer is odd the site is a
        // recursion site.  The real site is obtained by dereferencing
        // the even pointer.
        uintptr_t rec = (uintptr_t)targets;
        return rec & 1 ? ((Site*)(rec & -2)) : this;
      }
    };

    struct Call
    {
      Call* up;
      Site* site;
      address_type return_address;

      Call(Call* _up, Site* _site, address_type ra) : up(_up), site(_site), return_address(ra) {}
    };

    loc_type& loc() { return current->loc; }

    void enter(address_type to_address, address_type from_address)
    {
      Site* site = getsite(to_address, from_address);
      current = site;
      calls = new Call( calls, site, from_address );
    }

    Site* getsite( address_type to_address, address_type from_address )
    {
      // Look for a known call
      for (Site* site = current->targets; site; site = site->siblings)
        {
          if (not site->loc.match(to_address, from_address))
            continue;
          return site->resolve_recursion();
        }

      // Look for recursion
      for (Call* call = calls; call; call = call->up)
        {
          Site* site = call->site;
          if (not site->loc.match(to_address, from_address))
            continue;
          current->targets = new Site(current->targets, to_address, from_address, site);
          return site;
        }

      return (current->targets = new Site(current->targets, to_address, from_address));
    }

    void leave( address_type to_address, address_type from_address )
    {
      if (calls->return_address != to_address)
        throw 0;

      Call* out = calls;
      calls = calls->up;
      current = calls->site;

      delete out;
    }
  
    Site* current;
    Call* calls;
  };

} // end of namespace trace
} // end of namespace util
} // end of namespace unisim
