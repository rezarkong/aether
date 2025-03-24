from f import *
from utils import *

_z0 = int2bits(0x428a2f98d728ae227137449123ef65cd, 128)
_z1 = int2bits(0xb5c0fbcfec4d3b2fe9b5dba58189dbbc, 128)
_z2 = int2bits(0x7137449123ef65cd428a2f98d728ae22, 128)

def _xor(x, y):
    return [ x[i] ^ y[i] for i in range(128) ]

def roundupdate0(s, x0, f):
    s0 = _xor(f(s[3]), x0)
    s1 = _xor(f(s[0]), s[1])
    s2 = _xor(f(s[1]), s[3])
    s3 = _xor(f(s[2]), s[0])
    s[0], s[1], s[2], s[3] = s0, s1, s2, s3

def roundupdate1(s, x0, x1, f):
    s0 = _xor(f(s[5]), x0)
    s1 = _xor(f(s[0]), x1)
    s2 = _xor(f(s[1]), s[5])
    s3 = _xor(f(s[2]), s[0])
    s4 = _xor(f(s[3]), s[2])
    s5 = _xor(f(s[4]), s[1])
    s[0], s[1], s[2], s[3], s[4], s[5] = s0, s1, s2, s3, s4, s5

def roundupdate2(s, x0, x1, x2, f):
    s0 = _xor(f(s[8]), x0)
    s1 = _xor(f(s[0]), x1)
    s2 = _xor(f(s[1]), s[2])
    s3 = _xor(f(s[2]), x2)
    s4 = _xor(f(s[3]), s[0])
    s5 = _xor(f(s[4]), s[1])
    s6 = _xor(f(s[5]), s[3])
    s7 = _xor(f(s[6]), s[8])
    s8 = _xor(f(s[7]), s[4])
    s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8] = s0, s1, s2, s3, s4, s5, s6, s7, s8
    
def initialization0(n, k0, k1, f):
    s = [ k1[:], _xor(n, k0), _z0[:], _z1[:], [0]*128 ]
    
    for _ in range(20):
        roundupdate0(s, _z0, f)

    s[0] = _xor(s[0], k0)
    s[1] = _xor(s[1], k1)
    return s

def initialization1(n, k0, k1, f):
    s = [ k1[:], _xor(n, k0), _z0[:], _z1[:], [0]*128, [0]*128, [0]*128 ]
    
    for _ in range(20):
        roundupdate1(s, _z0, _z1, f)

    s[0] = _xor(s[0], k0)
    s[1] = _xor(s[1], k1)
    return s

def initialization2(n, k0, k1, f):
    s = [ k1[:], _xor(n, k0), _z0[:], _z1[:], [0]*128,[0]*128, [0]*128, [0]*128, [0]*128, [0]*128]
    
    for _ in range(20):
        roundupdate2(s, _z0, _z1, _z2, f)

    s[0] = _xor(s[0], k0)
    s[1] = _xor(s[1], k1)
    return s

def absorbad0(s, ad0, adlen, f):
    for i in range(adlen):
        roundupdate0(s, ad0[i], f)

def absorbad1(s, ad0, ad1, adlen, f):
    for i in range(adlen):
        roundupdate1(s, ad0[i], ad1[i], f)

def absorbad2(s, ad0, ad1, ad2, adlen, f):
    for i in range(adlen):
        roundupdate2(s, ad0[i], ad1[i], ad2[i], f)

def absorbmsg0(s, msg0, msglen, f):
    func = lambda i : _xor(_xor(f(_xor(s[0], s[1])), s[3]), msg0[i])

    ct = []
    for i in range(msglen):
        ct0 = func(i)
        ct.append(ct0)
        roundupdate0(s, msg0[i], f)
    return ct

def absorbmsg1(s, msg0, msg1, msglen, f):
    func = lambda i : (
        _xor(_xor(f(_xor(s[0], s[1])), s[3]), msg0[i]),
        _xor(_xor(f(_xor(s[2], s[3])), s[4]), msg1[i]),
    )

    ct = []
    for i in range(msglen):
        ct0, ct1 = func(i)
        ct.append(ct0 + ct1)
        roundupdate1(s, msg0[i], msg1[i], f)
    return ct

