
###########################################################
### Directory define
###########################################################
PRODIR = ./
WIFIINC = ./source/wifi/include
INCDIR = $(WIFIINC) 
SRCDIR = $(PRODIR)
OTDRRUNDIR = ./

###########################################################
### Cmd line args & Make variables
###########################################################
#G = 1
ifeq ($(G),1)
DBGINF = -g 
USERMACRO += -D CORE_DUMP
endif

V = 0
ifeq ($(V),1)
PRTINF = -v
endif

#如果SDCARD=1，则应用程序所引用的资源在SDCARD上，否则在NANDFLASH上
ifeq ($(SDCARD), 1)
USERMACRO += -D WNDPROC_SDCARD
endif

#ifeq ($(USER_LOG), 1)
#USERMACRO += -D CODER_EYES
#endif
#截图
ifeq ($(SAVE_SCREEN), 1)
USERMACRO += -D SAVE_SCREEN
endif
#加载触摸屏
ifeq ($(TOUCH_SCREEN), 1)
USERMACRO += -D TOUCH_SCREEN
endif
#保存otdr测试数据
ifeq ($(SAVE_DATA), 1)
USERMACRO += -D SAVE_DATA
endif
#加载光模块设备即读写FPGA寄存器
ifeq ($(OPTIC_DEVICE), 1)
USERMACRO += -D OPTIC_DEVICE
endif

#加载EEPROM数据
ifeq ($(EEPROM_DATA), 1)
USERMACRO += -D EEPROM_DATA
endif

#加载端面检测仪设备
ifeq ($(FIP_DEVICE), 1)
USERMACRO += -D FIP_DEVICE
endif

#加载电源设备
ifeq ($(POWER_DEVICE), 1)
USERMACRO += -D POWER_DEVICE
endif

#加载不同的分辨率(320*240)
ifeq ($(SKIN_320_240), 1)
USERMACRO += -D SKIN_320_240
endif

#加载不同的分辨率(640*480)
ifeq ($(SKIN_640_480), 1)
USERMACRO += -D SKIN_640_480
endif

#加载不同的升级方式（测试打开）
ifeq ($(TEST_UPDATE), 1)
USERMACRO += -D TEST_UPDATE
endif

USERMACRO += -D MINI1
OTDRRUNDIR = /run/mini1/

USERMACRO += -D PROJECT_NAME=\"mini1\"

#LEVEL_LOG定义了打印级别
USERMACRO += -D LOG_LEVEL=$(LEVEL_LOG)
#远程交叉编译需要用到绝对路径
CROSS_COMPILE = /usr/local/arm-2009q3/bin/arm-none-linux-gnueabi-

INCPATH = -I. -I./run/sqlite/include -I./run/flib/include -I./run/iniparser/include -I.$(OTDRRUNDIR)include -I./run/persianReshape/include -I./run/pdfharu/include -I./run/hangulime/include -I./run/zlib/include -I./run/matio/include
LIBPATH = -L. -L./run/sqlite/lib -L./run/flib/lib -L./run/iniparser/lib -L.$(OTDRRUNDIR)lib -L./run/persianReshape/lib -L./run/pdfharu/lib -L./run/pdfharu/lib -L./run/hangulime/lib -L./run/zlib/lib -L./run/matio/lib
LIBS = -lm -lpthread -lsgui -lswifi -lsqlite3 -lflib -lskin -lotdr -lpersian -ljpeg -lepdf -lhangulime  -lz -lmatio

CC = $(CROSS_COMPILE)gcc -I$(INCDIR) -I./source
CFLAGS =  -Wall -Wno-strict-aliasing -O2 $(USERMACRO) $(DBGINF) $(PRTINF) $(INCPATH)
LDFLAGS = $(LIBPATH) $(LIBS)

LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar


###########################################################
### File list
###########################################################
GUISRC = $(wildcard ./source/gui*.c)
GUIOBJ = $(patsubst %.c,%.o,$(GUISRC))

WNDSRC = $(wildcard ./source/wnd*.c)
WNDOBJ = $(patsubst %.c,%.o,$(WNDSRC))

APPSRC = $(wildcard ./source/app*.c)
APPOBJ = $(patsubst %.c,%.o,$(APPSRC))

WIFISRC = $(wildcard ./source/wifi/source/*.c)
WIFIOBJ = $(patsubst %.c,%.o,$(WIFISRC))

INPSRC = $(wildcard ./source/input*.c)
INPOBJ = $(patsubst %.c, %.o, $(INPSRC))

SRCLST = $(GUISRC) $(WNDSRC) $(APPSRC) $(INPSRC) $(MAINSRC)
OBJLST = $(patsubst %.c,.%.o,$(SRCLST)) $(patsubst %.c,%.o,$(WIFISRC))
PRELST = $(SRCLST:.c=.i) $(WIFISRC:.c=.i)
DEPLST = $(patsubst %.c,%.d,$(SRCLST)) $(patsubst %.c,%.d,$(WIFISRC))


###########################################################
### Phony object
###########################################################
.PHONY: all clean

all: sgui swifi wndproc

put:
	@echo $(SRCLST)
	@echo $(OBJLST)
	@echo $(PRELST)
	@echo $(DEPLST)
clean:
	@ echo "Delete object file ..."
	@ -rm -f libsgui.a libswifi.a wndproc
	@ -rm -f ./object/*.o ./source/*.i ./source/*.d  ./source/*.o  
	@ -rm -f ./object/wifi/source/*.o ./source/wifi/source/*.o ./source/wifi/source/*.d ./depend/wifi/source/*.d
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
sgui: $(GUIOBJ)
	$(AR) -rc libsgui.a $(GUIOBJ)

swifi: $(WIFIOBJ)
	$(AR) -rc libswifi.a $(WIFIOBJ)

# Window process
wndproc: sgui swifi $(WNDOBJ) $(APPOBJ) $(INPOBJ)
	@$(CC) $(CFLAGS) -o wndproc $(WNDOBJ) $(APPOBJ) $(INPOBJ) $(LDFLAGS)


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

 
