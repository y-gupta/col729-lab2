

class ShouldHaveOnlyOnePredecessor(AssertionError): pass
class ShouldHaveOnlyOneSuccessor(AssertionError): pass

class CFGNode:
    def __init__(self):
        self.cfg_prevS = []
        self.cfg_nextS = []

    def add_cfg_next(self, inst):
        self.cfg_nextS.append(inst)

    def add_cfg_prev(self, inst):
        self.cfg_prevS.append(inst)

    def remove_cfg_next(self, inst):
        self.cfg_nextS.remove(inst)

    def remove_cfg_prev(self, inst):
        self.cfg_prevS.remove(inst)

    @property
    def cfg_prev(self):
        if len(self.cfg_prevS) != 1:
            raise ShouldHaveOnlyOnePredecessor()
        return self.cfg_prevS[0]

    @property
    def cfg_next(self):
        if len(self.cfg_nextS) != 1:
            raise ShouldHaveOnlyOneSuccessor()
        return self.cfg_nextS[0]

    def replace_cfg_prev(self, old_prev, new_prev):
        self.remove_cfg_prev(old_prev)
        self.add_cfg_prev(new_prev)

    def replace_cfg_next(self, old_next, new_next):
        self.remove_cfg_next(old_next)
        self.add_cfg_next(new_next)
