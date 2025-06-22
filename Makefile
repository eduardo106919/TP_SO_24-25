
SRC_DIR = src
INC_DIR = include
BLD_DIR = build
DOC_DIR = docs
TMP_DIR = tmp
BIN_DIR = bin
RST_DIR = results
SRP_DIR = scripts

CC = gcc
CFLAGS = -Wall -g -I$(INC_DIR) # -fsanitize=address
LDFLAGS =


CLIENT_BIN = $(BIN_DIR)/dclient
SERVER_BIN = $(BIN_DIR)/dserver


SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BLD_DIR)/%.o, $(SRC_FILES))


all: folders $(CLIENT_BIN) $(SERVER_BIN)


.PHONY: folders
folders:
	@mkdir -p $(BLD_DIR) $(BIN_DIR) $(TMP_DIR) $(RST_DIR)

$(CLIENT_BIN): $(filter-out $(BLD_DIR)/dserver.o, $(OBJS))
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SERVER_BIN): $(filter-out $(BLD_DIR)/dclient.o, $(OBJS))
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BLD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $^ -o $@ $(LDFLAGS)


.PHONY: fmt
fmt:
	@-clang-format -verbose -i $(SRC_FILES) $(INC_DIR)/*.h
	@shfmt -w -i 2 -l -ci .


.PHONY: docs
docs:
	@doxygen -q $(DOC_DIR)/Doxyfile
	@firefox $(DOC_DIR)/html/index.html

.PHONY: tests
tests: all
	./$(SRP_DIR)/test_s.sh


.PHONY: trash
trash:
	rm -f $(TMP_DIR)/*.bin $(TMP_DIR)/server_fifo $(TMP_DIR)/client_fifo_*


.PHONY: clean
clean:
	rm -f $(BLD_DIR)/*.o
	rm -f $(BIN_DIR)/*
	rm -f $(TMP_DIR)/*

