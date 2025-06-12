all: main.exe

main.exe: main.c banking.h utils.o transaction.o menu.o auth.o account.o
	gcc main.c utils.o transaction.o menu.o auth.o account.o -o main.exe

utils.o: utils.c banking.h
	gcc -c utils.c -o utils.o

transaction.o: transaction.c banking.h
	gcc -c transaction.c -o transaction.o

menu.o: menu.c banking.h
	gcc -c menu.c -o menu.o

auth.o: auth.c banking.h
	gcc -c auth.c -o auth.o

account.o: account.c banking.h
	gcc -c account.c -o account.o

clean:
	rm -f *.exe *.o