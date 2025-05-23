CC = clang
CFLAGS = -Wall -Wextra -Werror
TARGET = main
SRC = main.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
