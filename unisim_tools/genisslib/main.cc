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

#include <main.hh>
#include <cli.hh>
#include <isa.hh>
#include <generator.hh>
#include <scanner.hh>
#include <product.hh>
#include <strtools.hh>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
using namespace std;

#define DEFAULT_OUTPUT "iss"

Opts::Opts()
  : outputprefix( DEFAULT_OUTPUT ), expandname( 0 ), inputname( 0 ), depfilename( 0 ),
    minwordsize( -1 ), sourcelines( true ), specialization( true )
    
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
    if( _args.match( "-I", "<directory>", "include a directory into the search path for includes." ) ) {
      if( this->add_lookupdir( _args.pop_front() ) ) return;
      cerr << GENISSLIB ": '-I' must be followed by a directory.\n";
      help();
      throw CLI::Exit_t( 1 );
    }
    
    if( _args.match( "-o,--output", "<output>", "Outputs the instruction set simulator source code into "
                     "<output>.hh and <output>.cc/<output>.tcc (default is <output>=\"" DEFAULT_OUTPUT "\")." ) ) {
      if( (outputprefix = _args.pop_front()) ) return;
      cerr << GENISSLIB ": '-o' must be followed by an output name.\n";
      help();
      throw CLI::Exit_t( 1 );
    }
    
    if( _args.match( "-w,--word-size", "<size>", "Uses <size> as the minimum bit size for holding an operand bit field." ) ) {
      if( this->setminwordsize( _args.pop_front() ) ) return;
      cerr << GENISSLIB ": '-w' must be followed by a size.\n";
      help();
      throw CLI::Exit_t( 1 );
    }
    
    if( _args.match( "--specialization", "on/off", "Toggles specialized operation generation (default: on)." ) ) {
      if( this->on_off( &GIL::specialization, _args.pop_front() ) ) return;
      cerr << GENISSLIB ": '--specialization' must be followed by 'on' or 'off'.\n";
      help();
      throw CLI::Exit_t( 1 );
    }

    if( _args.match( "--source-lines", "on/off", "Toggles on/off source line reference in generated files (default: on)." ) ) {
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

    if( _args.match( "-E,--expand", "<filename>", "Expands the preprocessed input file to <filename>." ) ) {
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
main( int argc, char** argv, char** envp ) {
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
  
    if( gil.expandname ) {
      ofstream expandfile( gil.expandname );
      if( not expandfile ) {
        cerr << GENISSLIB ": can't open output file `" << gil.expandname << "'" << endl;
        throw CLI::Exit_t( 1 );
      }
      
      isa.expand( expandfile );
    }
    
    if( gil.depfilename ) {
      ofstream depfile( gil.depfilename );
      isa.deps( depfile, gil.outputprefix );
    }
    
    Product_t product( gil.outputprefix, gil.sourcelines );
    
    if( not isa.sanity_checks() ) throw CLI::Exit_t( 1 );

    // Specialization
    if( gil.specialization ) isa.specialize();

    auto_ptr<Generator> generator = isa.generator();
    
    generator->init( isa );
    
    try {
      // Back-end specific preprocess
      generator->finalize();
      // ISS Generation
      generator->iss( product );
    } catch( Generator::Exception_t ) {
      cerr << GENISSLIB ": compilation aborted.\n";
      throw CLI::Exit_t( 1 );
    }
  } catch( CLI::Exit_t _code ) {
    return _code.m_value;
  }
  
  return 0;
}
