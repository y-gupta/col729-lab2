
import sys
from globalvars import OSTREAM
from operands import makeOperand, VirtualRegister, Constant, LocalScalarVariable, Label, FormalParameter
from nodeinfo import CFGNode, SourceCodeInfo, VisitInfo
from utils import prop
from utils.flyweight import FlyWeight

# import weakref
import re

# This instruction is never dead.
class NeverDead: pass

# Deals with destination registers of this instruction
class WritesToDestRegister:
    @property
    def dest_variable(self):
        return VirtualRegister(self.number)

class DoesNotWriteToDestRegister:
    @property
    def dest_variable(self):
        return None

# Deals with src registers of this instruction
class HasNoSourceOperand:
    @property
    def src_variables(self):
        return []

class HasOneSourceOperand:
    @property
    def src_variables(self):
        regs = [ self.arg0 ]
        return [ i for i in regs if i.is_register ]

class HasTwoSourceOperands:
    @property
    def src_variables(self):
        regs = [ self.arg0, self.arg1 ]
        return [ i for i in regs if i.is_register ]

class BranchInstruction(NeverDead, DoesNotWriteToDestRegister): pass

class ArithmeticInstruction(WritesToDestRegister): pass

class Instruction(CFGNode, SourceCodeInfo, VisitInfo, object):
    def __init__(self, number, opcode):
        CFGNode.__init__(self)
        self.number = number
        self.opcode = opcode
        self.basic_block = None
        self.next_instid_in_stream = None

    def writeLabel(self):
        OSTREAM.write('label_%d: ; ' % self.number)

    def notImplemented(self, s):
        raise NotImplementedError("Instruction.%s(): instr: %d " % (s, self.number))

    def write(self):
        self.notImplemented('write')

    def emit_3addr(self):
        return '    instr %d: %s%s' % (self.number, Opcodes.code2str(self.opcode), self.emit_3addr_args())

    def write_3addr(self):
        OSTREAM.write(self.emit_3addr())

    def emit_3addr_args(self):
        self.notImplemented('emit_3addr_args')

    @property
    def expression(self):
        return None

    @property
    def local_variables(self):
        """Local variables used by this instruction. We need to declare
        them."""
        return []

    @property
    def successors(self):
        """
        The id of successors.
        """
        return [ self.next_instid_in_stream ]

    @property
    def dest_variables(self):
        d = self.dest_variable
        return [ d ] if (d is not None) else []

    @property
    def src_variables(self):
        self.notImplemented('src_variables')

    @property
    def starts_basic_block(self):
        return (self.basic_block.first_inst is self)

class ZeroArgInstruction(HasNoSourceOperand, Instruction):
    def __init__(self, number, opcode, args):
        Instruction.__init__(self, number, opcode)
        assert (len(args) == 0)

    def write(self):
        OSTREAM.write('ZeroArgInstruction: NotImplemented')

    def emit_3addr_args(self):
        return ''

class OneArgInstruction(Instruction):
    def __init__(self, number, opcode, args):
        Instruction.__init__(self, number, opcode)
        assert (len(args) == 1)
        self.arg0 = makeOperand(args[0])

    def write(self):
        OSTREAM.write('long long register_%d = (%s %s);' % (self.number, Opcodes.c_symbol(self.opcode), self.arg0.emit()))

    @property
    def local_variables(self):
        if isinstance(self.arg0, LocalScalarVariable):
            return [ self.arg0.name ]
        else:
            return []

    def emit_3addr_args(self):
        return ' %s' % self.arg0.emit_3addr()

class TwoArgInstruction(Instruction):

    def __init__(self, number, opcode, args):
        Instruction.__init__(self, number, opcode)
        assert (len(args) == 2)
        self.arg0 = makeOperand(args[0])
        self.arg1 = makeOperand(args[1])

    def write(self):
        OSTREAM.write('long long register_%d = (%s %s %s);' % (self.number, self.arg0.emit(), Opcodes.c_symbol(self.opcode), self.arg1.emit()))

    def emit_3addr_args(self):
        return ' %s %s' % (self.arg0.emit_3addr(), self.arg1.emit_3addr())

    @property
    def local_variables(self):
        l = []
        if isinstance(self.arg0, LocalScalarVariable):
            l.append(self.arg0.name)
        if isinstance(self.arg1, LocalScalarVariable):
            l.append(self.arg1.name)
        return l

