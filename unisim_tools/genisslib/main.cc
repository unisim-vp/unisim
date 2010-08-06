/***************************************************************************
                                   main.cc
                             -------------------
    begin                : Thu May 25 2003
    copyright            : (C) 2003-2007 CEA and Universite Paris Sud
    authors              : Gilles Mouchard, Yves Lhuillier
    email                : gilles.mouchard@cea.fr, yves.lhuillier@cea.fr
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

/** @file GenISSLib.c
    @brief generates the instruction set simulator library
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <main.hh>
#include <cli.hh>
#include <isa.hh>
#include <generator.hh>
#include <scanner.hh>
#include <strtools.hh>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#if defined(GIL_XPROC) && defined(HAVE_DLFCN_H)
#include <dlfcn.h>
#define GIL_MAIN base_main
#else
#define GIL_MAIN main
#endif // defined(GIL_XPROC) && defined(HAVE_DLFCN_H)

using namespace std;

#define DEFAULT_OUTPUT "iss"

Opts::Opts()
  : outputprefix( DEFAULT_OUTPUT ), expandname( 0 ), inputname( 0 ), depfilename( 0 ),
    minwordsize( 32 ), sourcelines( true ), specialization( true )
    
{ s_shared = this; }

Opts* Opts::s_shared = 0;
Opts& Opts::shared() { return *s_shared; }

struct GIL : public CLI, public Opts {
  void description() {
    cerr << "Generator of instruction set simulators." << endl;
  }
  
  void version() {
    cerr << "       " << m_displayname << " v" GENISSLIB_VERSION "\n"
         << "built date : " __DATE__ "\n"
         << "copyright  : " COPYRIGHT "\n"
         << "authors    : " AUTHORS "\n"
         << "emails     : " EMAILS << endl;
  }
  
  GIL() {}
  
  bool add_lookupdir( char const* _arg ) {
    if( not _arg ) return false;
    Scanner::add_lookupdir( _arg );
    return true;
  }
  
  bool setminwordsize( char const* _arg ) {
    if( not _arg ) return false;
    minwordsize = strtoul( _arg, 0, 0 );
    return true;
  }
  
  bool on_off( bool GIL::*_member, char const* _arg ) {
    if( not _arg ) return false;
    this->*_member = (strcmp( _arg, "on" ) == 0);
    return true;
  }
  
  void parse( CLI::Args_t& _args ) {
    if( _args.match( "-I", "<directory>", "Adds the directory <directory> "
                     "to the search paths for include directives." ) ) {
      if( this->add_lookupdir( _args.pop_front() ) ) return;
      cerr << GENISSLIB ": '-I' must be followed by a directory.\n";
      help();
      throw CLI::Exit_t( 1 );
    }
    
    if( _args.match( "-o,--output", "<output>", "Sets the prefix "
                     "of the generated source files to <output>; "
                     "(default is <output>=\"" DEFAULT_OUTPUT "\")." ) ) {
      if( (outputprefix = _args.pop_front()) ) return;
      cerr << GENISSLIB ": '-o' must be followed by an output name.\n";
      help();
      throw CLI::Exit_t( 1 );
    }
    
    if( _args.match( "-w,--min-word-size", "<size>", "Uses <size> as the "
                     "minimum bit size for holding an operand bit field." ) ) {
      if( this->setminwordsize( _args.pop_front() ) ) return;
      cerr << GENISSLIB ": '-w' must be followed by a size.\n";
      help();
      throw CLI::Exit_t( 1 );
    }
    
    if( _args.match( "--specialization", "on/off", "Toggles specialized "
                     "operation generation (default: on)." ) ) {
      if( this->on_off( &GIL::specialization, _args.pop_front() ) ) return;
      cerr << GENISSLIB ": '--specialization' must be followed by 'on' or 'off'.\n";
      help();
      throw CLI::Exit_t( 1 );
    }

    if( _args.match( "--source-lines", "on/off", "Toggles the output of "
                     "source line references in generated files (default: on)." ) ) {
      if( this->on_off( &GIL::sourcelines, _args.pop_front() ) ) return;
      cerr << GENISSLIB ": '--source-lines' must be followed by 'on' or 'off'.\n";
      help();
      throw CLI::Exit_t( 1 );
    }

    if( _args.match( "-v,--version", "", "Displays " GENISSLIB " version and exits." ) ) {
      version();
      throw CLI::Exit_t( 0 );
    }
    
    if( _args.match( "-M", "<filename>", "Output a rule file (<filename>) suitable for make "
                     "describing the dependencies of the main source file." ) ) {
      if( (depfilename = _args.pop_front()) ) return;
      cerr << GENISSLIB ": '-M' must be followed by a file name.\n";
      help();
      throw CLI::Exit_t( 1 );
    }

    if( _args.match( "-E,--expand", "<filename>", "Expands the preprocessed input file to "
                     "<filename>." ) ) {
      if( (expandname = _args.pop_front()) ) return;
      cerr << GENISSLIB ": '-E' must be followed by a file name.\n";
      help();
      throw CLI::Exit_t( 1 );
    }
    
    if( _args.match( not inputname, "<inputfile>", "The input file to process" ) ) {
      inputname = _args.pop_front();
      return;
    }
  }
};

int
GIL_MAIN (int argc, char** argv, char** envp) {
  GIL gil;
  gil.set( GENISSLIB, argv[0] );
  try {
    gil.process( argc-1, argv+1 );
    
    if( not gil.inputname ) {
      cerr << GENISSLIB ": no input file.\n";
      gil.help();
      throw CLI::Exit_t( 1 );
    }
    
    Isa isa;
    if( not Scanner::parse( gil.inputname, isa ) )
      throw CLI::Exit_t( 1 );
  
    if (gil.expandname) {
      ofstream expandfile( gil.expandname );
      if (not expandfile.good()) {
        cerr << GENISSLIB ": can't open output file `" << gil.expandname << "'" << endl;
        throw CLI::Exit_t( 1 );
      }
      
      isa.expand( expandfile );
    }
    
    if (gil.depfilename) {
      ofstream depfile( gil.depfilename );
      if (not depfile.good()) {
        cerr << GENISSLIB ": can't open output file `" << gil.depfilename << "'" << endl;
        throw CLI::Exit_t( 1 );
      }
      
      isa.deps( depfile, gil.outputprefix );
    }
    
    if( not isa.sanity_checks() ) throw CLI::Exit_t( 1 );

    // Specialization
    if( gil.specialization ) isa.specialize();

    auto_ptr<Generator> generator = isa.generator();
    
    generator->init( isa );
    
    try {
      // Back-end specific preprocess
      generator->finalize();
      // ISS Generation
      generator->iss( gil.outputprefix, gil.sourcelines );
    } catch( Generator::Exception_t ) {
      cerr << GENISSLIB ": compilation aborted.\n";
      throw CLI::Exit_t( 1 );
    }
  } catch( CLI::Exit_t _code ) {
    return _code.m_value;
  }
  
  return 0;
}

#if defined(GIL_XPROC) && defined(HAVE_DLFCN_H)

bool
log_dl_failure( std::ostream& _sink ) {
  char const* err = dlerror();
  if (err) _sink << GENISSLIB ": " << err << std::endl;
  return err;
}

typedef int (*gxp_abi_t) ();
typedef int (*gxp_main_t) (int argc, char** argv, char** envp);

int
main (int argc, char** argv, char** envp)
{
  if (argc < 2 or (strncmp( argv[1], "xproc=", 6 ) != 0))
    return base_main (argc, argv, envp);
  
  char const* xproc_path = argv[1] + 6;
  
  // Open the xproc module.
  void* handle = dlopen( xproc_path, RTLD_NOW );
  if (log_dl_failure (std::cerr)) return 1;
  
  // Check that xproc module and genisslib abi match.
  gxp_abi_t gxp_abi = (gxp_abi_t)dlsym (handle, "gxp_abi");
  if (log_dl_failure (std::cerr)) return 1;
  if (gxp_abi() != 1) {
    cerr << GENISSLIB ": process extension '" << xproc_path
         << "' doesn't match current version of genisslib.\n";
    return 1;
  }
  
  // Launch xproc module main procedure
  gxp_main_t gxp_main = (gxp_main_t)dlsym (handle, "gxp_main");
  if (log_dl_failure (std::cerr)) return 1;
  argv[1] = argv[0];
  return gxp_main (argc-1, argv+1, envp);
}

#endif // defined(GIL_XPROC) && defined(HAVE_DLFCN_H)
