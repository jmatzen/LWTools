dirname := $(dir $(lastword $(MAKEFILE_LIST)))

lwar_srcs_local := add.c extract.c list.c lwar.c main.c remove.c replace.c

lwar_srcs := $(lwar_srcs) $(addprefix $(dirname),$(lwar_srcs_local))
