CC             = gcc
MODULE_NAME    = basedfs

ROOT_DIR      := $(PWD)
SRC_DIR       := $(ROOT_DIR)/src
BIN_DIR       := $(ROOT_DIR)/bin
K_DIR         ?= /lib/modules/`uname -r`/build

$(MODULE_NAME)-objs = src/basedfs.c

obj-m		:= src/$(MODULE_NAME).o

all:
	$(MAKE) -C $(K_DIR) M=$$PWD modules
	$(CC) -o $(BIN_DIR)/basedDaemon $(SRC_DIR)/basedDaemon.c
	rm $(ROOT_DIR)/*odule*.*
	mv $(SRC_DIR)/$(MODULE_NAME).ko $(BIN_DIR)

.PHONY: clean
clean:
	rm -f $(SRC_DIR)/*.o $(SRC_DIR)/*mod.c
	rm $(BIN_DIR)/*
