inlcude = -Iinclude

test_neda:
	gcc -O3 -Wall -Wextra -Wpedantic -Werror -std=c89 $(inlcude) src/test_neda.c -o test_neda
	test_neda

test_neda_sort:
	gcc -O3 -Wall -Wextra -Wpedantic -Werror -std=c89 $(inlcude) src/test_neda_sort.c -o test_neda_sort
	test_neda_sort

test_nemap:
	gcc -Ofast -Wall -Wextra -Werror -std=c89 $(inlcude) src/test_nemap.c -o test_nemap
	test_nemap

test_nejson:
	gcc -Ofast -Wall -Wextra -Wpedantic -Werror -std=c89 $(inlcude) src/test_nejson.c -o test_nejson
	test_nejson