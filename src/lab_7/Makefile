TARGET = main.out

.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(TARGET) *.o

main.o: main.c
	gcc -c -o main.o main.c -pthread

$(TARGET): main.o
	gcc -o $(TARGET) main.o -pthread