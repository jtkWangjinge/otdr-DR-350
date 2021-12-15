
###########################################################
### Directory define
###########################################################
PRODIR = ./
#WIFIINC = ./source/wifi/include
INCDIR = $(WIFIINC) 
SRCDIR = $(PRODIR)

###########################################################
### Cmd line args & Make variables
###########################################################
#G = 1
ifeq ($(G),1)
DBGINF = -g
endif

V = 0
ifeq ($(V),1)
PRTINF = -v
endif

#如果SDCARD=1，则应用程序所引用的资源在SDCARD上，否则在NANDFLASH上
ifeq ($(SDCARD), 1)
USERMACRO = -D WNDPROC_SDCARD
endif

#LEVEL_LOG定义了打印级别
USERMACRO += -D LOG_LEVEL=$(LEVEL_LOG)

CROSS_COMPILE = arm-none-linux-gnueabi-

INCPATH = -I. -I./run/sqlite/include
LIBPATH = -L. -L./run/sqlite/lib
LIBS = -lm -lpthread -lswifi -L.

CC = $(CROSS_COMPILE)gcc -I$(INCDIR) -I./source
CFLAGS = -Wall -Wno-strict-aliasing -O2 $(USERMACRO) $(DBGINF) $(PRTINF) $(INCPATH)
LDFLAGS = $(LIBPATH) $(LIBS)

LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar


###########################################################
### File list
###########################################################
APPSRC = $(wildcard ./source/*.c)
APPOBJ = $(patsubst %.c,%.o,$(APPSRC))

WIFISRC = $(wildcard ./source/wifi/source/*.c)
WIFIOBJ = $(patsubst %.c,%.o,$(WIFISRC))

SRCLST = $(APPSRC)
OBJLST = $(patsubst %.c,.%.o,$(SRCLST)) $(patsubst %.c,%.o,$(WIFISRC))
PRELST = $(SRCLST:.c=.i) $(WIFISRC:.c=.i)
DEPLST = $(patsubst %.c,%.d,$(SRCLST)) $(patsubst %.c,%.d,$(WIFISRC))


###########################################################
### Phony object
###########################################################
.PHONY: all clean

all: swifi wndadjust

put:
	@echo $(SRCLST)
	@echo $(OBJLST)
	@echo $(PRELST)
	@echo $(DEPLST)
clean:
	@ echo "Delete object file ..."
	@ -rm libsgui.a libswifi.a wndadjust
	@ -rm  ./object/*.o ./source/*.i ./source/*.d  ./source/*.o  
	@ echo "Delete all object file OK!"
	@ echo "Reserved file list: '.o'"
	@ -find . -type f -name "*.o"|xargs echo
	@ echo "Reserved file list: '.i'"
	@ -find . -type f -name "*.i"|xargs echo
	@ echo "Reserved file list: '.d'"
	@ -find . -type f -name "*.d"|xargs echo


###########################################################
### Create object
###########################################################
# GUI library

swifi: $(WIFIOBJ)
	$(AR) -rc libswifi.a $(WIFIOBJ)

# Window process
wndadjust: swifi $(APPOBJ)
	@$(CC) $(CFLAGS) -o wndadjust $(APPOBJ) $(LDFLAGS)


###########################################################
### For debug
###########################################################
test: opt_tst.o $(OBJLST)
	$(CC) $(CFLAGS) -o test opt_tst.o -lpthread -lsgui -L.

cpre: $(PRELST)


###########################################################
### Rule define
###########################################################
# Pre rule
%.i: %.c
	@$(CC) -E $< -o $@

# Comp rule
%.o: %.c
	@$(CC) -c $(CFLAGS) -o $@ $< 

# Build depend
%.d: %.c
	@ $(CC) -MM $(CFLAGS) $< > $@.tmp
	@ sed 's,\($*\)\.o[:]*,\1.o $@ :,g' < $@.tmp > $@
	@ rm -f $@.tmp

###########################################################
### Read depend
###########################################################
-include $(DEPLST)

 
