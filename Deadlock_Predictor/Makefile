all:
	gcc -pthread -o abba abba.c
	gcc -pthread -o dinning dinning_deadlock.c
	gcc -pthread  -shared -fPIC -o dmonitor.so dmonitor.c -ldl
