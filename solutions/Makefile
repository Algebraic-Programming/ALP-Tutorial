
## Minimal Makefile for Exercise 8
## export ALP_INSTALL_DIR to point to the proper location.

ALP_INSTALL_DIR ?= $(HOME)/Repos/graphblas/install

CXX      := g++
SRC      := ex8/exercise8.cpp
BIN      := exercise8.x


.PHONY: all run clean

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) -std=c++14 $< -o $@ -I$(ALP_INSTALL_DIR)/include -D_GRB_WITH_REFERENCE -D_GRB_WITH_OMP -L$(ALP_INSTALL_DIR)/lib/sequential -lgraphblas -lnuma -lpthread -lm -fopenmp

run: $(BIN)
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(ALP_INSTALL_DIR)/lib/sequential ./$(BIN)

clean:
	rm -f $(BIN)

