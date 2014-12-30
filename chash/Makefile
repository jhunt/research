CFLAGS := -g -Wall
LDLIBS := -lvigor

all: hash ring
hash: hash.o
ring: ring.o dist.o murmur.o

example: hash
	@./hash $$(/bin/bash -c 'echo host0{1,2}.example.com:{cpu,memory,load} www.example.com:requests_per_second')
