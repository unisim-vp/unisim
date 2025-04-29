# Settings

Use the "set" and "show" commands to handle the settings.

## language

Current source language.

Default value: C

Allowed values: "arduino", "bash", "c", "cpp", "csharp", "css", "diff", "go", "graphql", "ini", "java", "javascript", "json", "kotlin", "less", "lua", "makefile", "markdown", "objectivec", "perl", "php", "php-template", "plaintext", "python", "python-repl", "r", "ruby", "rust", "scss", "shell", "sql", "swift", "typescript", "vbnet", "wasm", "xml", "yaml", "1c", "abnf", "accesslog", "actionscript", "ada", "angelscript", "apache", "applescript", "arcade", "armasm", "asciidoc", "aspectj", "autohotkey", "autoit", "avrasm", "awk", "axapta", "basic", "bnf", "brainfuck", "cal", "capnproto", "ceylon", "clean", "clojure", "clojure-repl", "cmake", "coffeescript", "coq", "cos", "crmsh", "crystal", "csp", "d", "dart", "delphi", "django", "dns", "dockerfile", "dos", "dsconfig", "dts", "dust", "ebnf", "elixir", "elm", "erb", "erlang", "erlang-repl", "excel", "fix", "flix", "fortran", "fsharp", "gams", "gauss", "gcode", "gherkin", "glsl", "gml", "golo", "gradle", "groovy", "haml", "handlebars", "haskell", "haxe", "hsp", "http", "hy", "inform7", "irpf90", "isbl", "jboss-cli", "julia", "julia-repl", "lasso", "latex", "ldif", "leaf", "lisp", "livecodeserver", "livescript", "llvm", "lsl", "mathematica", "matlab", "maxima", "mel", "mercury", "mipsasm", "mizar", "mojolicious", "monkey", "moonscript", "n1ql", "nestedtext", "nginx", "nim", "nix", "node-repl", "nsis", "ocaml", "openscad", "oxygene", "parser3", "pf", "pgsql", "pony", "powershell", "processing", "profile", "prolog", "properties", "protobuf", "puppet", "purebasic", "q", "qml", "reasonml", "rib", "roboconf", "routeros", "rsl", "ruleslanguage", "sas", "scala", "scheme", "scilab", "smali", "smalltalk", "sml", "sqf", "stan", "stata", "step21", "stylus", "subunit", "taggerscript", "tap", "tcl", "thrift", "tp", "twig", "vala", "vbscript", "vbscript-html", "verilog", "vhdl", "vim", "wren", "x86asm", "xl", "xquery" or "zephir".

## endian

Endianness of target.

Default value: little

Allowed values: "little" or "big".

## pagination

Whether to paginate.

Default value: on

Allowed values: "on", "1", "off" or "0".

## disassemble-next-line

Whether to disassemble next source line or instruction when execution stops:

  - ON: display both source line and instruction;
  - AUTO: display source line, otherwise instruction when source line can't be displayed;
  - OFF: do not display neither source line nor instruction
.

Default value: off

Allowed values: "on", "1", "off", "0" or "auto".

## listsize

Number of source lines to list by default.

Default value: 10

## print

### print elements

Limit on string chars or array elements to print.

Default value: 200

### print pretty

Whether to print structures and arrays in an indented format with one member/element per line, or in a compact format.

Default value: off

Allowed values: "on", "1", "off" or "0".

### print raw-values

Whether to print in raw form.

Default value: off

Allowed values: "on", "1", "off" or "0".

## style

### style enabled

Whether styling is enabled.

Default value: off

Allowed values: "on", "1", "off" or "0".

### style address

#### style address foreground

Foreground color for an address.

Default value: blue

#### style address background

Background color for an address.

Default value: none

#### style address intensity

Intensity for an address.

Default value: normal

### style function

#### style function foreground

Foreground color for an function.

Default value: yellow

#### style function background

Background color for an function.

Default value: none

#### style function intensity

Intensity for an function.

Default value: normal

### style filename

#### style filename foreground

Foreground color for an filename.

Default value: green

#### style filename background

Background color for an filename.

Default value: none

#### style filename intensity

Intensity for an filename.

Default value: normal

