OBJ = main.o
all: fryzjer_cond
fryzjer_cond: $(OBJ)
	gcc $(OBJ) -o fryzjer_cond -pthread
.PHONY: clean
clean:
	rm -f *.o fryzjer_cond