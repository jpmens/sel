#!/usr/bin/env python

import warnings
import binascii
import base64
import sys

def loadkey(path):
    f = open(path)
    b64blob = f.read()
    f.close()
    return b64blob

def load_secretkey(user):
    return loadkey("%s.secret" % user)

def load_publickey(user):
    return loadkey("%s.pub" % user)

with warnings.catch_warnings():
    ''' Suppress cffi/vengine_cpy.py:166: UserWarning: reimporting '_cffi__x332a1fa9xefb54d7c' might overwrite older definitions '''
    warnings.simplefilter('ignore')

    import nacl.utils
    from nacl.public import PrivateKey, PublicKey, Box
    from nacl.encoding import Base64Encoder

from_secretkey = load_secretkey('alice')
to_public      = load_publickey('eve')

sender_privatekey = PrivateKey(from_secretkey, encoder=Base64Encoder)
sender_publickey = sender_privatekey.public_key

recipient_publickey = PublicKey(to_public, encoder=Base64Encoder)

# SENDER wishes to send RECIPIENT an encrypted message
# So SENDER must make a Box with its private key and RECIPIENT's public key
box = Box(sender_privatekey, recipient_publickey)

# This is our message to send, it must be a bytestring as Box will
#   treat is as just a binary blob of data.
# message = "The daughter is leaving on a journey"
message = sys.stdin.read()

# This is a nonce, it *MUST* only be used once, but it is not considered
#   secret and can be transmitted or stored alongside the ciphertext. A
#   good source of nonce is just 24 random bytes.
nonce = nacl.utils.random(Box.NONCE_SIZE)

encrypted = box.encrypt(message, nonce)

out_nonce = encrypted[0:24]
out_crypted = encrypted[24:]

print "%s:%s" % (base64.b64encode(out_nonce), base64.b64encode(out_crypted))

