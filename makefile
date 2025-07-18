TARGET := pascal-cli

CC := clang
CFLAGS := -Wall -Wextra -Wpedantic -std=gnu99 -flto -O2
LDFLAGS := $(CFLAGS) -lm -lpulse-simple

CFILES := pascal-cli.c pascal-ogg.c stb_vorbis.c
OFILES := $(patsubst %.c, %.o, $(CFILES))

default: $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin

clean:
	@rm -f $(TARGET) $(DBG_TARGET) $(OFILES) pascal-ogg.c

$(TARGET): $(OFILES)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

pascal-ogg.c:
	xxd -i pascal.ogg pascal-ogg.c

.PHONY: default install clean
