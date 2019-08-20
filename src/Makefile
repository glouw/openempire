# Dependencies:
#   SDL2-devel
#   SDL2-devel-ttf

BIN = ../openempires

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
SRCS += Palette.c
SRCS += Point.c
SRCS += Points.c
SRCS += Quad.c
SRCS += Rect.c
SRCS += Registrar.c
SRCS += Scanline.c
SRCS += State.c
SRCS += Slp.c
SRCS += Stack.c
SRCS += Surface.c
SRCS += Table.c
SRCS += Terrain.c
SRCS += Text.c
SRCS += Tile.c
SRCS += Tiles.c
SRCS += Unit.c
SRCS += Units.c
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

SANITIZE = 0

FLAGS = -Wall -Wextra -Wshadow -Wpedantic

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

ifeq (1, $(SANITIZE))
FLAGS += -fsanitize=address -g -Og
else
FLAGS += -flto -O3 -march=native
endif

LIBS = -lSDL2 -lSDL2_ttf

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

$(BIN): $(OBJS) $(DEPS)
	$(COMPILER) $(FLAGS) $(OBJS) $(LIBS) -o $(BIN)

%.o : %.c %.d Makefile
	$(COMPILER) $(FLAGS) -MMD -MP -MT $@ -MF $*.d -c $<

-include *.d

%.d : ;

.PHONY clean:
clean:
	rm -f $(BIN)
	rm -f $(OBJS)
	rm -f $(DEPS)

.PHONY analysis:
analysis:
	pvs-studio-analyzer trace -- make -j8
	pvs-studio-analyzer analyze -j2 -l ~/pvs-lic -o log
	plog-converter -a GA:1,2 -t tasklist -o tasks log
	cat tasks
	rm log
	rm strace_out
	rm tasks
