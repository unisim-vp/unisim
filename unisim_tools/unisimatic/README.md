# Unisimatic 

## Overview

Unisimatic provides UNISIM-VP developpers with a two-pass packaging system:

- 1st pass: dependency generation of repositories (i.e. command 'gendeps'),
- 2nd pass: distribution of files from multiple repositories and projects into a single distribution directory (i.e. command 'dist').

Unisimatic also provides a shell for querying package database, and a building capability intended for CI/CD.

## Usage

	Usage: unisimatic [<options>] <command> [<command arguments>...]
	
	
	Commands:
	
	  gendeps [<repository>...]
	  
	        Generate dependencies of repositories
	        
	  dist <source> <distribution directory>
	  
	        Distribute a project from a source.
	        
	        Sources:
	        
	          - project directory
	          - project distribution file (e.g. distrib.conf)
	          - name of a project declared in ~/.config/unisimatic/config.yaml
	        
	  shell
	  
	        Start an interactive shell for querying the database of repository packages
	
	  build [<build>...]
	
	        Build
	
	Options:
	
	  -h, --help
	        Display help and exit.
	
	  --detailed-help
	        Display detailed help and exit.
	        
	  -v, --version
	        Output version information and exit.
	
	  --verbose[=<level>]
	        Increase or set verbosity level (default: 3)
	        
	  --config[=<config file>]
	        Dump configuration or set configuration file to use instead of ~/.config/unisimatic/config.yaml
	
	  --include-untracked-files
	        Generate dependencies for untracked files too (Git)

## Description

A 'repository' is a bare directory on the file system, containing files (source codes, data, ...) shared with projects 
or other repositories, and with a companion directory (package_dir) for indexing packages that provides this repository. 
A 'repository package' is a subdirectory 'package_dir' directory that indexes individual files, but not only. Repository package can depends 
on other repository packages. There is one repository package per subdirectory of repository, plus repository packages (mostly manually written) for 
expressing system dependencies (third party library with their headers, system headers, ...). A project is a bare 
directory which contains the source code of a tool (such as a simulator, an ISS generator, or an executable binary 
analyzer) and one or more project distribution files (i.e. distrib.conf).

Command 'gendeps' is for dependency generation of repositories, while Command 'dist' is for distributing all necessary 
material to build a project into a distribution directory. When invoking Unisimatic through a symbolic link named 
'gendeps' or 'dist', no command shall be passed as argument because Unisimatic infers the command from the basename of 
symbolic link.

When generating dependencies, if no repository is specified as argument, Unisimatic considers all the enabled 
repositories. For each sub-directory of the repository, i.e. a repository package, Unisimatic creates file lists. It applies 
special rules for 'unisim/service/interfaces' so that one file is considered as a repository package. The dependencies, file lists, 
and code snippets, are stored in 'package_dir' of repository.

Unisimatic configuration file, written in YAML, is stored in ~/.config/unisimatic/config.yaml.
When unspecified by user, default repository is the first enabled one.
Default project distribution file is 'distrib.conf'.

Below is a sample configuration file for Unisimatic:

	repositories:
	  - name: unisim_lib
	    path: /path/to/unisim_lib
	    package_dir: ../package
	    enabled: true
	    default: true
	  - name: genisslib
	    path: /path/to/genisslib
	    package_dir: package
	    enabled: true
	projects:
	  - name: simulator
	    path: /path/to/simulator
	    config_file: distrib.conf
	    enabled: true
	builds:
	  - name: simulator
	    project: simulator
	    build_dir: /path/to/build_dir
	    dist_dir: /path/to/dist_dir
	    enabled: true
	    command: |
	      ./configure
	      make
      
---    

A project directory shall contains at least one project distribution file at the root (e.g. distrib.conf).

A project distribution file is a Bash script with preloaded builtin functions (i.e. directives).

A project distribution file can:

- declare package that the distribute file provides, along with version, full name, and authors,
- use another project using directive 'subproject <project name>',
  provided that it is declared and enabled in ~/.config/unisimatic/config.yaml.
- import repository packages from repositories using directive 'import <repository name> <package>'
  or simply 'import <package>' (when a default repository exists).
- distribute source code files from imported repository packages using directive 'copy [<list name>...]'
- distribute project files using directive 'dist_copy [<file>...]'
- use a file list using directive 'files [<list>...]' (e.g. for automake)
- use code snippets using directive 'lines [<list>...]' (e.g. for autoconf or automake)
- write an autoconf file using directive 'output_configure_ac'
- write an automake file using directive 'output_makefile_am'
- generate 'configure' using directive 'build_configure'

## Directives

	help [<command>]
	
	    Get help.
	
	package [<package name>]
	
	    Get or set package name.
	
	version [<package version>]
	
	    Get or set version of package
	
	full_name [<package full name>]
	
	    Get or set package full name.
	
	author [<author>]
	
	    Get authors or add an author.
	
	authors [<author>...]
	
	    Get authors or add authors.
	
	subproject <project name> [<subdirectory>]
	
	    Distribute a subproject.
	
	    Example:
	
	    subproject genisslib
	
	use <repository name> [<subdirectory>]
	
	    Use an implicit repository for subsequent imports.
	
	    Example:
	
	    use unisim_lib
	
	import [<repository name>] <package>
	
	    Import a package.
	    This creates "lists" (e.g. source, header,...).
	
	    Example:
	
	    import unisim/kernel
	
	copy [<list name>...]
	
	    Distribute files of specified lists from imported repository packages.
	
	    Example:
	
	    copy source header
	
	dist_copy [<file>...]
	
	    Distribute files of project.
	
	    Example:
	
	    dist_copy main.cc
	
	lines [<list name>...]
	
	    Get the content of specified lists, with one entry per line (convenient for autoconf or automake snippets).
	
	    Example:
	
	    SNIPPET="$(lines sh)"
	
	files [<list name>[:<only-in>]...]
	
	    Get content of specified lists, with each entry separated by space (convenient for file lists in Makefile).
	    If <only-in> is specified, only files from this directory are returned.
	
	    Example:
	
	    MY_FILES=$(files source header)
	
	includes
	
	    Output search path of headers as C preprocessor options.
	
	    Example:
	
	    gcc $(includes) ...
	
	output_configure_ac [<file>]
	
	    Output configure.ac using file specified as argument.
	
	    Example:
	
	    output_configure_ac <(cat << EOF
	    $(lines ac_common_header)
	    $(lines ac)
	    $(lines ac_common_footer)
	    EOF
	  )
	
	output_makefile_am <file>
	
	    Output Makefile.am using file specified as argument.
	
	    Example:
	
	    output_makefile_am <(cat << EOF
	    $(lines am_common_header)
	    $(lines am)
	    EOF
	    )
	
	am_cppflags
	
	    Output CPPFLAGS for automake to put in Makefile.am.
	
	    Example:
	
	    output_makefile_am <(cat << EOF
	    ...
	    AM_CPPFLAGS=\$(am_cppflags)
	    ...
	    EOF
	    )
	
	build_configure
	
	    Build configure.
