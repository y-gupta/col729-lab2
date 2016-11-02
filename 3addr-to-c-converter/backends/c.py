

# $Id: c.py 849 2007-09-25 20:06:42Z suriya $

"""
A backend to generate C code for a file.
"""

from globalvars import OSTREAM

class BackToCBackend:
    @classmethod
    def writeMacros(kls):
        OSTREAM.write('#include <stdio.h>\n')
        OSTREAM.write('#define WriteLine() printf("\\n");\n')
        OSTREAM.write('#define WriteLong(x) printf(" %lld", x);\n')
        OSTREAM.write('#define ReadLong(a) if (fscanf(stdin, "%lld", &a) != 1) a = 0;\n')

    @classmethod
    def writeGPArray(kls):
        OSTREAM.write('long long global_variables[32768/8];\n')
        OSTREAM.write('void *GP = global_variables;\n')

    @classmethod
    def write(kls, program):
        kls.writeMacros()
        kls.writeGPArray()
        for func in program.functions:
            kls.writeFunction(func)
        OSTREAM.write('\n')

    @classmethod
    def writeFunction(kls, function):
        function.write()
