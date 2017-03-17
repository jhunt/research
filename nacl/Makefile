BINS :=
BINS += encrypt
BINS += decrypt
BINS += decrypt2
BINS += decrypt3

default: $(BINS)
clean:
	rm -f tweetnacl.o $(BINS:=.o) $(BINS)

encrypt: encrypt.o tweetnacl.o
decrypt: decrypt.o tweetnacl.o
decrypt2: decrypt2.o tweetnacl.o
decrypt3: decrypt3.o tweetnacl.o
