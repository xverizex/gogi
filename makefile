all:
	gcc main.c -o gogi
install:
	install gogi /usr/local/bin
clean:
	rm gogi
