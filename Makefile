OUT = build/main
CC = g++
CFLAGS = 
ODIR = build
SDIR = src
INC = -Iinc

SOURCES = $(wildcard $(SDIR)/*.cpp) \
	      $(wildcard $(SDIR)/impl/*.cpp)
OBJS    = $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))

debug: CFLAGS += -DDEBUG -g
debug: $(OUT)

$(ODIR):
	mkdir $(ODIR)
	mkdir $(ODIR)/impl

$(ODIR)/%.o: $(SDIR)/%.cpp 
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

$(OUT): $(OBJS) $(ODIR)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUT)
