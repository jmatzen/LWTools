dirname := $(dir $(lastword $(MAKEFILE_LIST)))

lwlib_srcs_local := lw_alloc.c lw_realloc.c lw_free.c lw_error.c lw_expr.c \
	lw_stack.c lw_string.c lw_stringlist.c lw_cmdline.c

lwlib_srcs := $(lwlib_srcs) $(addprefix $(dirname),$(lwlib_srcs_local))
