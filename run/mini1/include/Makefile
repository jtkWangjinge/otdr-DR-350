
###########################################################
### Directory define
###########################################################

###########################################################
### Cmd line args & Make variables
###########################################################
#G = 1
ifeq ($(G),1)
DBGINF = -g 
endif


CROSS_COMPILE = arm-none-linux-gnueabi-

INCPATH = -I. 
LIBPATH = -L.
LIBS = -lm

CC = $(CROSS_COMPILE)gcc
CFLAGS =  -Wall -Wno-strict-aliasing -O2 $(DBGINF) $(PRTINF) $(INCPATH)
LDFLAGS = $(LIBPATH) $(LIBS)

LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar


###########################################################
### File list
###########################################################
SRCLST = $(wildcard ./*.c)
OBJLST = $(patsubst %.c,%.o,$(SRCLST))
TARGET = libotdr.a

###########################################################
### Phony object
###########################################################
.PHONY: all clean

all: $(TARGET)

put:
	@echo $(SRCLST)
	@echo $(OBJLST)

clean:
	@ echo "Delete object file ..."
	@ -rm -f $(OBJLST) $(TARGET)

install:
	@cp ./libotdr.a ./../lib -v
#	@cp ./source/*.h ./otdr/include -v
#	@cp ./source/*.c ./otdr/include -v

###########################################################
### Create object
###########################################################
# GUI library
$(TARGET): $(OBJLST)
	$(AR) -rc $@ $^


###########################################################
### Rule define
###########################################################

# Comp rule
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $< 

 
