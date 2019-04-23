/* loader.c - program loader routines */

/* SimpleScalar(TM) Tool Suite
 * Copyright (C) 1994-2001 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 * All Rights Reserved. 
 * 
 * THIS IS A LEGAL DOCUMENT, BY USING SIMPLESCALAR,
 * YOU ARE AGREEING TO THESE TERMS AND CONDITIONS.
 * 
 * No portion of this work may be used by any commercial entity, or for any
 * commercial purpose, without the prior, written permission of SimpleScalar,
 * LLC (info@simplescalar.com). Nonprofit and noncommercial use is permitted
 * as described below.
 * 
 * 1. SimpleScalar is provided AS IS, with no warranty of any kind, express
 * or implied. The user of the program accepts full responsibility for the
 * application of the program and the use of any results.
 * 
 * 2. Nonprofit and noncommercial use is encouraged. SimpleScalar may be
 * downloaded, compiled, executed, copied, and modified solely for nonprofit,
 * educational, noncommercial research, and noncommercial scholarship
 * purposes provided that this notice in its entirety accompanies all copies.
 * Copies of the modified software can be delivered to persons who use it
 * solely for nonprofit, educational, noncommercial research, and
 * noncommercial scholarship purposes provided that this notice in its
 * entirety accompanies all copies.
 * 
 * 3. ALL COMMERCIAL USE, AND ALL USE BY FOR PROFIT ENTITIES, IS EXPRESSLY
 * PROHIBITED WITHOUT A LICENSE FROM SIMPLESCALAR, LLC (info@simplescalar.com).
 * 
 * 4. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 * 
 * 5. Noncommercial and nonprofit users may distribute copies of SimpleScalar
 * in compiled or executable form as set forth in Section 2, provided that
 * either: (A) it is accompanied by the corresponding machine-readable source
 * code, or (B) it is accompanied by a written offer, with no time limit, to
 * give anyone a machine-readable copy of the corresponding source code in
 * return for reimbursement of the cost of distribution. This written offer
 * must permit verbatim duplication by anyone, or (C) it is distributed by
 * someone who received only the executable form, and is accompanied by a
 * copy of the written offer of source code.
 * 
 * 6. SimpleScalar was developed by Todd M. Austin, Ph.D. The tool suite is
 * currently maintained by SimpleScalar LLC (info@simplescalar.com). US Mail:
 * 2395 Timbercrest Court, Ann Arbor, MI 48105.
 * 
 * Copyright (C) 2000-2001 by The Regents of The University of Michigan.
 * Copyright (C) 1994-2001 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 */

#include <stdio.h>
#include <stdlib.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "endian.h"
#include "regs.h"
#include "memory.h"
#include "sim.h"
#include "eio.h"
#include "loader.h"

#ifdef SMT_SS
#include "context.h"
#endif
#ifdef BFD_LOADER
#include <bfd.h>
#else /* !BFD_LOADER */
#include "ecoff.h"
#include "elf64.h"
#endif /* BFD_LOADER */

/* amount of tail padding added to all loaded text segments */
#define TEXT_TAIL_PADDING 0 /* was: 128 */

AlphaSystemState _system;

/* program bss segment base */
md_addr_t ld_bss_base = 0;

/* program bss size in bytes */
unsigned int ld_bss_size = 0;

/* top of the data segment */
md_addr_t ld_brk_point = 0;

uint64_t addr, zero=0;



void initialize_system_state()
{
  // this function is called by the loader to initialize the system state,
  // which is a struct containing variables needed by certain syscalls.
  _system.brk_point = (ld_bss_base + ld_bss_size + 0xffffLL) & 0xffffffffffff0000LL;
  
  _system.mmap_end = 0x180000000; 
}


