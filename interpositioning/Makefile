all:
	gcc -pthread -o abba abba.c
	gcc -pthread -o dinning dinning_deadlock.c
	gcc -shared -fPIC -o mymutex.so mymutex.c -ldl
