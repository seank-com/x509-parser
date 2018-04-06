
CFLAGS=\
	-c \
	-I../azure-iot-sdk-c/provisioning_client/adapters \

SOURCES=\
	custom_hsm.c \
	base64.c \
	pem2bin.c \
	x509.c \

OBJECTS=$(SOURCES:.c=.o)

LIBRARY=custom_hsm.a

all: $(SOURCES) $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	ar rcs $(LIBRARY) $(OBJECTS)

.c.o:
	gcc $(CFLAGS) -o $@ $<
