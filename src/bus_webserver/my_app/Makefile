PWD=$(shell pwd)

IFLAGS		+=-I $(LINK_PATH)/inc -I$(PWD)/include
CFLAGS          += -fPIC   -g

ifeq ($(LT_ARCH),PL_RALINK)
        CC    = mipsel-linux-cc
        AR    = mipsel-linux-ar
        STRIP    = mipsel-linux-strip
        CFLAGS   += -c -O2 -DMIPSEL -mips3 -D__LITTLE_ENDIAN__ -D__ralink__
        SRC_PREFIX = src
        H_PREFIX = h
        LINK_PATH=$(PWD)/../linux-mipsel-static
else ifeq ($(LT_ARCH),PL_OCTEON)
        CC    = mips64-octeon-linux-gnu-gcc
        AR    = mips64-octeon-linux-gnu-ar
        STRIP = mips64-linux-strip
        CFLAGS += -c -O2 -D__MIPS__ -msoft-float -D__MIPS64__ -EB -D__BIG_ENDIAN__ -D__octeon__ -Dstrlcpy=strncpy
        SRC_PREFIX = src
        H_PREFIX = h
else
        CC    = gcc
        AR    = ar
        STRIP = strip
        CFLAGS   = -c -O2 -D__LITTLE_ENDIAN__ -lm
        LINK_PATH=$(PWD)/../linux-x86-static
endif

LINK_S_PATH	= $(LINK_PATH)/bin
LINK_S_LIBS	= -lgo  -L$(ARCH_NAME)/a

CFLAGS             += -fPIC   -w
DFLAGS             += -D_REENTRANT -DPIC  $(patsubst %,-D%,$(filter BIT_%,$(MAKEFLAGS))) -DBIT_PACK_EST=$(BIT_PACK_EST) -DBIT_PACK_SSL=$(BIT_PACK_SSL)
LDFLAGS            += '-Wl,--enable-new-dtags' '-Wl,-rpath,$$ORIGIN/' '-rdynamic'
LINK_S_LIBS        += -lpthread -lm -lrt -ldl

TARGET_NAME	= my_app

SRC_BASE=$(PWD)
OBJ_DIR=$(ARCH_NAME)/o

FILES = $(wildcard $(SRC_BASE)/*.c)
TMP_C = $(notdir $(FILES))
OBJS = $(TMP_C:%.c=$(OBJ_DIR)/%.o)


$(OBJ_DIR):
	-mkdir $(ARCH_NAME)
	-mkdir $(OBJ_DIR)
	-mkdir $(ARCH_NAME)/exe
	-mkdir $(ARCH_NAME)/a


CONFIG=$(LINK_PATH)
BIT_PACK_EST	= 1
DEPS_31 += $(CONFIG)/inc/est.h
DEPS_31 += $(CONFIG)/inc/bit.h
DEPS_31 += $(CONFIG)/inc/bitos.h
DEPS_31 += $(CONFIG)/obj/estLib.o
ifeq ($(BIT_PACK_EST),1)
    DEPS_31 += $(CONFIG)/bin/libest.a
endif
DEPS_31 += $(CONFIG)/inc/goahead.h
DEPS_31 += $(CONFIG)/inc/js.h
DEPS_31 += $(CONFIG)/obj/action.o
DEPS_31 += $(CONFIG)/obj/alloc.o
DEPS_31 += $(CONFIG)/obj/auth.o
DEPS_31 += $(CONFIG)/obj/cgi.o
DEPS_31 += $(CONFIG)/obj/crypt.o
DEPS_31 += $(CONFIG)/obj/file.o
DEPS_31 += $(CONFIG)/obj/fs.o
DEPS_31 += $(CONFIG)/obj/http.o
DEPS_31 += $(CONFIG)/obj/js.o
DEPS_31 += $(CONFIG)/obj/jst.o
DEPS_31 += $(CONFIG)/obj/options.o
DEPS_31 += $(CONFIG)/obj/osdep.o
DEPS_31 += $(CONFIG)/obj/rom-documents.o
DEPS_31 += $(CONFIG)/obj/route.o
DEPS_31 += $(CONFIG)/obj/runtime.o
DEPS_31 += $(CONFIG)/obj/socket.o
DEPS_31 += $(CONFIG)/obj/upload.o
DEPS_31 += $(CONFIG)/obj/est.o
DEPS_31 += $(CONFIG)/obj/matrixssl.o
DEPS_31 += $(CONFIG)/obj/nanossl.o
DEPS_31 += $(CONFIG)/obj/openssl.o

$(ARCH_NAME)/a/libgo.a: $(DEPS_31)
	@echo '      [Link] $@'
	$(AR) r  $@ $^



$(OBJ_DIR)/%.o : $(SRC_BASE)/%.c
	$(CC) $(CFLAGS) $(CFLAGS_$(<:.c=.o)) $(IFLAGS) $(IFLAGS_$(<:.c=.o)) -o$@ $<


all: $(OBJ_DIRS) lib $(OBJS) $(TARGET_NAME)

lib: $(ARCH_NAME)/a/libgo.a

target: $(OBJ_DIR) $(OBJS)
	
$(TARGET_NAME):lib $(OBJ_DIR) $(OBJS)
	$(CC) -O2 -lm $(OBJS) $(IFLAGS) $(LINK_S_LIBS)  -o $(TARGET_NAME)
	$(STRIP) $@

clean:
	rm -rf $(ARCH_NAME)
