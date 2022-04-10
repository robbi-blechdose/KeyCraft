CC          	= gcc

CFLAGS			= -O3 -flto
LDFLAGS     	= -lSDL -lSDL_image -lm -lSDL_mixer -lmikmod

PNAME			= main.elf

TINGYGL_LIB		= /home/julius/Dokumente/tinygl/lib/libTinyGL.a

SDL_INCLUDE		= -I/opt/funkey-sdk/arm-funkey-linux-musleabihf/sysroot/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
TINYGL_INCLUDE	= -I/home/julius/Dokumente/tinygl/include
INCLUDES		= -I/opt/funkey-sdk/include $(SDL_INCLUDE) $(TINYGL_INCLUDE)

ifdef TARGET
include $(TARGET).mk
endif

# Files to be compiled
SRCDIR 		=  ./src ./src/engine
VPATH		= $(SRCDIR)
SRC_C		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJS		= $(notdir $(patsubst %.c, %.o, $(SRC_C)))

# Rules to make executable
$(PNAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(PNAME) $^ $(LDFLAGS) $(TINGYGL_LIB)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

oclean:
	rm *.o