## these are set in ../Makefile
## CC, AR, AROPT, CFLAGS, RANLIB

#
# common objects
#
OBJ	= libexo.$(OEXT) exolex.$(OEXT)


#
# all targets
#
libexo.$(LEXT):	$(OBJ)
	$(RM) libexo.$(LEXT)
	$(AR) $(AROPT)libexo.$(LEXT) $(OBJ)
	$(RANLIB) libexo.$(LEXT)

.c.$(OEXT):
	$(CC) $(CFLAGS) -c $<


clean:
	-$(RM) *.o   *.a



# DO NOT DELETE THIS LINE -- make depend depends on it.

libexo.$(OEXT): ../host.h ../misc.h ../machine.h ../host.h ../misc.h
libexo.$(OEXT): ../machine.def libexo.h libexo.c
exolex.$(OEXT): ../host.h ../misc.h libexo.h ../machine.h ../host.h ../misc.h
exolex.$(OEXT): ../machine.def exolex.c exolex.l

.l.$(OEXT):
	flex -o $<