LIBS=$(wildcard libs/*)
INCLUDE_PATH=libs/

CC=gcc
CFLAGS=-I

main_exec: libraries
	$(CC) main/main.c $(wildcard libraries/*) $(CFLAGS)$(INCLUDE_PATH) -o ejecutable

libraries:
	mkdir libraries/
	for dir in $(LIBS); do \
		cd $$dir; \
		$(CC) -c *.c $(CFLAGS)../; \
		mv *.o ../../libraries; \
		cd -; \
	done

clean:
	rm -rf libraries/ ejecutable
