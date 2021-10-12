SRC_DIR=./src/
OUTPUT=main
##################################
##     Compiler and linker      ##
##################################

SANITIZER_FLAG=
#-fsanitize-address-use-after-scope -fsanitize=address -fsanitize=undefined  
CFLAGS=-g -O0 -fPIE -fPIC -xc $(SANITIZER_FLAG) -I$(SRC_DIR) -O0
LFLAGS=-g -static -fPIE -fPIC -static-libasan $(SANITIZER_FLAG) -lm -lole32 -lpthread 

C_COMPILER=i686-w64-mingw32-gcc
LINKER=i686-w64-mingw32-gcc

##################################
SRCS=$(shell find $(SRC_DIR) -regex .+[.]c)
OBJS=$(SRCS:.c=.o)

.PHONY: all 
all: $(OBJS) 
	$(MAKE) link

$(OBJS): %.o: %.c
	$(C_COMPILER) $(CFLAGS) -c -o $@ $<

link:
	$(LINKER) $(OBJS) $(LFLAGS) -o $(OUTPUT)  

clean:
	rm $(OBJS) $(OUTPUT) 







