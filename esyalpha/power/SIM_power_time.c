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

#include <math.h>
#include <assert.h>
#include "SIM_power.h"
#include "SIM_power_array_internal.h"


static double logtwo(double x)
{
    assert(x > 0);
    return log10(x)/log10(2);
}


/*----------------------------------------------------------------------*/

double SIM_power_gatecap(width,wirelength) /* returns gate capacitance in Farads */
double width;		/* gate width in um (length is Leff) */
double wirelength;	/* poly wire length going to gate in lambda */
{

    double overlapCap;
    double gateCap;
    double l = 0.1525;

#if defined(Pdelta_w)
    overlapCap = (width - 2*Pdelta_w) * PCov;
    gateCap  = ((width - 2*Pdelta_w) * (l * LSCALE - 2*Pdelta_l) *
                PCg) + 2.0 * overlapCap;

    return gateCap;
#endif
    return(width*Leff*PARM(Cgate)+wirelength*Cpolywire*Leff);
    /* return(width*Leff*PARM(Cgate)); */
    /* return(width*CgateLeff+wirelength*Cpolywire*Leff);*/
}


double SIM_power_gatecappass(width,wirelength) /* returns gate capacitance in Farads */
double width;           /* gate width in um (length is Leff) */
double wirelength;      /* poly wire length going to gate in lambda */
{
    return(SIM_power_gatecap(width,wirelength));
    /* return(width*Leff*PARM(Cgatepass)+wirelength*Cpolywire*Leff); */
}


/*----------------------------------------------------------------------*/

/* Routine for calculating drain capacitances.  The draincap routine
 * folds transistors larger than 10um */
double SIM_power_draincap(width,nchannel,stack)  /* returns drain cap in Farads */
double width;		/* um */
int nchannel;		/* whether n or p-channel (boolean) */
int stack;		/* number of transistors in series that are on */
{
    double Cdiffside,Cdiffarea,Coverlap,cap;

    double overlapCap;
    double swAreaUnderGate;
    double area_peri;
    double diffArea;
    double diffPeri;
    double l = 0.4 * LSCALE;


    diffArea = l * width;
    diffPeri = 2 * l + 2 * width;

#if defined(Pdelta_w)
    if(nchannel == 0)
    {
        overlapCap = (width - 2 * Pdelta_w) * PCov;
        swAreaUnderGate = (width - 2 * Pdelta_w) * PCjswA;
        area_peri = ((diffArea * PCja)
                     +  (diffPeri * PCjsw));

        return(stack*(area_peri + overlapCap + swAreaUnderGate));
    }
    else
    {
        overlapCap = (width - 2 * Ndelta_w) * NCov;
        swAreaUnderGate = (width - 2 * Ndelta_w) * NCjswA;
        area_peri = ((diffArea * NCja * LSCALE)
                     +  (diffPeri * NCjsw * LSCALE));

        return(stack*(area_peri + overlapCap + swAreaUnderGate));
    }
#endif

    Cdiffside = (nchannel) ? PARM(Cndiffside) : PARM(Cpdiffside);
    Cdiffarea = (nchannel) ? PARM(Cndiffarea) : PARM(Cpdiffarea);
    Coverlap = (nchannel) ? (PARM(Cndiffovlp)+PARM(Cnoxideovlp)) :
               (PARM(Cpdiffovlp)+PARM(Cpoxideovlp));
    /* calculate directly-connected (non-stacked) capacitance */
    /* then add in capacitance due to stacking */
    if (width >= 10)
    {
        cap = 3.0*Leff*width/2.0*Cdiffarea + 6.0*Leff*Cdiffside +
              width*Coverlap;
        cap += (double)(stack-1)*(Leff*width*Cdiffarea +
                                  4.0*Leff*Cdiffside + 2.0*width*Coverlap);
    }
    else
    {
        cap = 3.0*Leff*width*Cdiffarea + (6.0*Leff+width)*Cdiffside +
              width*Coverlap;
        cap += (double)(stack-1)*(Leff*width*Cdiffarea +
                                  2.0*Leff*Cdiffside + 2.0*width*Coverlap);
    }
    return(cap);
}


/*----------------------------------------------------------------------*/

/* The following routines estimate the effective resistance of an
   on transistor as described in the tech report.  The first routine
   gives the "switching" resistance, and the second gives the
   "full-on" resistance */
double SIM_power_transresswitch(width,nchannel,stack)  /* returns resistance in ohms */
double width;		/* um */
int nchannel;		/* whether n or p-channel (boolean) */
int stack;		/* number of transistors in series */
{
    double restrans;
    restrans = (nchannel) ? (Rnchannelstatic):
               (Rpchannelstatic);
    /* calculate resistance of stack - assume all but switching trans
       have 0.8X the resistance since they are on throughout switching */
    return((1.0+((stack-1.0)*0.8))*restrans/width);
}


/*----------------------------------------------------------------------*/

double SIM_power_transreson(width,nchannel,stack)  /* returns resistance in ohms */
double width;           /* um */
int nchannel;           /* whether n or p-channel (boolean) */
int stack;              /* number of transistors in series */
{
    double restrans;
    restrans = (nchannel) ? Rnchannelon : Rpchannelon;

    /* calculate resistance of stack.  Unlike transres, we don't
         multiply the stacked transistors by 0.8 */
    return(stack*restrans/width);
}


/*----------------------------------------------------------------------*/

/* This routine operates in reverse: given a resistance, it finds
 * the transistor width that would have this R.  It is used in the
 * data wordline to estimate the wordline driver size. */
double SIM_power_restowidth(res,nchannel)  /* returns width in um */
double res;            /* resistance in ohms */
int nchannel;          /* whether N-channel or P-channel */
{
    double restrans;

    restrans = (nchannel) ? Rnchannelon : Rpchannelon;

    return(restrans/res);
}


/*----------------------------------------------------------------------*/

