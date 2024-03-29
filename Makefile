CC = gcc -Wall
HEADER_FILES_DIR = .
INCLUDES = -I $(HEADER_FILES_DIR)
OUTPUT = main
SOURCES = main.c meteo.c
LIB_HEADERS = $(HEADER_FILES_DIR)/meteo.h

$(OUTPUT): $(SOURCES) $(LIB_HEADERS)
	$(CC) -o $(OUTPUT) $(SOURCES) $(INCLUDES)