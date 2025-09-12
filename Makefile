# compilador y flags
CC = gcc
CFLAGS = -Wall -pedantic -Isrc/include -g
LDFLAGS = -lm

# directorios
SRC_DIR = src
LIB_DIR = libs
BIN_DIR = bin

# fuentes comunes (librerías)
COMMON = $(wildcard $(SRC_DIR)/$(LIB_DIR)/*.c)

# .c de /src sin librerías
SRCS = $(filter-out $(COMMON), $(wildcard $(SRC_DIR)/*.c))

# ejecutables (mismo nombre que su .c)
TARGETS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRCS))

# crear carpeta bin si no existe
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# regla genérica para cada ejecutable
$(BIN_DIR)/%: $(SRC_DIR)/%.c $(COMMON) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	

# reglas principales
.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf $(BIN_DIR)/*



#NOTA
# se deja el ChompChamps binario en el directorio /root
# por ahora se usa eso para ejecutar programas
# más adelante se haría ejecutaría con /bin/master
