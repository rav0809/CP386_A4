warnings_BAD: *.c
	gcc -Werror -Wall -g -std=gnu99 -o main *.c -lrt -lpthread
	
warnings_OK: *.c
	gcc -Wall -g -std=gnu99 -o main *.c -lrt -lpthread
	
clean: *.c
	rm main
