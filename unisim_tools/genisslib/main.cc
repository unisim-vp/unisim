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
#include <scanner.hh>
#include <product.hh>
#include <strtools.hh>
#include <iostream>
#include <fstream>
using namespace std;

#define DEFAULT_OUTPUT "iss"

struct GIL : public CLI {
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
  
  char const* outputprefix;
  char const* expandname;
  char const* inputname;
  int         minwordsize;
  
  GIL() : outputprefix( DEFAULT_OUTPUT ), expandname( 0 ), inputname( 0 ), minwordsize( -1 ) {}
  
  void parse( CLI::Args_t& _args ) {
    if( _args.match( CLI::Unlimited, "-I", 0,
                     "<directory>", "include a directory into the search path for includes.\n" ) )
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
                          "<output>.hh and <output>.cc/<output>.tcc (default is <output>=\"" DEFAULT_OUTPUT "\").\n" ) )
      {
        if( not (outputprefix = _args.pop()) ) { 
          cerr << GENISSLIB ": '-o' must be followed by an output name.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
      }
    else if( _args.match( CLI::AtMostOnce, "-w", "--word-size", 0,
                          "<size>", "Uses <size> as the minimum bit size for holding an operand bit field.\n" ) )
      {
        char const* arg;
        if( not (arg = _args.pop()) ) {
          cerr << GENISSLIB ": '-w' must be followed by a size.\n";
          help();
          throw CLI::Exit_t( 1 );
        }
        minwordsize = strtoul( arg, 0, 0 );
      }
    else if( _args.match( CLI::AtMostOnce, "-v", "--version", 0,
                          "", "Displays " GENISSLIB " version and exits.\n" ) )
      {
        version();
        throw CLI::Exit_t( 0 );
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
    
    Product_t product( gil.outputprefix );
    
    if( not isa.compile( product, gil.minwordsize ) ) {
      cerr << GENISSLIB ": compilation aborted.\n";
      throw CLI::Exit_t( 1 );
    }
    
  } catch( CLI::Exit_t _code ) {
    return _code.m_value;
  }
  
  return 0;
}
