all:
	gcc -pthread -o abba abba.c
	gcc -pthread -o dinning dinning_deadlock.c
	gcc -pthread -o test test.c
	gcc -pthread  -shared -fPIC -o mymutex.so mymutex.c -ldl
