#Makefile hecho para el proyecto PI-ROBOT

LIBS=$(wildcard libs/*)
INCLUDE_PATH=libs/

CC=gcc
CROSS_CC=arm-linux-gnueabihf-gcc-4.7
CFLAGS=-Wall -fsingle-precision-constant -I
LFLAGS=-lm -lrt -pthread

#tener cuidado con -fsingle-precision-constant a la hora de hacer operaciones matematicas

NAME=ejecutable

cross: bibliotecas
	$(CROSS_CC) main/main.c $(wildcard bibliotecas/*) $(LFLAGS) $(CFLAGS)$(INCLUDE_PATH) -o $(NAME)

native: bibliotecas_native
	$(CC) main/main.c $(wildcard bibliotecas/*) $(LFLAGS) $(CFLAGS)$(INCLUDE_PATH) -o $(NAME)

bibliotecas:
	mkdir bibliotecas/
	for dir in $(LIBS); do \
		cd $$dir; \
		$(CROSS_CC) $(DEFS) -c *.c $(LFLAGS) $(CFLAGS)../; \
		mv *.o ../../bibliotecas; \
		cd -; \
	done

bibliotecas_native:
	mkdir bibliotecas/
	for dir in $(LIBS); do \
		cd $$dir; \
		$(CC) -c *.c $(CFLAGS)../; \
		mv *.o ../../bibliotecas; \
		cd -; \
	done

copy:
	scp $(NAME) beaglebone:/root
	rm -rf bibliotecas/ $(NAME)

clean:
	rm -rf bibliotecas/ $(NAME)
