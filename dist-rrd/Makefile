CFLAGS := -g -Wall

all: hash
hash: hash.o

example: hash
	@./hash $$(/bin/bash -c 'echo host0{1,2}.example.com:{cpu,memory,load} www.example.com:requests_per_second')
