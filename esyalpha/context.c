#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if 0
#include "context.h"
#include "regs.h"
#include "memory.h"

#include "smt.h"
#endif
#include "headers.h"

#ifdef SMT_SS

int numcontexts = 0;
context *thecontexts[MAXTHREADS];
int MTAcontexts = 0;

#ifdef PROG_OUT_FROM_SIM_OUT
extern char * sim_simout;

/* This makes the program output file name be prefixed
 * by the simulation output file's name. */
char * get_prog_out_file(const char * bnc_value) {
	char * filename;
	char * prog_out_file;
	char * simout_end;

	filename = strrchr(bnc_value, '/');
	if (filename)
		++filename;
	else
		filename=(char *) bnc_value;

	/* now we have removed any path prefix from the bnc-specified prog_out */

	prog_out_file = calloc(strlen(sim_simout)+strlen(filename)+1,sizeof(char));

	/* we remove ".sim_out" from the simulation output file name, if it is present */

	simout_end = strstr(sim_simout,".sim_out");

	if (simout_end) {
		int simoutlen = simout_end - sim_simout + 1; //+1 to get the period
		strncpy(prog_out_file,sim_simout,simoutlen);
		strcpy(prog_out_file + simoutlen,filename);
	} else {
		sprintf(prog_out_file,"%s.%s",sim_simout,filename);
	}

	return prog_out_file;
}
#endif

//char *sim_progout = NULL;

  /* read argument file *.bnc for each thread */
