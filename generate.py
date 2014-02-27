#!/usr/bin/env python

import warnings
import binascii
import sys
import os
import base64

with warnings.catch_warnings():
    ''' Suppress cffi/vengine_cpy.py:166: UserWarning: reimporting '_cffi__x332a1fa9xefb54d7c' might overwrite older definitions '''
    warnings.simplefilter('ignore')

    import nacl.utils
    from nacl.public import PrivateKey, PublicKey, Box
    from nacl.encoding import HexEncoder, Base64Encoder

def store(username, sk, pk):
    s_path = '%s.secret' % username
    p_path = '%s.pub' % username

    if os.path.exists(s_path):
        print "file %s exists: abort" % s_path
        sys.exit(1)
    if os.path.exists(p_path):
        print "file %s exists: abort" % p_path
        sys.exit(1)

    s = open(s_path, 'w')
    s.write("%s\n" % base64.b64encode(sk))
    s.close()

    p = open(p_path, 'w')
    p.write("%s\n" % base64.b64encode(pk))
    p.close()



if __name__ == '__main__':
    if len(sys.argv) != 2:
        print "Usage: username"
        sys.exit(1)

    username = sys.argv[1]

    secret_key = PrivateKey.generate()
    public_key = secret_key.public_key

    store(username, secret_key.__bytes__(), public_key.__bytes__())


print "secret key (hex)    ", binascii.hexlify(secret_key.__bytes__())
print "secret key (b64)    ", base64.b64encode(secret_key.__bytes__())
print "public key (hex)    ", binascii.hexlify(public_key.__bytes__())
print "public key (b64)    ", base64.b64encode(public_key.__bytes__())

