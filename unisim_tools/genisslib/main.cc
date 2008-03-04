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
using namespace std;

#define DEFAULT_OUTPUT "iss"

Opts::Opts()
  : outputprefix( DEFAULT_OUTPUT ), expandname( 0 ), inputname( 0 ), depfilename( 0 ),
    minwordsize( -1 ), subdecoder( 0 ), sourcelines( true ), specialization( true ),
    actiontext( false )
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
  
  void parse( CLI::Args_t& _args ) {
    if( _args.match( CLI::Unlimited, "-I", 0,
                     "<directory>", "include a directory into the search path for includes." ) )
      {
        char const* arg;
        if( not (arg = _args.pop()) ) {
          cerr << GENISSLIB ": '-I' must be followed by a directory.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
        Scanner::add_lookupdir( arg );
      }
    else if( _args.match( CLI::AtMostOnce, "-o", "--output", 0,
                          "<output>", "Outputs the instruction set simulator source code into "
                          "<output>.hh and <output>.cc/<output>.tcc (default is <output>=\"" DEFAULT_OUTPUT "\")." ) )
      {
        if( not (outputprefix = _args.pop()) ) { 
          cerr << GENISSLIB ": '-o' must be followed by an output name.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
      }
    else if( _args.match( CLI::AtMostOnce, "-w", "--word-size", 0,
                          "<size>", "Uses <size> as the minimum bit size for holding an operand bit field." ) )
      {
        char const* arg;
        if( not (arg = _args.pop()) ) {
          cerr << GENISSLIB ": '-w' must be followed by a size.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
        minwordsize = strtoul( arg, 0, 0 );
      }
    else if( _args.match( CLI::AtMostOnce, "--specialization", 0,
                          "on/off", "Toggles specialized operation generation (default: on)." ) )
      {
        char const* arg;
        if( not (arg = _args.pop()) ) {
          cerr << GENISSLIB ": '--specialization' must be followed by 'on' or 'off'.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
        specialization = ( strcmp( arg, "on" ) == 0 );
      }
    else if( _args.match( CLI::AtMostOnce, "--source-lines", 0,
                          "on/off", "Toggles on/off source line reference in generated files (default: on)." ) )
      {
        char const* arg;
        if( not (arg = _args.pop()) ) {
          cerr << GENISSLIB ": '--source-lines' must be followed by 'on' or 'off'.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
        sourcelines = ( strcmp( arg, "on" ) == 0 );
      }
    else if( _args.match( CLI::AtMostOnce, "--action-text", 0,
                          "on/off", "Toggles on/off action code text availablility (default: off)." ) )
      {
        char const* arg;
        if( not (arg = _args.pop()) ) {
          cerr << GENISSLIB ": '--action-text' must be followed by 'on' or 'off'.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
        actiontext = ( strcmp( arg, "on" ) == 0 );
      }
    else if( _args.match( CLI::AtMostOnce, "-v", "--version", 0,
                          "", "Displays " GENISSLIB " version and exits." ) )
      {
        version();
        throw CLI::Exit_t( 0 );
      }
    else if( _args.match( CLI::AtMostOnce, "-S", "--subdecoder", 0, "<subdecoder name>",
                          "Generate a subdecoder instead of a full ISS (create a <output>.sdh "
                          "header file to be included in the main decoder)." ) )
      {
        if( not (subdecoder = _args.pop()) ) {
          cerr << GENISSLIB ": '-S' must be followed by a subdecoder name.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
      }
    else if( _args.match( CLI::AtMostOnce, "-M", 0, "<filename>",
                          "Output a rule file (<filename>) suitable for make describing the "
                          "dependencies of the main source file." ) )
      {
        if( not (depfilename = _args.pop()) ) {
          cerr << GENISSLIB ": '-M' must be followed by a file name.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
      }
    else if( _args.match( CLI::AtMostOnce, "-E", "--expand", 0,
                          "<filename>", "Expands the preprocessed input file to <filename>." ) )
      {
        if( not (expandname = _args.pop()) ) {
          cerr << GENISSLIB ": '-E' must be followed by a file name.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
      }
    else if( _args.match( CLI::Once, 0,
                          "<inputfile>", "The input file to process" )
             )
      {
        inputname = _args.pop();
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
