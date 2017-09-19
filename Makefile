CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
AR = $(CROSS_COMPILE)ar
#CPPFLAGS := -nostdinc
CPPFLAGS := 
CFLAGS := -fno-builtin

SRC = ./src/
INC = -I ./msrv -I ./mapi/mm -I ./common -I ./msrv/event_manager -I ./mapi/input -I ./mapi/network
#OBJ =  $(SRC)event_manager/event_manager.cpp $(SRC)applet/applet.cpp $(SRC)app_fb_event.cpp

MSRV_OBJ = $(subst .cpp,.o,$(wildcard ./msrv/app_fb.cpp))
MSRV_OBJ += $(subst .cpp,.o,$(wildcard ./common/com_event.cpp))

MAPI_OBJ = $(subst .cpp,.o,$(wildcard ./mapi/mm/*.cpp))
MAPI_OBJ += $(subst .cpp,.o,$(wildcard ./mapi/input/*.cpp))
MAPI_OBJ += $(subst .cpp,.o,$(wildcard ./mapi/network/*.cpp))

LIB = -ljpeg -lpthread

export CROSS_COMPILE CC LD OBJCOPY CPPFLAGS CFLAGS AR

app_fb:$(SRC)app_fb.cpp
	$(CC) $^ -o app $(LIB)
	cp app /home/robin/share/kernel/linux-3.0.8/drivers/char/app


app:$(SRC)app.cpp
	$(CC) $(SRC)app.cpp -o app
	cp app /home/robin/share/kernel/linux-3.0.8/drivers/char/app

test:$(SRC)test.cpp
	$(CC) $(SRC)test.cpp -o test

fb:$(OBJ)
	$(CC) $(INC) $^ -o $@ $(LIB)
	cp $@ /home/robin/share/kernel/linux-3.0.8/drivers/leds

cpp:$(SRC)event_manager/event_manager.cpp
	g++ $^ -o test -lpthread




msrv_event:./msrv/event_manager/msrv_event.o
	$(CC) $^ -o app $(LIB) $(INC)
	cp app /home/robin/share/kernel/linux-3.0.8/drivers/char/app


msrv:$(MSRV_OBJ)
	$(CC) $(MAPI_OBJ) $^ -o app $(LIB) $(INC)
	cp app /home/robin/share/kernel/linux-3.0.8/drivers/char/app

mapi:$(MAPI_OBJ)


%.o:%.cpp
	$(CC) $< -o $@ -c $(LIB) $(INC)

clean:
	-rm app
	-rm test
	-rm fb
	-find -name *.o | xargs rm -rf


#all:
#	arm-linux-gcc start.S -o start.o -c
#	arm-linux-ld -Ttext 0x20000000 -o led.elf start.o
#	arm-linux-objcopy -O binary led.elf robin.bin
#

