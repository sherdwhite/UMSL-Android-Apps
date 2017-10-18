all : oss user
clean :
	rm oss.o user.o
oss : oss.o
	gcc -g -o oss oss.o
oss.o : oss.c
	gcc -c -g oss.c
user : user.o
	gcc -g -o user user.o
user.o : user.c
	gcc -c -g user.c
