
from nodeinfo import SourceCodeInfo
import re
from utils.flyweight import FlyWeight
# Don't bother about why we are using weakref. It is for optimization
# reasons.
# import weakref

class Operand(SourceCodeInfo, object):
    def emit(self):
        raise NotImplementedError("Operand.emit not implemented")
    def emit_3addr(self):
        raise NotImplementedError("Operand.emit_3addr not implemented: %s" % self.source_code)
    @property
    def constant(self):
        """
        Return the constant this operand refers to. Will return None, if
        this is not a constant.
        """

class NameAndAddressOperand(Operand):
    format_str = None
    format_str_3addr = '%s#%s'
    def __init__(self, name, offset):
        Operand.__init__(self)
        self.name = name
        self.offset = offset
    def emit(self):
        return self.format_str % (self.name, self.offset)
    def emit_3addr(self):
        return self.format_str_3addr % (self.name, self.offset)
    @property
    def constant(self):
        return self.offset

class NumberOperand(Operand):
    format_str = None
    format_str_3addr = None
    def __init__(self, number):
        Operand.__init__(self)
        self.number = number
    def emit(self):
        return self.format_str % self.number
    def emit_3addr(self):
        return self.format_str_3addr % self.number
    @property
    def constant(self):
        return None

class Pointer(Operand):
    is_register = False
    is_constant = False
    format_str = None
    format_str_3addr = None
    def emit(self):
        return self.format_str
    def emit_3addr(self):
        return self.format_str_3addr
    @property
    def constant(self):
        return None

class GlobalPointer(Pointer):
    source_code = format_str = format_str_3addr = 'GP'
class FramePointer(Pointer):
    source_code = format_str = format_str_3addr = 'FP'

class Label(FlyWeight, NumberOperand):

    _FlyWeight_Pool = {}

    is_register = False
    is_constant = False
    format_str = 'label_%d'
    format_str_3addr = '[%d]'

class Constant(FlyWeight, NumberOperand):
    """
    >>> c1 = Constant(23)
    >>> c2 = Constant(24)
    >>> c1 == c2
    False
    >>> c1 == Constant(23)
    True
    >>> c2 is Constant(24)
    True
    """

    _FlyWeight_Pool = {}

    is_register = False
    is_constant = True
    format_str = '%d'
    format_str_3addr = '%d'
    @property
    def constant(self):
        return self.number

class VirtualRegister(FlyWeight, NumberOperand):

    _FlyWeight_Pool = {}

    is_register = True
    is_constant = False
    format_str = 'register_%d'
    format_str_3addr = '(%d)'

class BaseAddress(FlyWeight, NameAndAddressOperand):

    _FlyWeight_Pool = {}

    is_register = False
    is_constant = True
    format_str = '/* %s */ %d'

class LocalScalarVariable(FlyWeight, NameAndAddressOperand):

    _FlyWeight_Pool = {}

    is_register = True
    is_constant = False
    # This is a local variable. We are going to register allocate it rather
    # than getting it from the local_vars, array
    format_str = '%s /* %d */'
    @property
    def constant(self):
        return None

class FormalParameter(FlyWeight, NameAndAddressOperand):
    _FlyWeight_Pool = {}

    is_register = True
    is_constant = False
    def emit(self):
        return '/* %s */ formal_param_%d' % (self.name, (self.offset/8)-2)
    @property
    def constant(self):
        return None

class Offset(FlyWeight, NameAndAddressOperand):

    _FlyWeight_Pool = {}

    is_register = False
    is_constant = True
    format_str = '/* %s */ %d'

GP = GlobalPointer()
FP = FramePointer()

def makeOperand(s):
    """
    >>> o1 = makeOperand('[23]')
    >>> o2 = makeOperand('[23]')
    >>> o1 is o2
    True
    >>> o1 == o2
    True
    >>> o1 is Label(23)
    True
    """
    # GP
    if (s == 'GP'):
        return GP

    if (s == 'FP'):
        return FP

    # a label
    match = re.match(r'\[(\d+)\]', s)
    if match is not None:
        label = int(match.group(1))
        ret = Label(label)
        ret.source_code = s
        return ret

    # a constant like 342
    match = re.match(r'\d+', s)
    if match is not None:
        ret = Constant(int(s))
        ret.source_code = s
        return ret

    # a register like (4)
    match = re.match(r'\((\d+)\)', s)
    if match is not None:
        reg = int(match.group(1))
        ret = VirtualRegister(reg)
        ret.source_code = s
        return ret

    # a base address
    match = re.match(r'(\w+_base)#([-\d]+)', s)
    if match is not None:
        name = match.group(1)
        offset = int(match.group(2))
        ret = BaseAddress(name, offset)
        ret.source_code = s
        return ret

    # an offset
    match = re.match(r'(\w+_offset)#([-\d]+)', s)
    if match is not None:
        name = match.group(1)
        offset = int(match.group(2))
        ret = Offset(name, offset)
        ret.source_code = s
        return ret

    # parameters, local variables
    match = re.match(r'(\w+)#([-\d]+)', s)
    if match is not None:
        name = match.group(1)
        offset = int(match.group(2))
        if offset < 0:
            ret = LocalScalarVariable(name, offset)
            ret.source_code = s
            return ret
        else:
            ret = FormalParameter(name, offset)
            ret.source_code = s
            return ret

    raise NotImplementedError('Unknown operand: %s' % s)

if __name__ == '__main__':
    import doctest
    doctest.testmod()
