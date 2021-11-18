#!/usr/bin/python
############################################################################
#                               genisslib.py                               #
#                           --------------------                           #
#   begin               :  Thu May 25 2003                                 #
AUTHORS                 = "Gilles Mouchard, Yves Lhuillier"
COPYRIGHT               = "(C) 2003-2021 CEA and Universite Paris Sud"
EMAILS                  = "gilles.mouchard@cea.fr, yves.lhuillier@cea.fr"
############################################################################

############################################################################
#                                                                          #
#    This program is free software; you can redistribute it and/or modify  #
#    it under the terms of the GNU General Public License version 2        #
#    as published by the Free Software Foundation.                         #
#                                                                          #
############################################################################

import sys, os, re, platform, itertools

DEFAULT_OUTPUT =    'iss'
GENISSLIB_VERSION = '4.1.0'
GENISSLIB =         'GenISSLib'

class Abort(BaseException):
    def __init__(self, msg):
        self.msg = msg

class TODO(BaseException):
    pass

class GIL:
    def __init__(self, callname):
        self.callname = callname
        self.inputname, self.depfilename = None, None
        self.lookupdirs = []
        self.outputprefix = DEFAULT_OUTPUT
        self.verbosity = 1
        self.minwordsize = 32
        self.sourcelines = True
        self.privatemembers = True
        self.specialization = True

    def description(self, sink):
        sink.write( "Generator of instruction set simulators.\n" )

    def version(self):
        return """\
     {name} v{version}
built date : {date}
copyright  : {copyright}
authors    : {authors}
emails     : {emails}
""".format(name=GENISSLIB, version=GENISSLIB_VERSION, date="Friday October 22nd 2021", copyright=COPYRIGHT, authors=AUTHORS, emails=EMAILS)

    def help(self,sink):
        sink.write('Usage: ' + self.callname)
        def proto(switch, sdesc, ldesc):
            if type(switch) == type(True):
                sink.write(' ' + sdesc)
        self.parse(proto)
        sink.write('\n')
        self.description(sink)
        def ots(switch, sdesc, ldesc):
            if type(switch) == type(True): return False
            l = 3 + len(sdesc)
            for optname in switch:
                l += 2 + len(optname)
            if l > ots.opttabstop: ots.opttabstop = l
        ots.opttabstop = 0
        self.parse(ots)
        sink.write('options:\n')
        def optdump(switch, sdesc, ldesc):
            if type(switch) == type(True): return False
            left = '  ' + ', '.join(switch) + ' ' + sdesc
            sink.write( left )
            sink.write( ' ' * (optdump.opttabstop - len(left)) )
            sink.write( ldesc + '\n' )
        optdump.opttabstop = ots.opttabstop
        self.parse(optdump)

    def process(self, args):
        if ('--help' in args) or ('-h' in args):
            self.help(sys.stdout)
            raise Abort(None)

        class ValueArgs:
            def __init__(self, args):
                self.args, self.idx, self.tail = args, 0, None
            def __call__(self, switch, sdesc, ldesc):
                if type(switch) == type(True):
                    return switch
                front = self.args[self.idx]
                if front not in switch:
                    if front[:2] not in switch or front[1:2] == '-':
                        return False
                    self.args[self.idx:self.idx+1] = [front[:2],front[2:]]
                self.idx += 1
                return True
            def pop_front(self):
                if self.idx >= len(self.args):
                    raise Abort('missing program argument at the end')
                front, self.idx = self.args[self.idx], self.idx + 1
                return front
            def last(self):
                return self.args[self.idx-1]
            def available(self):
                if self.idx == self.tail:
                    raise Abort('unmatched argument {!r}'.format(self.pop_front()))
                self.tail = self.idx
                return self.idx < len(self.args)
            def onoff(self):
                front = self.pop_front()
                if front == 'on':  return True
                if front == 'off': return False
                raise Abort('expected "on" or "off" after {} but got {%r}'.format(self.last(), front))
            def integer(self):
                front = self.pop_front()
                try:
                    return int(front,base=0)
                except ValueError:
                    raise Abort('expected an integer after {} (got {!r})'.format(self.last(), front))

        vargs = ValueArgs(args)

        while vargs.available():
            self.parse(vargs)

    def parse(self, match):
        """parse(self, match)

        Argument parsing definition
        """
        if   match(('-I',), '<directory>', 'Adds the directory <directory> to the search paths for include directives.'):
            self.add_lookupdir(match.pop_front())

        elif match(('-o','--output'), '<output>', 'Sets the prefix of the generated source files to <output> (default is {!r}).'.format(DEFAULT_OUTPUT) ):
            self.outputprefix = match.pop_front()

        elif match(('-w','--min-word-size'), '<size>', 'Uses <size> as the minimum bit size for holding an operand bit field.' ):
            self.minwordsize = match.integer()

        elif match( ('--specialization',), 'on/off', 'Toggles specialized operation generation (default: on).' ):
            self.specialization = match.onoff()

        elif match( ('--source-lines',), 'on/off', 'Toggles the output of source line references in generated files (default: on).' ):
            self.sourcelines = match.onoff()

        elif match( ('--private-members',), 'on/off', 'Toggles the output of private class specifier to protect generated class members (default: on).' ):
            self.privatemembers = match.onoff()

        elif match( ('-V','--version'), '', 'Displays {} version and exits.'.format(GENISSLIB) ):
            sys.stderr.write(self.version())
            raise Abort(None)

        elif match( ('-M',), '<filename>', 'Output a rule file (<filename>) suitable for make describing the dependencies of the main source file.' ):
            self.depfilename = match.pop_front()

        elif match( ('-v','--verbosity',), '<level>', 'Sets verbosity level to <level>.' ):
            self.verbosity = match.integer()

        elif match( self.inputname is None, '<inputfile>', 'The input file to process' ):
            self.inputname = match.pop_front()

    def add_lookupdir(self, dirname):
        if platform.system().startswith('CYGWIN'):
            dirname = dirname.replace('\\','/')
        self.lookupdirs.append( dirname )

    def opening(self, filename):
        self.log(1).write('Opening {!r}\n'.format(filename))

    def locate(self, filename):
        for lookupdir in self.lookupdirs:
            candidate = os.path.join(lookupdir, filename)
            if os.access(candidate, os.R_OK):
                return candidate
        return filename

    class LogVoid:
        def write(self, what): return

    logvoid = LogVoid()

    def log(self, level):
        return sys.stderr if level <= self.verbosity else self.logvoid

class ActionProto:
    def __init__(self, name, returns, param_list, constness, defaultcode):
        self.name, self.returns, self.param_list, self.constness, self.defaultcode = name, returns, param_list, constness, defaultcode

class Action:
    def __init__(self, operation, actionproto, sourcecode, fileloc):
        self.operation, self.actionproto, self.sourcecode, self.fileloc = operation, actionproto, sourcecode, fileloc

class Variable:
    def __init__(self, name, c_type, c_init):
        self.name, self.c_type, self.c_init = name, c_type, c_init

class OpCode:
    def __init__(self, capacity):
        self.capacity = capacity
        self.mask, self.bits = 0,0

    def setbits(self, beg, end, value):
        self.bits |= value << (beg % self.capacity)

    def setmask(self, beg, end, value):
        self.mask |= value << (beg % self.capacity)

    def getmask(self, beg, end):
        return (self.mask >> (beg % self.capacity)) % (1 << (end-beg))

    def getbits(self, beg, end):
        return (self.bits >> (beg % self.capacity)) % (1 << (end-beg))

    def locate(self, rhs):
        some_a_outside_B, some_b_outside_A = False, False

        if (self.mask & rhs.mask) & (self.bits ^ rhs.bits):
            return 'outside'
        if ~self.mask & rhs.mask:
            some_a_outside_B = True
        if self.mask & ~rhs.mask:
            some_b_outside_A = True

        table = ['equal', 'inside', 'contains', 'overlaps']
        return table[1*some_a_outside_B | 2*some_b_outside_A]

class Operation:
    def __init__(self, name, bitfields, fileloc):
        self.name, self.bitfields, self.fileloc = name, bitfields, fileloc
        self.actions, self.variables, self.condition = {}, {}, None

        
        self.cname = 'Op' + self.name[0].upper() + self.name[1:]

    def match_info(self, little_endian):
        def symbol(m, b):
            return str(b&1) if m & 1 else '.'

        prn = []
        for beg, end in CodeBytes(self.size, little_endian):
            bits, mask = self.code.getbits(beg, end), self.code.getmask(beg, end)
            size = min(8,end-beg)
            prn.append(''.join(symbol(mask >> bit, bits >> bit) for bit in range(size-1,-1,-1)))

        return '{{size: {}, code: {}}}'.format(self.size, ' '.join(prn))

class Group:
    def __init__(self, name, fileloc, operations):
        self.name, self.fileloc, self.operations = name, fileloc, operations

class Specialization:
    def __init__(self, operation, sloc):
        self.operation, self.sloc = operation, sloc
        self.constraints = {}

    def newop(self,log):
        name = '__spec__{}_{}'.format(self.operation.name, '_'.join( '{}_{:x}'.format(*x) for x in self.constraints.items() ))
        # Condition, actions and variables are duplicated
        unused = list(self.constraints.keys())
        op = Operation(name, [self.specbf(bf, unused) for bf in self.operation.bitfields], self.sloc)
        if unused:
            log(2).write('{}: warning: unused constraints in specialization ({!r})'.format(self.sloc, unused))
        op.condition = self.operation.condition
        op.actions = self.operation.actions
        op.variables = self.operation.variables

        return op

    def specbf(self, bf, unused):
        if not isinstance(bf,OperandBitField) or bf.name not in self.constraints:
            return bf
        unused.remove(bf.name)
        return SpOperandBitField(bf, self.constraints[bf.name])

class FileLoc:
    def __init__(self, filename, location):
        self.filename, self.location = filename, location
    def getline(self):
        return int(self.location[0])
    def __str__(self):
        return '{}:{}:{}'.format(self.filename, self.location[0], self.location[1])

class SourceCode:
    def __init__(self, content, fileloc):
        self.content, self.fileloc = content, fileloc

class WordList:
    def __init__(self,tail=None):
        self.tail = tail
        self.bitfields = []

    def append(self, bf):
        self.bitfields.append(bf)

    def ordered_bitfields(self, rev_worder, rev_forder):
        for step in (2,1,0) if rev_worder else (0,1,2):
            if      step == 0:
                if self.tail is not None:
                    for bf in self.tail.ordered_bitfields(rev_worder, rev_forder):
                        yield bf
            elif    step == 1:
                yield SeparatorBitField()
            else: # step == 2
                for bf in reversed(self.bitfields) if rev_forder else self.bitfields:
                    yield bf

# BitField + FixedSizeBitField + OpcodeBitField
#          |                   + OperandBitField
#          |                   + UnusedBitField
#          |                   + SpOperandBitField
#          + SubOpBitField
#          + SeparatorBitField

class BitField:
    def maxsize(self):
        sizes = getattr(self, 'sizes', None)
        if sizes is not None: return max(sizes())
        return getattr(self, 'size', 0)

class SeparatorBitField(BitField):
    pass

class SubOpBitField(BitField):
    def __init__(self, name, sdinstance):
        self.name, self.sdinstance = name, sdinstance

    def sizes(self):
        return self.sdinstance.sdclass.insnsizes

class FixedSizeBitField(BitField):
    def __init__(self, size):
        self.size = size

    def mask(self):
        return (1 << self.size) - 1

class OpcodeBitField(FixedSizeBitField):
    def __init__(self, size, bits):
        FixedSizeBitField.__init__(self, size)
        self.bits = bits

class UnusedBitField(FixedSizeBitField):
    def __init__(self, size):
        FixedSizeBitField.__init__(self, size)

class OperandBitField(FixedSizeBitField):
    def __init__(self, size, name, lshift, final_size, sext):
        FixedSizeBitField.__init__(self, size)
        self.name, self.lshift, self.final_size, self.sext = name, lshift, final_size, sext

    def dstsize(self):
        return max( self.size + self.lshift, self.final_size )

