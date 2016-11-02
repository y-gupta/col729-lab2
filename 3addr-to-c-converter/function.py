
import logging

from instructions import (Opcodes, BranchInstruction, EnterInstruction,
        RetInstuction, ParamInstruction, CallInstruction)
from operands import VirtualRegister
from utils import pairwise

from globalvars import OSTREAM

class Function:
    def __init__(self, entry_pc_inst):
        """
        entry_pc_inst is an instance of EntryPcInstruction. This is not None if
        this is the main function. Otherwise this is None.
        """
        self.entry_pc_inst = entry_pc_inst
        self.instructions = []
        self.basicblocks = None
        # param instructions before the next call.
        self.params = []
        self.local_variables = set()
        # We will accept no more instructions.
        self.accept_no_more = False

    @property
    def function_name(self):
        return 'function_label_%d' % self.enter.number

    @property
    def enter(self):
        """The first instruction of the function."""
        inst = self.instructions[0]
        assert isinstance(inst, EnterInstruction)
        return inst

    @property
    def ret(self):
        """The last instruction of the function."""
        inst = self.instructions[-1]
        assert isinstance(inst, RetInstuction)
        return inst

    def validate_cfg(self):
        pass
    # Remove an instruction from the function.
    def remove_instruction(self, inst):
        pass

    def replace_instruction(self, inst, new_inst):
        pass

    def compute_basic_blocks(self):
        pass
    def compute_various_info(self):
        """Compute information like Basicblocks, CFG, etc."""
        self.compute_basic_blocks()

    def add(self, instruction):
        # Sanity check, have we already done the processing.
        assert not self.accept_no_more

        # Add this instruction to the list
        self.instructions.append(instruction)

        # Update local variables
        self.local_variables.update(instruction.local_variables)

        # Any other special processing.
        if isinstance(instruction, ParamInstruction):
            self.params.append(instruction.dest_variable)
        elif isinstance(instruction, CallInstruction):
            instruction.params = self.params
            self.params = []
        elif isinstance(instruction, RetInstuction):
            self.accept_no_more = True
            self.compute_various_info()

    def write(self):
        # Function header
        OSTREAM.write('void %s(' % self.function_name)
        OSTREAM.write(', '.join('long long formal_param_%d' % i for i in xrange(self.ret.num_params / 8)))
        OSTREAM.write(')\n')
        # Process the enter instruction first
        self.enter.write()
        # Declare the scalar local variables that have to be register
        # allocated
        OSTREAM.write('// Scalar local variable declared below\n')
        for i in self.local_variables:
            OSTREAM.write('long long %s;\n' % i)
        # Process all instructions, but the enter instruction
        for i in self.instructions[1:]:
            i.writeLabel()
            i.write()
            OSTREAM.write('\n')
        if self.entry_pc_inst is not None:
            OSTREAM.write('int main() { %s(); return 0; }' % self.function_name)

    def write_3addr(self):
        if self.entry_pc_inst is not None:
            self.entry_pc_inst.write_3addr()
            OSTREAM.write('\n')
        for i in self.instructions:
            i.write_3addr()
            OSTREAM.write('\n')
