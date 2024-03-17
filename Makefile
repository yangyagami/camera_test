SRC := test.c camera.c button.c

test: $(SRC) libuvc.so libuvc.so.0
	gcc $(SRC) -o test -O0 -ggdb -I. -Iinclude -L. -lm -luvc -l:libraylib.a -Wl,--rpath=. -Wall -Werror

check: $(SRC) libuvc.so libuvc.so.0
	gcc $(SRC) -o test -O0 -I. -Iinclude -L. -lm -luvc -l:libraylib.a -Wl,--rpath=. -Wall -Werror -fsanitize=address

clean:
	rm test
