#NAME: Henry MacArthur
#EMAIL: HenryMac16@gmail.com
#ID: 705096169
default:
	gcc -lmraa -Wall -Wextra lab4c_tcp.c -o lab4c_tcp -lm
	gcc -lmraa -Wall -Wextra lab4c_tls.c -o lab4c_tls -lm -lssl -lcrypto
clean:
	rm -rf *.tar.gz lab4c_tcp lab4c_tls
dist:
	tar -vczf lab4c-705096169.tar.gz lab4c_tcp.c lab4c_tls.c README Makefile


