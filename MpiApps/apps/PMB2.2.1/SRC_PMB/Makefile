##### User configurable options #####
#include make_sun
include make_i86
#include make_solaris
#include make_dec
#include make_ibm_sp
#include make_sr2201
#include make_vpp
#include make_t3e
#include make_sgi
#include make_sx4
#include make_Linux
### End User configurable options ###
.SUFFIXES: 
.SUFFIXES: .c .o


SHELL       = /bin/sh
PROFLIB     =

SRC1 = pmb.c declare.c  pmb_init.c Mem_Manager.c Parse_Name_MPI1.c  BenchList.c strgs.c \
Err_Handler.c g_info.c  Warm_Up.c Output.c \
PingPong.c PingPing.c Allreduce.c Reduce_scatter.c Reduce.c Exchange.c Bcast.c Barrier.c Allgather.c Allgatherv.c Alltoall.c Sendrecv.c Init_Transfer.c chk_diff.c CPU_Exploit.c


OBJ1 = pmb.o declare.o  pmb_init.o Mem_Manager.o Parse_Name_MPI1.o  BenchList.o strgs.o \
Err_Handler.o g_info.o  Warm_Up.o Output.o \
PingPong.o PingPing.o Allreduce.o Reduce_scatter.o Reduce.o Exchange.o Bcast.o Barrier.o Allgather.o Allgatherv.o Alltoall.o Sendrecv.o Init_Transfer.o  chk_diff.o CPU_Exploit.o


SRCEXT = pmb.c declare.c  pmb_init.c  Mem_Manager.c BenchList.c Parse_Name_EXT.c strgs.c \
Err_Handler.c g_info.c  Warm_Up.c Output.c \
Window.c OneS_unidir.c OneS_bidir.c OneS_accu.c Init_Transfer.c User_Set_Info.c chk_diff.c 

OBJEXT = pmb.o declare.o  pmb_init.o  Mem_Manager.o BenchList.o Parse_Name_EXT.o strgs.o \
Err_Handler.o g_info.o  Warm_Up.o Output.o \
Window.o OneS_unidir.o OneS_bidir.o OneS_accu.o Init_Transfer.o User_Set_Info.o chk_diff.o 

SRCIO= pmb.c declare.c  pmb_init.c  Mem_Manager.c Init_File.c User_Set_Info.c\
BenchList.c Parse_Name_IO.c strgs.c User_Set_Info.c \
Err_Handler.c g_info.c  Warm_Up.c Output.c CPU_Exploit.c\
Open_Close.c Write.c Read.c Init_Transfer.c chk_diff.c  

OBJIO= pmb.o declare.o  pmb_init.o  Mem_Manager.o Init_File.o \
BenchList.o Parse_Name_IO.o strgs.o User_Set_Info.o CPU_Exploit.o\
Err_Handler.o g_info.o  Warm_Up.o Output.o \
Open_Close.o Write.o Read.o Init_Transfer.o chk_diff.o  

default: PMB-MPI1
all: PMB-IO  PMB-EXT PMB-MPI1     

PMB-MPI1: $(SRC1) declare.h exe_mpi1
	$(MAKE) MPI1 CPP=MPI1
PMB-EXT:$(SRCEXT) declare.h  exe_ext
	$(MAKE) EXT CPP=EXT
PMB-IO: $(SRCIO) declare.h exe_io
	$(MAKE) IO CPP=MPIIO

exe_io:
	touch $@ *.c; rm -rf exe_ext exe_mpi1
exe_ext:
	touch $@ *.c; rm -rf exe_io exe_mpi1
exe_mpi1:
	touch $@ *.c; rm -rf exe_io exe_ext

MPI1: $(OBJ1) 
	$(CLINKER) -o PMB-MPI1 $(OBJ1)  $(LIB_PATH) $(LIBS)

EXT : $(OBJEXT) 
	$(CLINKER) -o PMB-EXT $(OBJEXT)  $(LIB_PATH) $(LIBS)

IO: $(OBJIO) 
	$(CLINKER) -o PMB-IO $(OBJIO)  $(LIB_PATH) $(LIBS)

# Make sure that we remove executables for specific architectures
clean:
	/bin/rm -f *.o *~ PI* core PMB-IO PMB-EXT PMB-MPI1 exe_io exe_ext exe_mpi1

.c.o:
	$(CC) -I$(MPI_INCLUDE) $(CPPFLAGS) -D$(CPP) $(OPTFLAGS) -c $*.c

# DEPENDENCIES
declare.h:	settings.h comm_info.h settings_io.h Bnames_MPI1.h \
                Bnames_EXT.h Bnames_IO.h err_check.h appl_errors.h
	touch declare.h

pmb.o:              declare.h Benchmark.h
pmb_init.o:	    declare.h Benchmark.h
Mem_Manager.o:	    declare.h Benchmark.h
BenchList.o:        declare.h Benchmark.h 
Output.o:	    declare.h Benchmark.h
Warm_Up.o:          declare.h Benchmark.h
chk_diff.o:         declare.h chk_diff.c
declare.o:	    declare.h
g_info.o: 	    declare.h Benchmark.h
Err_Handler.o:	    declare.h appl_errors.h
Init_Transfer.o:    declare.h Benchmark.h Comments.h

# PMB-MPI1:
Parse_Name_MPI1.o:  declare.h Benchmark.h Comments.h
PingPing.o:	    declare.h
PingPong.o:	    declare.h
Sendrecv.o:	    declare.h
Exchange.o:	    declare.h
Reduce.o:	    declare.h
Reduce_scatter.o:   declare.h
Allreduce.o:	    declare.h
Bcast.o:	    declare.h
Allgather.o:        declare.h
Allgatherv.o:       declare.h
Barrier.o:	    declare.h

# PMB-EXT:
Parse_Name_EXT.o:   declare.h Benchmark.h Comments.h
Window.o:           declare.h
OneS_unidir.o:      declare.h
OneS_bidir.o:       declare.h
OneS_accu.o:        declare.h

# PMB-IO:
Parse_Name_IO.o:     declare.h Benchmark.h Comments.h
Init_File.o:         declare.h Benchmark.h Comments.h
Open_Close.o:        declare.h Benchmark.h 
Write.o:             declare.h Benchmark.h 
Read.o:              declare.h Benchmark.h 
