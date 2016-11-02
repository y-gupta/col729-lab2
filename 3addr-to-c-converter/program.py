
# $Id: program.py 849 2007-09-25 20:06:42Z suriya $

class Program:
    def __init__(self):
        self.functions = []
    def add_function(self, func):
        self.functions.append(func)
