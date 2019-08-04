#  Copyright (c) 2016, Antonio SJ Musumeci <trapexit@spawn.link>
#
#  Permission to use, copy, modify, and/or distribute this software for any
#  purpose with or without fee is hereby granted, provided that the above
#  copyright notice and this permission notice appear in all copies.
#
#  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

TARGET=bbf

MKDIR = $(shell which mkdir)
TOUCH = $(shell which touch)
RM    = $(shell which rm)

CFLAGS=-O2 -g
LDLIBS=-lrt
LDFLAGS=

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:src/%.cpp=obj/%.o)

all: $(TARGET)

$(TARGET): obj/obj-stamp $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDLIBS) $(LDFLAGS)

static: obj/obj-stamp $(OBJ)
	$(CXX) -static $(CFLAGS) $(OBJ) -o $(TARGET) $(LDLIBS) $(LDFLAGS)

obj/obj-stamp:
	$(MKDIR) -p obj
	$(TOUCH) $@

obj/%.o: src/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -rf obj $(TARGET)

include $(wildcard obj/*.d)
