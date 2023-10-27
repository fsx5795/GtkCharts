CFLAGS = `pkg-config --cflags --libs gtk+-3.0`
gtkcharts : main.c
	$(CC) -o gtkcharts -g main.c ${CFLAGS} -lm