def absorbmsg2(s, msg0, msg1, msg2, msglen, f):
    func = lambda i : (
        _xor(_xor(f(_xor(s[0], s[1])), s[3]), msg0[i]),
        _xor(_xor(f(_xor(s[2], s[3])), s[4]), msg1[i]),
        _xor(_xor(f(_xor(s[0], s[3])), s[7]), msg2[i]),
    )

    ct = []
    for i in range(msglen):
        ct0, ct1, ct2 = func(i)
        ct.append(ct0 + ct1 + ct2)
        roundupdate2(s, msg0[i], msg1[i], msg2[i], f)
    return ct

def outputtag0(s, adlen, msglen, f):
    for i in range(20):
        roundupdate0(s, int2bits(adlen, 128), f)
    y = s[0]
    for i in range(1, 5):
        y = _xor(y, s[i])
    return y

def outputtag1(s, adlen, msglen):
    for i in range(20):
        roundupdate1(s, int2bits(adlen, 128), int2bits(msglen, 128), f)
    y = s[0]
    for i in range(1, 7):
        y = _xor(y, s[i])
    return y

def outputtag2(s, adlen, msglen):
    for i in range(20):
        roundupdate2(s, int2bits(adlen, 128), int2bits(msglen, 128), _z0, f)
    y = s[0]
    for i in range(1, 10):
        y = _xor(y, s[i])
    return y

def encrypt0(n, ad, msg, k, f):
    k0, k1 = k[0:128], k[128:256]
    adlen, msglen = len(ad), len(msg)
    ad0  = [ ad[i][0:128]   for i in range(adlen)  ]
    msg0 = [ msg[i][0:128]  for i in range(msglen) ]

    ct = []
    
    s = initialization0(n, k0, k1, f)
    if adlen > 0:
        absorbad0(s, ad0,  adlen, f)
    if msglen > 0:
        ct = absorbmsg0(s, msg0, msglen, f)
    t = outputtag0(s, adlen, msglen, f) 
    return ct, t

def encrypt1(n, ad, msg, k, f):
    k0, k1 = k[0:128], k[128:256]
    adlen, msglen = len(ad), len(msg)
    ad0  = [ ad[i][0:128]    for i in range(adlen)  ]
    ad1  = [ ad[i][128:256]  for i in range(adlen)  ]
    msg0 = [ msg[i][0:128]   for i in range(msglen) ]
    msg1 = [ msg[i][128:256] for i in range(msglen) ]

    ct = []
    
    s = initialization1(n, k0, k1, f)
    if adlen > 0:
        absorbad1(s, ad0, ad1, adlen, f)
    if msglen > 0:
        ct = absorbmsg1(s, msg0, msg1, msglen, f)
    t = outputtag1(s, adlen, msglen, f) 
    return ct, t

def encrypt2(n, ad, msg, k, f):
    k0, k1 = k[0:128], k[128:256]
    adlen, msglen = len(ad), len(msg)
    ad0  = [ ad[i][0:128]    for i in range(adlen)  ]
    ad1  = [ ad[i][128:256]  for i in range(adlen)  ]
    ad2  = [ ad[i][256:384]  for i in range(adlen)  ]
    msg0 = [ msg[i][0:128]   for i in range(msglen) ]
    msg1 = [ msg[i][128:256] for i in range(msglen) ]
    msg2 = [ msg[i][256:384] for i in range(msglen) ]

    ct = []
    
    s = initialization2(n, k0, k1, f)
    if adlen > 0:
        absorbad2(s, ad0, ad1, ad2, adlen, f)
    if msglen > 0:
        ct = absorbmsg2(s, msg0, msg1, msg2, msglen, f)
    t = outputtag2(s, adlen, msglen, f) 
    return ct, t

n   = int2bits(0x000102030405060708090A0B0C0D0E0F, 128)
k   = int2bits(0x000102030405060708090A0B0C0D0E0F000102030405060708090A0B0C0D0E0F, 256)
ad  = int2bits(0x000102030405060708090A0B0C0D0E0F, 128)
msg = int2bits(0x000102030405060708090A0B0C0D0E0F, 128)
# msg = int2bytes(0xAF803CE25906F1D19FB6C6804E06EA28AB178F457AF6B493B7439EC6D4290062, 256)
# k   = int2bytes(0x9E3225A9F133B5DEA168F4E2851F072FCC00FCAA7CA62061717A48E52E29A3FA, 256)

ct, t = encrypt0(n, [ad], [msg], k, f_enae)
print(bits2hex(ct[0]), bits2hex(t))
    