class SpOperandBitField(OperandBitField):
    def __init__(self, opbf, bits):
        OperandBitField.__init__(self, opbf.size, opbf.name, opbf.lshift, opbf.final_size, opbf.sext)
        self.bits = bits

    def constval(self):
        if not self.sext:
            return hex(self.bits);
        sign_bit = 1 << (op.size-1)
        return hex((self.bits & (sign_bit - 1)) - (self & sign_bit))

class SDClass:
    def __init__(self, nmspc, insnsizes, sdloc):
        self.nmspc, self.insnsizes = nmspc, insnsizes
        # Creating and artificial source code with the normalized
        # namespace and the file location where subdecoder was declared
        self.nmcode = SourceCode( '::'.join(nmspc), sdloc )

class SDInstance:
    def __init__(self, name, tpscheme, sdclass, sdloc):
        self.name, self.template_scheme, self.sdclass, self.sdloc = name, tpscheme, sdclass, sdloc

def FieldIterator( bitfields, little_endian ):
    beg, end = 0, 0

    for bitfield in bitfields:
        size = bitfield.maxsize()
        if little_endian:  beg, end = end, end + size
        else:              beg, end = beg - size, beg
        yield beg, end, bitfield

def CodeBytes( size, little_endian ):
    beg, end = 0, 0

    for idx in range((size + 7) >> 3):
        if little_endian:  beg, end = end, end + 8
        else:              beg, end = beg - 8, beg
        yield max(beg,-size), min(end,size)

class Isa:
    def __init__(self):
        self.decoder = ('scalar',None) #< Decoder type
        self.is_subdecoder = False     #< Subdecoder or full decoder
        self.withcache = False         #< Generate cache structure
        self.withsource = False        #< Action source code accessible or not
        self.withencode = False        #< Action source code accessible or not
        self.little_endian = False     #< Endianness of isa (false: big endian, true: little endian)
        self.asc_forder = False        #< bitfields ordering (false: descending)
        self.asc_worder = False        #< words ordering (false: descending)
        self.minwordsize = 0           #< minimum C type size for operand fields
        self.namespace = ()            #< Encapsulating namespace of the iss
        self.tparams = []              #< Template parameters of the iss
        self.variables = {}            #< Global variables used by the iss
        self.actionprotos = {}         #< Action prototypes of operations
        self.opmap = {}                #< Defined instructions
        self.operations = []           #< Defined instructions
        self.groups = {}               #< Defined groups
        self.sdclasses = {}            #< Defined subdecoder classes
        self.sdinstances = {}          #< Defined subdecoder instances
        self.decl_srccodes = []        #< Code to insert in header file
        self.impl_srccodes = []        #< Code to insert in source file
        self.addrtype = None           #< C type for instructions addresses
        self.includes = []             #< files included by the isa main file
        self.specializations = []      #< Requested specializations
        self.inheritances = []         #< Defined inheritances for operation class
        self.group_accs = {}           #< Active group accumulators
        self.user_orderings = []       #< Explicit user information about operation ordering

    def add_operation(self, op):
        """Add an operation object to the global operation list and to active group accumulators
        """
        self.check_name_validity("operation", op.name, op.fileloc)
        self.opmap[op.name] = op
        self.operations.append(op)
        for ga in self.group_accs.values():
            ga.operations.add( op )

    def check_name_validity(self, kind, name, floc):
        for collection, objtype in ((self.opmap,'operation'),(self.groups,'group')):
            obj = collection.get(name)
            if obj is None:
                continue
            msg='{}: {} name conflict error\n{}: {} {!r} previously defined here\n'
            raise Abort(msg.format(floc, kind, obj.fileloc, objtype, name) )

    class NotFound(BaseException):
        def __init__(self, name=None):
            self.name = name

    def ops(self, name):
        op = self.opmap.get(name)
        if op is not None:
            yield op
        grp = self.groups.get(name)
        if grp is not None:
            for op in grp.operations:
                yield op
        elif op is None:
            raise self.NotFound(name)

    def generator(self, opts):
        decoder, decfl = self.decoder
        if   decoder == 'scalar': return ScalarGenerator(self, opts)
        elif decoder == 'buffer': return BufferGenerator(self, opts)
        else: raise Abort('{} unknown decoder {!r}'.format(decfl, decoder))

    def deps(self, sink, prefix):
        """Dump to <sink> a rule file suitable for make (when building <prefix>).
        describing the dependencies of the main source file."""
        sink.write( '{0}.{1} {0}.hh:'.format(prefix, 'tcc' if self.tparams else 'cc') )
        for include in self.includes:
            sink.write( ' \\\n' + include)
        sink.write('\n\n')

    def sanity_checks(self):
        if self.group_accs:
            raise Abort('parse error: unclosed groups: {!r}\n'.format(self.group_accs))
        if self.addrtype is None:
            raise Abort('error: no architecture address type given.')

        if not self.operations:
            raise Abort('error: no operation in ISA description.')

        # Looking for potential operation member variable name conflicts
        gvar = {v.name: v.c_type.fileloc for v in self.variables.values()}
        for op in self.operations:
            chk = gvar.copy()
            for bf in op.bitfields:
                name = getattr(bf, 'name', None)
                if name is None: continue
                oldfloc = chk.get(name)
                if oldfloc is not None:
                    msg = '{}: bitfield name conflict {!r} in operation {!r}\n{}: conflicting name here'
                    raise Abort(msg.format(op.fileloc, name, op.name, oldfloc))
                chk[name] = op.fileloc

            for v in op.variables.values():
                oldfloc = chk.get(v.name)
                if oldfloc is not None:
                    msg = '{}: variable name conflict {!r} in operation {!r}\n{}: conflicting name here'
                    raise Abort(msg.format(v.c_type.fileloc, v.name, op.name, oldfloc))
                chk[name] = v.c_type.fileloc

    def specialize(self, log):
        for spec in self.specializations:
            op = spec.newop( log )
            self.check_name_validity("operation",op.name,op.fileloc)
            self.opmap[op.name] = op
            self.operations.append(op)

    def setparam(self, key, value, valfl):
        dirdict = {'fields_order': 'asc_forder', 'words_order':'asc_worder'}
        booldict = {'subdecoder_p': 'is_subdecoder', 'withcache_p': 'withcache', 'withsource_p': 'withsource', 'withencode_p': 'withencode'}
        if key == 'codetype':
            self.decoder = (value, valfl)

        elif key in booldict:
            varname = booldict[key]
            if   value == 'true':  setattr(self,varname,True)
            elif value == 'false': setattr(self,varname,False)
            else:
                raise Abort('{}: expected a boolean for property {} (got: {!r})'.format(valfl,key,value))

        elif key == 'endianness':
            if   value == 'little': self.little_endian = True
            elif value == 'big':    self.little_endian = False
            else:
                raise Abort('{}: expected [little|big] for property endianness (got: {!r})'.format(valfl,value))
            return

        elif key in dirdict:
            varname = dirdict[key]
            if   value == 'descending': setattr(self,varname,False)
            elif value == 'ascending':  setattr(self,varname,True)
            else:
                raise Abort('{}: expected a [descending|ascending] for property {} (got: {!r})'.format(valfl,key,value))

        elif key == 'addressclass':
            # TODO: keep (and exploit) the source code ?
            self.addrtype = value.content

        elif key == 'minwordsize':
            if not isinstance(value, int):
                raise Abort('{}: expected an int for property minwordsize (got: {!r})'.format(valfl,value))
            self.minwordsize = value

        else:
            raise Abort('{}: unknown property {}'.format(valfl,key))

    def dshift(self):
        return '>>' if self.little_endian else '<<'

    def ashift(self):
        return '<<' if self.little_endian else '>>'

    def rev_forder(self):
        return bool(self.asc_forder) != bool(self.little_endian)

    def rev_worder(self):
        return bool(self.asc_worder) != bool(self.little_endian)

    def maxsize(self):
        return max(self.insnsizes)

    def finalize(self, log):
        if self.is_subdecoder:
            self.withcache = False

        # Computing instruction size properties
        self.insnsizes = set()
        for op in self.operations:
            vlen, common = False, 0
            insnsizes = set([0])
            for bitfield in op.bitfields:
                common += getattr(bitfield,'size',0)
                bfsizes = getattr(bitfield,'sizes',None)
                if bfsizes is not None:
                    vlen = True
                    insnsizes = set(x+y for x,y in itertools.product(insnsizes, bfsizes()))
                    continue
            op.size = max(insnsizes) + common
            op.vlen = vlen
            self.insnsizes.update(x+common for x in insnsizes)

        # Creating OpCode structures
        for op in self.operations:
            op.code = OpCode(self.maxsize())
            vlen = False
            for beg, end, bf in FieldIterator(op.bitfields, self.little_endian):
                if hasattr(bf,'sizes'):
                    vlen = True
                    continue
                fbits = getattr(bf,'bits',None)
                if fbits is None:
                    continue
                # has opcode
                assert not vlen
                op.code.setbits(beg, end, fbits)
                op.code.setmask(beg, end, bf.mask())

        # Performing Topological Sort
        class OpNode:
            def __init__(self, op):
                self.op, self.belowlist, self.abovecount = op, set(), 0

            def ontopof(self, nbelow):
                redundant = None
                for below in self.belowlist:
                    if nbelow is below: return
                    loc = nbelow.op.code.locate(below.op.code)
                    if loc == 'inside': return
                    if loc != 'contains': continue
                    # edge is redundant
                    below.abovecount -= 1
                    self.belowlist.remove(below)
                    # No more related edges
                    break
                self.belowlist.add(nbelow)
                nbelow.abovecount += 1

            def above(self,op):
                seen = set()
                front = set(self.belowlist)

                while front:
                    seen.update(front)
                    front, visit = set(), front
                    for pivot in visit:
                        if pivot is op:
                            return True
                        for n in pivot.belowlist:
                            if n in seen:
                                continue
                            front.add(n)
                return False

            def unlink(self):
                for below in self.belowlist:
                    below.abovecount -= 1
                self.belowlist = set()

        nodes = {op:OpNode(op) for op in self.operations}

        # 1. Computing implicit edges
        for n1, n2 in itertools.combinations_with_replacement(nodes.values(),2):
            if n1.op is n2.op:
                continue
            oc1, oc2 = n1.op.code, n2.op.code
            loc = oc1.locate(oc2)
            if loc == 'equal': raise Abort('duplicated encoding:\n{}: operation {!r} and,\n{} operation {!r}'.format(n1.op.fileloc,n1.op.name,n2.op.fileloc,n2.op.name))
            elif loc == 'contains': n1.ontopof(n2)
            elif loc == 'inside': n2.ontopof(n1)

        # 2. log if verbose enough
        if hasattr(log(2),'name'):
            for n in nodes.values():
                if not n.belowlist:
                    continue
                log(2).write('operation {} is a specialization of operation {}\n'.format(n.op.name, ', '.join(x.op.name for x in n.belowlist) ))

        # 3. Adding explicit edges
        for sloc, top_op, under_ops in self.user_orderings:
            abovelist, belowlist = set(), set()
            try:
                for op in self.ops(top_op):
                    abovelist.add(op)
                for under_op in under_ops:
                    for op in self.ops(under_op):
                        belowlist.add(op)
            except self.NotFound as nf:
                raise Abort('{}: no such operation or group {!r}'.format(sloc, nf.name))

            for above, below in itertools.product(abovelist, belowlist):
                loc = above.code.locate(below.code)
                action = 'specialization of {!r} with {!r}'.format(below.name,above.name)
                if   loc == 'outside':
                    log(1).write('{}: warning: useless {} (no relation)\n'.format(sloc, action))
                elif loc == 'contains':
                    log(1).write('{}: warning: useless {} (implicit specialization)\n'.format(sloc, action))
                elif loc == 'inside':
                    raise Abort('{}: error: illegal {} (would hide the former)'.format(sloc, action))
                elif loc == 'overlaps':
                    nodes[above].ontopof(nodes[below])
                    log(2).write('info: forced {}\n'.format(action))

        # 4. Finally check if overlaps are resolved
        for n1, n2, in itertools.combinations_with_replacement(nodes.values(),2):
            if n1 is n2:
                continue
            if n1.op.code.locate(n2.op.code) != 'overlaps' or n1.above(n2) or n2.above(n1):
                continue
            msg  = '{}: error: operation {!r} conflicts with\n'.format(n1.op.fileloc, n1.op.name)
            msg += '{}: operation {!r}\n'.format(n2.op.fileloc, n2.op.name)
            msg += '{}: {}\n'.format(n1.op.name, n1.op.match_info(self.little_endian))
            msg += '{}: {}\n'.format(n2.op.name, n2.op.match_info(self.little_endian))
            raise Abort(msg)

        # 5. perform the topological sort itself
        noperations = []
        opcount = len(self.operations)
        sopidx = opcount
        counter = opcount
        inf_loop_tracker = opcount

        while counter > 0:
            sopidx = (sopidx + opcount - 1) % opcount
            op = self.operations[sopidx]
            node = nodes[op]
            if node is None: continue
            if node.abovecount > 0:
                # There is some operations to be placed before this one
                --inf_loop_tracker
                assert inf_loop_tracker >= 0
                continue
            nodes[op] = None
            counter -= 1
            inf_loop_tracker = opcount
            noperations.append(node.op)
            node.unlink()
        self.operations = noperations[::-1]

    def dumpstats(self, log):
        sink = log(1)
        if not hasattr(sink, 'name'):
            return
        sink.write('Instruction Set info:\n')
        if len(self.insnsizes) == 1:
            insnsize = next(iter(self.insnsizes))
        else:
            insnsize = '{} (gcd={})'.format(sorted(self.insnsizes), self.gcd())
        sink.write('  Size: {}\n  Encoding: {}\n  Count: {}\n'.format(insnsize, "little-endian" if self.little_endian else "big-endian", len(self.operations)))
        #   /* Statistics about actions */
        actionstats = {}
        log(3).write("  Action details:\n")
        for op in self.operations:
            log(3).write( '    {}:'.format(op.name) )
            for ap, action in op.actions.items():
                log(3).write( ' .' + ap )
                actionstats[ap] = actionstats.get(ap,0) + 1
            log(3).write( '\n' )

        if hasattr(log(2), 'name'):
            log(2).write('  Action count:\n')
            for name, count in actionstats.items():
                log(2).write( '   .{}: {}\n'.format(name, count) )

    def gcd(self):
        res = next(iter(self.insnsizes))
        for cur in self.insnsizes:
            while True:
                rem = cur % res
                if rem == 0: break
                cur, res = res, rem
        return res

