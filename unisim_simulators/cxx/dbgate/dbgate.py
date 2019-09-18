"""
Python master module for dbgate usage
"""

from ._dbgate import DBGated

import os, gdb, re

gdbdefs = os.path.abspath(os.path.join(os.path.dirname(__file__),'dbgate.gdb'))

PORT=12345
ROOT="/tmp/dbgate"

SERVER=None

write=None
open=None
close=None

def setup():
    gdb.execute("source %s" % gdbdefs)
    global SERVER, write, open, close 
    SERVER = DBGated(PORT,ROOT)
    write = SERVER.write
    open = SERVER.open
    close = SERVER.close

CALLRE = re.compile('^(\w*) *\((.*)\)$')

def call( callstr ):
    match = CALLRE.match(callstr)
    if not match:
        raise Exception( "malformed %r" % callstr )
    fun, args = match.groups()
    fd = None
    if '@' in args:
        before, path, after = args.split( '@', 2 )
        fd = open(path)
        args = '%s%d%s' % (before,fd,after)

    gdb.execute('break %s' % fun)
    try:
        gdb.execute('call %s(%s)' % (fun,args))
    except gdb.error as err:
        if not err.args[0].startswith('The program being debugged stopped while in a function called from GDB'):
            raise
    gdb.execute('cont')

    if fd is not None:
        close(fd)