/* register simulator-specific statistics */
void
ld_reg_stats(struct stat_sdb_t *sdb, int threadid)	/* stats data base */
{
  context *current;
  current = thecontexts[threadid];

  stat_reg_uint(sdb, "threadid",
		"Thread id",
		&current->id, current->id, "0x%010p");
  stat_reg_addr(sdb, "ld_text_base",
		"program text (code) segment base",
		&current->ld_text_base, current->ld_text_base, "0x%010p");
  stat_reg_uint(sdb, "ld_text_size",
		"program text (code) size in bytes",
		&current->ld_text_size, current->ld_text_size, NULL);
  stat_reg_addr(sdb, "ld_data_base",
		"program initialized data segment base",
		&current->ld_data_base, current->ld_data_base, "0x%010p");
  stat_reg_uint(sdb, "ld_data_size",
		"program init'ed `.data' and uninit'ed `.bss' size in bytes",
		&current->ld_data_size, current->ld_data_size, NULL);
  stat_reg_addr(sdb, "ld_stack_base",
		"program stack segment base (highest address in stack)",
		&current->ld_stack_base, current->ld_stack_base, "0x%010p");
  stat_reg_uint(sdb, "ld_stack_size",
		"program initial stack size",
		&current->ld_stack_size, current->ld_stack_size, NULL);
  stat_reg_addr(sdb, "ld_prog_entry",
		"program entry point (initial PC)",
		&current->ld_prog_entry, current->ld_prog_entry, "0x%010p");
  stat_reg_addr(sdb, "ld_environ_base",
		"program environment base address address",
		&current->ld_environ_base, current->ld_environ_base, "0x%010p");
  stat_reg_int(sdb, "ld_target_big_endian",
	       "target executable endian-ness, non-zero if big endian",
	       &current->ld_target_big_endian, current->ld_target_big_endian, NULL);
}


/* load program text and initialized data into simulated virtual memory
   space and initialize program segment range variables */
