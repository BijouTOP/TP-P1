COMPILE = gcc
CFLAGS = -g -Wall -Wextra -Werror -std=c99 -Wpedantic
CFLAGSCPP = --enable=all --std=c99 --suppress=missingIncludeSystem

INCLUDES = -isystem./libraries/
LDFLAGS = -L./libraries/
LIBS= -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
FOLDERPATH = ./src/

TARGET = output

all: $(TARGET)

$(TARGET): $(FOLDERPATH)main.c
	$(COMPILE) $(CFLAGS) $(INCLUDES) $(LDFLAGS) $(FOLDERPATH)main.c -o $(TARGET) $(LIBS)

check:
	cppcheck  $(CFLAGSCPP) $(INCLUDES) $(LDFLAGS)-Dbool=_Bool $(FOLDERPATH)main.c

clean:
	rm -f $(TARGET)