double SIM_power_horowitz(inputramptime,tf,vs1,vs2,rise)
double inputramptime,    /* input rise time */
tf,               /* time constant of gate */
vs1,vs2;          /* threshold voltages */
int rise;                /* whether INPUT rise or fall (boolean) */
{
    double a,b,td;

    a = inputramptime/tf;
    if (rise==RISE)
    {
        b = 0.5;
        td = tf*sqrt(fabs( log(vs1)*log(vs1)+2*a*b*(1.0-vs1))) +
             tf*(log(vs1)-log(vs2));
    }
    else
    {
        b = 0.4;
        td = tf*sqrt(fabs( log(1.0-vs1)*log(1.0-vs1)+2*a*b*(vs1))) +
             tf*(log(1.0-vs1)-log(1.0-vs2));
    }

    return(td);
}



/*======================================================================*/



/*
 * This part of the code contains routines for each section as
 * described in the tech report.  See the tech report for more details
 * and explanations */

/*----------------------------------------------------------------------*/

/* Decoder delay:  (see section 6.1 of tech report) */
double SIM_power_decoder_delay(C, B, A, Ndwl, Ndbl, Nspd, Ntwl, Ntbl, Ntspd, Tdecdrive,
                               Tdecoder1, Tdecoder2,outrisetime)
int C,B,A,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd;
double *Tdecdrive,*Tdecoder1,*Tdecoder2,*outrisetime;
{
    double Ceq,Req,Rwire,rows,tf,nextinputtime,vth,tstep,m,a,b,c;
    int numstack;
    double z = 0.0;

    /* Calculate rise time.  Consider two inverters */

    Ceq = SIM_power_draincap(Wdecdrivep,PCH,1)+SIM_power_draincap(Wdecdriven,NCH,1) +
          SIM_power_gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*SIM_power_transreson(Wdecdriven,NCH,1);
    nextinputtime = SIM_power_horowitz(z,tf,PARM(VTHINV100x60),PARM(VTHINV100x60),FALL)/
                    (PARM(VTHINV100x60));

    Ceq = SIM_power_draincap(Wdecdrivep,PCH,1)+SIM_power_draincap(Wdecdriven,NCH,1) +
          SIM_power_gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*SIM_power_transreson(Wdecdriven,NCH,1);
    nextinputtime = SIM_power_horowitz(nextinputtime,tf,PARM(VTHINV100x60),PARM(VTHINV100x60),
                                       RISE)/
                    (1.0-PARM(VTHINV100x60));

    /* First stage: driving the decoders */

    rows = C/(8*B*A*Ndbl*Nspd);
    Ceq = SIM_power_draincap(Wdecdrivep,PCH,1)+SIM_power_draincap(Wdecdriven,NCH,1) +
          4*SIM_power_gatecap(Wdec3to8n+Wdec3to8p,10.0)*(Ndwl*Ndbl)+
          Cwordmetal*0.25*8*B*A*Ndbl*Nspd;
    Rwire = Rwordmetal*0.125*8*B*A*Ndbl*Nspd;
    tf = (Rwire + SIM_power_transreson(Wdecdrivep,PCH,1))*Ceq;
    *Tdecdrive = SIM_power_horowitz(nextinputtime,tf,PARM(VTHINV100x60),PARM(VTHNAND60x90),
                                    FALL);
    nextinputtime = *Tdecdrive/PARM(VTHNAND60x90);

    /* second stage: driving a bunch of nor gates with a nand */

    numstack =
        ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));
    if (numstack==0) numstack = 1;
    if (numstack>5) numstack = 5;
    Ceq = 3*SIM_power_draincap(Wdec3to8p,PCH,1) +SIM_power_draincap(Wdec3to8n,NCH,3) +
          SIM_power_gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0))*rows +
          Cbitmetal*rows*8;

    Rwire = Rbitmetal*rows*8/2;
    tf = Ceq*(Rwire+SIM_power_transreson(Wdec3to8n,NCH,3));

    /* we only want to charge the output to the threshold of the
       nor gate.  But the threshold depends on the number of inputs
       to the nor.  */

    switch(numstack)
    {
    case 1:
        vth = PARM(VTHNOR12x4x1);
        break;
    case 2:
        vth = PARM(VTHNOR12x4x2);
        break;
    case 3:
        vth = PARM(VTHNOR12x4x3);
        break;
    case 4:
        vth = PARM(VTHNOR12x4x4);
        break;
    case 5:
        vth = PARM(VTHNOR12x4x4);
        break;
    default:
        printf("error:numstack=%d\n",numstack);
    }
    *Tdecoder1 = SIM_power_horowitz(nextinputtime,tf,PARM(VTHNAND60x90),vth,RISE);
    nextinputtime = *Tdecoder1/(1.0-vth);

    /* Final stage: driving an inverter with the nor */

    Req = SIM_power_transreson(WdecNORp,PCH,numstack);
    Ceq = (SIM_power_gatecap(Wdecinvn+Wdecinvp,20.0)+
           numstack*SIM_power_draincap(WdecNORn,NCH,1)+
           SIM_power_draincap(WdecNORp,PCH,numstack));
    tf = Req*Ceq;
    *Tdecoder2 = SIM_power_horowitz(nextinputtime,tf,vth,PARM(VSINV),FALL);
    *outrisetime = *Tdecoder2/(PARM(VSINV));
    return(*Tdecdrive+*Tdecoder1+*Tdecoder2);
}


/*----------------------------------------------------------------------*/

/* Decoder delay in the tag array (see section 6.1 of tech report) */
double SIM_power_decoder_tag_delay(C, B, A, Ndwl, Ndbl, Nspd, Ntwl, Ntbl, Ntspd,
                                   Tdecdrive, Tdecoder1, Tdecoder2,outrisetime)
