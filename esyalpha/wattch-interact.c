#include <math.h>
#include <stdio.h>
#include "power.h"


/* this is all needed to satisfy externs from power.c, but is not used in
   interactive mode
 */
int ruu_decode_width;
int ruu_issue_width;
int ruu_commit_width;
int RUU_size;
int LSQ_size;
int data_width;
int res_ialu;
int res_fpalu;
int res_memport;
int verbose;
int rename_access, window_access, bpred_access, lsq_access, regfile_access;
int icache_access, dcache_access, dcache2_access, alu_access, resultbus_access;
int window_preg_access, window_selection_access;
int lsq_wakeup_access, lsq_preg_access, ialu_access, falu_access;
int window_wakeup_access, lsq_store_data_access, lsq_load_data_access;
int window_total_pop_count_cycle, window_num_pop_count_cycle;
int lsq_total_pop_count_cycle, lsq_num_pop_count_cycle;
int regfile_total_pop_count_cycle, regfile_num_pop_count_cycle;
int resultbus_num_pop_count_cycle, resultbus_total_pop_count_cycle;

int ras_size;

struct cache_t *cache_dl1;
struct cache_t *cache_il1;
struct cache_t *cache_dl2;
struct cache_t *dtlb;
struct cache_t *itlb;

int btb_config[2] =
  { /* nsets */512, /* assoc */4 };
int twolev_config[4] =
  { /* l1size */1, /* l2size */1024, /* hist */8, /* xor */FALSE};
int bimod_config[1] =
  { /* bimod tbl size */2048 };
int comb_config[1] =
  { /* meta_table_size */1024 };



int num_regs;

double simple_array_decoder_power();
double simple_array_wordline_power();
double simple_array_bitline_power();

double array_decoder_power();
double array_wordline_power();
double array_bitline_power();

double cam_array();
double cam_tagdrive();
double cam_tagmatch();

double senseamp_power();

void calculate_time(time_result_type *result,time_parameter_type *parameters);

void output_data(time_result_type *result,time_parameter_type *parameters);


