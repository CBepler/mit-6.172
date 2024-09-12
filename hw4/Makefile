CC := clang

EXTRA_CFLAGS ?=
CFLAGS := -Wall -std=gnu99 -g

ifeq ($(DEBUG),1)
  CFLAGS += -O0
else
  CFLAGS += -O3 -DNDEBUG
endif

ifeq ($(ASSEMBLE),1)
  CFLAGS += -S
endif

ifeq ($(NPARALLEL), 1)
else
	CFLAGS += -fopenmp -fsanitize=thread
endif

CFLAGS += $(EXTRA_CFLAGS)

LDFLAGS := -lrt

SRC := quicksort.c

OUTPUT := out

OLD_CFLAGS := $(shell cat .cflags 2> /dev/null)
ifneq ($(CFLAGS),$(OLD_CFLAGS))
.cflags::
	@echo "$(CFLAGS)" > .cflags
endif


all: compile

compile: 
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC)

clean::
	rm -f $(OUTPUT) *.o *.s .cflags perf.data */perf.data
