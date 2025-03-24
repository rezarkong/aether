""" Common integer mangling helper functions """

def int2bits(x, bits):
    """ Convert an integer of size 'bits' to an array of 0, 1 integers.

    >>> int2bits(0x00, 8)
    [0, 0, 0, 0, 0, 0, 0, 0]
    >>> int2bits(0x01, 8)
    [0, 0, 0, 0, 0, 0, 0, 1]
    >>> int2bits(0x01, 3)
    [0, 0, 1]
    >>> int2bits(0x05, 3)
    [1, 0, 1]
    """
    return [ (x >> (i)) & 0x1 for i in range(bits) ][::-1]

def bits2int(x):
    """ Convert a 0, 1 integer array to an integer.

    >>> bits2int([0, 0, 0, 0])
    0
    >>> bits2int([1, 1, 1])
    7
    >>> bits2int([1, 1, 1, 0])
    14
    """
    return int(''.join(str(i) for i in x), 2) 

def bits2nibbles(x):
    """ Convert a bit array to array of nibbles.

    >>> bits2nibbles([0, 0, 0, 0])
    [0]
    >>> bits2nibbles([0, 0, 0, 0, 0, 0, 0, 0])
    [0, 0]
    >>> bits2nibbles([1, 0, 0, 0, 0, 0, 1, 1])
    [8, 3]
    >>> bits2nibbles([1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0])
    [12, 3, 2]
    """
    n = len(x)//4
    y = [0] * n
    for i in range(n):
        nb = x[i*4:i*4+4]
        y[i] = int(''.join(str(j) for j in nb), 2)
    return y

def nibbles2bits(x):
    """ Convert an nibble array into a bit array.

    >>> nibbles2bits([0x0])
    [0, 0, 0, 0]
    >>> nibbles2bits([0x1, 0x0])
    [0, 0, 0, 1, 0, 0, 0, 0]
    >>> nibbles2bits([0xF, 0x5])
    [1, 1, 1, 1, 0, 1, 0, 1]
    >>> nibbles2bits([0xF, 0x5, 0x8])
    [1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0]
    """

    b = []
    for i in range(len(x)):
        b.extend(int2bits(x[i], 4))
    return b

def bits2hex(b):
    """ Convert a bit array into a hex string. 

    >>> bits2hex([0, 0, 0, 0, 0, 0, 1, 0])
    '02'
    """
    if len(b) == 0: return ""
    return '%0*X' % (len(b)  // 4, int("".join(map(str, b)), 2))

def nibbles2hex(b):
    """ Convert an array of nibble values to a hex string.

    >>> nibbles2hex([0x0])
    '0'
    >>> nibbles2hex([0x1, 0xF])
    '1F'
    >>> nibbles2hex([0x6, 0x1, 0xF])
    '61F'
    >>> nibbles2hex([0x1, 0x6, 0x1, 0xF])
    '161F'
    """
    s = ""
    for i in b:
        s = s + "%01X" % (i)
    return s

if __name__ == "__main__":
    import doctest
    doctest.testmod()
