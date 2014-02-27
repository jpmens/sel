#!/bin/sh

case $# in
	0) INPUT="data/b.txt";;
	*) INPUT="$*";;
esac
#                   				     FROMpubkey-file TOprivkey-file
cat $INPUT | ./encrypt.py | ./jp-decrypt eve.pub  alice.secret
