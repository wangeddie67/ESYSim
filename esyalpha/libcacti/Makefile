## these are set in ../Makefile
## CC, AR, AROPT, CFLAGS, RANLIB

#
# all the sources
#
SRC	= io.c time.c
HDR	= def.h

#
# common objects
#
OBJ	= io.$(OEXT) time.$(OEXT)


#
# all targets
#
libcacti.$(LEXT):	$(OBJ)
	$(RM) libcacti.$(LEXT)
	$(AR) $(AROPT)libcacti.$(LEXT) $(OBJ)
	$(RANLIB) libcacti.$(LEXT)

.c.$(OEXT):
	$(CC) $(CFLAGS) -c $<


clean:
	-$(RM) *.o *.a 



# DO NOT DELETE THIS LINE -- make depend depends on it.


io.$(OEXT): io.c def.h

time.$(OEXT): time.c def.h
