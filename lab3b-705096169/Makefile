#SILENT:default

default:
	rm -f lab3b
	@echo 'python3 lab3b.py $$1' > lab3b
	@echo "lab3b executable created"
	@chmod u+x lab3b

clean:
	rm -rf lab3b-705096169.tar.gz lab3b

dist:
	tar -czvf lab3b-705096169.tar.gz README lab3b.py Makefile