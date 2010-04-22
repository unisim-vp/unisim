from distutils.core import setup, Extension
from distutils import sysconfig, ccompiler
import distutils

print ( distutils.util.get_platform() )
# print ( sysconfig.get_config_vars() )
setup(name="simulator", version="1.0",
      ext_modules=[Extension("simulator", ["../src/python_lib.cc"],
      				include_dirs=['.','../src','/Users/gracia/opt/systemc-2.2.0/include','/Users/gracia/opt/tlm20/include/tlm'],
      				library_dirs=['lib'],
      				libraries=['armemu-0.2.0'])])