class BinaryOpInstruction(ArithmeticInstruction, HasTwoSourceOperands, TwoArgInstruction):
    @property
    def expression(self):
        return Expression(self.opcode, self.arg0, self.arg1)

class UnaryOpInstruction(ArithmeticInstruction, HasOneSourceOperand, OneArgInstruction):
    @property
    def expression(self):
        return Expression(self.opcode, self.arg0, None)

class EnterInstruction(NeverDead, DoesNotWriteToDestRegister, HasNoSourceOperand, OneArgInstruction):
    def __init__(self, number, opcode, args):
        OneArgInstruction.__init__(self, number, opcode, args)
        assert isinstance(self.arg0, Constant)
        self.num_locals = self.arg0.number
        assert ((self.num_locals % 8) == 0)

    def write(self):
        """Generate code for local variables."""
        OSTREAM.write('{\n')
        if self.num_locals > 0:
            n = (self.num_locals / 8)
            OSTREAM.write('long long local_vars[%d];\n' % n)
            OSTREAM.write('void *FP = &(local_vars[%d]);\n' % n)
        else:
            OSTREAM.write('// No local variable\n');

class RetInstuction(NeverDead, DoesNotWriteToDestRegister, HasNoSourceOperand, OneArgInstruction):
    def __init__(self, number, opcode, args):
        OneArgInstruction.__init__(self, number, opcode, args)
        assert isinstance(self.arg0, Constant)
        self.num_params = self.arg0.number
        assert ((self.num_params % 8) == 0)

    def write(self):
        OSTREAM.write('}')

    @property
    def successors(self):
        return []

class ParamInstruction(NeverDead, WritesToDestRegister, HasOneSourceOperand, OneArgInstruction): pass

class EntryPcInstruction(NeverDead, DoesNotWriteToDestRegister, ZeroArgInstruction): pass

class CallInstruction(BranchInstruction, OneArgInstruction):
    params = None

    def write(self):
        assert (self.params is not None)
        params = ', '.join(i.emit() for i in reversed(self.params))
        OSTREAM.write('function_%s(%s);' % (self.arg0.emit(), params))

    @property
    def src_variables(self):
        assert (self.params is not None)
        return self.params

    @prop
    def branch_target():
        """The branch target."""
        def fget(self):
            return self.arg0
        def fset(self, value):
            self.arg0 = value
        return locals()

class NopInstruction(DoesNotWriteToDestRegister, ZeroArgInstruction): pass

class ConditionalBranch(BranchInstruction, HasOneSourceOperand, TwoArgInstruction):
    """
    >>> inst = ConditionalBranch(34, Opcodes.BLBC, [ '(33)', '[40]' ])
    >>> print inst.emit_3addr()
        instr 34: blbc (33) [40]
    >>> print inst.branch_target.emit_3addr()
    [40]
    >>> label = Label(41, source_code=None)
    >>> inst.branch_target = label
    >>> print inst.branch_target.emit_3addr()
    [41]
    >>> print inst.emit_3addr()
        instr 34: blbc (33) [41]
    """
    def __init__(self, number, opcode, args):
        TwoArgInstruction.__init__(self, number, opcode, args)
        assert isinstance(self.arg1, Label)

    @property
    def successors(self):
        return [ self.arg1.number, self.next_instid_in_stream ]

    def write(self):
        """Generate code for a conditional branch."""
        OSTREAM.write('if (%s(%s)) goto %s;' % (Opcodes.c_symbol(self.opcode), self.arg0.emit(), self.arg1.emit()))

    @prop
    def branch_target():
        """The branch target."""
        def fget(self):
            return self.arg1
        def fset(self, value):
            self.arg1 = value
        return locals()

