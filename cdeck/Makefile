# cdc
.POSIX:
include config.mk

all: cdc

cdc: cdc.cpp cdc.hpp passes/*
	@$(CXX) $(CDC_CXXFLAGS) cdc.cpp -o cdc $(CDC_LDFLAGS)

.PHONY: all

