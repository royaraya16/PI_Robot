LIBS=$(wildcard libs/*)
INCLUDE_PATH=libs/

main_exec: libraries
	gcc main/main.c $(wildcard libraries/*) -I$(INCLUDE_PATH) -o ejecutable

libraries:
	mkdir libraries/
	for dir in $(LIBS); do \
		cd $$dir; \
		gcc -c *.c -I../; \
		mv *.o ../../libraries; \
		cd -; \
	done

clean:
	rm -rf libraries/ ejecutable