int C,B,A,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd;
double *Tdecdrive,*Tdecoder1,*Tdecoder2,*outrisetime;
{
    double Ceq,Req,Rwire,rows,tf,nextinputtime,vth,tstep,m,a,b,c;
    int numstack;


    /* Calculate rise time.  Consider two inverters */

    Ceq = SIM_power_draincap(Wdecdrivep,PCH,1)+SIM_power_draincap(Wdecdriven,NCH,1) +
          SIM_power_gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*SIM_power_transreson(Wdecdriven,NCH,1);
    nextinputtime = SIM_power_horowitz(0.0,tf,PARM(VTHINV100x60),PARM(VTHINV100x60),FALL)/
                    (PARM(VTHINV100x60));

    Ceq = SIM_power_draincap(Wdecdrivep,PCH,1)+SIM_power_draincap(Wdecdriven,NCH,1) +
          SIM_power_gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*SIM_power_transreson(Wdecdriven,NCH,1);
    nextinputtime = SIM_power_horowitz(nextinputtime,tf,PARM(VTHINV100x60),PARM(VTHINV100x60),
                                       RISE)/
                    (1.0-PARM(VTHINV100x60));

    /* First stage: driving the decoders */

    rows = C/(8*B*A*Ntbl*Ntspd);
    Ceq = SIM_power_draincap(Wdecdrivep,PCH,1)+SIM_power_draincap(Wdecdriven,NCH,1) +
          4*SIM_power_gatecap(Wdec3to8n+Wdec3to8p,10.0)*(Ntwl*Ntbl)+
          Cwordmetal*0.25*8*B*A*Ntbl*Ntspd;
    Rwire = Rwordmetal*0.125*8*B*A*Ntbl*Ntspd;
    tf = (Rwire + SIM_power_transreson(Wdecdrivep,PCH,1))*Ceq;
    *Tdecdrive = SIM_power_horowitz(nextinputtime,tf,PARM(VTHINV100x60),PARM(VTHNAND60x90),
                                    FALL);
    nextinputtime = *Tdecdrive/PARM(VTHNAND60x90);

    /* second stage: driving a bunch of nor gates with a nand */

    numstack =
        ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));
    if (numstack==0) numstack = 1;
    if (numstack>5) numstack = 5;

    Ceq = 3*SIM_power_draincap(Wdec3to8p,PCH,1) +SIM_power_draincap(Wdec3to8n,NCH,3) +
          SIM_power_gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0))*rows +
          Cbitmetal*rows*8;

    Rwire = Rbitmetal*rows*8/2;
    tf = Ceq*(Rwire+SIM_power_transreson(Wdec3to8n,NCH,3));

    /* we only want to charge the output to the threshold of the
       nor gate.  But the threshold depends on the number of inputs
       to the nor.  */

    switch(numstack)
    {
    case 1:
        vth = PARM(VTHNOR12x4x1);
        break;
    case 2:
        vth = PARM(VTHNOR12x4x2);
        break;
    case 3:
        vth = PARM(VTHNOR12x4x3);
        break;
    case 4:
        vth = PARM(VTHNOR12x4x4);
        break;
    case 5:
        vth = PARM(VTHNOR12x4x4);
        break;
    case 6:
        vth = PARM(VTHNOR12x4x4);
        break;
    default:
        printf("error:numstack=%d\n",numstack);
    }
    *Tdecoder1 = SIM_power_horowitz(nextinputtime,tf,PARM(VTHNAND60x90),vth,RISE);
    nextinputtime = *Tdecoder1/(1.0-vth);

    /* Final stage: driving an inverter with the nor */

    Req = SIM_power_transreson(WdecNORp,PCH,numstack);
    Ceq = (SIM_power_gatecap(Wdecinvn+Wdecinvp,20.0)+
           numstack*SIM_power_draincap(WdecNORn,NCH,1)+
           SIM_power_draincap(WdecNORp,PCH,numstack));
    tf = Req*Ceq;
    *Tdecoder2 = SIM_power_horowitz(nextinputtime,tf,vth,PARM(VSINV),FALL);
    *outrisetime = *Tdecoder2/(PARM(VSINV));
    return(*Tdecdrive+*Tdecoder1+*Tdecoder2);
}


/*----------------------------------------------------------------------*/

/* Data array wordline delay (see section 6.2 of tech report) */
double SIM_power_wordline_delay(B,A,Ndwl,Nspd,inrisetime,outrisetime)
int B,A,Ndwl,Nspd;
double inrisetime,*outrisetime;
{
    double Rpdrive,nextrisetime;
    double desiredrisetime,psize,nsize;
    double tf,nextinputtime,Ceq,Req,Rline,Cline;
    int cols;
    double Tworddrivedel,Twordchargedel;

    cols = 8*B*A*Nspd/Ndwl;

    /* Choose a transistor size that makes sense */
    /* Use a first-order approx */

    desiredrisetime = krise*log((double)(cols))/2.0;
    Cline = (SIM_power_gatecappass(Wmemcella,0.0)+
             SIM_power_gatecappass(Wmemcella,0.0)+
             Cwordmetal)*cols;
    Rpdrive = desiredrisetime/(Cline*log(PARM(VSINV))*-1.0);
    psize = SIM_power_restowidth(Rpdrive,PCH);
    if (psize > Wworddrivemax)
    {
        psize = Wworddrivemax;
    }

    /* Now that we have a reasonable psize, do the rest as before */
    /* If we keep the ratio the same as the tag wordline driver,
       the threshold voltage will be close to VSINV */

    nsize = psize * Wdecinvn/Wdecinvp;

    Ceq = SIM_power_draincap(Wdecinvn,NCH,1) + SIM_power_draincap(Wdecinvp,PCH,1) +
          SIM_power_gatecap(nsize+psize,20.0);
    tf = SIM_power_transreson(Wdecinvn,NCH,1)*Ceq;

    Tworddrivedel = SIM_power_horowitz(inrisetime,tf,PARM(VSINV),PARM(VSINV),RISE);
    nextinputtime = Tworddrivedel/(1.0-PARM(VSINV));

    Cline = (SIM_power_gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             SIM_power_gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             Cwordmetal)*cols+
            SIM_power_draincap(nsize,NCH,1) + SIM_power_draincap(psize,PCH,1);
    Rline = Rwordmetal*cols/2;
    tf = (SIM_power_transreson(psize,PCH,1)+Rline)*Cline;
    Twordchargedel = SIM_power_horowitz(nextinputtime,tf,PARM(VSINV),PARM(VSINV),FALL);
    *outrisetime = Twordchargedel/PARM(VSINV);

    return(Tworddrivedel+Twordchargedel);
}