def isname( ch ):
    return ch.isalpha() or ch == '_'

class Scanner:
    def __init__(self, filename):
        self.sourcename = filename
        try:
            self.reader = self.read2(open(filename))
        except FileNotFound:
            raise Abort('cannot open {!r}'.format(filename))
        self.loc = None

    # Track position
    def read0(self, source):
        lidx, cidx = 1, 1
        while True:
            res = source.read(1)
            yield (res, (lidx, cidx))
            if res == '\n': lidx, cidx = lidx+1, 1
            else:           cidx += 1

    # Skip escaped newline
    def read1(self, source):
        source = self.read0(source)
        while True:
            tok = next(source)
            if tok[0] != '\\':
                yield tok
                continue
            ntk = next(source)
            if ntk[0] == '\n':
                continue
            yield tok
            yield ntk

    # Recognize spaces, comments, alphanumerical tokens and strings
    def read2(self, source):
        source = self.read1(source)
        tok = next(source)
        while True:
            nc, lexloc = tok
            if nc.isspace():
                content = ''
                while nc.isspace():
                    content += nc
                    tok = next(source)
                    nc = tok[0]
                yield (content, lexloc)
                continue

            if nc == '/':
                # Maybe a comment
                tok1 = next(source)
                style = tok1[0]
                if  style == '*':
                    comment= '/*' + next(source)[0] + next(source)[0]
                    while comment[-2:] != '*/':
                        comment += next(source)[0]
                elif style == '/':
                    comment = '//'
                    while comment[-1] != '\n':
                        comment += next(source)[0]
                else:
                    # not a comment
                    yield tok
                    tok = tok1
                    continue

                yield (comment, lexloc)
                tok = next(source)
                continue

            if nc.isalpha() or nc.isdigit() or nc == '_':
                content = ''
                while nc.isalpha() or nc.isdigit() or nc == '_':
                    content += nc
                    tok = next(source)
                    nc = tok[0]
                yield (content, lexloc)
                continue

            if nc == '"' or nc == "'":
                delim, content = nc, nc

                while True:
                    nc = next(source)[0]
                    content += nc
                    if nc == delim:
                        break
                    if nc == '\\':
                        content += next(source)[0]

                yield (content, lexloc)
                tok = next(source)
                continue

            if nc == ':':
                ntok = next(source)
                if ntok[0] != ':':
                    yield tok
                    tok = ntok
                    continue
                yield ('::', lexloc)
                tok = next(source)
                continue

            yield tok
            tok = next(source)

    # Return next lexeme, skiping spaces and comments
    def getLexeme(self):
        nl = ' '
        while nl[:1].isspace() or nl.startswith('//') or nl.startswith('/*'):
            nl, self.loc = next(self.reader)
        return nl

    def expect(self, lx, nl):
        if nl != lx:
            self.parse_error( 'expected {!r} here (got {!r})'.format(lx, nl) )

    def expectName(self, name):
        if not isname( name[0] ):
            self.parse_error('expected a name here (got {!r})'.format(name))
        return name

    def expectInt(self, value):
        try:
            # Python2 and 3 don't handle octal notations the same way
            # and historically genisslib does not conisder octal
            # notation at all...  Legacy PPC ISA imposes '0[0-9]+' to
            # be treated as decimal. Python3's convention (explicit
            # '0o') solves backward compatibility and allow octal
            # notation introduction so we now align with Python3.
            if value.startswith('0o'):
                value = int(value[2:], 8)
            elif value.startswith('0') and not value[1:2].isalpha():
                value = int(value, 10)
            else:
                value = int(value, 0)
        except ValueError:
            self.parse_error('expected an integer here (got {!r})'.format(value))
        return value

    def getArraySize(self):
        self.expect('[', self.getLexeme())
        res = self.expectInt(self.getLexeme())
        self.expect(']', self.getLexeme())
        return res

    def getSourceCode(self):
        depth = 0
        content = ''
        scfl = self.getFileLoc()
        while True:
            nl, loc = next(self.reader)
            if    nl == '{': depth += 1
            elif  nl == '}': depth -= 1
            if depth < 0: break
            content += nl
        return SourceCode(content, scfl)

    def getVarListAndLexeme(self):
        nxt, varlist = ',', []
        while nxt == ',':
            varname = self.expectName(self.getLexeme())
            self.expect(':', self.getLexeme())
            self.expect( '{}'[0], self.getLexeme() )
            c_type = self.getSourceCode()
            c_init = None
            nxt = self.getLexeme()
            if nxt == '=':
                self.expect( '{}'[0], self.getLexeme() )
                c_init = self.getSourceCode()
                nxt = self.getLexeme()
            varlist.append( Variable(varname, c_type, c_init) )
        return varlist, nxt

    def getNamespaceAndLexeme(self):
        nmspc = []
        nxt = '::'
        while nxt == '::':
            name = self.expectName(self.getLexeme())
            nmspc.append(name)
            nxt = self.getLexeme()
        return tuple(nmspc), nxt


    def getOp(self,isa):
        name = self.expectName(self.getLexeme())
        namefl = self.getFileLoc()
        self.expect( '()'[0], self.getLexeme() )
        wordlist = WordList()
        while True:
            top = self.getLexeme()
            if top == '>':
                self.expect( '<', self.getLexeme() )
                wordlist = WordList(wordlist)
            elif top[0].isdigit():
                bits = self.expectInt(top)
                size = self.getArraySize()
                if (bits >> size) != 0:
                    self.parse_error( 'opcode overflow ({}[%u])'.format(top, size) )
                wordlist.append( OpcodeBitField( size, bits ) )
            elif top == '?':
                size = self.getArraySize()
                wordlist.append( UnusedBitField( size ) )
            elif top == '*':
                # SubOp
                bfname = self.expectName(self.getLexeme())
                self.expect( '[]'[0], self.getLexeme() )
                sdname = self.expectName(self.getLexeme())
                self.expect( '[]'[1], self.getLexeme() )
                sdinstance = isa.sdinstances.get( sdname )
                if sdinstance is None:
                    self.parse_error( 'subdecoder instance {!r} not declared'.format(sdname) )
                wordlist.append( SubOpBitField( bfname, sdinstance ) )
            else:
                lshift, sext = 0, False
                if isname( top ):
                    while True:
                        if top == 'shl':
                            self.expect( '<', self.getLexeme() )
                            lshift = self.expectInt(self.getLexeme() )
                            self.expect( '>', self.getLexeme() )
                        elif top == 'sext':
                            sext = True
                        else:
                            break
                        top = self.getLexeme()
                        if not isname(top):
                            break
                final_size = 0
                if top == '<':
                    final_size = self.expectInt(self.getLexeme())
                    self.expect('>', self.getLexeme())
                    top = self.getLexeme()

                self.expectName(top)
                size = self.getArraySize()
                wordlist.append(OperandBitField( size, top, lshift, final_size, sext ))
                #self.parse_error('unexpected {!r}'.format(top))
            top = self.getLexeme()
            if top == '()'[1]:
                break
            self.expect( ':', top )

        return Operation(name, list(wordlist.ordered_bitfields(isa.rev_worder(), isa.rev_forder())), namefl)

    def getFileLoc(self):
        return FileLoc(self.sourcename, self.loc)

    def parse_error(self, msg):
        raise Abort( 'parse error:\n{}: {}'.format(self.getFileLoc(), msg) )

