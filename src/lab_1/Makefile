TARGET = main.out

.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(TARGET) *.o

main.o: main.c
	gcc -c -o main.o main.c

$(TARGET): main.o
	gcc -o $(TARGET) main.o
