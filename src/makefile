
all: CFLAGS= -c -x c -DSEANK_DEBUG
release: CFLAGS=	-c -x c

SOURCES=\
	base64.c \
	pem2bin.c \
	x509.c \
	main.c \

OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=x509test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	gcc -o $(EXECUTABLE) $(OBJECTS)

.c.o:
	gcc $(CFLAGS) -o $@ $<

clean:
	rm $(OBJECTS)

release: $(SOURCES) $(EXECUTABLE)
