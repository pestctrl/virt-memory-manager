OUT = build/main
CC = g++
CFLAGS = 
ODIR = build
SDIR = src
INC = -Iinc

_OBJS = main.o simulations.o IReplacementAlgorithm.o \
	ReplacementAlgorithm.o semaphores.o everything.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

debug: CFLAGS += -DDEBUG -g
debug: $(OUT)

$(ODIR)/%.o: $(SDIR)/%.cpp 
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

$(OUT): $(OBJS) 
	$(CC) $(CFLAGS) -o $(OUT) $(OBJS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUT)