/*----------------------------------------------------------------------*/

/* Tag array wordline delay (see section 6.3 of tech report) */
double SIM_power_wordline_tag_delay(C,A,Ntspd,Ntwl,inrisetime,outrisetime)
int C,A,Ntspd,Ntwl;
double *outrisetime;
double inrisetime;
{
    double tf,m,a,b,c;
    double Cline,Rline,Ceq,nextinputtime;
    int tagbits;
    double Tworddrivedel,Twordchargedel;

    /* number of tag bits */

    tagbits = PARM(ADDRESS_BITS)+2-(int)logtwo((double)C)+(int)logtwo((double)A);

    /* first stage */

    Ceq = SIM_power_draincap(Wdecinvn,NCH,1) + SIM_power_draincap(Wdecinvp,PCH,1) +
          SIM_power_gatecap(Wdecinvn+Wdecinvp,20.0);
    tf = SIM_power_transreson(Wdecinvn,NCH,1)*Ceq;

    Tworddrivedel = SIM_power_horowitz(inrisetime,tf,PARM(VSINV),PARM(VSINV),RISE);
    nextinputtime = Tworddrivedel/(1.0-PARM(VSINV));

    /* second stage */
    Cline = (SIM_power_gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             SIM_power_gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             Cwordmetal)*tagbits*A*Ntspd/Ntwl+
            SIM_power_draincap(Wdecinvn,NCH,1) + SIM_power_draincap(Wdecinvp,PCH,1);
    Rline = Rwordmetal*tagbits*A*Ntspd/(2*Ntwl);
    tf = (SIM_power_transreson(Wdecinvp,PCH,1)+Rline)*Cline;
    Twordchargedel = SIM_power_horowitz(nextinputtime,tf,PARM(VSINV),PARM(VSINV),FALL);
    *outrisetime = Twordchargedel/PARM(VSINV);
    return(Tworddrivedel+Twordchargedel);
}


/*----------------------------------------------------------------------*/

/* Data array bitline: (see section 6.4 in tech report) */
double SIM_power_bitline_delay(C,A,B,Ndwl,Ndbl,Nspd,inrisetime,outrisetime)
int C,A,B,Ndbl,Nspd;
double inrisetime,*outrisetime;
{
    double Tbit,Cline,Ccolmux,Rlineb,r1,r2,c1,c2,a,b,c;
    double m,tstep;
    double Cbitrow;    /* bitline capacitance due to access transistor */
    int rows,cols;

    Cbitrow = SIM_power_draincap(Wmemcella,NCH,1)/2.0; /* due to shared contact */
    rows = C/(B*A*Ndbl*Nspd);
    cols = 8*B*A*Nspd/Ndwl;
    if (Ndbl*Nspd == 1)
    {
        Cline = rows*(Cbitrow+Cbitmetal)+2*SIM_power_draincap(Wbitpreequ,PCH,1);
        Ccolmux = 2*SIM_power_gatecap(WsenseQ1to4,10.0);
        Rlineb = Rbitmetal*rows/2.0;
        r1 = Rlineb;
    }
    else
    {
        Cline = rows*(Cbitrow+Cbitmetal) + 2*SIM_power_draincap(Wbitpreequ,PCH,1) +
                SIM_power_draincap(Wbitmuxn,NCH,1);
        Ccolmux = Nspd*Ndbl*(SIM_power_draincap(Wbitmuxn,NCH,1))+2*SIM_power_gatecap(WsenseQ1to4,10.0);
        Rlineb = Rbitmetal*rows/2.0;
        r1 = Rlineb +
             SIM_power_transreson(Wbitmuxn,NCH,1);
    }
    r2 = SIM_power_transreson(Wmemcella,NCH,1) +
         SIM_power_transreson(Wmemcella*Wmemcellbscale,NCH,1);
    c1 = Ccolmux;
    c2 = Cline;


    tstep = (r2*c2+(r1+r2)*c1)*log((Vbitpre)/(Vbitpre-Vbitsense));

    /* take input rise time into account */

    m = Vdd/inrisetime;
    if (tstep <= (0.5*(Vdd-Vt)/m))
    {
        a = m;
        b = 2*((Vdd*0.5)-Vt);
        c = -2*tstep*(Vdd-Vt)+1/m*((Vdd*0.5)-Vt)*
            ((Vdd*0.5)-Vt);
        Tbit = (-b+sqrt(b*b-4*a*c))/(2*a);
    }
    else
    {
        Tbit = tstep + (Vdd+Vt)/(2*m) - (Vdd*0.5)/m;
    }

    *outrisetime = Tbit/(log((Vbitpre-Vbitsense)/Vdd));
    return(Tbit);
}


/*----------------------------------------------------------------------*/

