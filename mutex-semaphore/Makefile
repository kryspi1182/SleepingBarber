OBJ = main.o
all: fryzjer
fryzjer: $(OBJ)
	gcc $(OBJ) -o fryzjer -pthread
.PHONY: clean
clean:
	rm -f *.o fryzjer