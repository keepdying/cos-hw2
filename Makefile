build_thread:
	gcc -o thread thread.c shopping.c -lpthread

run_thread:
	./thread

build_multiprocessing:
	gcc -o multiprocessing multiprocessing.c shopping.c -lpthread

run_multiprocessing:
	./multiprocessing