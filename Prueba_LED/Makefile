LIBS=$(wildcard libs/*)
INCLUDE_PATH=libs/

CC=gcc
CROSS_CC=arm-linux-gnueabihf-gcc-4.7
CFLAGS=-I

NAME=ejecutable

cross: bibliotecas
	$(CROSS_CC) main/main.c $(wildcard bibliotecas/*) $(CFLAGS)$(INCLUDE_PATH) -o $(NAME)

native: bibliotecas
	$(CC) main/main.c $(wildcard bibliotecas/*) $(CFLAGS)$(INCLUDE_PATH) -o $(NAME)

bibliotecas:
	mkdir bibliotecas/
	for dir in $(LIBS); do \
		cd $$dir; \
		$(CC) -c *.c $(CFLAGS)../; \
		mv *.o ../../bibliotecas; \
		cd -; \
	done

copy:
	scp $(NAME) beaglebone:/root

clean:
	rm -rf bibliotecas/ $(NAME)
