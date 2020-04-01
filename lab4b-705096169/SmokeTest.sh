
#!/bin/bash

#test sensor init!

echo -e "START\nOFF\n" | ./lab4b &> /dev/null; \

if [[ $? -ne 0 ]]; then \
    echo "FAILED sensor init"; \
else \
    echo "PASSED: sensors properly initialized"; \
fi
    #first test bad argument

echo "OFF\n" | ./lab4b wrong_input &> /dev/null; \

if [[ $? -ne 1 ]]; then \
    echo "FAILED CASE: invalid argument \n"; \
else \
    echo "PASSED CASe: invalid argument \n"; \
fi

echo -e "SCALE=F\nSTOP\nSCALE=C\nSTART\nLOG sometext\nOFF\n" | ./lab4b --log=RANDOM &> /dev/null

if [[ $? -ne 0 ]]; then \
    echo "FAILED GENERAL CASE: wrong error code, exited with failure \n"; \
else
    echo "PASSED EXIT CODE \n"; \
fi

if [ -f RANDOM ]; then
    echo "PASS: Properly creates log file"
else
    echo "FAIL: does not create log file"
fi

#now test contents of log FILE
cat RANDOM | tr '\n' ' ' | grep -E "SCALE=F STOP SCALE=C START LOG sometext OFF .* SHUTDOWN" &> /dev/null

if [[ $? -ne 0 ]]; then
    echo "FAILED: log file contents pt1"
else
    echo "PASSED: log file contents pt2"
fi

echo -e "adsfafasdfjasdflasdhflaskjdfhlasdkjhfalskjdhflkasjdflkajshdflkjashdflkajhsdflkjasdhflkjasdhflaksdjhflasdjhflaksdjhflaskdjfhasdlkjfhalskdjfhasldkfjhasldk
sadfnba,lsjdkhfasjdkfhadjkfhalsdfhalsdkjfhlskjdhflakjdhflasdkhfjlksdjhflakjsdhflkjasdhflkajhdflkasjhdflkjashdflkjashdflkajsdhflkajsdhflkajsdhfa\nOFF\n" | ./lab4b --log=RANDOM &> /dev/null

cat RANDOM | grep -E "^[0-9][0-9]:[0-9][0-9]:[0-9][0-9] [0-9][0-9].[0-9]$" &> /dev/null
#grep -E "SCALE=F\n
if [[ $? -ne 0 ]]; then
    echo "FAILED: data not logged correctly"
else
    echo "PASSED: data from sensor correctly logged"
fi

rm -rf RANDOM
