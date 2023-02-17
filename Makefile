TARGET = wfmt
CC = gcc
INCLUDEP_PATH=-Isrc/

LDFLAGS = -g
CFLAGS = $(INCLUDEP_PATH) -g

src =$(shell find src/ -name '*.c' -not -name 'main.c')
obj = $(src:.c=.o)

all: $(TARGET)

$(TARGET): $(obj) src/main.c
	$(CC) $(LDFLAGS) -o $@ $(obj) src/main.c

test: $(TARGET)
	scripts/runtests.sh $(shell find tests/ -name '*.fth')

$(obj):%.o:%.c
	$(CC) -c $(CFLAGS) $< -MD -MF $@.d -o $@


clean:
	-rm $(TARGET)

DEPS := $(shell find . -name *.d)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin

ifneq ($(DEPS),)
include $(DEPS)
endif
