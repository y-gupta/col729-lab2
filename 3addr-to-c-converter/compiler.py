#!/usr/bin/env python

import sys
import logging
import atexit

from program import Program
from instructions import makeInstruction, EnterInstruction, RetInstuction, EntryPcInstruction
from function import Function
from globalvars import ISTREAM, OSTREAM

from backends import BackToCBackend

def initialize_logger(logfile):
    logging.basicConfig(level=logging.DEBUG,
      format='%(asctime)s %(levelname)-8s %(message)s',
      datefmt='%a, %d %b %Y %H:%M:%S',
      filename=logfile, filemode='w')
    logging.info('Initializing log')
    atexit.register(logging.info, 'Finalizing log')

def cmdline(argv):
    opts = []
    backend = None
    OPTIMIZATIONS = {

    }
    BACKENDS = {
       'c':     BackToCBackend,
    }
    for i in argv[1:]:
        if i.startswith('-opt='):
            opts = [ OPTIMIZATIONS[o] for o in i[5:].split(',') ]
        if i.startswith('-backend='):
            b = i[9:]
            backend = BACKENDS[b]
    return opts, backend

def frontend(stream):
    program = Program()
    func = None
    entry_pc_inst = None
    for line in stream:
        instruction = makeInstruction(line)
        if isinstance(instruction, EntryPcInstruction):
            entry_pc_inst = instruction
        if isinstance(instruction, EnterInstruction):
            func = Function(entry_pc_inst)
            entry_pc_inst = None
        if func is not None:
            func.add(instruction)
        if isinstance(instruction, RetInstuction):
            program.add_function(func)
            func = None
    return program

def doAll(stream):
    opts, backend = cmdline(sys.argv)
    program = frontend(stream)
    if backend is not None:
        backend.write(program)

initialize_logger('logfile.txt')
doAll(ISTREAM)
