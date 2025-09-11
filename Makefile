# compilador y flags
CC = gcc
CFLAGS = -Wall -pedantic -Isrc/include 
LDFLAGS = -lm

# directorios
SRC_DIR = src
LIB_DIR =libs
BIN_DIR = bin

# fuentes comunes (sin main)
COMMON = $(wildcard $(SRC_DIR)/$(LIB_DIR)/*.c)

# ejecutables
TARGETS = $(BIN_DIR)/master $(BIN_DIR)/player $(BIN_DIR)/view $(BIN_DIR)/bonaquackte $(BIN_DIR)/autoblock

# crear carpeta bin si no existe
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# reglas para cada ejecutable
$(BIN_DIR)/master: $(SRC_DIR)/master.c $(COMMON) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/player: $(SRC_DIR)/player.c $(COMMON) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/view: $(SRC_DIR)/view.c $(COMMON) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/upper: $(SRC_DIR)/upper.c $(COMMON) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)	

$(BIN_DIR)/autoblock: $(SRC_DIR)/autoblock.c $(COMMON) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)	

$(BIN_DIR)/bonaquackte: $(SRC_DIR)/bonaquackte.c $(COMMON) | $(BIN_DIR)
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