class UnconditionalBranch(BranchInstruction, HasNoSourceOperand, OneArgInstruction):
    def __init__(self, number, opcode, args):
        OneArgInstruction.__init__(self, number, opcode, args)
        assert isinstance(self.arg0, Label)

    @property
    def successors(self):
        return [ self.arg0.number ]

    def write(self):
        """Generate code for an unconditional branch."""
        OSTREAM.write('goto %s;' % self.arg0.emit())

    @prop
    def branch_target():
        """The branch target."""
        def fget(self):
            return self.arg0
        def fset(self, value):
            self.arg0 = value
        return locals()

class MovInstruction(HasOneSourceOperand, TwoArgInstruction):

    def __init__(self, number, opcode, args):
        TwoArgInstruction.__init__(self, number, opcode, args)
        assert isinstance(self.arg1, (FormalParameter, LocalScalarVariable))

    def write(self):
        OSTREAM.write('%s = %s;' % (self.arg1.emit(), self.arg0.emit()))

    @property
    def dest_variable(self):
        return self.arg1

class StoreInstruction(NeverDead, DoesNotWriteToDestRegister, HasTwoSourceOperands, TwoArgInstruction):
    def write(self):
        OSTREAM.write('*((long long *)%s) = %s;' % (self.arg1.emit(), self.arg0.emit()))

class LoadInstruction(WritesToDestRegister, HasOneSourceOperand, OneArgInstruction):
    def write(self):
        OSTREAM.write('long long register_%d = *((long long *)%s);' % (self.number, self.arg0.emit()))

class UnknownInstruction(Instruction):
    def __init__(self, number, opcode, args):
        raise NotImplementedError("Uknown instruction: instr %d: Not implemented this opcode: %d" % (number, opcode))

class ReadInstruction(NeverDead, WritesToDestRegister, ZeroArgInstruction):
    def write(self):
        OSTREAM.write('long long register_%d; ReadLong(register_%d);' % (self.number, self.number))

class WriteInstruction(NeverDead, DoesNotWriteToDestRegister, HasOneSourceOperand, OneArgInstruction):
    def write(self):
        OSTREAM.write('WriteLong(%s);' % self.arg0.emit())

class WriteLineInstruction(NeverDead, DoesNotWriteToDestRegister, ZeroArgInstruction):
    def write(self):
        OSTREAM.write('WriteLine();')

