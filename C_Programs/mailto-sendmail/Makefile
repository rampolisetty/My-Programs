# My Makefile

mailto_sendmail: mailto_sendmail.c
	@gcc -g -Wall -Werror mailto_sendmail.c -o mailto_sendmail -lcurl

all: mailto_sendmail

clean: 
	@rm -f *.o
	@rm -f mailto_sendmail

