
from itertools import tee, izip

def pairwise(iterable):
    "s -> (s0,s1), (s1,s2), (s2, s3), ..."
    a, b = tee(iterable)
    try:
        b.next()
    except StopIteration:
        pass
    return izip(a, b)

def prop(func):
  """Function decorator for defining property attributes

  The decorated function is expected to return a dictionary
  containing one or more of the following pairs:
      fget - function for getting attribute value
      fset - function for setting attribute value
      fdel - function for deleting attribute
  This can be conveniently constructed by the locals() builtin
  function; see:
  http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/205183
  """
  return property(doc=func.__doc__, **func())