main()
{
  double predeclength, wordlinelength, bitlinelength;

  double regfile_power, regfile_decoder, regfile_wordline, regfile_wordline16, regfile_wordline33, regfile_bitline;

  int scale_factor;
  int data_width;
  int rports, wports;

  int switch_arg;

  printf("1. Simple Register File\n");
  printf("2. Simple Cache Structure\n");
  printf("3. Simple CAM Structure\n");
  printf("4. Complex Cache (Auto-Sized)\n");

  scanf("%d",&switch_arg);

  printf("note these are MAX powers (assuming full switching)\n");

  switch(switch_arg){
    case 1:
      {
	printf("Enter Reg File Params:\n");
	printf("Number of Registers: ");
	scanf("%d",&num_regs);
	printf("Data Width: ");
	scanf("%d",&data_width);
	printf("Number of Read Ports: ");
	scanf("%d",&rports);
	printf("Number of Write Ports: ");
	scanf("%d",&wports);
	
	printf("%d-entryx%d-width,%d-rdport,%d-wrport: %f (W)\n",num_regs,data_width,rports,wports,simple_array_power(num_regs,data_width,rports,wports,0));
	printf(" decode_power (W): %f\n",simple_array_decoder_power(num_regs,data_width,rports,wports,0));
	printf(" wordline_power (W): %f\n",simple_array_wordline_power(num_regs,data_width,rports,wports,0));
	printf(" bitline_power (W): %f\n",simple_array_bitline_power(num_regs,data_width,rports,wports,0));
	break;
      }

    case 2:
      {
	printf("Enter Cache Params:\n");
	printf("Size of cache: ");
	scanf("%d",&num_regs);
	printf("Data Width: ");
	scanf("%d",&data_width);
	printf("Number of Read Ports: ");
	scanf("%d",&rports);
	printf("Number of Write Ports: ");
	scanf("%d",&wports);
	
	printf("%d-entryx%d-width,%d-rdport,%d-wrport: %f (W)\n",num_regs,data_width,rports,wports,simple_array_power(num_regs,data_width,rports,wports,1));
	printf(" decode_power (W): %f\n",simple_array_decoder_power(num_regs,data_width,rports,wports,1));
	printf(" wordline_power (W): %f\n",simple_array_wordline_power(num_regs,data_width,rports,wports,1));
	printf(" bitline_power (W): %f\n",simple_array_bitline_power(num_regs,data_width,rports,wports,1));
	break;
      }

    case 3:
      {
	printf("Enter CAM Params:\n");
	printf("Entries in CAM: ");
	scanf("%d",&num_regs);
	printf("Tag Width: ");
	scanf("%d",&data_width);
	printf("Number of Read Ports: ");
	scanf("%d",&rports);
	printf("Number of Write Ports: ");
	scanf("%d",&wports);
	
	printf("%d-entryx%d-tagwidth,%d-rdport,%d-wrport: %f (W)\n",num_regs,data_width,rports,wports,cam_array(num_regs,data_width,rports,wports));
	printf(" tagdrive_power (W): %f\n",cam_tagdrive(num_regs,data_width,rports,wports));
	printf(" tagmatch_power (W): %f\n",cam_tagmatch(num_regs,data_width,rports,wports));
	break;
      }

    case 4:
      {
	int nsets, bsize, assoc,res_memport, tagsize;
	int ndwl, ndbl, nspd, ntwl, ntbl, ntspd, c,b,a,cache, rowsb, colsb, trowsb, tcolsb;
	double cache_decoder, cache_wordline, cache_bitline, 
	  cache_senseamp, cache_tagarray, total_cache_power;
	time_result_type time_result;
	time_parameter_type time_parameters;
	int va_size = 48;

	printf("Enter Cache Params:\n");
	printf("Number of Sets in cache: ");
	scanf("%d",&nsets);
	printf("Block Size (bytes): ");
	scanf("%d",&bsize);
	printf("Associativity: ");
	scanf("%d",&assoc);
	printf("Number of Memory Ports: ");
	scanf("%d",&res_memport);

	printf("note tagarray size is estimated based on assuming 48-bit virtual addresses\n");

	cache = 1;

	time_parameters.cache_size = nsets * bsize * assoc; /* C */
	time_parameters.block_size = bsize; /* B */
	time_parameters.associativity = assoc; /* A */
	time_parameters.number_of_sets = nsets; /* C/(B*A) */

	calculate_time(&time_result,&time_parameters);
	output_data(&time_result,&time_parameters);

	ndwl=time_result.best_Ndwl;
	ndbl=time_result.best_Ndbl;
	nspd=time_result.best_Nspd;
	ntwl=time_result.best_Ntwl;
	ntbl=time_result.best_Ntbl;
	ntspd=time_result.best_Ntspd;
	c = time_parameters.cache_size;
	b = time_parameters.block_size;
	a = time_parameters.associativity; 

	rowsb = c/(8*b*a*ndbl*nspd);
	colsb = 8*b*a*nspd/ndwl;

	tagsize = va_size - ((int)logtwo(nsets) + (int)logtwo(bsize));
	trowsb = c/(8*b*a*ntbl*ntspd);
	tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;

	predeclength = rowsb * (RegCellHeight + WordlineSpacing);
	wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
	bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

	cache_decoder = res_memport*ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,cache);
	cache_wordline = res_memport*ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,cache);
	cache_bitline = res_memport*ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,cache);
	cache_senseamp = res_memport*ndwl*ndbl*senseamp_power(colsb);
	cache_tagarray = res_memport*ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,cache));
	
	total_cache_power = cache_decoder + cache_wordline + cache_bitline + cache_senseamp + cache_tagarray;

	fprintf(stderr,"%d KB %d-way cache (%d-byte block size):\n",c,a,b);
	fprintf(stderr,"ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
	fprintf(stderr,"%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
	fprintf(stderr,"tagsize == %d\n",tagsize);
	fprintf(stderr,"\nntwl == %d, ntbl == %d, ntspd == %d\n",ntwl,ntbl,ntspd);
	fprintf(stderr,"%d sets of %d rows x %d cols\n",ntwl*ntbl,trowsb,tcolsb);
	
	printf("Total Power (W): %f\n",total_cache_power);
	printf(" decode_power (W): %f\n",cache_decoder);
	printf(" wordline_power (W): %f\n",cache_wordline);
	printf(" bitline_power (W): %f\n",cache_bitline);
	printf(" senseamp_power (W): %f\n",cache_senseamp);
	printf(" tagarray_power (W): %f\n",cache_tagarray);
	break;
      }
 default:

  }


}