class Opcodes:

    (ADD, SUB, MUL, DIV, MOD, NEG, PARAM, ENTER, LEAVE, RET, END, CALL, BR,
     BLBC, BLBS, CMPEQ, CMPLE, CMPLT, READ, WRITE, WRL, LOAD, STORE, MOVE,
     NOP, ENTRYPC) = range(26)

    str2code_hash = {
      "add"   : ADD,
      "sub"   : SUB,
      "mul"   : MUL,
      "div"   : DIV,
      "mod"   : MOD,
      "neg"   : NEG,
      "param" : PARAM,
      "enter" : ENTER,
      "leave" : LEAVE,
      "ret"   : RET,
      "end"   : END,
      "call"  : CALL,
      "br"    : BR,
      "blbc"  : BLBC,
      "blbs"  : BLBS,
      "cmpeq" : CMPEQ,
      "cmple" : CMPLE,
      "cmplt" : CMPLT,
      "read"  : READ,
      "write" : WRITE,
      "wrl"   : WRL,
      "load"  : LOAD,
      "store" : STORE,
      "move"  : MOVE,
      "nop"   : NOP,
      "entrypc": ENTRYPC,
    }

    code2str_hash = {
      ADD     : "add",
      SUB     : "sub",
      MUL     : "mul",
      DIV     : "div",
      MOD     : "mod",
      NEG     : "neg",
      PARAM   : "param",
      ENTER   : "enter",
      LEAVE   : "leave",
      RET     : "ret",
      END     : "end",
      CALL    : "call",
      BR      : "br",
      BLBC    : "blbc",
      BLBS    : "blbs",
      CMPEQ   : "cmpeq",
      CMPLE   : "cmple",
      CMPLT   : "cmplt",
      READ    : "read",
      WRITE   : "write",
      WRL     : "wrl",
      LOAD    : "load",
      STORE   : "store",
      MOVE    : "move",
      NOP     : "nop",
      ENTRYPC : "entrypc",
    }

    constructor = {
      ADD   : BinaryOpInstruction,
      SUB   : BinaryOpInstruction,
      MUL   : BinaryOpInstruction,
      DIV   : BinaryOpInstruction,
      MOD   : BinaryOpInstruction,
      NEG   : UnaryOpInstruction,
      PARAM : ParamInstruction,
      ENTER : EnterInstruction,
      LEAVE : UnknownInstruction,
      RET   : RetInstuction,
      END   : UnknownInstruction,
      CALL  : CallInstruction,
      BR    : UnconditionalBranch,
      BLBC  : ConditionalBranch,
      BLBS  : ConditionalBranch,
      CMPEQ : BinaryOpInstruction,
      CMPLE : BinaryOpInstruction,
      CMPLT : BinaryOpInstruction,
      READ  : ReadInstruction,
      WRITE : WriteInstruction,
      WRL   : WriteLineInstruction,
      LOAD  : LoadInstruction,
      STORE : StoreInstruction,
      MOVE  : MovInstruction,
      NOP   : NopInstruction,
      ENTRYPC: EntryPcInstruction,
    }

    c_symbol_hash = {
      ADD   : '+',
      SUB   : '-',
      MUL   : '*',
      DIV   : '/',
      MOD   : '%',
      NEG   : '-',
      CMPEQ : '==',
      CMPLE : '<=',
      CMPLT : '<',
      BLBC  : '!',
      BLBS  : '',
      PARAM : '',
    }

    @classmethod
    def c_symbol(kls, s):
        return kls.c_symbol_hash[s]

    @classmethod
    def str2code(kls, s):
        return kls.str2code_hash[s]

    @classmethod
    def code2str(kls, opcode):
        return kls.code2str_hash[opcode]

def makeInstruction(line):
    """A factory of Instruction(s). Used to create any instruction."""
    line = line.strip()
    l = line.split()
    assert l[0] == 'instr'
    line_num = int(l[1][:-1])
    opcode = Opcodes.str2code(l[2])
    args = l[3:]
    inst = Opcodes.constructor[opcode](line_num, opcode, args)
    inst.source_code = line
    return inst

class Expression(FlyWeight):
    """
    >>> from operands import makeOperand
    >>> o1 = makeOperand('a#24')
    >>> o2 = makeOperand('(13)')
    >>> e1 = Expression(Opcodes.ADD, o1, o2)
    >>> e2 = Expression(Opcodes.ADD, o1, makeOperand('(13)'))
    >>> e1 is e2
    True
    >>> e1 == Expression(Opcodes.SUB, o1, o2)
    False
    >>> o3 = makeOperand('j#-280')
    >>> o4 = makeOperand('2')
    >>> e3 = Expression(Opcodes.MUL, o3, o4)
    >>> e4 = Expression(Opcodes.MUL, makeOperand('j#-280'), makeOperand('2'))
    >>> e3 == e4
    True
    >>> e3 is e4
    True
    """

    _FlyWeight_Pool = {}

    def __init__(self, opcode, arg0, arg1):
        self.opcode = opcode
        self.arg0 = arg0
        self.arg1 = arg1

    @property
    def operands(self):
        op = [ self.arg0, self.arg1 ]
        return [ i for i in op if i is not None ]

    def __repr__(self):
        a0 = self.arg0.emit_3addr()
        if self.arg1:
            a1 = self.arg1.emit_3addr()
        else:
            a1 = a1
        return '<%s %s %s>' % (Opcodes.code2str(self.opcode), a0, a1)

if __name__ == '__main__':
    import doctest
    doctest.testmod()