void
ld_load_prog(char *fname,		/* program to load */
	     int argc, char **argv,	/* simulated program cmd line args */
	     char **envp,		/* simulated program environment */
	     struct regs_t *regs,	/* registers to initialize for load */
	     struct mem_t *mem,		/* memory space to load prog into */
	     int zero_bss_segs,		/* zero uninit data segment? */
             int threadid)               /*  context id */
{
  int i;
  qword_t temp;
  md_addr_t sp, data_break = 0, null_ptr = 0, argv_addr, envp_addr;

  context *current;
  current = thecontexts[threadid];

  if (eio_valid(fname))
    {
      if (argc != 1)
	{
	  fprintf(stderr, " thread %d error:  EIO file has arguments\n",threadid);
	  exit(1);
	}

      fprintf(stderr, " thread %d sim:  loading EIO file: %s\n",threadid, fname);

      current->sim_eio_fname = mystrdup(fname);

      /* open the EIO file stream */
      current->sim_eio_fd = eio_open(fname);

      /* load initial state checkpoint */
      if (eio_read_chkpt(regs, mem, current->sim_eio_fd) != -1)
	fatal(" thread %d bad initial checkpoint in EIO file",threadid);

      /* load checkpoint? */
      if (current->sim_chkpt_fname != NULL)
	{
	  counter_t restore_icnt;

	  FILE *chkpt_fd;

	  fprintf(stderr, "sim: thread %d loading checkpoint file: %s\n",threadid,
		  current->sim_chkpt_fname);

	  if (!eio_valid(current->sim_chkpt_fname))
	    fatal(" thread %d file `%s' does not appear to be a checkpoint file",threadid,
		  current->sim_chkpt_fname);

	  /* open the checkpoint file */
	  chkpt_fd = eio_open(current->sim_chkpt_fname);

	  /* load the state image */
	  restore_icnt = eio_read_chkpt(regs, mem, chkpt_fd);

	  /* fast forward the baseline EIO trace to checkpoint location */
	  myfprintf(stderr, "sim: thread %d fast forwarding to instruction %n\n",threadid,
		    restore_icnt);
	  eio_fast_forward(current->sim_eio_fd, restore_icnt);
	}

      /* computed state... */
      current->ld_environ_base = regs->regs_R[MD_REG_SP];
      current->ld_prog_entry = regs->regs_PC;

      /* fini... */
      return;
    }
#ifdef MD_CROSS_ENDIAN
  else
    {
      warn("endian of `%s' does not match host", fname);
      warn("running with experimental cross-endian execution support");
      warn("****************************************");
      warn("**>> please check results carefully <<**");
      warn("****************************************");

    }
#endif /* MD_CROSS_ENDIAN */

  if (current->sim_chkpt_fname != NULL)
    fatal(" thread %d checkpoints only supported while EIO tracing",threadid);



  {
    FILE *fobj;
    long floc;
    Elf64_Ehdr fhdr;
    Elf64_Phdr phdr;
    Elf64_Shdr shdr;
    char *shdr_strs; // Byte array of section header strings
    struct ecoff_aouthdr ahdr;
//    struct ecoff_scnhdr shdr;

    char *p;

    /* record profile file name */
    current->ld_prog_fname = argv[0];

    /* load the program into memory, try both endians */
#if defined(__CYGWIN32__) || defined(_MSC_VER)
    fobj = fopen(argv[0], "rb");
#else
    fobj = fopen(argv[0], "r");
#endif
    if (!fobj)
      fatal(" thread %d cannot open executable `%s'",threadid, argv[0]);

//    if (fread(&fhdr, sizeof(struct ecoff_filehdr), 1, fobj) < 1)
    if (fread(&fhdr, sizeof(Elf64_Ehdr), 1, fobj) < 1)
      fatal(" thread %d cannot read header from executable `%s'",threadid, argv[0]);

    /* record endian of target */
  if ((fhdr.e_ident[0] != '\x7f') ||
      (fhdr.e_ident[1] != 'E') ||
      (fhdr.e_ident[2] != 'L') ||
      (fhdr.e_ident[3] != 'F'))
    fatal("ArchLib: %s did not contain a valid magic number", argv[0]);
  if (fhdr.e_ident[4] != ELFCLASS64)
    fatal("ArchLib: %s was not a 64-bit ELF binary", argv[0]);
  if (fhdr.e_ident[5] != ELFDATA2LSB)
    fatal("ArchLib: %s did not contain little-endian object file strutcs", 
        argv[0]);
  if (fhdr.e_type != ET_EXEC)
    fatal("ArchLib: %s is not an executable file", argv[0]);


/*    if (fhdr.e_ident[5] != ELFDATA2LSB)
      fatal("ArchLib: %s did not contain little-endian object file strutcs", argv[0]);
    else if (fhdr.f_magic == MD_SWAPH(ECOFF_EB_MAGIC)
	     || fhdr.f_magic == MD_SWAPH(ECOFF_EL_MAGIC)
	     || fhdr.f_magic == MD_SWAPH(ECOFF_EB_OTHER)
	     || fhdr.f_magic == MD_SWAPH(ECOFF_EL_OTHER))
      fatal("Alpha simulator cannot run PISA binary `%s'", argv[0]);
    else
      fatal("bad magic number in executable `%s' (not an executable)", fhdr.f_magic,
	    argv[0]);
*/

  for (i = 0; i < fhdr.e_phnum; i++)
  {
    if (fseek(fobj, fhdr.e_phoff + (i*fhdr.e_phentsize), SEEK_SET) == -1)
      fatal("ArchLib: error parsing %s", argv[0]);
    size_t len = fread(&phdr, 1, sizeof(phdr), fobj);
    if (len != sizeof(phdr))
      fatal("ArchLib: error parsing %s", argv[0]);

    switch (phdr.p_type)
    {
      case PT_NULL:
        //printf("ArchLib: ignored ELF64 program segment PT_NULL\n");
        break;
      case PT_NOTE:
        //printf("ArchLib: ignored ELF64 program segment PT_NOTE\n");
        break;
      case PT_SHLIB:
        //printf("ArchLib: ignored ELF64 program segment PT_SHLIB\n");
        break;
      case PT_LOOS:
        //printf("ArchLib: ignored ELF64 program segment PT_LOOS\n");
        break;
      case PT_HIOS:
        //printf("ArchLib: ignored ELF64 program segment PT_HIOS\n");
        break;
      case PT_LOPROC:
        //printf("ArchLib: ignored ELF64 program segment PT_LOPROC\n");
        break;
      case PT_HIPROC:
        //printf("ArchLib: ignored ELF64 program segment PT_HIPROC\n");
        break;

      case PT_LOAD:
      case PT_DYNAMIC:
      case PT_INTERP:
      case PT_PHDR:
        // seek to the beginning of the segment
        if (fseek(fobj, phdr.p_offset, SEEK_SET) == -1)
          fatal("ArchLib: error seeking to program seg %i from %s", 
              i, argv[0]);
        // read the segment
	p = (char*)calloc(phdr.p_filesz, sizeof(char));
        if (!p) 
          fatal("ArchLib: out of virtual memory");
        if (fread(p, phdr.p_filesz, 1, fobj) < 1)
          fatal("ArchLib: error reading program segment %i from %s", 
              i, argv[0]);
	// memory->bcopy(true, phdr.p_vaddr, p, phdr.p_filesz);
	mem_bcopy(mem_access, current->mem, Write, MD_SWAPQ(phdr.p_vaddr), p, MD_SWAPQ(phdr.p_filesz), current->id);
        free(p);
        break;
      default:
        //printf("ArchLib: ignored ELF64 program segment UNKNOWN\n");
        break;
    }
  }

//  current->ld_text_base = MD_SWAPQ(phdr.text_start);

  if (fseek(fobj, 
        fhdr.e_shoff + (fhdr.e_shstrndx*fhdr.e_shentsize), 
        SEEK_SET) == -1)
    fatal("ArchLib: error parsing %s", argv[0]);
  size_t len = fread(&shdr, 1, sizeof(shdr), fobj);
  if (len != sizeof(shdr))
    fatal("ArchLib: error parsing %s", argv[0]);
  if (shdr.sh_type != SHT_STRTAB)
    fatal("ArchLib: error parsing %s", argv[0]);
  if (fseek(fobj, shdr.sh_offset, SEEK_SET) == -1)
    fatal("ArchLib: error parsing %s", argv[0]);
  shdr_strs = (char*)calloc(shdr.sh_size, sizeof(char));
  len = fread(shdr_strs, 1, shdr.sh_size, fobj);
  if (len != shdr.sh_size)
    fatal("ArchLib: error parsing %s", argv[0]);
    
  // initialize text, data, and bss sections
  current->ld_prog_entry = fhdr.e_entry;
  current->ld_text_size = current->ld_text_base = 0;
  current->ld_data_size = current->ld_data_base = 0;

  // pull out program map from section headers
  for (i = 0; i < fhdr.e_shnum; i++)
  {
    if (fseek(fobj, fhdr.e_shoff + (i*fhdr.e_shentsize), SEEK_SET) == -1)
      fatal("ArchLib: error parsing %s", argv[0]);
    size_t len = fread(&shdr, 1, sizeof(shdr), fobj);
    if (len != sizeof(shdr))
      fatal("ArchLib: error parsing %s", argv[0]);

    // store size and base of text segment
    if (!strcmp(".text", shdr_strs + shdr.sh_name))
    {
      current->ld_text_size = MD_SWAPQ(shdr.sh_size);
      current->ld_text_base = MD_SWAPQ(shdr.sh_addr);
//      current->ld_prog_entry = shdr.sh_addr;
    }
    // store size and base of data segment
    if (!strcmp(".data", shdr_strs + shdr.sh_name))
    {
      current->ld_data_size = MD_SWAPQ(shdr.sh_size);
      current->ld_data_base = MD_SWAPQ(shdr.sh_addr);
    }
    // store size and base of bss segment
    if (!strcmp(".bss", shdr_strs + shdr.sh_name))
    {
//      current->ld_prog_entry = shdr.sh_addr;
      ld_bss_size = MD_SWAPQ(shdr.sh_size);
      current->ld_data_size +=ld_bss_size;
      ld_bss_base = MD_SWAPQ(shdr.sh_addr);
    }
  }

  free(shdr_strs);

/*    if (fread(&ahdr, sizeof(struct ecoff_aouthdr), 1, fobj) < 1)
      fatal("cannot read AOUT header from executable `%s'", argv[0]);

    current->ld_text_base = MD_SWAPQ(ahdr.text_start);
    current->ld_text_size = MD_SWAPQ(ahdr.tsize);
    current->ld_prog_entry = MD_SWAPQ(ahdr.entry);
    current->ld_data_base = MD_SWAPQ(ahdr.data_start);
    current->ld_data_size = MD_SWAPQ(ahdr.dsize) + MD_SWAPQ(ahdr.bsize);
    regs->regs_R[MD_REG_GP] = MD_SWAPQ(ahdr.gp_value);
*/    
    
    /* compute data segment size from data break point */
    data_break = current->ld_data_base + current->ld_data_size;

    /* seek to the beginning of the first section header, the file header comes
       first, followed by the optional header (this is the aouthdr), the size
       of the aouthdr is given in Fdhr.f_opthdr */
//    fseek(fobj, sizeof(struct ecoff_filehdr) + MD_SWAPH(fhdr.f_opthdr), 0);

    debug("processing %d sections in `%s'...",
	  MD_SWAPH(fhdr.e_phnum), argv[0]);



    /* done with the executable, close it */
    if (fclose(fobj))
      fatal("could not close executable `%s'", argv[0]);

  }

// old_version
  /* perform sanity checks on segment ranges */
  if (!current->ld_text_base || !current->ld_text_size)
    fatal(" thread %d executable is missing a `.text' section",threadid);
  if (!current->ld_data_base || !current->ld_data_size)
    fatal(" thread %d executable is missing a `.data' section",threadid);
  if (!current->ld_prog_entry)
    fatal(" thread %d program entry point not specified",threadid);

  /* determine byte/words swapping required to execute on this host */
  current->sim_swap_bytes = (endian_host_byte_order() != endian_target_byte_order(threadid));
  current->sim_swap_words = (endian_host_word_order() != endian_target_word_order(threadid));

  /* set up a local stack pointer, this is where the argv and envp
     data is written into program memory */
  current->ld_stack_base = current->ld_text_base - (409600+4096);

  sp = current->ld_stack_base - MD_MAX_ENVIRON;
  current->ld_stack_size = current->ld_stack_base - sp;
  current->stack_base = current->ld_stack_base - STACKSIZE;


  /* initial stack pointer value */
  current->ld_environ_base = sp;

  /* write [argc] to stack */
  temp = MD_SWAPQ(argc);
  mem_access(current->mem, Write, sp, &temp, sizeof(qword_t), current->id);
  regs->regs_R[MD_REG_A0] = temp;
  sp += sizeof(qword_t);

  /* skip past argv array and NULL */
  argv_addr = sp;
  regs->regs_R[MD_REG_A1] = argv_addr;
  sp = sp + (argc + 1) * sizeof(md_addr_t);

  /* save space for envp array and NULL */
  envp_addr = sp;
  for (i=0; envp[i]; i++)
    sp += sizeof(md_addr_t);
  sp += sizeof(md_addr_t);

  /* fill in the argv pointer array and data */
  for (i=0; i<argc; i++)
    {
      /* write the argv pointer array entry */
      temp = MD_SWAPQ(sp);
      mem_access(current->mem, Write, argv_addr + i*sizeof(md_addr_t), &temp, sizeof(md_addr_t), current->id);
      /* and the data */
      mem_strcpy(mem_access, current->mem, Write, sp, argv[i], current->id);
      sp += strlen(argv[i])+1;
    }
  /* terminate argv array with a NULL */
  mem_access(current->mem, Write, argv_addr + i*sizeof(md_addr_t), &null_ptr, sizeof(md_addr_t), current->id);

  /* write envp pointer array and data to stack */
  for (i = 0; envp[i]; i++)
    {
      /* write the envp pointer array entry */
      temp = MD_SWAPQ(sp);
      mem_access(current->mem, Write, envp_addr + i*sizeof(md_addr_t), &temp, sizeof(md_addr_t), current->id);
      /* and the data */
      mem_strcpy(mem_access, current->mem, Write, sp, envp[i], current->id);
      sp += strlen(envp[i]) + 1;
    }
  /* terminate the envp array with a NULL */
  mem_access(current->mem, Write, envp_addr + i*sizeof(md_addr_t), &null_ptr, sizeof(md_addr_t), current->id);

  /* did we tromp off the stop of the stack? */
  if (sp > current->ld_stack_base)
    {
      /* we did, indicate to the user that MD_MAX_ENVIRON must be increased,
	 alternatively, you can use a smaller environment, or fewer
	 command line arguments */
      fatal(" thread %d environment overflow, increase MD_MAX_ENVIRON in alpha.h",threadid);
    }

  /* initialize the bottom of heap to top of data segment */
  current->ld_brk_point = ROUND_UP(current->ld_data_base + current->ld_data_size, MD_PAGE_SIZE);

  /* set initial minimum stack pointer value to initial stack value */
  current->ld_stack_min = regs->regs_R[MD_REG_SP];

  regs->regs_R[MD_REG_SP] = current->ld_environ_base;
  regs->regs_PC = current->ld_prog_entry;
  printf(" thread %d ld_text_base: 0x%08x  ld_text_size: 0x%08x",threadid,
	current->ld_text_base, current->ld_text_size);
  printf(" thread %d ld_data_base: 0x%08x  ld_data_size: 0x%08x",threadid,
	current->ld_data_base, current->ld_data_size);
  printf(" thread %d ld_stack_base: 0x%08x  ld_stack_size: 0x%08x",threadid,
	current->ld_stack_base, current->ld_stack_size);
  printf(" thread %d ld_prog_entry: 0x%08x",threadid, current->ld_prog_entry);
    fflush (stdout);
  debug(" thread %d ld_text_base: 0x%08x  ld_text_size: 0x%08x",threadid,
	current->ld_text_base, current->ld_text_size);
  debug(" thread %d ld_data_base: 0x%08x  ld_data_size: 0x%08x",threadid,
	current->ld_data_base, current->ld_data_size);
  debug(" thread %d ld_stack_base: 0x%08x  ld_stack_size: 0x%08x",threadid,
	current->ld_stack_base, current->ld_stack_size);
  debug(" thread %d ld_prog_entry: 0x%08x",threadid, current->ld_prog_entry);
// old_version

// new_version
//  uint64_t stack_base = (current->ld_text_base - 1024*1024*8) & 0xFFFFFFFFFFFF0000LL;
//  uint64_t stack_min = current->ld_stack_base;
//
//  // calculate size of argv and envp pointer and data arrays
//  uint64_t argv_data_size = 0;
//  uint64_t envp_data_size = 0;
//  uint64_t argv_array_size;
//  uint64_t envp_array_size;
//  
//  for (argv_array_size = 0; argv[argv_array_size]; argv_array_size++)
//    argv_data_size += strlen(argv[argv_array_size]) + 1;
//  for (envp_array_size = 0; envp[envp_array_size]; envp_array_size++)
//    envp_data_size += strlen(envp[envp_array_size]) + 1;
//
//  uint64_t argc_value = argv_array_size;
//  argv_array_size = (argv_array_size + 1) * 8; // null terminated pointer array
//  envp_array_size = (envp_array_size + 1) * 8; // null terminated pointer array
//
//  // push stack to allow for argc, argv, envp, argv data, envp data
//  stack_min -= (8 + // argc is 64-bits
//      argv_array_size + 
//      envp_array_size + 
//      argv_data_size + 
//      envp_data_size);
//  // align the stack min, arbitrarily to 8kb
//  stack_min &= 0xFFFFFFFFFFFFE000LL;
//
//  // map argv and envp
//  uint64_t argv_array_base = stack_min + 8; // allow for argc
//  uint64_t envp_array_base = argv_array_base + argv_array_size;
//  uint64_t argv_data_base = envp_array_base + envp_array_size;
//  uint64_t envp_data_base = argv_data_base + argv_data_size;
// 
//  // write argc to memory
//  mem_bcopy(mem_access, current->mem, Write,
//		  /*buf*/stack_min, &argc_value, 8, current->id);
//  // memory->bcopy(true, stack_min, &argc_value, 8);
//  
//  // write argv array and data to memory
//  addr = argv_data_base;
//  for (i = 0; argv[i]; i++)
//  {
//    // write argv pointer
//    mem_bcopy(mem_access, current->mem, Write,
//		  /*buf*/argv_array_base + (i*8), &addr, 8, current->id);
////    memory->bcopy(true, argv_array_base + (i*8), &addr, 8);
//    // write argv data
//    mem_bcopy(mem_access, current->mem, Write,
//		  /*buf*/addr, argv[i], strlen(argv[i])+1, current->id);
////    memory->bcopy(true, addr, argv[i], strlen(argv[i])+1);
//    // increment data address
//    addr += strlen(argv[i])+1;
//  }
//  // null terminate argv array 
//   mem_bcopy(mem_access, current->mem, Write,
//		  /*buf*/argv_array_base + (i*8), &zero, 8, current->id);
////   memory->bcopy(true, argv_array_base + (i*8), &zero, 8); 
//  
//  // write envp array and data to memory
//  addr = envp_data_base;
//  for (i = 0; envp[i]; i++)
//  {
//    // write envp pointer
//    mem_bcopy(mem_access, current->mem, Write,
//		  /*buf*/envp_array_base + (i*8), &addr, 8, current->id);
////    memory->bcopy(true, envp_array_base + (i*8), &addr, 8);
//    // write envp data
//    mem_bcopy(mem_access, current->mem, Write,
//		  /*buf*/addr, envp[i], strlen(envp[i])+1, current->id);
////    memory->bcopy(true, addr, envp[i], strlen(envp[i])+1);
//    // increment data address
//    addr += strlen(envp[i])+1;
//  }
//  // null terminate argv array 
//  mem_bcopy(mem_access, current->mem, Write,
//		  /*buf*/envp_array_base + (i*8), &zero, 8, current->id);
////  memory->bcopy(true, envp_array_base + (i*8), &zero, 8); 
//  
//  // initialize registers
//  uint64_t pc = 0;
//  for (i = 0; i < 66 /*NUM_REGS*/; i++) regs->regs_R[i] = 0;
//
//  // write pc, sp, argc register, **argv register,
//  printf("PC: %llX\n", current->ld_prog_entry);
////  getchar();
//  regs->regs_PC = current->ld_prog_entry;
//
//  sp = stack_base;
//  current->ld_stack_size = current->ld_stack_base - sp;
//  current->ld_environ_base = sp;
//
////  uint64_t pc = ehdr.e_entry;
//  regs->regs_R[30] = stack_min;
//  regs->regs_R[16] = argc_value;
//  regs->regs_R[17] = argv_array_base;
//  regs->regs_R[18] = envp_array_base;
//
//  current->ld_brk_point = 0;
// new_version

  // clear the program complete flag
  program_complete = 0;

  // initialize the operating system state
  
  initialize_system_state();



  printf("FINISHED LOADER\n");
}
