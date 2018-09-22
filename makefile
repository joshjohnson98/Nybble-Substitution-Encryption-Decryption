all: disp nsub
disp: disp.c
	gcc -Wall -o disp disp.c
nsub: nsub.cpp
	g++ -Wall -o nsub nsub.cpp
