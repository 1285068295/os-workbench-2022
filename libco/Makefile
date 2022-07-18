NAME := $(shell basename $(PWD))
export MODULE := M2
all: $(NAME)-64.so $(NAME)-32.so
CFLAGS += -g -U_FORTIFY_SOURCE

include ../Makefile