def parse(filename, app, isa):
    app.opening(filename)
    isa.includes.append(filename)
    source = Scanner(filename)

    root = source.getLexeme()

    while root != '':
        rootloc = source.getFileLoc()

        if root == ';':
            root = source.getLexeme()

        elif root == '{}'[0]:
            sc = source.getSourceCode()
            root = source.getLexeme()
            if root == ':':
                source.expect( 'op', source.getLexeme() )
                op = source.getOp(isa)
                op.condition = sc
                isa.add_operation(op)
            else:
                isa.decl_srccodes.append(sc)
            root = source.getLexeme()

        elif not isname( root[0] ):
            source.parse_error('expected a name here.')

        elif root == 'decl' or root == 'impl':
            codes = isa.decl_srccodes if root == 'decl' else isa.impl_srccodes
            source.expect( '{}'[0], source.getLexeme() )
            codes.append(source.getSourceCode())
            root = source.getLexeme()

        elif root == 'action':
            returns = None
            symbol = source.getLexeme()
            if symbol == '{}'[0]:
                returns = source.getSourceCode()
                symbol = source.getLexeme()
            aploc = source.getFileLoc()
            prev = isa.actionprotos.get(symbol)
            if prev is not None:
                source.parse_error( 'action prototype {!r} redefined\n{}: previously defined here'.format(symbol, prev.fileloc) )
            source.expect( '()'[0], source.getLexeme() )

            param_list = []
            while True:
                param = source.getLexeme()
                if param == '()'[1]: break
                pair = [None,None]
                for idx in range(2):
                    if param != '{}'[0]:
                        source.parse_error("expected {}.".format('{}'[0]))
                    pair[idx] = source.getSourceCode()
                    param = source.getLexeme()
                param_list.append(tuple(pair))
                if param == '()'[1]: break
                if param != ',':
                    source.parse_error('expected {!r} or {!r} (got {!r})'.format(',', '()'[1], param))

            root = source.getLexeme()
            isconst = False
            if root == 'const':
                isconst = True
                root = source.getLexeme()

            source.expect( '{}'[0], root )
            default_sourcecode = source.getSourceCode()
            ap = ActionProto(symbol, returns, param_list, isconst, default_sourcecode)
            isa.actionprotos[ap.name] = ap
            root = source.getLexeme()

        elif root == 'include':
            filename = source.getLexeme()
            if filename[0] != '"':
                source.parse_error( 'expected a string here (got {!r})'.format(filename) )
            filename = app.locate( eval(filename) )
            # TODO: dont force absolute please...
            if not os.path.isabs(filename):
                filename = os.getcwd() + '/' + filename
            try:
                parse(filename, app, isa)
            except Abort as abort:
                raise Abort( 'From {}\n'.format(source.getFileLoc()) + abort.msg )
            root = source.getLexeme()

        elif root == 'namespace':
            nmspc, root = source.getNamespaceAndLexeme()
            isa.namespace += nmspc

        elif root == 'set':
            key = source.expectName(source.getLexeme())
            value = source.getLexeme()
            valfl = source.getFileLoc()
            if value == '{}'[0]:
                isa.setparam(key, source.getSourceCode(), valfl)
            elif isname( value[0] ):
                isa.setparam(key, value, valfl)
            elif isdigit( value[0] ):
                isa.setparam(key, source.expectInt(value), valfl)
            else:
                source.parse_error('not a valid set-value')
            root = source.getLexeme()

        elif root == 'subdecoder':
            nmspc, root = source.getNamespaceAndLexeme()
            sdclass = isa.sdclasses.get(nmspc)
            sdloc = source.getFileLoc()
            if root == '[]'[0]:
                if sdclass is not None:
                    source.parse_error('subdecoder class redeclared.\n{}: subdecoder class previously declared here.'.format(sdclass.fileloc))
                insnsizes = set()
                value = ','
                while value == ',':
                    value = source.getLexeme()
                    if not value[0].isdigit():
                        source.parse_error('expected an integer value')
                    insnsizes.add(int(value, 0))
                    value = source.getLexeme()
                if value != '[]'[1]:
                    source.parse_error('expected {!r} or {!r} (got {!r})'.format(',', '[]'[1], value))
                prev = isa.sdclasses.get(nmspc)
                if prev is not None:
                    source.parse_error( 'subdecoder {!r} redeclared\n{}: previously declared here'.format(nmpsc, prev.fileloc) )
                isa.sdclasses[nmspc] = SDClass(nmspc, insnsizes, sdloc)
                root = source.getLexeme()
            elif isname( root[0] ):
                instname = root
                if sdclass is None:
                    source.parse_error('subdecoder has not been declared')
                sdinstance = isa.sdinstances.get(root)
                if sdinstance is not None:
                    source.parse_error('subdecoder instance {!r} redefined.\n{} previously defined here.'.format(root,sdinstance.fileloc))
                root = source.getLexeme()
                tpscheme = None
                if root == '<>'[0]:
                    source.expect( '{}'[0], source.getLexeme() )
                    tpscheme = source.getSourceCode()
                    source.expect( '<>'[1], source.getLexeme() )
                    root = source.getlexeme()
                isa.sdinstances[instname] = SDInstance(instname, tpscheme, sdclass, sdloc)
            else:
                source.parse_error('expected name or array, got {!r}'.format(root))

        elif root == 'template':
            source.expect( '<>'[0], source.getLexeme() )
            sep = ','
            while sep == ',':
                pair = [None,None]
                for idx in range(2):
                    source.expect( '{}'[0], source.getLexeme() )
                    pair[idx] = source.getSourceCode()
                isa.tparams.append(tuple(pair))
                sep = source.getLexeme()
            if sep != '<>'[1]:
                source.parse_error('expected {!r} here'.format('<>'[1]))
            root = source.getLexeme()

        elif root == 'op':
            isa.add_operation(source.getOp(isa))
            root = source.getLexeme()

        elif root == 'specialize':
            name = source.expectName(source.getLexeme())
            namefl = source.getFileLoc()
            operation = isa.opmap.get( name )
            if operation is None:
                source.parse_error('operation {!r} not defined'.format(name))
            spec = Specialization(operation, namefl)

            source.expect( '()'[0], source.getLexeme() )

            param_list = []
            sep = ','
            while sep == ',':
                name = source.expectName(source.getLexeme())
                source.expect( '=', source.getLexeme() )
                value = source.expectInt(source.getLexeme())
                prev = spec.constraints.get(name)
                if prev is not None:
                    source.parse_error('bitfield {!r} specialized multiple times'.format(name))
                spec.constraints[name] = value
                sep = source.getLexeme()
                if sep == '()'[1]: break
                if sep != ',':
                    source.parse_error('expected [,{}] got {!r}'.format('()'[1], sep))
            isa.specializations.append(spec)
            root = source.getLexeme()

        elif root == 'var':
            variables, root = source.getVarListAndLexeme()
            for v in variables:
                p = isa.variables.get(v.name)
                if p is not None:
                    msg = 'parse error:\n{}: variable {!r} redefined\n{}: previously defined here.'
                    raise Abort( msg.format(v.c_type.fileloc, v.name, p.c_type.fileloc) )
                isa.variables[v.name] = v

        elif root == 'group':
            name = source.expectName(source.getLexeme())
            namefl = source.getFileLoc()
            cmd = source.getLexeme()
            if cmd == 'begin':
                isa.check_name_validity("group", name, namefl)
                grp = Group(name, namefl, set())
                isa.group_accs[name] = grp
                isa.groups[name] = grp
            elif cmd == 'end':
                del isa.group_accs[name]
            elif cmd == '()'[0]:
                isa.check_name_validity("group", name, namefl)
                ops = set()
                sep = ','
                while sep == ',':
                    param = source.expectName(source.getLexeme())
                    try:
                        for op in isa.ops(param):
                            ops.add(op)
                    except isa.NotFound:
                        source.parse_error('unknown operation or group {!r}'.format(param))
                    sep = source.getLexeme()
                if sep != '()'[1]:
                    source.parse_error('expected {!r} here'.format('()'[1]))
                isa.groups[name] = Group(name, namefl, ops)
            else:
                source.parse_error('expected command or name list')
            root = source.getLexeme()
        else:
            name = root
            source.expect( '.', source.getLexeme() )
            attribute = source.expectName(source.getLexeme())
            attrfl = source.getFileLoc()
            if attribute == 'var':
                variables, root = source.getVarListAndLexeme()
                try:
                    for op in isa.ops(name):
                        for v in variables:
                            p = op.variables.get(v.name)
                            if p is not None:
                                msg = 'parse error:\n{}: variable {!r} redefined\n{}: previously defined here.'
                                raise Abort( msg.format(v.c_type.fileloc, v.name, p.c_type.fileloc) )
                            op.variables[v.name] = v
                except isa.NotFound:
                    raise Abort( '{}: unknown operation or group {!r}'.format(rootloc, name) )
            elif attribute == 'specialize':
                source.expect( '()'[0], source.getLexeme() )
                under_ops = []
                sep = ','
                while sep == ',':
                    under_ops.append( source.expectName(source.getLexeme()) )
                    sep = source.getLexeme()
                source.expect( '()'[1], sep )
                isa.user_orderings.append( (rootloc, name, under_ops) )
                root = source.getLexeme()
            else:
                source.expect( '=', source.getLexeme() )
                source.expect( '{}'[0], source.getLexeme() )
                sc = source.getSourceCode()
                ap = isa.actionprotos.get(attribute)
                if ap is None:
                    raise Abort('{}: error: undefined action prototype {!r}', attrfl, attribute)
                try:
                    for op in isa.ops(name):
                        prev = op.actions.get( ap.name )
                        if prev is not None:
                            redef = "action '{}.{}' redefined\n{}: previously defined here"
                            source.parse_error( redef.format(name, attribute, prev.fileloc) )
                        op.actions[ap.name] = Action(op, ap, sc, attrfl)
                except isa.NotFound:
                    source.parse_error('unknown operation or group {!r}'.format(name))
                root = source.getLexeme()

def tokenize(s):
    return ''.join(c if c.isalnum() else '_' for c in s)

def least_ctype_size(bits):
    size = 8
    while size < bits:
        size *= 2
    if size > 64:
        raise Abort('Cannot encode more than 64 bits with standard C/C++ integer types')
    return size

class Product:
    def __init__(self, filename, sourcelines):
        self.filename, self.sourcelines = filename, sourcelines
        self.line, self.lineno, self.indentations = '', 1, [0]

    def code(self, fmt, *args):
        return self.write( fmt % args )

    def usercode(self, sourcecode, before=None, after=None):
        if self.sourcelines:
            self.require_newline()
            self.code( "#line %u \"%s\"\n", sourcecode.fileloc.getline(), sourcecode.fileloc.filename )

        if before is not None: self.write(before)
        self.write( sourcecode.content )
        if after is not None: self.write(after)

        if self.sourcelines:
            self.require_newline()
            self.code( "#line %u \"%s\"\n", self.lineno + 1, self.filename )

        return self

    def flatten_indentation(self):
        if not self.indentations:
            return
        try:
            flat = next(x for x in reversed(self.indentations) if x < self.indentations[-1])
        except StopIteration:
            return
        self.indentations = [min(flat, x) for x in self.indentations]

    def ns_enter(self, namespace):
        self.write( ' '.join( "namespace %s {" % part for part in namespace ) )
        self.write( '\n' )
        self.flatten_indentation()
        return self

    def ns_leave(self, namespace):
        # TODO: "{" self.write( ' '.join( "} /*end of namespace %s*/" % part for part in namespace[::-1] ) )
        self.write( ' '.join( "}" for part in namespace[::-1] ) )
        self.write( '\n' )
        return self

    def require_newline(self):
        if self.line:
            self.write( '\n' )
        return self

    def template_signature(self, tparams):
        if not tparams:
            return self
        self.write('template <')
        sep = ''
        for ctype, csymbol in tparams:
            self.write( sep ).write('\t').usercode(ctype).write('\t').usercode(csymbol)
            sep = ','
        self.write('>\n')
        return self

    def template_abbrev(self, tparams):
        if not tparams:
            return self
        self.write('<')
        sep = ''
        for ctype, csymbol in tparams:
            self.write( sep ).write('\t').usercode(csymbol)
            sep = ','
        self.write('>')
        return self

    def write(self, chars):
        if chars is None:
            return self
        for ch in chars:
            if ch == '\n':
                line = self.line.rstrip()
                if line:
                    current_indentation = self.indentations[-1]
                    # Computing brace depth and right stripping blank characters.
                    braces = line.count('{') - line.count('}')
                    if braces > 0:
                        for x in range(braces-1):
                            self.indentations.append( current_indentation )
                        self.indentations.append( current_indentation + 1 )
                    elif braces < 0:
                        nlength = len(self.indentations) + braces
                        if nlength > 0:
                            self.indentations = self.indentations[:nlength]
                        else:
                            sys.stderr.write( '{}:{}: internal indentation error.\n'.format(self.filename, self.lineno))
                            self.indentations = [0]

                    if line[0] == '#':               current_indentation = 0
                    elif line[0] == '{}'[1]:         current_indentation = self.indentations[-1]
                    elif line[-1] == ':':            current_indentation -= 1

                    self.xwrite( current_indentation*'\t')
                    self.xwrite( line )

                self.xwrite('\n')
                self.line = ''
                self.lineno += 1
            elif self.line or not ch.isspace():
                self.line += ch
        return self

class FProduct(Product):
    def __init__(self, filename, sourcelines):
        Product.__init__(self, filename, sourcelines)
        try:
            self.sink = open(filename, 'w')
        except FileNotFoundError:
            raise Abort('cannot open target file {!r}.\n'.format(filename))

    def xwrite(self, chars):
        self.sink.write( chars )

class SProduct(Product):
    def __init__(self, filename, sourcelines):
        Product.__init__(self, filename, sourcelines)
        self.sink = []

    def xwrite(self, chars):
        self.sink.append( chars )

    def string(self):
        return ''.join(self.sink)

