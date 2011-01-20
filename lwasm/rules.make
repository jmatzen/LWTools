dirname := $(dir $(lastword $(MAKEFILE_LIST)))

lwasm_srcs_local :=  debug.c input.c insn_bitbit.c insn_gen.c insn_indexed.c \
	insn_inh.c insn_logicmem.c insn_rel.c insn_rlist.c insn_rtor.c insn_tfm.c \
	instab.c list.c lwasm.c macro.c main.c os9.c output.c pass1.c pass2.c \
	pass3.c pass4.c pass5.c pass6.c pass7.c pragma.c pseudo.c section.c \
	struct.c symbol.c

lwasm_srcs := $(lwasm_srcs) $(addprefix $(dirname),$(lwasm_srcs_local))

