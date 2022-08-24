all:
	gcc -g *.c -o client -ljson-c `mysql_config --cflags --include --libs`
clean:
	-rm client
