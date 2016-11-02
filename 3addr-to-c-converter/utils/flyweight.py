
class FlyWeight(object):

    # Copied from http://codesnipers.com/?q=python-flyweights
    _FlyWeight_Pool = None
    def __new__(cls, *args):
        key = args
        obj = cls._FlyWeight_Pool.get(key, None)
        if obj is None:
            obj = super(FlyWeight, cls).__new__(cls, *args)
            cls._FlyWeight_Pool[key] = obj
        return obj

class Base(object):
    def __init__(self, value):
        self.value = value

class A(FlyWeight, Base):
    """
    >>> a1 = A(1)
    >>> a2 = A(2)
    >>> a3 = A(3)
    >>> a4 = A(1)
    >>> print (a1 is a4)
    True
    """
    _FlyWeight_Pool = {}

    def __repr__(self):
        '<repr A %d>' % self.value

if __name__ == '__main__':
    import doctest
    doctest.testmod()
