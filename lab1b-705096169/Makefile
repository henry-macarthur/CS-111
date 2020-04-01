default: lab1b-client.c lab1b-server.c
	gcc -g -Wall -Wextra lab1b-client.c -o lab1b-client -lz
	gcc -Wall -Wextra lab1b-server.c -o lab1b-server -lz

server: lab1b-server.c
	gcc -Wall -Wextra lab1b-server.c -o lab1b-server -lz

client:
	gcc -Wall -Wextra lab1b-client.c -o lab1b-client -lz
clean: 
	rm -rf lab1b-client
	rm -rf lab1b-server
	rm -rf lab1b-705096169.tar.gz

dist: lab1b-client.c lab1b-server.c README Makefile
	tar -czvf lab1b-705096169.tar.gz lab1b-client.c lab1b-server.c README Makefile