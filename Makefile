test.exe : disk.c disk.h fs.c fs.h testcase.c validate.o
	gcc -no-pie disk.c disk.h fs.c fs.h testcase.c validate.o -o test.exe
