/*------------------------------------------------------------
 *  Copyright 1994 Digital Equipment Corporation and Steve Wilton
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein,
 * and hereby grant back to Digital a non-exclusive, unrestricted, royalty-
 * free right and license under any changes, enhancements or extensions
 * made to the core functions of the software, including but not limited to
 * those affording compatibility with other hardware or software
 * environments, but excluding applications which incorporate this software.
 * Users further agree to use their best efforts to return to Digital any
 * such changes, enhancements or extensions that they make and inform Digital
 * of noteworthy uses of this software.  Correspondence should be provided
 * to Digital at:
 *
 *                       Director of Licensing
 *                       Western Research Laboratory
 *                       Digital Equipment Corporation
 *                       100 Hamilton Avenue
 *                       Palo Alto, California  94301
 *
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------*/






/*  The following are things you might want to change
 *  when compiling
 */

/*
 * The output can be in 'long' format, which shows everything, or
 * 'short' format, which is just what a program like 'grap' would
 * want to see
 */

#define LONG 1
#define SHORT 2

#define OUTPUTTYPE LONG

/*
 * Address bits in a word, and number of output bits from the cache
 */

#define ADDRESS_BITS 32
#define BITOUT 64


/* limits on the various N parameters */

#define MAXN 8            /* Maximum for Ndwl,Ntwl,Ndbl,Ntbl */
#define MAXSUBARRAYS 8    /* Maximum subarrays for data and tag arrays */
#define MAXSPD 8          /* Maximum for Nspd, Ntspd */



/*
 * The following scale factor can be used to scale between technologies.
 * To convert from 0.8um to 0.5um, make FUDGEFACTOR = 1.6
 */

#define FUDGEFACTOR 4.4


/*===================================================================*/

/*
 * Cache layout parameters and process parameters
 */


/*
 * CMOS 0.8um model parameters
 *   - directly from Appendix II of tech report
 */


#define Cwordmetal    1.8e-15   /* corresponds to 8um of m3 @ 225ff/um */
#define Cbitmetal     4.4e-15   /* corresponds to 16um of m2 @ 275ff/um */
#define Cndiffarea    0.137e-15	/* fF/um2 at 1.5V */
#define Cpdiffarea    0.343e-15	/* fF/um2 at 1.5V */
#define Cndiffside    0.275e-15	/* fF/um at 1.5V */
#define Cpdiffside    0.275e-15	/* fF/um at 1.5V */
#define Cndiffovlp    0.138e-15	/* fF/um at 1.5V */
#define Cpdiffovlp    0.138e-15	/* fF/um at 1.5V */
#define Cnoxideovlp   0.263e-15	/* fF/um assuming 25% Miller effect */
#define Cpoxideovlp   0.338e-15	/* fF/um assuming 25% Miller effect */
#define Leff          0.8	/* um */
#define Cgate         (1.95e-15)	/* fF/um2 */
#define Cgatepass     1.45e-15  /* fF/um2 */
/* note that the value of Cgatepass will be different depending on
   whether or not the source and drain are at different potentials or
   the same potential.  The two values were averaged */
#define Cpolywire     0.25e-15  /* fF/um */
#define Rnchannelstatic 25800   /* ohms*um of channel width */
#define Rpchannelstatic 61200   /* ohms*um of channel width */
#define Rnchannelon 9723
#define Rpchannelon 22400
#define Rbitmetal     0.320     /* corresponds to 16um of m2 @ 48mO/sq */
#define Rwordmetal    0.080     /* corresponds to  8um of m3 @ 24mO/sq */
#define Vdd           5

/* Threshold voltages (as a proportion of Vdd)
   If you don't know them, set all values to 0.5 */

