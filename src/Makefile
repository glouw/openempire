BIN = ../openempires
ASM = $(BIN).s

LIBS  = -lSDL2
LIBS += -lSDL2_ttf
LIBS += -lSDL2_net

SRCS  = Animation.c
SRCS += Args.c
SRCS += Backup.c
SRCS += Bits.c
SRCS += Blendomatic.c
SRCS += Channels.c
SRCS += Color.c
SRCS += Cache.c
SRCS += Data.c
SRCS += Direction.c
SRCS += Drs.c
SRCS += Field.c
SRCS += File.c
SRCS += Frame.c
SRCS += Graphics.c
SRCS += Grid.c
SRCS += Button.c
SRCS += Buttons.c
SRCS += Image.c
SRCS += Input.c
SRCS += Interfac.c
SRCS += Lines.c
SRCS += main.c
SRCS += Map.c
SRCS += Meap.c
SRCS += Mode.c
SRCS += Outline.c
SRCS += Overview.c
SRCS += Window.c
SRCS += Part.c
SRCS += Parts.c
SRCS += Parity.c
SRCS += Packet.c
SRCS += Packets.c
SRCS += Palette.c
SRCS += Ping.c
SRCS += Point.c
SRCS += Points.c
SRCS += Quad.c
SRCS += Rect.c
SRCS += Rects.c
SRCS += Registrar.c
SRCS += Scanline.c
SRCS += Sock.c
SRCS += Sockets.c
SRCS += State.c
SRCS += Slp.c
SRCS += Stack.c
SRCS += Surface.c
SRCS += Table.c
SRCS += Trait.c
SRCS += Terrain.c
SRCS += Text.c
SRCS += Tile.c
SRCS += Tiles.c
SRCS += Unit.c
SRCS += Units.c
SRCS += Util.c
SRCS += Video.c
SRCS += Vram.c

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

CLANG = 0

CPP = 0

DUMP=objdump

SANITIZE_ADDRESS = 0

SANITIZE_THREAD = 0

SANITIZE_UNDEFINED = 0

FLAGS = -O3 -march=native -flto

DEBUG = -Og -g

ifeq (1, $(SANITIZE_ADDRESS))
FLAGS = $(DEBUG) -fsanitize=address
endif

ifeq (1, $(SANITIZE_THREAD))
FLAGS = $(DEBUG) -fsanitize=thread
endif

ifeq (1, $(SANITIZE_UNDEFINED))
FLAGS = $(DEBUG) -fsanitize=undefined
endif

FLAGS += -Wall -Wextra -Wshadow -Wpedantic

CSTD=-std=c11
CPPSTD=-std=c++11

ifeq (00, $(CLANG)$(CPP))
COMPILER = gcc $(CSTD)
endif

ifeq (01, $(CLANG)$(CPP))
COMPILER = g++ $(CPPSTD)
endif

ifeq (10, $(CLANG)$(CPP))
COMPILER = clang $(CSTD)
endif

ifeq (11, $(CLANG)$(CPP))
COMPILER = clang++ $(CPPSTD)
endif

$(BIN): $(OBJS) $(DEPS)
	$(COMPILER) $(FLAGS) $(OBJS) $(LIBS) -o $(BIN)
	$(DUMP) -M intel -d $(BIN) > $(ASM)

%.o : %.c %.d Makefile
	$(COMPILER) $(FLAGS) -DSANITIZE_THREAD=$(SANITIZE_THREAD) -DSANITIZE_UNDEFINED=$(SANITIZE_UNDEFINED) -DSANITIZE_ADDRESS=$(SANITIZE_ADDRESS) -MMD -MP -MT $@ -MF $*.d -c $<

-include *.d

%.d : ;

.PHONY clean:
clean:
	rm -f $(BIN)
	rm -f $(ASM)
	rm -f $(OBJS)
	rm -f $(DEPS)