/* Tag array bitline: (see section 6.4 in tech report) */
double SIM_power_bitline_tag_delay(C,A,B,Ntwl,Ntbl,Ntspd,inrisetime,outrisetime)
int C,A,B,Ntbl,Ntspd;
double inrisetime,*outrisetime;
{
    double Tbit,Cline,Ccolmux,Rlineb,r1,r2,c1,c2,a,b,c;
    double m,tstep;
    double Cbitrow;    /* bitline capacitance due to access transistor */
    int rows,cols;

    Cbitrow = SIM_power_draincap(Wmemcella,NCH,1)/2.0; /* due to shared contact */
    rows = C/(B*A*Ntbl*Ntspd);
    cols = 8*B*A*Ntspd/Ntwl;
    if (Ntbl*Ntspd == 1)
    {
        Cline = rows*(Cbitrow+Cbitmetal)+2*SIM_power_draincap(Wbitpreequ,PCH,1);
        Ccolmux = 2*SIM_power_gatecap(WsenseQ1to4,10.0);
        Rlineb = Rbitmetal*rows/2.0;
        r1 = Rlineb;
    }
    else
    {
        Cline = rows*(Cbitrow+Cbitmetal) + 2*SIM_power_draincap(Wbitpreequ,PCH,1) +
                SIM_power_draincap(Wbitmuxn,NCH,1);
        Ccolmux = Ntspd*Ntbl*(SIM_power_draincap(Wbitmuxn,NCH,1))+2*SIM_power_gatecap(WsenseQ1to4,10.0);
        Rlineb = Rbitmetal*rows/2.0;
        r1 = Rlineb +
             SIM_power_transreson(Wbitmuxn,NCH,1);
    }
    r2 = SIM_power_transreson(Wmemcella,NCH,1) +
         SIM_power_transreson(Wmemcella*Wmemcellbscale,NCH,1);

    c1 = Ccolmux;
    c2 = Cline;

    tstep = (r2*c2+(r1+r2)*c1)*log((Vbitpre)/(Vbitpre-Vbitsense));

    /* take into account input rise time */

    m = Vdd/inrisetime;
    if (tstep <= (0.5*(Vdd-Vt)/m))
    {
        a = m;
        b = 2*((Vdd*0.5)-Vt);
        c = -2*tstep*(Vdd-Vt)+1/m*((Vdd*0.5)-Vt)*
            ((Vdd*0.5)-Vt);
        Tbit = (-b+sqrt(b*b-4*a*c))/(2*a);
    }
    else
    {
        Tbit = tstep + (Vdd+Vt)/(2*m) - (Vdd*0.5)/m;
    }

    *outrisetime = Tbit/(log((Vbitpre-Vbitsense)/Vdd));
    return(Tbit);
}


/*----------------------------------------------------------------------*/

/* It is assumed the sense amps have a constant delay
   (see section 6.5) */
double SIM_power_sense_amp_delay(inrisetime,outrisetime)
double inrisetime,*outrisetime;
{
    *outrisetime = tfalldata;
    return(tsensedata);
}


/*--------------------------------------------------------------*/

double SIM_power_sense_amp_tag_delay(inrisetime,outrisetime)
double inrisetime,*outrisetime;
{
    *outrisetime = tfalltag;
    return(tsensetag);
}


/*----------------------------------------------------------------------*/

/* Comparator Delay (see section 6.6) */
double SIM_power_compare_time(C,A,Ntbl,Ntspd,inputtime,outputtime)
int C,A,Ntbl,Ntspd;
double inputtime,*outputtime;
{
    double Req,Ceq,tf,st1del,st2del,st3del,nextinputtime,m;
    double c1,c2,r1,r2,tstep,a,b,c;
    double Tcomparatorni;
    int cols,tagbits;

    /* First Inverter */

    Ceq = SIM_power_gatecap(Wcompinvn2+Wcompinvp2,10.0) +
          SIM_power_draincap(Wcompinvp1,PCH,1) + SIM_power_draincap(Wcompinvn1,NCH,1);
    Req = SIM_power_transreson(Wcompinvp1,PCH,1);
    tf = Req*Ceq;
    st1del = SIM_power_horowitz(inputtime,tf,PARM(VTHCOMPINV),PARM(VTHCOMPINV),FALL);
    nextinputtime = st1del/PARM(VTHCOMPINV);

    /* Second Inverter */

    Ceq = SIM_power_gatecap(Wcompinvn3+Wcompinvp3,10.0) +
          SIM_power_draincap(Wcompinvp2,PCH,1) + SIM_power_draincap(Wcompinvn2,NCH,1);
    Req = SIM_power_transreson(Wcompinvn2,NCH,1);
    tf = Req*Ceq;
    st2del = SIM_power_horowitz(inputtime,tf,PARM(VTHCOMPINV),PARM(VTHCOMPINV),RISE);
    nextinputtime = st1del/(1.0-PARM(VTHCOMPINV));

    /* Third Inverter */

    Ceq = SIM_power_gatecap(Wevalinvn+Wevalinvp,10.0) +
          SIM_power_draincap(Wcompinvp3,PCH,1) + SIM_power_draincap(Wcompinvn3,NCH,1);
    Req = SIM_power_transreson(Wcompinvp3,PCH,1);
    tf = Req*Ceq;
    st3del = SIM_power_horowitz(nextinputtime,tf,PARM(VTHCOMPINV),PARM(VTHEVALINV),FALL);
    nextinputtime = st1del/(PARM(VTHEVALINV));

    /* Final Inverter (virtual ground driver) discharging compare part */

    tagbits = PARM(ADDRESS_BITS) - (int)logtwo((double)C) + (int)logtwo((double)A);
    cols = tagbits*Ntbl*Ntspd;

    r1 = SIM_power_transreson(Wcompn,NCH,2);
    r2 = SIM_power_transresswitch(Wevalinvn,NCH,1);
    c2 = (tagbits)*(SIM_power_draincap(Wcompn,NCH,1)+SIM_power_draincap(Wcompn,NCH,2))+
         SIM_power_draincap(Wevalinvp,PCH,1) + SIM_power_draincap(Wevalinvn,NCH,1);
    c1 = (tagbits)*(SIM_power_draincap(Wcompn,NCH,1)+SIM_power_draincap(Wcompn,NCH,2))
         +SIM_power_draincap(Wcompp,PCH,1) + SIM_power_gatecap(Wmuxdrv12n+Wmuxdrv12p,20.0) +
         cols*Cwordmetal;

    /* time to go to threshold of mux driver */

    tstep = (r2*c2+(r1+r2)*c1)*log(1.0/PARM(VTHMUXDRV1));

    /* take into account non-zero input rise time */

    m = Vdd/nextinputtime;

    if ((tstep) <= (0.5*(Vdd-Vt)/m))
    {
        a = m;
        b = 2*((Vdd*PARM(VTHEVALINV))-Vt);
        c = -2*(tstep)*(Vdd-Vt)+1/m*((Vdd*PARM(VTHEVALINV))-Vt)*((Vdd*PARM(VTHEVALINV))-Vt);
        Tcomparatorni = (-b+sqrt(b*b-4*a*c))/(2*a);
    }
    else
    {
        Tcomparatorni = (tstep) + (Vdd+Vt)/(2*m) - (Vdd*PARM(VTHEVALINV))/m;
    }
    *outputtime = Tcomparatorni/(1.0-PARM(VTHMUXDRV1));

    return(Tcomparatorni+st1del+st2del+st3del);
}


