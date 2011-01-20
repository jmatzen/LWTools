CC := gcc

CPPFLAGS += -I lwlib -DPACKAGE_STRING='"lwtools 4.0-pre"' -DPACKAGE_BUGREPORT='"lost@l-w.ca"'

LDFLAGS += -L$(PWD)/lwlib -llw

MAIN_TARGETS := lwasm/lwasm lwlink/lwlink lwar/lwar

.PHONY: all
all: $(MAIN_TARGETS)

subdirs := lwasm lwlink lwar lwlib

-include $(subdirs:=/rules.make)

lwasm_objs := $(lwasm_srcs:.c=.o)
lwlink_objs := $(lwlink_srcs:.c=.o)
lwar_objs := $(lwar_srcs:.c=.o)
lwlib_objs := $(lwlib_srcs:.c=.o)

lwasm_deps := $(lwasm_srcs:.c=.d)
lwlink_deps := $(lwlink_srcs:.c=.d)
lwar_deps := $(lwar_srcs:.c=.d)
lwlib_deps := $(lwlib_srcs:.c=.d)
lwobjdump_deps := $(lwobjdump_srcs:.c=.d)

,PHONY: lwlink lwasm lwar
lwlink: lwlink/lwlink
lwasm: lwasm/lwasm
lwar: lwar/lwar
lwobjdump: lwlink/lwobjdump

lwasm/lwasm: $(lwasm_objs) lwlib
	$(CC) -o $@ $(lwasm_objs) $(LDFLAGS)

lwlink/lwlink: $(lwlink_objs)
	$(CC) -o $@ $(lwlink_objs)

lwlink/lwobjdump: $(lwobjdump_objs)
	$(CC) -o $@ $(lwobjdump_objs)

lwar/lwar: $(lwar_objs)
	$(CC) -o $@ $(lwar_objs)


.phony: lwlib
lwlib: lwlib/liblw.a

lwlib/liblw.a: $(lwlib_objs)
	$(AR) rc $@ $^

%.d: %.c
	@echo "Building dependencies for $@"
	@$(CC) -MM $(CPPFLAGS) -o $*.d $<
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o $*.d:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

-include $(lwasm_deps) $(lwlink_deps) $(lwar_deps) $(lwlib_deps) $(lwobjdump_deps)

extra_clean := $(extra_clean) *~ */*~

.PHONY: clean
clean:
	rm -f $(lwasm_deps) $(lwlink_deps) $(lwar_deps) $(lwlib_deps) $(lwobjdump_deps)
	rm -f lwlib/liblw.a lwasm/lwasm lwlink/lwlink lwlink/lwobjdump lwar/lwar
	rm -f $(lwasm_objs) $(lwlink_objs) $(lwar_objs) $(lwlib_objs) $(lwobjdump_objs)
	rm -f $(extra_clean)
	