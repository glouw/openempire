BIN = ../openempires

LIBS  = -lSDL2
LIBS += -lSDL2_ttf

SRCS  = Animation.c
SRCS += Args.c
SRCS += Blendomatic.c
SRCS += Color.c
SRCS += Data.c
SRCS += Direction.c
SRCS += Drs.c
SRCS += Field.c
SRCS += File.c
SRCS += Frame.c
SRCS += Graphics.c
SRCS += Grid.c
SRCS += Icon.c
SRCS += Icons.c
SRCS += Image.c
SRCS += Input.c
SRCS += Interfac.c
SRCS += Lines.c
SRCS += Log.c
SRCS += main.c
SRCS += Map.c
SRCS += Meap.c
SRCS += Mode.c
SRCS += Outline.c
SRCS += Overview.c
SRCS += Window.c
SRCS += Palette.c
SRCS += Point.c
SRCS += Points.c
SRCS += Quad.c
SRCS += Rect.c
SRCS += Rects.c
SRCS += Registrar.c
SRCS += Scanline.c
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
SRCS += Units0.c
SRCS += Units1.c
SRCS += Units2.c
SRCS += Util.c
SRCS += Video.c
SRCS += Vram.c

# 0: Compile with gcc.
# 1: Compile with clang.

CLANG = 0

# 0: Compile as C.
# 1: Compile as C++ (Better type checking).

CPP = 0

# 0: Do not use address sanitizer.
# 1: Use address sanitizer.

SANITIZE_ADDRESS = 0

# 0: Do not use thread sanitizer.
# 1: Use thread sanitizer.

SANITIZE_THREAD = 0

FLAGS = -O3 -march=native -flto

DEBUG = -Og -g

ifeq (1, $(SANITIZE_ADDRESS))
FLAGS = $(DEBUG) -fsanitize=address
endif

ifeq (1, $(SANITIZE_THREAD))
FLAGS = $(DEBUG) -fsanitize=thread
endif

FLAGS += -Wall -Wextra -Wshadow -Wpedantic

ifeq (00, $(CLANG)$(CPP))
COMPILER = gcc -std=c99
endif

ifeq (01, $(CLANG)$(CPP))
COMPILER = g++ -std=c++11
endif

ifeq (10, $(CLANG)$(CPP))
COMPILER = clang -std=c11
endif

ifeq (11, $(CLANG)$(CPP))
COMPILER = clang++ -std=c++11
endif

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

$(BIN): $(OBJS) $(DEPS)
	$(COMPILER) $(FLAGS) $(OBJS) $(LIBS) -o $(BIN)

%.o : %.c %.d Makefile
	$(COMPILER) $(FLAGS) -DSANITIZE_THREAD=$(SANITIZE_THREAD) -MMD -MP -MT $@ -MF $*.d -c $<

-include *.d

%.d : ;

.PHONY clean:
clean:
	rm -f $(BIN)
	rm -f $(OBJS)
	rm -f $(DEPS)