/*----------------------------------------------------------------------*/

/* Delay of the multiplexor Driver (see section 6.7) */
double SIM_power_mux_driver_delay(C,B,A,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,inputtime,outputtime)
int C,B,A,Ndbl,Nspd,Ndwl,Ntbl,Ntspd;
double inputtime,*outputtime;
{
    double Ceq,Req,tf,nextinputtime;
    double Tst1,Tst2,Tst3;

    /* first driver stage - Inverte "match" to produce "matchb" */
    /* the critical path is the DESELECTED case, so consider what
       happens when the address bit is true, but match goes low */

    Ceq = SIM_power_gatecap(WmuxdrvNORn+WmuxdrvNORp,15.0)*(8*B/PARM(BITOUT)) +
          SIM_power_draincap(Wmuxdrv12n,NCH,1) + SIM_power_draincap(Wmuxdrv12p,PCH,1);
    Req = SIM_power_transreson(Wmuxdrv12p,PCH,1);
    tf = Ceq*Req;
    Tst1 = SIM_power_horowitz(inputtime,tf,PARM(VTHMUXDRV1),PARM(VTHMUXDRV2),FALL);
    nextinputtime = Tst1/PARM(VTHMUXDRV2);

    /* second driver stage - NOR "matchb" with address bits to produce sel */

    Ceq = SIM_power_gatecap(Wmuxdrv3n+Wmuxdrv3p,15.0) + 2*SIM_power_draincap(WmuxdrvNORn,NCH,1) +
          SIM_power_draincap(WmuxdrvNORp,PCH,2);
    Req = SIM_power_transreson(WmuxdrvNORn,NCH,1);
    tf = Ceq*Req;
    Tst2 = SIM_power_horowitz(nextinputtime,tf,PARM(VTHMUXDRV2),PARM(VTHMUXDRV3),RISE);
    nextinputtime = Tst2/(1-PARM(VTHMUXDRV3));

    /* third driver stage - invert "select" to produce "select bar" */

    Ceq = PARM(BITOUT)*SIM_power_gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0)+
          SIM_power_draincap(Wmuxdrv3p,PCH,1) + SIM_power_draincap(Wmuxdrv3n,NCH,1) +
          Cwordmetal*8*B*A*Nspd*Ndbl/2.0;
    Req = (Rwordmetal*8*B*A*Nspd*Ndbl/2)/2 + SIM_power_transreson(Wmuxdrv3p,PCH,1);
    tf = Ceq*Req;
    Tst3 = SIM_power_horowitz(nextinputtime,tf,PARM(VTHMUXDRV3),PARM(VTHOUTDRINV),FALL);
    *outputtime = Tst3/(PARM(VTHOUTDRINV));

    return(Tst1 + Tst2 + Tst3);

}


/*----------------------------------------------------------------------*/

/* Valid driver (see section 6.9 of tech report)
   Note that this will only be called for a direct mapped cache */
double SIM_power_valid_driver_delay(C,A,Ntbl,Ntspd,inputtime)
int C,A,Ntbl,Ntspd;
double inputtime;
{
    double Ceq,Tst1,tf;

    Ceq = SIM_power_draincap(Wmuxdrv12n,NCH,1)+SIM_power_draincap(Wmuxdrv12p,PCH,1)+Cout;
    tf = Ceq*SIM_power_transreson(Wmuxdrv12p,PCH,1);
    Tst1 = SIM_power_horowitz(inputtime,tf,PARM(VTHMUXDRV1),0.5,FALL);

    return(Tst1);
}


/*----------------------------------------------------------------------*/

/* Data output delay (data side) -- see section 6.8
   This is the time through the NAND/NOR gate and the final inverter
   assuming sel is already present */
double SIM_power_dataoutput_delay(C,B,A,Ndbl,Nspd,Ndwl,
                                  inrisetime,outrisetime)
int C,B,A,Ndbl,Nspd,Ndwl;
double *outrisetime,inrisetime;
{
    double Ceq,Rwire,Rline;
    double aspectRatio;     /* as height over width */
    double ramBlocks;       /* number of RAM blocks */
    double tf;
    double nordel,outdel,nextinputtime;
    double hstack,vstack;

    /* calculate some layout info */

    aspectRatio = (2.0*C)/(8.0*B*B*A*A*Ndbl*Ndbl*Nspd*Nspd);
    hstack = (aspectRatio > 1.0) ? aspectRatio : 1.0/aspectRatio;
    ramBlocks = Ndwl*Ndbl;
    hstack = hstack * sqrt(ramBlocks/ hstack);
    vstack = ramBlocks/ hstack;

    /* Delay of NOR gate */

    Ceq = 2*SIM_power_draincap(Woutdrvnorn,NCH,1)+SIM_power_draincap(Woutdrvnorp,PCH,2)+
          SIM_power_gatecap(Woutdrivern,10.0);
    tf = Ceq*SIM_power_transreson(Woutdrvnorp,PCH,2);
    nordel = SIM_power_horowitz(inrisetime,tf,PARM(VTHOUTDRNOR),PARM(VTHOUTDRIVE),FALL);
    nextinputtime = nordel/(PARM(VTHOUTDRIVE));

    /* Delay of final output driver */

    Ceq = (SIM_power_draincap(Woutdrivern,NCH,1)+SIM_power_draincap(Woutdriverp,PCH,1))*
          ((8*B*A)/PARM(BITOUT)) +
          Cwordmetal*(8*B*A*Nspd* (vstack)) + Cout;
    Rwire = Rwordmetal*(8*B*A*Nspd* (vstack))/2;

    tf = Ceq*(SIM_power_transreson(Woutdriverp,PCH,1)+Rwire);
    outdel = SIM_power_horowitz(nextinputtime,tf,PARM(VTHOUTDRIVE),0.5,RISE);
    *outrisetime = outdel/0.5;
    return(outdel+nordel);
}