#define VSINV         0.456
#define VTHINV100x60  0.438   /* inverter with p=100,n=60 */
#define VTHNAND60x90  0.561   /* nand with p=60 and three n=90 */
#define VTHNOR12x4x1  0.503   /* nor with p=12, n=4, 1 input */
#define VTHNOR12x4x2  0.452   /* nor with p=12, n=4, 2 inputs */
#define VTHNOR12x4x3  0.417   /* nor with p=12, n=4, 3 inputs */
#define VTHNOR12x4x4  0.390   /* nor with p=12, n=4, 4 inputs */
#define VTHOUTDRINV    0.437
#define VTHOUTDRNOR   0.431
#define VTHOUTDRNAND  0.441
#define VTHOUTDRIVE   0.425
#define VTHCOMPINV    0.437
#define VTHMUXDRV1    0.437
#define VTHMUXDRV2    0.486
#define VTHMUXDRV3    0.437
#define VTHEVALINV    0.267

/* transistor widths in um (as described in tech report, appendix 1) */

#define Wdecdrivep   100.0
#define Wdecdriven    60.0
#define Wdec3to8n     90.0
#define Wdec3to8p     60.0
#define WdecNORn       2.4
#define WdecNORp      12.0
#define Wdecinvn      5.0
#define Wdecinvp      10.0
#define Wworddrivemax 100.0
#define Wmemcella      2.4
#define Wmemcellbscale 2  /* means 2x bigger than Wmemcella */
#define Wbitpreequ    80.0
#define Wbitmuxn      10.0
#define WsenseQ1to4    4.0
#define Wcompinvp1    10.0
#define Wcompinvn1     6.0
#define Wcompinvp2    20.0
#define Wcompinvn2    12.0
#define Wcompinvp3    40.0
#define Wcompinvn3    24.0
#define Wevalinvp     20.0
#define Wevalinvn     80.0

#define Wcompn        10.0
#define Wcompp        30.0
#define Wmuxdrv12n    30.0
#define Wmuxdrv12p    50.0
#define WmuxdrvNORn   20.0
#define WmuxdrvNORp   80.0
#define Wmuxdrv3n    120.0
#define Wmuxdrv3p    200.0
#define Woutdrvseln   12.0
#define Woutdrvselp   20.0
#define Woutdrvnandn  24.0
#define Woutdrvnandp  10.0
#define Woutdrvnorn    6.0
#define Woutdrvnorp   40.0
#define Woutdrivern   48.0
#define Woutdriverp   80.0

/* other stuff (from tech report, appendix 1) */

#define krise         0.4e-9
#define tsensedata    5.8e-10
#define tsensetag     2.6e-10
#define tfalldata     7e-10
#define tfalltag      7e-10
#define Vbitpre       3.3
#define Vt            1.09
#define Vbitsense     0.10
#define BitWidth      8.0	/* bit width of RAM cell in um */
#define BitHeight     16.0	/* bit height of RAM cell in um */
#define Cout          0.5e-12

/*===================================================================*/

/*
 * The following are things you probably wouldn't want to change.
 */


#define TRUE 1
#define FALSE 0
#define NULL 0
#define OK 1
#define ERROR 0
#define BIGNUM 1e30
#define DIVIDE(a,b) ((b)==0)? 0:(a)/(b)
#define MAX(a,b) (((a)>(b))?(a):(b))

/* Used to communicate with the horowitz model */

#define RISE 1
#define FALL 0
#define NCH  1
#define PCH  0


/* Used to pass values around the program */

typedef struct
{
    int cache_size;
    int number_of_sets;
    int associativity;
    int block_size;
} parameter_type;

typedef struct
{
    double access_time,cycle_time;
    int best_Ndwl,best_Ndbl;
    int best_Nspd;
    int best_Ntwl,best_Ntbl;
    int best_Ntspd;
    double decoder_delay_data,decoder_delay_tag;
    double dec_data_driver,dec_data_3to8,dec_data_inv;
    double dec_tag_driver,dec_tag_3to8,dec_tag_inv;
    double wordline_delay_data,wordline_delay_tag;
    double bitline_delay_data,bitline_delay_tag;
    double sense_amp_delay_data,sense_amp_delay_tag;
    double compare_part_delay;
    double drive_mux_delay;
    double selb_delay;
    double data_output_delay;
    double drive_valid_delay;
    double precharge_delay;

} result_type;
