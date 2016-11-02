project : encryption_decryption1.o
	cc encryption_decryption1.o -o project
encryption_decryption1.o : encryption_decryption1.c
	cc -c encryption_decryption1.c
