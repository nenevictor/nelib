inlcude = -Iinclude

test_neda:
	gcc -O3 -Wall -Wextra -Wpedantic -Werror -std=c11 $(inlcude) src/test_neda.c -o test_neda
	./test_neda

test_neda_sort:
	gcc -O3 -Wall -Wextra -Wpedantic -Werror -std=c89 $(inlcude) src/test_neda_sort.c -o test_neda_sort
	./test_neda_sort

test_nejson:
	gcc -O0 -Wall -Wextra -Wpedantic -Werror -std=c89 $(inlcude) src/test_nejson.c -o test_nejson
	./test_nejson

test_nemap:
	gcc -Ofast -Wall -Wextra -Werror -std=c89 $(inlcude) src/test_nemap.c -o test_nemap
	./test_nemap