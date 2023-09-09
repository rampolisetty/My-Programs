# My Makefile

palindrome_string_check_ut: palindrome_string_check.c
	@gcc -g -Wall -Werror -DUNIT_TEST palindrome_string_check.c -o palindrome_string_check

palindrome_string_check: palindrome_string_check.c
	@gcc -g -Wall -Werror palindrome_string_check.c -o palindrome_string_check

all: palindrome_string_check

ut: palindrome_string_check_ut

clean:
	@rm -f *.o
	@rm -f palindrome_string_check
