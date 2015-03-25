LIBS=libs/simpleGPIO
INCLUDE_PATH=libs/

main_exec: libraries
	gcc main/main.c $(wildcard libraries/*) -I$(INCLUDE_PATH) -o main_exec

libraries:
	mkdir libraries/
	for dir in $(LIBS); do \
		cd $$dir; \
		gcc -c *.c -I../; \
		mv *.o ../../libraries; \
		cd -; \
	done

clean:
	rm -rf libraries/ main_exec