/*----------------------------------------------------------------------*/

/* Sel inverter delay (part of the output driver)  see section 6.8 */
double SIM_power_selb_delay_tag_path(inrisetime,outrisetime)
double inrisetime,*outrisetime;
{
    double Ceq,Tst1,tf;

    Ceq = SIM_power_draincap(Woutdrvseln,NCH,1)+SIM_power_draincap(Woutdrvselp,PCH,1)+
          SIM_power_gatecap(Woutdrvnandn+Woutdrvnandp,10.0);
    tf = Ceq*SIM_power_transreson(Woutdrvseln,NCH,1);
    Tst1 = SIM_power_horowitz(inrisetime,tf,PARM(VTHOUTDRINV),PARM(VTHOUTDRNAND),RISE);
    *outrisetime = Tst1/(1.0-PARM(VTHOUTDRNAND));

    return(Tst1);
}


/*----------------------------------------------------------------------*/

/* This routine calculates the extra time required after an access before
 * the next access can occur [ie.  it returns (cycle time-access time)].
 */
double SIM_power_precharge_delay(worddata)
double worddata;
{
    double Ceq,tf,pretime;

    /* as discussed in the tech report, the delay is the delay of
       4 inverter delays (each with fanout of 4) plus the delay of
       the wordline */

    Ceq = SIM_power_draincap(Wdecinvn,NCH,1)+SIM_power_draincap(Wdecinvp,PCH,1)+
          4*SIM_power_gatecap(Wdecinvn+Wdecinvp,0.0);
    tf = Ceq*SIM_power_transreson(Wdecinvn,NCH,1);
    pretime = 4*SIM_power_horowitz(0.0,tf,0.5,0.5,RISE) + worddata;

    return(pretime);
}


/*======================================================================*/


/* returns 1 if the parameters make up a valid organization */
/* Layout concerns drive any restrictions you might add here */
int SIM_power_organizational_parameters_valid(rows,cols,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd)
int rows,cols,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd;
{
    /* don't want more than 8 subarrays for each of data/tag */

    if (Ndwl*Ndbl>PARM(MAXSUBARRAYS)) return(0);
    if (Ntwl*Ntbl>PARM(MAXSUBARRAYS)) return(0);

    /* add more constraints here as necessary */

    return(1);
}


/*----------------------------------------------------------------------*/