class Generator:
    def __init__(self, source, options):
        self.source, self.options = source, options
        self.minwordsize = least_ctype_size(max(options.minwordsize, source.minwordsize))

    def iss(self):
        # Header File
        sink = FProduct( self.options.outputprefix + ".hh", self.options.sourcelines )
        sink.code( "/*** File generated by %s version %s: DO NOT MODIFY ***/\n", GENISSLIB, GENISSLIB_VERSION )
        headerid = '__{}_{}_HH__'.format(tokenize(self.options.outputprefix), '__'.join(self.source.namespace))

        sink.code( "#ifndef %s\n", headerid )
        sink.code( "#define %s\n", headerid )
        sink.code( """
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <vector>
#include <inttypes.h>
#include <cstring>
""" )

        self.additional_decl_includes( sink )
        sink.ns_enter( self.source.namespace )
        self.codetype_decl( sink )
        self.decoder_decl( sink )
        sink.ns_leave( self.source.namespace )

        for sc in self.source.decl_srccodes:
            sink.usercode(sc)

        sink.ns_enter( self.source.namespace )

        self.operation_decl( sink )

        sink.ns_leave( self.source.namespace )

        sink.code( "#endif\n" )

        del sink # flush

        # Source File
        sink = FProduct( self.options.outputprefix + (".tcc" if self.source.tparams else ".cc" ), self.options.sourcelines )

        # sink.code( "/*** File generated by %s version %s: DO NOT MODIFY ***/\n", GENISSLIB, GENISSLIB_VERSION )

        sink.code( "#include \"%s.hh\"\n", self.options.outputprefix )

        self.additional_impl_includes( sink )

        for sc in self.source.impl_srccodes:
            sink.usercode(sc)

        sink.ns_enter( self.source.namespace )

        self.codetype_impl( sink )
        self.operation_impl( sink )

        self.isa_operations_decl( sink )
        self.isa_operations_methods( sink )
        self.isa_operations_ctors( sink )
        if self.source.withencode:
            self.isa_operations_encoders( sink )

        self.decoder_impl( sink )

        sink.ns_leave( self.source.namespace )

        del sink # flush

        # Subdecoder header file

        if self.source.is_subdecoder:
            sink = FProduct( self.options.outputprefix + "_sub.isa", self.options.sourcelines )

            sink.code( "subdecoder %s [%s]\n", '::'.join(self.source.namespace), ','.join(str(x) for x in sorted(self.source.insnsizes)) )

            del sink


    def operation_decl(self, product):
        product.template_signature( self.source.tparams )
        product.code( "class Operation\n" )
        product.code( "{\n" )
        product.code( "public:\n" )
        product.code( " Operation(%s code, %s addr, const char *name);\n", self.codetype_constref(), self.source.addrtype )
        product.code( " virtual ~Operation();\n" )
        if self.source.withencode:
            product.code( " virtual void Encode(%s code) const {}\n", self.codetype_ref() )
        product.code( " inline %s GetAddr() const { return addr; }\n", self.source.addrtype )
        product.code( " inline void SetAddr(%s _addr) { this->addr = _addr; }\n", self.source.addrtype )
        product.code( " inline %s GetEncoding() const { return encoding; }\n", self.codetype_constref() )
        self.op_getlen_decl( product )
        product.code( " inline const char *GetName() const { return name; }\n" )
        product.code( " inline bool Match(%s _addr, %s _code) const { return ", self.source.addrtype, self.codetype_constref())
        self.op_match( product, "_code" )
        product.code( " and GetAddr() == _addr; }\n" )

        product.code( " static unsigned int const minsize = %d;\n", min(self.source.insnsizes) )
        product.code( " static unsigned int const maxsize = %d;\n", max(self.source.insnsizes) )

        for variable in self.source.variables.values():
            product.code(" ").usercode( variable.c_type ).code( " %s;", variable.name )

        if self.source.withsource:
            # base declaration for internal encoding and decoding code
            # introspection methods
            if self.source.withencode:
                product.code( "virtual char const* Encode_text() const;\n" )
            product.code( "virtual char const* Decode_text() const;\n" )

        for ap in reversed(self.source.actionprotos.values()):
            if self.source.withsource:
                # base declaration for user-defined code introspection methods */
                product.code( " virtual char const* %s_text() const;\n", ap.name )
            product.code( " virtual " )
            if ap.returns is not None:
                product.usercode( ap.returns )
            else:
                product.code( "void" )

            product.code( " %s(", ap.name )

            if ap.param_list:
                sep = ' '
                for ctype, csymbol in ap.param_list:
                    product.code( sep ).usercode( ctype ).code( " " ).usercode( csymbol )
                    sep = ',\n'

            product.code( " )%s;\n", " const" if ap.constness else "" )

        product.code( "protected:\n" )

        product.code( " %s encoding;\n", self.codetype_name() )
        product.code( " %s addr;\n", self.source.addrtype )
        product.code( " const char *name;\n" )

        product.code( "};\n\n" )

    def operation_impl(self, product):
        product.template_signature( self.source.tparams )
        product.code( "Operation" )
        product.template_abbrev( self.source.tparams )
        product.code( "::Operation(%s _code, %s _addr, const char *_name)\n", self.codetype_constref(), self.source.addrtype )
        product.code( ": \n")

        for variable in self.source.variables.values():
            if variable.c_init is None: continue
            product.write( ' ' + variable.name ).usercode( variable.c_init ).write( "),\n" )

        product.code( " encoding(_code),\n" )
        product.code( " addr(_addr),\n" )
        product.code( " name(_name)\n" )
        product.code( "{\n" )
        product.code( "}\n\n" )
        product.template_signature( self.source.tparams )
        product.code( "Operation" )
        product.template_abbrev( self.source.tparams )
        product.code( "::~Operation()\n" )
        product.code( "{\n" )

        product.code( "}\n\n" )

        if self.source.withsource:
            # base implemantation for internal encoding and decoding
            # code introspection methods
            xxcodes = ["Decode"]
            if self.source.withencode:
                xxcodes.append(0, "Encode")
            for xxcode in xxcode:
                product.template_signature( self.source.tparams )
                product.code( " char const* Operation" )
                product.template_abbrev( self.source.tparams )
                product.code( "::%s_text() const { return\"\"; }\n", xxcode )

        for ap in reversed(self.source.actionprotos.values()):
            if self.source.withsource:
                # for cstring version of the method
                product.template_signature( self.source.tparams )
                product.code( " char const* Operation" )
                product.template_abbrev( self.source.tparams )
                product.code( "::%s_text() const\n", ap.name )
                product.code( " { return %s; }", json.dumps( ap.defaultcode.content ) )

            product.template_signature( self.source.tparams )

            if ap.returns is None:
                product.code( "\nvoid\n" )
            else:
                product.usercode( ap.returns )

            product.code( " Operation" )
            product.template_abbrev( self.source.tparams )
            product.code( "::%s(", ap.name )

            if ap.param_list:
                sep = ' '
                for ctype, csymbol in ap.param_list:
                    product.code( sep ).usercode( ctype ).code( " " ).usercode( csymbol )
                    sep = ',\n'

            product.code( ")%s\n{\n", " const" if ap.constness else "" )
            product.usercode( ap.defaultcode, "{", "}" )
            product.code( "}\n" )

    def decoder_decl(self, product):
        product.template_signature( self.source.tparams )
        product.code( "class Operation;\n" )

        if self.source.withcache:
            product.code( "const unsigned int NUM_OPERATIONS_PER_PAGE = 4096;\n" )
            product.template_signature( self.source.tparams )
            product.code( "class DecodeMapPage\n" )
            product.code( "{\n" )
            product.code( "public:\n" )
            product.code( " DecodeMapPage(%s key);\n", self.source.addrtype )
            product.code( " ~DecodeMapPage();\n" )
            product.code( " %s key;\n", self.source.addrtype )
            product.code( " DecodeMapPage *next;\n" )
            product.code( " Operation" )
            product.template_abbrev( self.source.tparams )
            product.code( " *operation[NUM_OPERATIONS_PER_PAGE];\n" )
            product.code( "};\n\n" )

        product.template_signature( self.source.tparams )
        product.code( "class DecodeTableEntry\n" )
        product.code( "{\n" )
        product.code( "public:\n" )
        product.code( " DecodeTableEntry(%s opcode, %s opcode_mask, Operation", self.codetype_constref(), self.codetype_constref() )
        product.template_abbrev( self.source.tparams )
        product.code( " *(*decode)(%s, %s));\n", self.codetype_constref(), self.source.addrtype )
        product.code( " %s opcode;\n", self.codetype_name() )
        product.code( " %s opcode_mask;\n", self.codetype_name() )
        product.code( " Operation" )
        product.template_abbrev( self.source.tparams )
        product.code( " *(*decode)(%s code, %s addr);\n", self.codetype_constref(), self.source.addrtype )
        product.code( "};\n" )

        if self.source.withcache:
            product.code( "const unsigned int NUM_DECODE_HASH_TABLE_ENTRIES = 4096;\n" )

        product.template_signature( self.source.tparams )
        product.code( "class Decoder\n" )
        product.code( "{\n" )
        product.code( "public:\n" )
        product.code( " typedef Operation" )
        product.template_abbrev( self.source.tparams )
        product.code( " operation_type;\n" )
        product.code( " typedef %s address_type;\n", self.source.addrtype )
        product.code( " typedef CodeType code_type;\n" )
        product.code( " enum { alignment = %u };\n\n", self.source.gcd() )
        product.code( " Decoder();\n" )
        product.code( " virtual ~Decoder();\n" )
        product.code( "\n" )
        if not self.source.is_subdecoder:
            product.code( " Operation" )
            product.template_abbrev( self.source.tparams )
            product.code( "*NCDecode(%s addr);\n", self.source.addrtype )
        product.code( " Operation" )
        product.template_abbrev( self.source.tparams )
        product.code( " *NCDecode(%s addr, %s code);\n", self.source.addrtype, self.codetype_constref() )
        if self.source.withcache:
            product.code( " Operation" )
            product.template_abbrev( self.source.tparams )
            product.code( " *Decode(%s addr, %s insn);\n", self.source.addrtype, self.codetype_constref() )
        product.code( " std::vector<DecodeTableEntry" )
        product.template_abbrev( self.source.tparams )
        product.code( " > const& GetDecodeTable() const { return decode_table; };\n" )
        if self.source.withcache:
            product.code( " void InvalidateDecodingCacheEntry(%s addr);\n", self.source.addrtype )
            product.code( " void InvalidateDecodingCache();\n\n" )
        product.code( " void SetLittleEndian();\n" )
        product.code( " void SetBigEndian();\n" )
        product.code( "private:\n" if self.options.privatemembers else "public:\n" )
        product.code( " std::vector<DecodeTableEntry" )
        product.template_abbrev( self.source.tparams )
        product.code( " > decode_table;\n" )
        if self.source.withcache:
            product.code( " DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( " *mru_page;\n" )
            product.code( " DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( " *decode_hash_table[NUM_DECODE_HASH_TABLE_ENTRIES];\n" )
            product.code( " DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( " *FindPage(%s page_key)\n", self.source.addrtype )
            product.code( "#if defined(__GNUC__) && (__GNUC__ >= 3 && (__GNUC__ != 3 || __GNUC_MINOR__ != 4 || __GNUC_PATCHLEVEL__ != 6))\n" )
            product.code( " __attribute__((always_inline))\n" )
            product.code( "#endif\n" )
            product.code( " ;\n" )
        product.code( "};\n\n" )
        if self.source.is_subdecoder:
            if self.source.tparams:
                product.template_signature( self.source.tparams )
            else:
                product.code( "inline\n" )
            product.code( "Operation *sub_decode(%s addr, %s code) {\n", self.source.addrtype, self.codetype_constref() )
            product.code( " static Decoder" )
            product.template_abbrev( self.source.tparams )
            product.code(" decoder;\n" )
            product.code( " return decoder.NCDecode( addr, code );\n" )
            product.code( "}\n" )

    def decoder_impl(self, product):
        if self.source.withcache:
            product.template_signature( self.source.tparams )
            product.code( "DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( "::DecodeMapPage(%s key)\n", self.source.addrtype )
            product.code( "{\n" )
            product.code( " this->key = key;\n" )
            product.code( " memset(operation, 0, sizeof(operation));\n" )
            product.code( " next = 0;\n" )
            product.code( "}\n\n" )

            product.template_signature( self.source.tparams )
            product.code( "DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( "::~DecodeMapPage()\n" )
            product.code( "{\n" )
            product.code( " unsigned int idx;\n" )
            product.code( " for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)\n" )
            product.code( "  delete operation[idx];\n" )
            product.code( "}\n\n" )

        product.template_signature( self.source.tparams )
        product.code( " DecodeTableEntry" )
        product.template_abbrev( self.source.tparams )
        product.code( "::DecodeTableEntry(%s opcode, %s opcode_mask, Operation", self.codetype_constref(), self.codetype_constref() )
        product.template_abbrev( self.source.tparams )
        product.code( " *(*decode)(%s, %s))\n", self.codetype_constref(), self.source.addrtype )
        product.code( " {\n" )
        product.code( "  this->opcode = opcode;\n" )
        product.code( "  this->opcode_mask = opcode_mask;\n" )
        product.code( "  this->decode = decode;\n" )
        product.code( " }\n\n" )


        product.template_signature( self.source.tparams )
        product.code( "Decoder" )
        product.template_abbrev( self.source.tparams )
        product.code( "::Decoder()\n" )
        member_init_separator = ": ";
        if self.source.withcache:
            product.code( "%smru_page( 0 )", member_init_separator )
            member_init_separator = ", ";
        product.code( "\n{\n" )
        if self.source.withcache:
            product.code( " memset(decode_hash_table, 0, sizeof(decode_hash_table));\n" )

        for op in reversed(self.source.operations):
            if op.condition is not None:
                product.code( "if(" ).usercode( op.condition ).code( ")" )
            # TODO: if and else body should go in compound statements (even if singled out)
            product.code( " decode_table.push_back(DecodeTableEntry" )
            product.template_abbrev( self.source.tparams )
            product.code( "(" )
            self.insn_bits_code( product, op )
            product.code( ", " )
            self.insn_mask_code( product, op )
            product.code( ", Decode%s", op.cname )
            product.template_abbrev( self.source.tparams )
            product.code( "));\n" )
        product.code( "}\n\n" )

        product.template_signature( self.source.tparams )
        product.code( "Decoder" )
        product.template_abbrev( self.source.tparams )
        product.code( "::~Decoder()\n" )
        product.code( "{\n" )
        if self.source.withcache:
            product.code( " InvalidateDecodingCache();\n" )
        product.code( "}\n\n" )

        # NCDecode( Address_t addr, CodeType_t code )
        product.template_signature( self.source.tparams )
        product.code( "Operation" )
        product.template_abbrev( self.source.tparams )
        product.code( " *Decoder" )
        product.template_abbrev( self.source.tparams )
        product.code( "::NCDecode(%s addr, %s code)\n", self.source.addrtype, self.codetype_constref() )
        product.code( "{\n" )
        product.code( " Operation" )
        product.template_abbrev( self.source.tparams )
        product.code( " *operation;\n" )
        product.code( " unsigned int count = decode_table.size();\n" )
        product.code( " unsigned int idx;\n" )
        product.code( " for(idx = 0; idx < count; idx++)\n" )
        product.code( " {\n" )
        self.insn_match_ifexpr( product, "code", "decode_table[idx].opcode_mask", "decode_table[idx].opcode" )
        product.code( "  {\n" )
        product.code( "   operation = decode_table[idx].decode(code, addr);\n" )
        product.code( "   return operation;\n" )
        product.code( "  }\n" )
        product.code( " }\n" )
        product.code( " operation = new Operation" )
        product.template_abbrev( self.source.tparams )
        product.code( "(code, addr, \"???\");\n" )
        product.code( " return operation;\n" )
        product.code( "}\n\n" )

        if self.source.withcache:
            # InvalidateDecodingCache()
            product.template_signature( self.source.tparams )
            product.code( "void Decoder" )
            product.template_abbrev( self.source.tparams )
            product.code( "::InvalidateDecodingCache()\n" )
            product.code( "{\n" )
            product.code( " uint32_t index;\n" )
            product.code( " mru_page = 0;\n" )
            product.code( " for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)\n" )
            product.code( " {\n" )
            product.code( "  DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( " *page, *next_page;\n" )

            product.code( "  page = decode_hash_table[index];\n" )
            product.code( "  if(page)\n" )
            product.code( "  {\n" )
            product.code( "   do\n" )
            product.code( "   {\n" )
            product.code( "   next_page = page->next;\n" )
            product.code( "   delete page;\n" )
            product.code( "   page = next_page;\n" )
            product.code( "   } while(page);\n" )
            product.code( "  decode_hash_table[index] = 0;\n" )
            product.code( "  }\n" )
            product.code( " }\n" )
            product.code( "}\n\n" )

            instid = self.insn_id_expr( "addr" );

            # InvalidateDecodingCacheEntry( Address_t addr ) ***/
            product.template_signature( self.source.tparams )
            product.code( "void Decoder" )
            product.template_abbrev( self.source.tparams )
            product.code( "::InvalidateDecodingCacheEntry(%s addr)\n", self.source.addrtype )
            product.code( "{\n" )
            product.code( " %s page_key = %s / NUM_OPERATIONS_PER_PAGE;\n", self.source.addrtype, instid )
            product.code( " if(mru_page && mru_page->key == page_key) mru_page = 0;\n" )
            product.code( " uint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" )
            product.code( " DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( " *prev, *cur;\n" )
            product.code( " cur = decode_hash_table[index];\n" )
            product.code( " if(cur)\n" )
            product.code( " {\n" )
            product.code( "  if(cur->key == page_key)\n" )
            product.code( "  {\n" )
            product.code( "   decode_hash_table[index] = cur->next;\n" )
            product.code( "   delete cur;\n" )
            product.code( "   return;\n" )
            product.code( "  }\n" )
            product.code( "  prev = cur;\n" )
            product.code( "  cur = cur->next;\n" )
            product.code( "  if(cur)\n" )
            product.code( "  {\n" )
            product.code( "   do\n" )
            product.code( "   {\n" )
            product.code( "    if(cur->key == page_key)\n" )
            product.code( "    {\n" )
            product.code( "     prev->next = cur->next;\n" )
            product.code( "     cur->next = 0;\n" )
            product.code( "     delete cur;\n" )
            product.code( "     return;\n" )
            product.code( "    }\n" )
            product.code( "    prev = cur;\n" )
            product.code( "   } while((cur = cur->next) != 0);\n" )
            product.code( "  }\n" )
            product.code( " }\n" )
            product.code( "}\n\n" )

            # FindPage( Address_t page_key ) ***/
            product.template_signature( self.source.tparams )
            product.code( "inline DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( " *Decoder" )
            product.template_abbrev( self.source.tparams )
            product.code( "::FindPage(%s page_key)\n", self.source.addrtype )
            product.code( "{\n" )
            product.code( " if(mru_page && mru_page->key == page_key) return mru_page;\n" )
            product.code( " uint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" )
            product.code( " DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( " *prev, *cur;\n" )
            product.code( " cur = decode_hash_table[index];\n" )
            product.code( " if(cur)\n" )
            product.code( " {\n" )
            product.code( "  if(cur->key == page_key)\n" )
            product.code( "  {\n" )
            product.code( "   mru_page = cur;\n" )
            product.code( "   return cur;\n" )
            product.code( "  }\n" )
            product.code( "  prev = cur;\n" )
            product.code( "  cur = cur->next;\n" )
            product.code( "  if(cur)\n" )
            product.code( "  {\n" )
            product.code( "   do\n" )
            product.code( "   {\n" )
            product.code( "    if(cur->key == page_key)\n" )
            product.code( "    {\n" )
            product.code( "     prev->next = cur->next;\n" )
            product.code( "     cur->next= decode_hash_table[index];\n" )
            product.code( "     decode_hash_table[index] = cur;\n" )
            product.code( "     mru_page = cur;\n" )
            product.code( "     return cur;\n" )
            product.code( "    }\n" )
            product.code( "    prev = cur;\n" )
            product.code( "   } while((cur = cur->next) != 0);\n" )
            product.code( "  }\n" )
            product.code( " }\n" )
            product.code( " return 0;\n" )
            product.code( "}\n\n" )

            # Decode( Address_t addr, CodeType_t const& insn ) ***/
            product.template_signature( self.source.tparams )
            product.code( "Operation" )
            product.template_abbrev( self.source.tparams )
            product.code( " *Decoder" )
            product.template_abbrev( self.source.tparams )
            product.code( "::Decode(%s addr, %s insn)\n", self.source.addrtype, self.codetype_constref() )
            product.code( "{\n" )
            product.code( " Operation" )
            product.template_abbrev( self.source.tparams )
            product.code( " *operation;\n" )
            product.code( " %s page_key = %s / NUM_OPERATIONS_PER_PAGE;\n", self.source.addrtype, instid )
            product.code( " DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( " *page;\n" )
            product.code( " page = FindPage(page_key);\n" )
            product.code( " if(!page)\n" )
            product.code( " {\n" )
            # product.code( "   fprintf(stderr, \"page miss at 0x%%08x\\n\", addr);\n" )
            product.code( "  page = new DecodeMapPage" )
            product.template_abbrev( self.source.tparams )
            product.code( " (page_key);\n" )
            product.code( "  uint32_t index = page_key %% NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key\n" )
            product.code( "  page->next = decode_hash_table[index];\n" )
            product.code( "  decode_hash_table[index] = page;\n" )
            product.code( "  mru_page = page;\n" )
            product.code( " }\n" )
            product.code( " operation = page->operation[(%s) & (NUM_OPERATIONS_PER_PAGE - 1)];\n", instid )
            product.code( " if(operation)\n" )
            product.code( " {\n" )
            # product.code( "  fprintf(stderr, \"hit at 0x%%08x\\n\", addr);\n" )
            product.code( "  if(operation->match(addr, insn))\n")
            product.code( "   return operation;\n" )
            product.code( "  delete operation;\n" )
            product.code( " }\n" )
            # product.code( " fprintf(stderr, \"miss at 0x%%08x\\n\", addr);\n" )
            product.code( " operation = NCDecode(addr, insn);\n" )
            product.code( " page->operation[(%s) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;\n", instid )
            product.code( " return operation;\n" )
            product.code( "}\n\n" )

    def membersize(self, size):
        # Operand's C type size is determined by 1/ the least C type
        # required by the ISA operand itself and 2/ the minimal C type
        # required for all operands.
        return max( least_ctype_size( size ), self.minwordsize )


    def isa_operations_decl(self, product):
        for op in self.source.operations:
            product.template_signature( self.source.tparams )
            product.code( "class %s : public Operation", op.cname )
            product.template_abbrev( self.source.tparams )
            product.code( "\n" )
            product.code( "{\n" )
            product.code( "public:\n" )
            product.code( " %s(%s code, %s addr);\n", op.cname, self.codetype_constref(), self.source.addrtype )
            self.insn_destructor_decl( product, op )
            self.insn_getlen_decl( product, op )
            if self.source.withencode:
                product.code( " void Encode(%s code) const;\n", self.codetype_ref() )

            for bf in op.bitfields:
                if isinstance(bf, OperandBitField):
                    product.code( "%sint%d_t %s;\n", "" if bf.sext else "u", self.membersize( bf.dstsize() ), bf.name )
                elif isinstance(bf, SpOperandBitField):
                    product.code( "static %sint%d_t const %s = %s\n", "" if bf.sext else "u", membersize( bf.dstsize() ), bf.name, bf.constval() )
                elif isinstance(bf, SubOpBitField):
                    sdinstance = bf.sdinstance
                    sdclass = sdinstance.sdclass
                    tpscheme =  sdinstance.template_scheme

                    product.code(" ").usercode( sdclass.nmcode ).code( "::Operation" )
                    if tpscheme:
                        product.code( "< " ).usercode( tpscheme ).code( " >")
                    product.code( "* %s;\n", bf.name )

            if op.variables:
                for variable in op.variables.values():
                    product.code( "   " ).usercode( variable.c_type, '', ' %s;' % variable.name )

            if self.source.withsource:
                # insn declaration for internal encoding and decoding
                # code introspection methods
                if self.source.withencode:
                    product.code( "char const* Encode_text() const;\n" )
                product.code( "char const* Decode_text() const;\n" )

            for action in op.actions.values():
                actionproto = action.actionproto

                if self.source.withsource:
                    # for cstring version of the action
                    product.code( " virtual char const* %s_text() const;\n", actionproto.name )

                product.code( " virtual\n " )

                if actionproto.returns is not None:
                    product.usercode( actionproto.returns )
                else:
                    product.code( "void\n" )

                product.code( " %s(", actionproto.name )

                if actionproto.param_list:
                    sep = " "
                    for ctype, csymbol in actionproto.param_list:
                        product.code( sep ).usercode( ctype ).code( " " ).usercode( csymbol )
                        sep = ',\n'
                    product.code( " " )

                product.code( ")%s;\n", " const" if actionproto.constness else "" )

            product.code( "private:\n" if self.options.privatemembers else "public:\n" )

            product.code( "};\n\n" )

    def isa_operations_methods(self, product):
        for op in self.source.operations:
            if self.source.withsource:
                # insn implementation for internal encoding and
                # decoding code introspection methods
                xxcodes = [("Decode", lambda x: insn_decode_impl( x, op, "code", "addr" ))]
                if self.source.withencode:
                    xxcodes.append(0, ("Encode", lambda x: insn_encode_impl( x, op, "code" )))
                for xxcode, insn_impl in xxcode:
                    xxcode_text = SProduct( product.filename, self.options.sourcelines )
                    insn_impl( xxcode_text )
                    product.template_signature( self.source.tparams )
                    product.code( " char const* %s", op.cname )
                    product.template_abbrev( self.source.tparams )
                    product.code( "::%s_text() const\n", xxcode )
                    product.code( " { return %s; }\n", json.dumps( xxcode_text.string() ) )

            for action in op.actions.values():
                actionproto = action.actionproto

                if self.source.withsource:
                    # for cstring version of the method
                    product.template_signature( self.source.tparams )
                    product.code( " char const* %s", op.cname )
                    product.template_abbrev( self.source.tparams )
                    product.code( "::%s_text() const\n", actionproto.name )
                    product.code( " { return %s; }", json.dumps( action.source_code.content ) )

                product.template_signature( self.source.tparams )

                if actionproto.returns is not None:
                    product.usercode( actionproto.returns )
                else:
                    product.code( "\nvoid\n" )

                product.code( ' ' ).code( op.cname )
                product.template_abbrev( self.source.tparams )
                product.code( "::%s(", actionproto.name )

                if actionproto.param_list:
                    sep = " "
                    for ctype, csymbol in actionproto.param_list:
                        product.code( sep ).usercode( ctype ).code( " " ).usercode( csymbol )
                        sep = ',\n'

                product.code( ")%s\n{\n", " const" if actionproto.constness else "" )
                product.usercode( action.sourcecode, "{", "}" )
                product.code( "}\n" )

            product.code( "\n" )

            product.template_signature( self.source.tparams )
            product.code( "static Operation" )
            product.template_abbrev( self.source.tparams )
            product.code( " *Decode%s(", op.cname )
            product.code( "%s code, %s addr)\n", self.codetype_constref(), self.source.addrtype )
            product.code( "{\n" )
            product.code( " return new %s", op.cname )
            product.template_abbrev( self.source.tparams )
            product.code( "(code, addr);\n" )
            product.code( "}\n\n" )

    def isa_operations_ctors(self, product):
        for op in self.source.operations:
            product.template_signature( self.source.tparams )
            product.code( op.cname )
            product.template_abbrev( self.source.tparams )
            product.code( "::%s(%s code, %s addr) : Operation", op.cname, self.codetype_constref(), self.source.addrtype );
            product.template_abbrev( self.source.tparams )
            product.code( "(code, addr, \"%s\")\n", op.name );
            product.code( "{\n" )

            self.insn_decode_impl( product, op, "code", "addr" )

            if op.variables:
                for variable in op.variables.values():
                    if variable.c_init is not None:
                        product.code( "%s = ", variable.name ).usercode( variable.c_init ).code( ";\n" )

            product.code( "}\n\n" )

            self.insn_destructor_impl( product, op )

class ScalarGenerator(Generator):
    def __init__(self, source, options):
        Generator.__init__(self, source, options)
        self.insn_ctype_size = least_ctype_size(self.source.maxsize())
        self.insn_ctype = 'uint{}_t'.format(self.insn_ctype_size)
        self.insn_cpostfix = 'ULL'[:len(bin(max(self.insn_ctype_size,16)))-6]

    def insn_destructor_decl( self, product, op ):
        pass
    def insn_destructor_impl( self, product, op ):
        pass
    def additional_impl_includes( self, product ):
        pass
    def additional_decl_includes( self, product ):
        pass
    def codetype_decl(self, product):
        product.code( "typedef %s CodeType;\n\n", self.insn_ctype )
    def codetype_impl(self, product):
        pass
    def codetype_name(self):     return "CodeType"
    def codetype_ref(self):      return "CodeType&"
    def codetype_constref(self): return "CodeType"

    def insn_bits_code(self, product, op):
        size = self.source.maxsize()
        beg, end = (0, size) if self.source.little_endian else (-size, 0)
        bits = op.code.getbits(beg, end)
        product.code( "0x%x%s", bits, self.insn_cpostfix )

    def insn_mask_code(self, product, op):
        size = self.source.maxsize()
        beg, end = (0, size) if self.source.little_endian else (-size, 0)
        mask = op.code.getmask(beg, end)
        product.code( "0x%x%s", mask, self.insn_cpostfix )

    def insn_match_ifexpr(self, product, code, mask, bits):
        product.code( "if((%s & %s) == %s)\n", code, mask, bits )

    def op_match(self, product, codename):
        delta = '(this->GetEncoding() ^ {})'.format( codename )
        maxsize = self.source.maxsize()
        vlen = len(self.source.insnsizes) != 1
        mask = '0x{:x}{}'.format( (1 << maxsize)-1, self.insn_cpostfix )
        skipmask = maxsize == self.insn_ctype_size
        
        if     self.source.little_endian and vlen:
            delta = '({} << ({} - this->GetLength()))'.format(delta, maxsize)
            skipmask = False # Defensive programming against C type promotions
        if not skipmask:
            delta = '({} & {})'.format(delta, mask)
        if not self.source.little_endian and vlen:
            delta = '({} >> ({} - this->GetLength()))'.format(delta, maxsize)
        product.code( '%s == 0', delta )
        
    def insn_id_expr(self, addrname):
        # TODO: generic method: addr / (gcd/8)
        if len(self.source.insnsizes) != 1:
            return addrname
        return '%s / %d' % (addrname, next(iter(self.source.insnsizes))/8)

    def op_getlen_decl(self, product):
        product.code( 'inline' if len(self.source.insnsizes) == 1 else 'virtual' )
        product.code( " unsigned int GetLength() const { return %d; }\n", self.source.maxsize() )

    def insn_getlen_decl(self, product, op):
        if len(self.source.insnsizes) == 1:
            return
        if op.vlen:
            product.code( "unsigned int gil_length;\n" )
            product.code( "unsigned int GetLength() const { return this->gil_length; }\n" )
        else:
            product.code( "unsigned int GetLength() const { return %d; }\n", op.size )

    def insn_encode_impl(self, product, op, codename):
        if op.vlen:
            raise Abort('Encode method does not work with variable length operations.')

        product.code( "%s = 0x%x%s;\n", _codename, op.code.bits, self.insn_cpostfix )

        for beg, end, bf in FieldIterator( op.bitfields, self.source.little_endian):
            if isinstance(bf, SubOpBitField):
                raise Abort('Encode method does not work with sub-operations.')
            elif isinstance(bf, OperandBitField):
                opsize = membersize( *opbf );

                value = 'uint%d_t( %s )' % (opsize, bf.name)
                if bf.lshift > 0:
                    value = "(%s >> %u)" % (value, bf.lshift)

                product.code( "%s |= ((%s & 0x%x) << %u);\n", codename, value, bf.mask(), beg % self.source.maxsize() )

    def insn_decode_impl(self, product, op, codename, addrname):
        if op.vlen:
            product.code( "this->gil_length = %u;\n", oc.m_size )

        for beg, end, bf in FieldIterator( op.bitfields, self.source.little_endian ):
            pos = beg % self.source.maxsize()

            if isinstance(bf, SubOpBitField):
                sdinstance = bf.sdinstance;
                sdclass = sdinstance.sdclass;
                tpscheme =  sdinstance.template_scheme;

                product.code( "%s = ", bf.name ).usercode( sdclass.nmcode ).code("::sub_decode" )
                if tpscheme:
                    product.code( "< " ).usercode( *tpscheme ).code( " >" )
                product.code( "( %s, ((%s >> %u) & 0x%x) )\n", addrname, codename, pos, (1 << sdclass.maxsize())-1 );

                if sdclass.minsize() != sdclass.maxsize():
                    product.code( "{\nunsigned int shortening = %u - %s->GetLength()\n", sdclass.maxsize(), bf.name );
                    product.code( "this->gil_length -= shortening;\n" )
                    product.code( "%s %s= shortening;\n}\n", codename, self.source.ashift() )

            elif isinstance(bf, OperandBitField):
                product.code( "%s = ", bf.name )

                opsize = self.membersize( bf.dstsize() );
                value = '(%s >> %u)' % (codename, pos) if pos else codename
                value = '%s & 0x%x' % (value, bf.mask())

                if bf.sext:
                    sext_shift = opsize - bf.size;
                    product.code( "((int%d_t)((%s) << %u) >> %u)", opsize, value, sext_shift, sext_shift )
                else:
                    product.code( "uint%d_t(%s)", opsize, value )

                if bf.lshift > 0:
                    product.code( " << %u", bf.lshift )
                product.code( ";\n" )

class BufferGenerator(Generator):
    def __init__(self, source, options):
        Generator.__init__(self, source, options)
        self.code_capacity = (self.source.maxsize()+7) >> 3

    def insn_destructor_decl( self, product, op ):
        for bf in op.bitfields:
            if isinstance(bf, SubOpBitField):
                product.code( "~%s();\n", op.cname )
                return

    def insn_destructor_impl( self, product, op ):
        subops = [bf.name for bf in op.bitfields if isinstance(bf, SubOpBitField)]
        if not subops:
            return
        product.template_signature(self.source.tparams)
        product.code( op.cname )
        product.template_abbrev(self.source.tparams)
        product.code( "::~%s()\n", op.cname )
        product.code( "{\n" )
        for subop in subops:
            product.code( "delete %s;\n", subop )
        product.code( "}\n\n" )

    def additional_decl_includes( self, product ):
        product.code( "#include <iosfwd>\n" )

    def additional_impl_includes( self, product ):
        product.code( "#include <ostream>\n" )
        product.code( "#include <cstring>\n" )

    def codetype_decl(self, product):
        little_endian = self.source.little_endian
        fmt={'capacity': self.code_capacity, 'lastbyte': self.code_capacity - 1,
             'ash': self.source.ashift(), 'dsh': self.source.dshift()}
        product.write( '''\
          struct CodeType {
            enum { capacity = %(capacity)d };
            unsigned int              size;
            uint8_t                   str[capacity];
            enum Exception_t { NotEnoughBytes };
            CodeType() : size( 0 ) { for (int idx = capacity; (--idx) >= 0;) str[idx] = 0; }
            CodeType( unsigned int sz )
              : size( std::min( sz, capacity*8u ) )
            {
              for (int idx = capacity; (--idx) >= 0;) str[idx] = 0;
            }
            CodeType( uint8_t const* src, unsigned int sz )
              : size( std::min( sz, capacity*8u ) )
            {
              for (int idx = capacity; (--idx) >= 0;) str[idx] = 0;
              for (int idx = (size+7)/8; (--idx) >= 0;) str[idx] = src[idx];
            }
        ''' % fmt )
        product.write( '''\
            CodeType( CodeType const& ct )
              : size( ct.size )
            {
              %s
            }
        ''' % ' '.join( 'str[%d] = ct.str[%d];' % (x,x) for x in range(self.code_capacity)) )
        product.write( '''\
            bool match( CodeType const& bits, CodeType const& mask ) const {
              unsigned int maskbound = (mask.size+7)/8, codebound = (size+7)/8;
              for (unsigned int idx = 0; idx < maskbound; ++idx) {
                if (idx >= codebound) throw NotEnoughBytes;
                if ((str[idx] & mask.str[idx]) != bits.str[idx]) return false;
              }
              return true;
            }
            bool match( CodeType const& bits ) const {
              if (size < bits.size) throw NotEnoughBytes;
              unsigned int end = bits.size/8;
              for (unsigned int idx = 0; idx < end; idx += 1)
                if (str[idx] != bits.str[idx]) return false;
              unsigned int tail = (bits.size %% 8);
              if (tail == 0) return true;
              uint8_t tailmask = 0xff %(dsh)s (8-tail);
              return ((str[end] ^ bits.str[end]) & tailmask) == 0;
            }
            CodeType& stretch_front( unsigned int shift ) {
              int hish = shift / 8, losh = shift %% 8;
              for (int dst = %(lastbyte)d, src = dst - hish; dst >= 0; dst-=1, src-=1) {
                if (src > 0) str[dst] = (str[src] %(ash)s losh) | (str[src-1] %(dsh)s (8-losh));
                else if (src == 0) str[dst] = (str[src] %(ash)s losh);
                else str[dst] = 0;
              }
              return *this;
            }
            CodeType& shrink_front( unsigned int shift ) {
              int hish = shift / 8, losh = shift %% 8;
              for (unsigned int dst = 0, src = dst + hish; dst < %(capacity)d; dst+=1, src+=1) {
                if (src < %(lastbyte)d) str[dst] = (str[src] %(dsh)s losh) | (str[src-1] %(ash)s (8-losh));
                else if (src == %(lastbyte)d) str[dst] = (str[src] %(dsh)s losh);
                else str[dst] = 0;
              }
              return *this;
            }
            CodeType& extend( uint8_t* src, unsigned int sz ) {
              CodeType tail( src, sz );
              unsigned int mod = this->size %% 8;
              if (mod) {
                tail.size = std::min( tail.size + mod, capacity*8u );
                tail.stretch_front( mod );
                tail.str[0] = (tail.str[0] & (0xff %(ash)s mod)) | (this->str[this->size/8] & (0xff %(dsh)s (8-mod)));
                this->size -= mod;
              }
              for (unsigned int src = 0, dst = this->size/8; (src < capacity*1u) and (dst < capacity*1u); src+=1, dst+=1) {
                this->str[dst] = tail.str[src];
              }
              this->size = std::min( this->size + tail.size, capacity*8u );
              return *this;
            }
            friend std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct );
          };
        ''' % fmt )

    def codetype_impl(self, product):
        if self.source.tparams:
            product.code( "inline\n" )
        product.code( "std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct ) {\n" )
        product.code( " if (_ct.size %% 8) {\n" )
        if self.source.little_endian:
            product.code( "  for (int idx = _ct.size; (--idx) >= 0; ) {\n" )
            product.code( "   _sink << (((_ct.str[idx/8] >> (idx%%8)) & 1) ? '1' : '0');\n" )
            product.code( "  }\n" )
        else:
            product.code( "  for (unsigned int idx = 0; idx < _ct.size; ++idx ) {\n" )
            product.code( "   _sink << (((_ct.str[idx/8] >> ((7-idx)%%8)) & 1) ? '1' : '0');" )
            product.code( "  }\n" )
        product.code( " } else {\n" )
        product.code( "  char const* xrepr = \"0123456789abcdef\";\n" )
        if self.source.little_endian:
            product.code( "  for (int idx = _ct.size/8; (--idx) >= 0; ) {\n" )
        else:
            product.code( "  for (unsigned int idx = 0; idx < _ct.size/8; ++idx ) {\n" )
        product.code( "    uint8_t byte = _ct.str[idx];\n" )
        product.code( "   _sink << xrepr[(byte >> 4) & 0xf] << xrepr[(byte >> 0) & 0xf];\n" )
        product.code( "  }\n" )
        product.code( " }\n" )
        product.code( " return _sink;\n" )
        product.code( "}\n" )

    def codetype_name(self):     return "CodeType"
    def codetype_ref(self):      return "CodeType&"
    def codetype_constref(self): return "CodeType const&"

    def prefix_size(self, op):
        little_endian = self.source.little_endian
        start = op.size if little_endian else -op.size
        beg, end = start, start
        for size in range(op.size,0,-1):
            if little_endian: end =  size; beg = end-1
            else:             beg = -size; end = beg+1
            if op.code.getmask(beg, end) != 0:
                return size
        return 0

    def insn_bits_code(self, product, op):
        codesize = self.prefix_size(op)
        product.code( "CodeType( (uint8_t*)( \"")
        for beg, end in CodeBytes((codesize+7) & -8, self.source.little_endian):
            byte = op.code.getbits(beg, end)
            product.code( '\\x%02x' % byte )
        product.code( "\" ), %d )", codesize )

    def insn_mask_code(self, product, op):
        codesize = self.prefix_size(op)
        product.code( "CodeType( (uint8_t*)( \"")
        for beg, end in CodeBytes((codesize+7) & -8, self.source.little_endian):
            byte = op.code.getmask(beg, end)
            product.code( '\\x%02x' % byte )
        product.code( "\" ), %d )", codesize )

    def insn_match_ifexpr(self, product, code, mask, bits):
        product.code( "if( %s.match( %s, %s) )\n", code, bits, mask )

    def op_match(self, product, codename ):
        product.code( "%s.match( GetEncoding() )", codename )

    def insn_id_expr(self, addrname):
        # TODO: generic method: addr / (gcd/8)
        return addrname

    def op_getlen_decl(self, product):
        product.code( "inline unsigned int GetLength() const { return this->encoding.size; }\n" )

    def insn_getlen_decl(self, product, op):
        pass

    def insn_encode_impl(self, product, op, codename):
        if op.vlen:
            raise Abort('Encode method does not work with variable length operations.')

        codesize = op.size
        product.code( "%s = CodeType( (uint8_t*)( \"", codename )
        for beg, end in CodeBytes((codesize+7) & -8, self.source.little_endian):
            bits = op.code.getbits(beg, end) & op.code.getmask(beg, end)
            product.code( '\\x%02x' % byte )
        product.code( "\" ), %d )", codesize )

        for beg, end, bf in FieldIterator(op.bitfields, self.source.little_endian):
            if isinstance(bf, SubOpBitField):
                raise Abort('Encode method does not work with sub-operations.')
            if isinstance(bf, OperandBitField):
                opsize = membersize( bf.dstsize() );
                shiftedop = 'uint%u_t( %s )' % (opsize, bf.name)
                if bf.lshift > 0:
                    shiftedop = "(%s >> %u)" % (shiftedop, bf.lshift)

                fstart   = beg if self.source.little_endian else -end
                fend     = end if self.source.little_endian else -beg
                substart = fstart

                while substart < fend:
                    subend = min((substart + 8) & -8, fend) # subend
                    bound = 1 << (subend - substart)
                    pos = (substart % 8) if self.source.little_endian else (-substart % 8)
                    bytepos = substart >> 3
                    srcpos = (substart - fstart) if self.source.little_endian else (fend - subend)
                    mask = bound - 1
                    assert mask != 0

                    product.code( "%s.str[%u] |= (((%s >> %u) & 0x%x) << %u);\n", codename, bytepos, shiftedop, srcpos, mask, pos )

                product.code( ";\n" )

    def insn_decode_impl(self, product, op, codename, addrname):
        product.code( "if (this->encoding.size < %u) throw CodeType::NotEnoughBytes;\n", op.size );
        product.code( "this->encoding.size = %u;\n", op.size );
        if op.vlen:
            oldcodename, codename = codename, "_code_"
            product.code( "CodeType %s( %s );\n", codename, oldcodename )

        for beg, end, bf in FieldIterator(op.bitfields, self.source.little_endian):
            if isinstance(bf, SubOpBitField):
                sdinstance = bf.sdinstance
                sdclass = sdinstance.sdclass
                tpscheme = sdinstance.template_scheme

                product.code( "{\n" )
                product.usercode(sdclass.nmcode).code( "::CodeType _subcode_;\n" )
                product.code( "_subcode_.size = %u;\n", max(sdclass.insnsizes) )

                shift = beg if self.source.little_endian else -end
                byteshift = shift >> 3
                shift = shift & 7
                subbytes = (max(sdclass.insnsizes) + 7) >> 3

                for idx in range(subbytes):
                    didx = idx + byteshift
                    product.code( "_subcode_.str[%u] = ", idx )
                    product.code( "uint8_t( %s.str[%u] %s %u )", codename, didx, self.source.dshift(), shift )
                    if shift > 0 and (didx + 1) < self.code_capacity:
                        product.code( "| uint8_t( %s.str[%u] %s %u )", codename, didx + 1, self.source.ashift(), 8 - shift )
                    product.code( ";\n" )

                product.code( "%s = ", bf.name ).usercode( sdclass.nmcode ).code( "::sub_decode" )
                if tpscheme:
                    product.code( "< " ).usercode( *tpscheme ).code( " >" )
                product.code( "( %s, _subcode_ );\n", addrname )
                product.code( "unsigned int shortening = %u - %s->GetLength();\n", max(sdclass.insnsizes), bf.name )
                product.code( "this->encoding.size -= shortening;\n" )
                product.code( "%s.stretch_front( shortening );\n", codename )
                product.code( "}\n" )

            elif isinstance(bf, OperandBitField):
                product.code( "%s = ", bf.name )
                opsize = self.membersize( bf.dstsize() )

                src, sep = beg, ''
                while src < end:
                    nsrc = min((src+8) & -8, end)
                    shift = src & 7
                    byte = (src >> 3) ^ ((src < 0) * -1)
                    mask = (1 << nsrc-src) - 1
                    assert mask != 0
                    dst = src-beg

                    component = 'uint%u_t( %s.str[%u] )' % (opsize, codename, byte)
                    if shift: component = '(%s >> %u)' % (component, shift)
                    component = '(%s & 0x%x)' % (component, mask)
                    if dst: component = '(%s << %u)' % (component, dst)
                    product.code( sep ).write( component )
                    
                    src = nsrc
                    sep = ' | '

                product.code( ";\n" )

                if bf.sext:
                    sext_shift = opsize - bf.size
                    product.code( "%s = (int%u_t)(%s << %u) >> %u;\n", bf.name, opsize, bf.name, sext_shift, sext_shift )
                if bf.lshift > 0:
                    product.code( "%s <<= %u;\n", bf.name, bf.lshift )


def main(callname, args):
    gil = GIL(callname)

    gil.process( args )

    if not gil.inputname:
        gil.help(sys.stderr)
        raise Abort('no input file.\n')

    isa = Isa()

    scanner = parse(gil.inputname, gil, isa)

    if gil.depfilename is not None:
        depfile = open(gil.depfilename, 'w')
        isa.deps( depfile, gil.outputprefix )
        depfile.close()

    isa.sanity_checks()

    if gil.specialization:
        isa.specialize( gil.log )

    isa.finalize( gil.log )

    # ISA statistics
    isa.dumpstats( gil.log )

    # Back-end specific preprocess
    generator = isa.generator( gil )

    # ISS Generation
    generator.iss()


if __name__ == "__main__":
    try:
        main(sys.argv[0], sys.argv[1:])
    except Abort as abort:
        if abort.msg is not None:
            sys.stderr.write('{}: {}\n'.format(GENISSLIB, abort.msg))
            sys.exit(1)

    # All went fine.
