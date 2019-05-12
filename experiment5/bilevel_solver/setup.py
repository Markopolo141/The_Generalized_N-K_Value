from distutils.core import setup, Extension
import os

#os.environ['CFLAGS'] = ' -O3 '
#os.environ['OPT'] = ' '
#os.environ['LDSHARED'] = 'x86_64-linux-gnu-gcc -pthread -shared -Wl,-Bsymbolic-functions -DNDEBUG   '

script_path = os.path.dirname(os.path.abspath(__file__))
module = Extension('bilevel_solver',
			include_dirs = [script_path],
			sources=['interface.cpp']#,
			#extra_compile_args=['-O3'],
			#extra_link_args=['-O3']
)

setup(name='bilevel_solver', version='1.0',  \
      ext_modules=[module])
