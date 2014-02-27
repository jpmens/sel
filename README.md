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
