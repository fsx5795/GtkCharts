APP = gtkcharts
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))
CFLAGS = `pkg-config --cflags --libs gtk+-3.0`
$(APP) : $(OBJS)
	$(CC) $^ -o $@ ${CFLAGS} -lm
%.o: %.c
	$(CC) -c $< -o $@ -g ${CFLAGS}
.PHONY: clean
clean:
	- $(RM) -r $(OBJS) $(APP)