int
read_thread_args(char *env[], char *filename, int *curcontext)
{

    FILE *argfile;
    int retcode,j,numthreads;
    int locargc = 0;
    char firststring[256];
    context *current;

    if(*curcontext<0 || *curcontext>=MAXTHREADS)
    {
        fprintf(stderr,"Error: simulator supports %d threads only\n",MAXTHREADS);
        exit(1);
    }
    current = thecontexts[*curcontext] = (context *) calloc(1,sizeof(context));
    if(current == NULL)
    {
        fprintf(stderr,"allocation fail in read_thread_args\n");
        exit(1);
    }
    current->argv = (char **) malloc(30*sizeof(char *));
    if(current->argv == NULL)
    {
        fprintf(stderr,"allocation fail in read_thread_args\n");
        exit(1);
    }
    for(j=0;j<30;j++)
    {
        current->argv[j] = NULL;
        current->argv[j] = (char *) malloc(240*sizeof(char));
        if(current->argv[j] == NULL)
        {
            fprintf(stderr,"allocation fail in read_thread_args\n");
            exit(1);
        }
    }
    current->argfile = argfile = fopen(filename, "r");
    if(current->argfile == NULL)
    {
        fprintf(stderr,"Error in 'context.c:read_thread_args':- No such a benchmark configuration file: '%s' \n",filename);
        exit(1);
    }

    current->argv[0] = (char *) malloc(240);
    if(current->argv[0] == NULL)
    {
        fprintf(stderr,"allocation fail in read_thread_args\n");
        exit(1);
    }

    current->sim_progout = (char *) malloc(240);
    if(current->sim_progout == NULL)
    {
        fprintf(stderr,"allocation fail in read_thread_args\n");
        exit(1);
    }
    current->sim_progin = (char *) malloc(240);
    if(current->sim_progin == NULL)
    {
        fprintf(stderr,"allocation fail in read_thread_args\n");
        exit(1);
    }
    current->argc  = 0;
    current->sim_inputfd = 0;
    current->sim_progfd = NULL;
    current->fastfwd_count = 0;
    /*    SMTSIM-1.0
    ** Skip first symbol - assumed to be a '#' character
    */

    /*    SMTSIM-2.0
    ** Anything before the first # is the ff distance.
    */
    retcode = fscanf(argfile, "%s", firststring);
    if (firststring[0] != '#') {
        current->fastfwd_count = atoll(firststring);
        fscanf(argfile, "%s", firststring); /* this should be the # */
    }
    else
    {
        fprintf(stderr,"Error %s file first parameter must be fastforward distance \n",filename);
        exit(1);
    }
    fscanf(argfile, "%d", &numthreads);
    if( numthreads < 1 || numthreads > MAXTHREADS )
    {
        fprintf(stderr,"Error  %d  is invalid numthreads in %s\n",numthreads,filename);
        exit(1);
    }
  
    while (1) 
    {

        /*
        ** Read until EOF or the next '#' symbol
        */
        retcode = fscanf(argfile, "%s", current->argv[locargc]);
        if (retcode == EOF || current->argv[locargc][0] == '#') 
        {

            current->fname = current->argv[0];
            if(!current->argc)
                current->argc = locargc;

            current->id = current->masterid = *curcontext;
      
            /*  moved from sim-outorder.c  */
            /* allocate and initialize register file */
            regs_init(&(current->regs));
            current->regs.threadid = current->id;
            /* allocate and initialize memory space */
            current->mem = mem_create("mem");
            mem_init(current->mem);
            current->mem->threadid = current->masterid;

            /*  moved from loader.c  */
            /* program text (code) segment base */
            current->ld_text_base = 0;

            /* program text (code) size in bytes */
            current->ld_text_size = 0;

            /* program initialized data segment base */
            current->ld_data_base = 0;

            /* top of the data segment */
            current->ld_brk_point = 0;

            /* program initialized ".data" and uninitialized ".bss" size in bytes */
            current->ld_data_size = 0;

            /* program stack segment base (highest address in stack) */
            current->ld_stack_base = 0;

            /* program initial stack size */
            current->ld_stack_size = 0;

            /* lowest address accessed on the stack */
            current->ld_stack_min = -1;

            /* program file name */
            current->ld_prog_fname = NULL;

            /* program entry point (initial PC) */
            current->ld_prog_entry = 0;

            /* program environment base address address */
            current->ld_environ_base = 0;

            /* target executable endian-ness, non-zero if big endian */
            current->ld_target_big_endian = 0;

            /*  moved from main.c  */
            /* EIO interfaces */
            current->sim_eio_fname = NULL;
            current->sim_chkpt_fname = NULL;
            current->sim_eio_fd = NULL;

            /*  moved from sim-outorder.c  */
            /*
            * simulator stats
            */
            current->owindow_head = 0;

            /* number of useful instructions executed */
            current->sim_num_insn = 0;

            /* number of total instructions executed */
            current->sim_total_insn = 0;
            /* number of useful instructions fetched */
            current->fetch_num_insn = 0;
            /* number of total instructions fetched */
            current->fetch_total_insn = 0;
            /* total number of memory references committed */
            current->sim_num_refs = 0;
            /* total number of memory references executed */
            current->sim_total_refs = 0;
            /* total number of loads committed */
            current->sim_num_loads = 0;
            /* total number of loads executed */
            current->sim_total_loads = 0;
            /* total number of branches committed */
            current->sim_num_branches = 0;
            /* total number of branches executed */
            current->sim_total_branches = 0;

            /*
             * simulator state variables
             */
            /* instruction sequence counter, used to assign unique id's to insts */
            current->inst_seq = 0;
            /* pipetrace instruction sequence counter */
            current->ptrace_seq = 0;
            /* speculation mode, non-zero when mis-speculating, i.e., executing
               instructions down the wrong path, thus state recovery will eventually have
               to occur that resets processor register and memory state back to the last
               precise state */
            current->spec_mode = FALSE;
            /* cycles until fetch issue resumes */
            current->ruu_fetch_issue_delay = 0;
            current->wait_for_fetch = 0;

#ifdef CACHE_MISS_STAT
            current->spec_rdb_miss_count = 0;
            current->spec_wrb_miss_count = 0;
            current->non_spec_rdb_miss_count = 0;
            current->non_spec_wrb_miss_count = 0;
            current->inst_miss_count = 0;
            current->load_miss_count = 0;
            current->store_miss_count = 0;
#endif

            current->fetch_num_thrd = 0;	/* num of instructions currently in fetch queue */
            current->iissueq_thrd = 0;	/* num of instructions currently in integer queue */
            current->fissueq_thrd = 0;	/* num of instructions currently in floating queue */
            current->icount_thrd = 0;	/* num of instructions currently in decode, rename issue queues */

            /* the last operation that ruu_dispatch() attempted to dispatch, for
               implementing in-order issue */
            current->last_op.next = NULL;
            current->last_op.rs = NULL;
            current->last_op.tag = 0;
            current->fetch_redirected = FALSE;
            current->bucket_free_list = NULL;

            fprintf(stderr, "thread %d %s\n", *curcontext, current->argv[0]);
            locargc = 0;
            current->running = 1;
            current->finish_cycle = 0;
	    current->parent = -1;
	    current->num_child = 0;
	    
	    current->isFFdone = 0;

#ifdef 	COLLECT_STAT_STARTUP
            current->fastfwd_done = 0;
            current->barrierReached = 0;
            current->startReached = 0;
#endif
#ifdef	EDA
            current->predQueue = NULL;
            current->lockStatus = 0;
            current->boqIndex = -1;
#endif
	    
            (*curcontext)++;   // REPLACING THE ABOVE STATEMENT
            return 1;
        }
        else if (current->argv[locargc][0] == '<') 
        {
            if(!current->argc)
            current->argc = locargc;
            fscanf(argfile, "%s", current->sim_progin);
            current->sim_inputfd = open(current->sim_progin,O_RDONLY,0666);
            if (current->sim_inputfd < 0 ) 
            {
                fatal("thread %d, unable to redirect program input to file `%s'", current->id, current->sim_progin);
	        exit(1);
            }
        }

        else if (current->argv[locargc][0] == '>') 
        {
            if(!current->argc)
                current->argc = locargc;
            fscanf(argfile, "%s", current->sim_progout);
#ifdef PROG_OUT_FROM_SIM_OUT
            current->sim_progout = get_prog_out_file(current->sim_progout);
#endif
            current->sim_progfd = fopen(current->sim_progout, "w");
            if (!current->sim_progfd)
            {
                fatal("thread %d, unable to redirect program output to file `%s'", current->id, current->sim_progout);
	        exit(1);
            }
        }
        else {
            locargc++;
        }
    }
}
#endif
