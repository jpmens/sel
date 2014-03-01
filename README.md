# Experiments with libsodium and PyNaCL

```
$ ./generate.py alice
secret key (hex)     5267d5f1de0304ca760e94f60a7d38e008425b68161fa67063b18d46eef8f84d
secret key (b64)     UmfV8d4DBMp2DpT2Cn044AhCW2gWH6ZwY7GNRu74+E0=
public key (hex)     7f452b4bba065c5588e12ff6cf133d24897c589fdf52534e8789a23b59ba3530
public key (b64)     f0UrS7oGXFWI4S/2zxM9JIl8WJ/fUlNOh4miO1m6NTA=

$ ./generate.py eve
secret key (hex)     08adcdf8558b53a4a0bbef76ee0a393c202ac9e664f4de98110a1e4bcbe1d376
secret key (b64)     CK3N+FWLU6Sgu+927go5PCAqyeZk9N6YEQoeS8vh03Y=
public key (hex)     068e4051db5e8b4ea1f470860994af59cca07f0b2c2344897a720b4be9daef54
public key (b64)     Bo5AUdtei06h9HCGCZSvWcygfwssI0SJenILS+na71Q=

$ ./test1.sh
In the submissive way of one long accustomed to obey under coercion, he
ate and drank what they gave him to eat and drink, and put on the cloak
and other wrappings, that they gave him to wear. He readily responded to
his daughter's drawing her arm through his, and took--and kept--her hand
in both his own.
$
```

## Requires

* pip install [pynacl](https://github.com/pyca/pynacl)
* brew install [libsodium](https://github.com/jedisct1/libsodium)

## See also

* [libsodium-php](https://github.com/jedisct1/libsodium-php) for an explanation
* [Questions about the NaCl crypto library](http://stackoverflow.com/questions/13663604/questions-about-the-nacl-crypto-library)
* [How do Ed5519 keys work?](http://blog.mozilla.org/warner/2011/11/29/ed25519-keys/)
* [A well documented version of libNaCl and libSodium's ABI.](https://gist.github.com/jpmens/9289631)
* [Threema Encryption Validation](https://threema.ch/validation/)

### An email I sent to sodium mailing list

```
I've been experimenting with libsodium and PyNaCl, and I think I'm a
little confused. The code I'm messing about with is at [1]. (A Python
program encrypts a message which a C program decrypts.)

If Alice encrypts a message to Eve, then Eve can decrypt this message
using Alice's public key and her (Eve's) private key:

        #                 PUBkey      SECRETkey
        eve$ ./jp-decrypt alice.pub   eve.secret    < /tmp/message

However, Alice can decrypt the same message with Alice's secret key and
Eve's public key:

        #                   PUBkey      SECRETkey
        alice$ ./jp-decrypt eve.pub     alice.secret  < /tmp/message

I thought I had basic understanding about public key crypto, but I'm
confused nevertheless, because this works....

Is this behaviour correct, or is my code horrendously broken?

Regards,

        -JP

[1] https://github.com/jpmens/sel
```

and a very helpful response:

```
Yes, that's expected behavior.

box(m, n, a, E) and box(m, n, e, A) give the same result.

This also has the important implication that you must ensure that you
never use the same nonce for a message Alice sends to Eve and for a
message Eve sends to Alice.

Box has two parts:

box_beforenm computes a shared key given a public key and a secret
key. This shared key is the same when you use Alice's public key `A`
and Eve's private key `e` and vice versa (using `E` and `a`).
This form of symmetry is an inherent property of Diffie-Hellman.

    shared-key = Diffie-Hellman(e, A) = Diffie-Hellman(a, E)

If you go one level deeper:

    A = scalar_mult(a, base)
    E = scalar_mult(e, base)
    shared-key = scalar_mult(a*e, base) = scalar_mult(a, E) = scalar_mult(e, A)

`box_afternm` is simply symmetric authenticated encryption which can
be reversed with `box_open_afternm` using the same symmetric key. In
fact it's identical to `secret_box`.

A related question on stackoverflow:

http://stackoverflow.com/q/13663604/445517

You could also take a look at the Curve25519 paper:

http://cr.yp.to/ecdh/curve25519-20060209.pdf

which has a graphic to illustrate Diffie-Hellman

Reading the wikipedia article on Diffie-Hellman could be a good idea as well.
```