void SIM_power_calculate_time(result,parameters)
time_result_type *result;
time_parameter_type *parameters;
{
    int Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,rows,columns,tag_driver_size1,tag_driver_size2;
    double access_time;
    double before_mux,after_mux;
    double decoder_data_driver,decoder_data_3to8,decoder_data_inv;
    double decoder_data,decoder_tag,wordline_data,wordline_tag;
    double decoder_tag_driver,decoder_tag_3to8,decoder_tag_inv;
    double bitline_data,bitline_tag,sense_amp_data,sense_amp_tag;
    double compare_tag,mux_driver,data_output,selb;
    double time_till_compare,time_till_select,driver_cap,valid_driver;
    double cycle_time, precharge_del;
    double outrisetime,inrisetime;

    rows = parameters->number_of_sets;
    columns = 8*parameters->block_size*parameters->associativity;

    /* go through possible Ndbl,Ndwl and find the smallest */
    /* Because of area considerations, I don't think it makes sense
       to break either dimension up larger than MAXN */

    result->cycle_time = BIGNUM;
    result->access_time = BIGNUM;
    for (Nspd=1; Nspd<=PARM(MAXSPD); Nspd=Nspd*2)
    {
        for (Ndwl=1; Ndwl<=PARM(MAXN); Ndwl=Ndwl*2)
        {
            for (Ndbl=1; Ndbl<=PARM(MAXN); Ndbl=Ndbl*2)
            {
                for (Ntspd=1; Ntspd<=PARM(MAXSPD); Ntspd=Ntspd*2)
                {
                    for (Ntwl=1; Ntwl<=1; Ntwl=Ntwl*2)
                    {
                        for (Ntbl=1; Ntbl<=PARM(MAXN); Ntbl=Ntbl*2)
                        {

                            if (SIM_power_organizational_parameters_valid
                                    (rows,columns,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd))
                            {


                                /* Calculate data side of cache */


                                decoder_data = SIM_power_decoder_delay(parameters->cache_size,parameters->block_size,
                                                                       parameters->associativity,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,
                                                                       &decoder_data_driver,&decoder_data_3to8,
                                                                       &decoder_data_inv,&outrisetime);


                                inrisetime = outrisetime;
                                wordline_data = SIM_power_wordline_delay(parameters->block_size,
                                                parameters->associativity,Ndwl,Nspd,
                                                inrisetime,&outrisetime);

                                inrisetime = outrisetime;
                                bitline_data = SIM_power_bitline_delay(parameters->cache_size,parameters->associativity,
                                                                       parameters->block_size,Ndwl,Ndbl,Nspd,
                                                                       inrisetime,&outrisetime);
                                inrisetime = outrisetime;
                                sense_amp_data = SIM_power_sense_amp_delay(inrisetime,&outrisetime);
                                inrisetime = outrisetime;
                                data_output = SIM_power_dataoutput_delay(parameters->cache_size,parameters->block_size,
                                              parameters->associativity,Ndbl,Nspd,Ndwl,
                                              inrisetime,&outrisetime);
                                inrisetime = outrisetime;


                                /* if the associativity is 1, the data output can come right
                                   after the sense amp.   Otherwise, it has to wait until
                                   the data access has been done. */

                                if (parameters->associativity==1)
                                {
                                    before_mux = decoder_data + wordline_data + bitline_data +
                                                 sense_amp_data + data_output;
                                    after_mux = 0;
                                }
                                else
                                {
                                    before_mux = decoder_data + wordline_data + bitline_data +
                                                 sense_amp_data;
                                    after_mux = data_output;
                                }


                                /*
                                 * Now worry about the tag side.
                                 */


                                decoder_tag = SIM_power_decoder_tag_delay(parameters->cache_size,
                                              parameters->block_size,parameters->associativity,
                                              Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,
                                              &decoder_tag_driver,&decoder_tag_3to8,
                                              &decoder_tag_inv,&outrisetime);
                                inrisetime = outrisetime;

                                wordline_tag = SIM_power_wordline_tag_delay(parameters->cache_size,
                                               parameters->associativity,Ntspd,Ntwl,
                                               inrisetime,&outrisetime);
                                inrisetime = outrisetime;

                                bitline_tag = SIM_power_bitline_tag_delay(parameters->cache_size,parameters->associativity,
                                              parameters->block_size,Ntwl,Ntbl,Ntspd,
                                              inrisetime,&outrisetime);
                                inrisetime = outrisetime;

                                sense_amp_tag = SIM_power_sense_amp_tag_delay(inrisetime,&outrisetime);
                                inrisetime = outrisetime;

                                compare_tag = SIM_power_compare_time(parameters->cache_size,parameters->associativity,
                                                                     Ntbl,Ntspd,
                                                                     inrisetime,&outrisetime);
                                inrisetime = outrisetime;

                                if (parameters->associativity == 1)
                                {
                                    mux_driver = 0;
                                    valid_driver = SIM_power_valid_driver_delay(parameters->cache_size,
                                                   parameters->associativity,Ntbl,Ntspd,inrisetime);
                                    time_till_compare = decoder_tag + wordline_tag + bitline_tag +
                                                        sense_amp_tag;

                                    time_till_select = time_till_compare+ compare_tag + valid_driver;


                                    /*
                                    		* From the above info, calculate the total access time
                                    		*/

                                    access_time = MAX(before_mux+after_mux,time_till_select);


                                }
                                else
                                {
                                    mux_driver = SIM_power_mux_driver_delay(parameters->cache_size,parameters->block_size,
                                                                            parameters->associativity,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,
                                                                            inrisetime,&outrisetime);

                                    selb = SIM_power_selb_delay_tag_path(inrisetime,&outrisetime);

                                    valid_driver = 0;

                                    time_till_compare = decoder_tag + wordline_tag + bitline_tag +
                                                        sense_amp_tag;

                                    time_till_select = time_till_compare+ compare_tag + mux_driver
                                                       + selb;

                                    access_time = MAX(before_mux,time_till_select) +after_mux;
                                }

                                /*
                                 * Calcuate the cycle time
                                 */

                                precharge_del = SIM_power_precharge_delay(wordline_data);

                                cycle_time = access_time + precharge_del;

                                /*
                                 * The parameters are for a 0.8um process.  A quick way to
                                     * scale the results to another process is to divide all
                                     * the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
                                 */

                                if (result->cycle_time+1e-11*(result->best_Ndwl+result->best_Ndbl+result->best_Nspd+result->best_Ntwl+result->best_Ntbl+result->best_Ntspd) > cycle_time/PARM(FUDGEFACTOR)+1e-11*(Ndwl+Ndbl+Nspd+Ntwl+Ntbl+Ntspd))
                                {
                                    result->cycle_time = cycle_time/PARM(FUDGEFACTOR);
                                    result->access_time = access_time/PARM(FUDGEFACTOR);
                                    result->best_Ndwl = Ndwl;
                                    result->best_Ndbl = Ndbl;
                                    result->best_Nspd = Nspd;
                                    result->best_Ntwl = Ntwl;
                                    result->best_Ntbl = Ntbl;
                                    result->best_Ntspd = Ntspd;
                                    result->decoder_delay_data = decoder_data/PARM(FUDGEFACTOR);
                                    result->decoder_delay_tag = decoder_tag/PARM(FUDGEFACTOR);
                                    result->dec_tag_driver = decoder_tag_driver/PARM(FUDGEFACTOR);
                                    result->dec_tag_3to8 = decoder_tag_3to8/PARM(FUDGEFACTOR);
                                    result->dec_tag_inv = decoder_tag_inv/PARM(FUDGEFACTOR);
                                    result->dec_data_driver = decoder_data_driver/PARM(FUDGEFACTOR);
                                    result->dec_data_3to8 = decoder_data_3to8/PARM(FUDGEFACTOR);
                                    result->dec_data_inv = decoder_data_inv/PARM(FUDGEFACTOR);
                                    result->wordline_delay_data = wordline_data/PARM(FUDGEFACTOR);
                                    result->wordline_delay_tag = wordline_tag/PARM(FUDGEFACTOR);
                                    result->bitline_delay_data = bitline_data/PARM(FUDGEFACTOR);
                                    result->bitline_delay_tag = bitline_tag/PARM(FUDGEFACTOR);
                                    result->sense_amp_delay_data = sense_amp_data/PARM(FUDGEFACTOR);
                                    result->sense_amp_delay_tag = sense_amp_tag/PARM(FUDGEFACTOR);
                                    result->compare_part_delay = compare_tag/PARM(FUDGEFACTOR);
                                    result->drive_mux_delay = mux_driver/PARM(FUDGEFACTOR);
                                    result->selb_delay = selb/PARM(FUDGEFACTOR);
                                    result->drive_valid_delay = valid_driver/PARM(FUDGEFACTOR);
                                    result->data_output_delay = data_output/PARM(FUDGEFACTOR);
                                    result->precharge_delay = precharge_del/PARM(FUDGEFACTOR);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
