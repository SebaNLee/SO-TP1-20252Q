
# compilador y flags
CC = gcc
CFLAGS = -Wall -pedantic -Isrc/include

# directorios
SRC_DIR = src
BIN_DIR = bin

# .c
SRC = $(wildcard $(SRC_DIR)/*.c)

# .o
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRC))


# crear carpeta bin si no existe
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# .o de cada .c en /bin
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@



# reglas
.PHONY: all clean

all: $(OBJ)

clean:
	rm -rf $(BIN_DIR)/*



#NOTA
# se deja el ChompChamps binario en el directorio /root
# por ahora se usa eso para ejecutar programas
# más adelante se haría ejecutaría con /bin/master
