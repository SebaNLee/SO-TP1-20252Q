# compilador y flags
CC = gcc
CFLAGS = -Wall -pedantic -Isrc/include

# directorios
SRC_DIR = src
LIB_DIR =libs
BIN_DIR = bin

# fuentes comunes (sin main)
COMMON = $(wildcard $(SRC_DIR)/$(LIB_DIR)/*.c)

# ejecutables
TARGETS = $(BIN_DIR)/master $(BIN_DIR)/player $(BIN_DIR)/view

# crear carpeta bin si no existe
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# reglas para cada ejecutable
$(BIN_DIR)/master: $(SRC_DIR)/master.c $(COMMON) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/player: $(SRC_DIR)/player.c $(COMMON) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/view: $(SRC_DIR)/view.c $(COMMON) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# reglas principales
.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf $(BIN_DIR)/*



#NOTA
# se deja el ChompChamps binario en el directorio /root
# por ahora se usa eso para ejecutar programas
# más adelante se haría ejecutaría con /bin/master
