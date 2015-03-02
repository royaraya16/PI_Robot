LIBS=$(wildcard libs/*)
INCLUDE_PATH=libs/

main_exec: libraries
	g++ main/main.cpp $(wildcard libraries/*) -I$(INCLUDE_PATH) -o ejecutable

libraries:
	mkdir libraries/
	for dir in $(LIBS); do \
		cd $$dir; \
		g++ -c *.c -I../; \
		g++ -c *.cpp -I../; \
		mv *.o ../../libraries; \
		cd -; \
	done

clean:
	rm -rf libraries/ ejecutable
