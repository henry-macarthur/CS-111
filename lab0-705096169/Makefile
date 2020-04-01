#NAME: Henry MacArthur
#EMAIL: HenryMac16@gmail.com
#ID: 705096169

.SILENT:check
.SILENT:default
.SILENT:test_bad_input
.SILENT:test_segfault
.SILENT:test_bad_arg
.SILENT:test_bad_out
.SILENT:test_ok_copy
default: lab0.c
	gcc lab0.c -Wall -Wextra  -g -o lab0

clean:
	rm -rf lab0 lab0-705096169.tar.gz



dist: breakpoint.png backtrace.png backtrace.png lab0.c README Makefile
	tar -czvf lab0-705096169.tar.gz breakpoint.png backtrace.png  lab0.c README Makefile

check: default test_bad_input test_segfault test_bad_arg test_bad_out test_ok_copy
	rm -rf errors; \
	rm -rf testOut; \
	rm -rf testOk; \
	rm -rf outTest; \

test_ok_copy:
	touch testOk; \
	echo "test" > testOk; \
	./lab0 --input=testOk --output=outTest 2> errors; \

	if [ $$?==0 ]; then \
		if cmp --s testOk outTest; then \
			echo "------"; \
			echo "input properly copied from source to destination"; \
			echo "------"; \
		else \
			echo "------"; \
			echo "FAIL: input is not properly copied from source to destination"; \
			echo "------"; \
		fi \
	else \
		echo "------"; \
		echo "FAIL: input is not properly copied from source to destination"; \
		echo "------"; \
	fi 



	
test_bad_arg:
	./lab0 --random 2> errors; \
	if [ $$?==1 ]; then \
		echo "------"; \
		echo "properly catches bad flags"; \
		echo "------"; \
	else \
		echo "does not properly catch bad flags"; \
	fi 

test_bad_out:
	touch testOut; \
	echo "test text" > testOut; \
	chmod u-rw testOut; \
	./lab0  --output=testOut 2> errors; \
	var=$$?
	if [ $$?==3 ]; then \
		echo "------"; \
		echo "properly catches trying to open bad output files"; \
		echo "------"; \
	else \
		echo "does not properly catch trying to open bad output files"; \
	fi \
	

	

test_bad_input:
	./lab0 --input=doesnotexist 2> errors; \
	if [ $$?==2 ]; then \
		echo "------"; \
		echo "properly catches trying to open nonexistant input files"; \
		echo "------"; \
	else \
		echo "does not properly catch trying to open nonexistant input files"; \
	fi 

test_segfault:
	./lab0  --segfault --catch 2> errors; \
	str="segmentation fault occured but caught"; \

	if [ $$?==4 ]; then \
		echo "------"; \
		echo "segmentation fault caught properly"; \
		echo "------"; \
	else \
		echo "------"; \
		echo "segmentation fault not caught properly" ; \
		echo "------"; \
	fi
