
# compilador y flags
CC = gcc
CFLAGS = -Wall -pedantic -Isrc/include

# directorios
SRC_DIR = src
BIN_DIR = bin

# .c
SRC = $(wildcard $(SRC_DIR)/*.c)

# .o
BINARIES = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRC))


# crear carpeta bin si no existe
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# crear binarios individuales
$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@



# reglas
.PHONY: all clean

all: $(BINARIES)

clean:
	rm -rf $(BIN_DIR)/*



#NOTA
# se deja el ChompChamps binario en el directorio /root
# por ahora se usa eso para ejecutar programas
# más adelante se haría ejecutaría con /bin/master
