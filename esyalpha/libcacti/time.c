
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
#include "def.h"


/*----------------------------------------------------------------------*/

double logtwo(x)
double x;
{
    if (x<=0) printf("%e\n",x);
    return( (double) (log(x)/log(2.0)) );
}
/*----------------------------------------------------------------------*/

double gatecap(width,wirelength) /* returns gate capacitance in Farads */
double width;		/* gate width in um (length is Leff) */
double wirelength;	/* poly wire length going to gate in lambda */
{
    return(width*Leff*Cgate+wirelength*Cpolywire*Leff);
}

double gatecappass(width,wirelength) /* returns gate capacitance in Farads */
double width;           /* gate width in um (length is Leff) */
double wirelength;      /* poly wire length going to gate in lambda */
{
    return(width*Leff*Cgatepass+wirelength*Cpolywire*Leff);
}


/*----------------------------------------------------------------------*/

/* Routine for calculating drain capacitances.  The draincap routine
 * folds transistors larger than 10um */

double draincap(width,nchannel,stack)  /* returns drain cap in Farads */
double width;		/* um */
int nchannel;		/* whether n or p-channel (boolean) */
int stack;		/* number of transistors in series that are on */
{
    double Cdiffside,Cdiffarea,Coverlap,cap;

    Cdiffside = (nchannel) ? Cndiffside : Cpdiffside;
    Cdiffarea = (nchannel) ? Cndiffarea : Cpdiffarea;
    Coverlap = (nchannel) ? (Cndiffovlp+Cnoxideovlp) :
               (Cpdiffovlp+Cpoxideovlp);
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

double transresswitch(width,nchannel,stack)  /* returns resistance in ohms */
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

double transreson(width,nchannel,stack)  /* returns resistance in ohms */
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

double restowidth(res,nchannel)  /* returns width in um */
double res;            /* resistance in ohms */
int nchannel;          /* whether N-channel or P-channel */
{
    double restrans;

    restrans = (nchannel) ? Rnchannelon : Rpchannelon;

    return(restrans/res);

}

/*----------------------------------------------------------------------*/

double horowitz(inputramptime,tf,vs1,vs2,rise)
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
        td = tf*sqrt( log(vs1)*log(vs1)+2*a*b*(1.0-vs1)) +
             tf*(log(vs1)-log(vs2));
    }
    else
    {
        b = 0.4;
        td = tf*sqrt( log(1.0-vs1)*log(1.0-vs1)+2*a*b*(vs1)) +
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


double decoder_delay(C, B, A, Ndwl, Ndbl, Nspd, Ntwl, Ntbl, Ntspd, Tdecdrive,
                     Tdecoder1, Tdecoder2,outrisetime)
int C,B,A,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd;
double *Tdecdrive,*Tdecoder1,*Tdecoder2,*outrisetime;
{
    double Ceq,Req,Rwire,rows,tf,nextinputtime,vth,tstep,m,a,b,c;
    int numstack;

    /* Calculate rise time.  Consider two inverters */

    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
          gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*transreson(Wdecdriven,NCH,1);
    nextinputtime = horowitz(0.0,tf,VTHINV100x60,VTHINV100x60,FALL)/
                    (VTHINV100x60);

    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
          gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*transreson(Wdecdriven,NCH,1);
    nextinputtime = horowitz(nextinputtime,tf,VTHINV100x60,VTHINV100x60,
                             RISE)/
                    (1.0-VTHINV100x60);

    /* First stage: driving the decoders */

    rows = C/(8*B*A*Ndbl*Nspd);
    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
          4*gatecap(Wdec3to8n+Wdec3to8p,10.0)*(Ndwl*Ndbl)+
          Cwordmetal*0.25*8*B*A*Ndbl*Nspd;
    Rwire = Rwordmetal*0.125*8*B*A*Ndbl*Nspd;
    tf = (Rwire + transreson(Wdecdrivep,PCH,1))*Ceq;
    *Tdecdrive = horowitz(nextinputtime,tf,VTHINV100x60,VTHNAND60x90,
                          FALL);
    nextinputtime = *Tdecdrive/VTHNAND60x90;

    /* second stage: driving a bunch of nor gates with a nand */

    numstack =
        ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));
    if (numstack==0) numstack = 1;
    if (numstack>5) numstack = 5;
    Ceq = 3*draincap(Wdec3to8p,PCH,1) +draincap(Wdec3to8n,NCH,3) +
          gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0))*rows +
          Cbitmetal*rows*8;

    Rwire = Rbitmetal*rows*8/2;
    tf = Ceq*(Rwire+transreson(Wdec3to8n,NCH,3));

    /* we only want to charge the output to the threshold of the
       nor gate.  But the threshold depends on the number of inputs
       to the nor.  */

    switch(numstack)
    {
    case 1:
        vth = VTHNOR12x4x1;
        break;
    case 2:
        vth = VTHNOR12x4x2;
        break;
    case 3:
        vth = VTHNOR12x4x3;
        break;
    case 4:
        vth = VTHNOR12x4x4;
        break;
    case 5:
        vth = VTHNOR12x4x4;
        break;
    default:
        printf("error:numstack=%d\n",numstack);
    }
    *Tdecoder1 = horowitz(nextinputtime,tf,VTHNAND60x90,vth,RISE);
    nextinputtime = *Tdecoder1/(1.0-vth);

    /* Final stage: driving an inverter with the nor */

    Req = transreson(WdecNORp,PCH,numstack);
    Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0)+
           numstack*draincap(WdecNORn,NCH,1)+
           draincap(WdecNORp,PCH,numstack));
    tf = Req*Ceq;
    *Tdecoder2 = horowitz(nextinputtime,tf,vth,VSINV,FALL);
    *outrisetime = *Tdecoder2/(VSINV);
    return(*Tdecdrive+*Tdecoder1+*Tdecoder2);
}


/*----------------------------------------------------------------------*/

/* Decoder delay in the tag array (see section 6.1 of tech report) */


double decoder_tag_delay(C, B, A, Ndwl, Ndbl, Nspd, Ntwl, Ntbl, Ntspd,
                         Tdecdrive, Tdecoder1, Tdecoder2,outrisetime)
int C,B,A,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd;
double *Tdecdrive,*Tdecoder1,*Tdecoder2,*outrisetime;
{
    double Ceq,Req,Rwire,rows,tf,nextinputtime,vth,tstep,m,a,b,c;
    int numstack;


    /* Calculate rise time.  Consider two inverters */

    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
          gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*transreson(Wdecdriven,NCH,1);
    nextinputtime = horowitz(0.0,tf,VTHINV100x60,VTHINV100x60,FALL)/
                    (VTHINV100x60);

    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
          gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*transreson(Wdecdriven,NCH,1);
    nextinputtime = horowitz(nextinputtime,tf,VTHINV100x60,VTHINV100x60,
                             RISE)/
                    (1.0-VTHINV100x60);

    /* First stage: driving the decoders */

    rows = C/(8*B*A*Ntbl*Ntspd);
    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
          4*gatecap(Wdec3to8n+Wdec3to8p,10.0)*(Ntwl*Ntbl)+
          Cwordmetal*0.25*8*B*A*Ntbl*Ntspd;
    Rwire = Rwordmetal*0.125*8*B*A*Ntbl*Ntspd;
    tf = (Rwire + transreson(Wdecdrivep,PCH,1))*Ceq;
    *Tdecdrive = horowitz(nextinputtime,tf,VTHINV100x60,VTHNAND60x90,
                          FALL);
    nextinputtime = *Tdecdrive/VTHNAND60x90;

    /* second stage: driving a bunch of nor gates with a nand */

    numstack =
        ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));
    if (numstack==0) numstack = 1;
    if (numstack>5) numstack = 5;

    Ceq = 3*draincap(Wdec3to8p,PCH,1) +draincap(Wdec3to8n,NCH,3) +
          gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0))*rows +
          Cbitmetal*rows*8;

    Rwire = Rbitmetal*rows*8/2;
    tf = Ceq*(Rwire+transreson(Wdec3to8n,NCH,3));

    /* we only want to charge the output to the threshold of the
       nor gate.  But the threshold depends on the number of inputs
       to the nor.  */

    switch(numstack)
    {
    case 1:
        vth = VTHNOR12x4x1;
        break;
    case 2:
        vth = VTHNOR12x4x2;
        break;
    case 3:
        vth = VTHNOR12x4x3;
        break;
    case 4:
        vth = VTHNOR12x4x4;
        break;
    case 5:
        vth = VTHNOR12x4x4;
        break;
    case 6:
        vth = VTHNOR12x4x4;
        break;
    default:
        printf("error:numstack=%d\n",numstack);
    }
    *Tdecoder1 = horowitz(nextinputtime,tf,VTHNAND60x90,vth,RISE);
    nextinputtime = *Tdecoder1/(1.0-vth);

    /* Final stage: driving an inverter with the nor */

    Req = transreson(WdecNORp,PCH,numstack);
    Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0)+
           numstack*draincap(WdecNORn,NCH,1)+
           draincap(WdecNORp,PCH,numstack));
    tf = Req*Ceq;
    *Tdecoder2 = horowitz(nextinputtime,tf,vth,VSINV,FALL);
    *outrisetime = *Tdecoder2/(VSINV);
    return(*Tdecdrive+*Tdecoder1+*Tdecoder2);
}


/*----------------------------------------------------------------------*/

/* Data array wordline delay (see section 6.2 of tech report) */


double wordline_delay(B,A,Ndwl,Nspd,inrisetime,outrisetime)
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
    Cline = (gatecappass(Wmemcella,0.0)+
             gatecappass(Wmemcella,0.0)+
             Cwordmetal)*cols;
    Rpdrive = desiredrisetime/(Cline*log(VSINV)*-1.0);
    psize = restowidth(Rpdrive,PCH);
    if (psize > Wworddrivemax)
    {
        psize = Wworddrivemax;
    }

    /* Now that we have a reasonable psize, do the rest as before */
    /* If we keep the ratio the same as the tag wordline driver,
       the threshold voltage will be close to VSINV */

    nsize = psize * Wdecinvn/Wdecinvp;

    Ceq = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) +
          gatecap(nsize+psize,20.0);
    tf = transreson(Wdecinvn,NCH,1)*Ceq;

    Tworddrivedel = horowitz(inrisetime,tf,VSINV,VSINV,RISE);
    nextinputtime = Tworddrivedel/(1.0-VSINV);

    Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             Cwordmetal)*cols+
            draincap(nsize,NCH,1) + draincap(psize,PCH,1);
    Rline = Rwordmetal*cols/2;
    tf = (transreson(psize,PCH,1)+Rline)*Cline;
    Twordchargedel = horowitz(nextinputtime,tf,VSINV,VSINV,FALL);
    *outrisetime = Twordchargedel/VSINV;

    return(Tworddrivedel+Twordchargedel);
}

/*----------------------------------------------------------------------*/

/* Tag array wordline delay (see section 6.3 of tech report) */


double wordline_tag_delay(C,A,Ntspd,Ntwl,inrisetime,outrisetime)
int C,A,Ntspd,Ntwl;
double *outrisetime;
double inrisetime;
{
    double tf,m,a,b,c;
    double Cline,Rline,Ceq,nextinputtime;
    int tagbits;
    double Tworddrivedel,Twordchargedel;

    /* number of tag bits */

    tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A);

    /* first stage */

    Ceq = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) +
          gatecap(Wdecinvn+Wdecinvp,20.0);
    tf = transreson(Wdecinvn,NCH,1)*Ceq;

    Tworddrivedel = horowitz(inrisetime,tf,VSINV,VSINV,RISE);
    nextinputtime = Tworddrivedel/(1.0-VSINV);

    /* second stage */
    Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             Cwordmetal)*tagbits*A*Ntspd/Ntwl+
            draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1);
    Rline = Rwordmetal*tagbits*A*Ntspd/(2*Ntwl);
    tf = (transreson(Wdecinvp,PCH,1)+Rline)*Cline;
    Twordchargedel = horowitz(nextinputtime,tf,VSINV,VSINV,FALL);
    *outrisetime = Twordchargedel/VSINV;
    return(Tworddrivedel+Twordchargedel);

}

/*----------------------------------------------------------------------*/

/* Data array bitline: (see section 6.4 in tech report) */


double bitline_delay(C,A,B,Ndwl,Ndbl,Nspd,inrisetime,outrisetime)
int C,A,B,Ndbl,Nspd;
double inrisetime,*outrisetime;
{
    double Tbit,Cline,Ccolmux,Rlineb,r1,r2,c1,c2,a,b,c;
    double m,tstep;
    double Cbitrow;    /* bitline capacitance due to access transistor */
    int rows,cols;

    Cbitrow = draincap(Wmemcella,NCH,1)/2.0; /* due to shared contact */
    rows = C/(B*A*Ndbl*Nspd);
    cols = 8*B*A*Nspd/Ndwl;
    if (Ndbl*Nspd == 1)
    {
        Cline = rows*(Cbitrow+Cbitmetal)+2*draincap(Wbitpreequ,PCH,1);
        Ccolmux = 2*gatecap(WsenseQ1to4,10.0);
        Rlineb = Rbitmetal*rows/2.0;
        r1 = Rlineb;
    }
    else
    {
        Cline = rows*(Cbitrow+Cbitmetal) + 2*draincap(Wbitpreequ,PCH,1) +
                draincap(Wbitmuxn,NCH,1);
        Ccolmux = Nspd*Ndbl*(draincap(Wbitmuxn,NCH,1))+2*gatecap(WsenseQ1to4,10.0);
        Rlineb = Rbitmetal*rows/2.0;
        r1 = Rlineb +
             transreson(Wbitmuxn,NCH,1);
    }
    r2 = transreson(Wmemcella,NCH,1) +
         transreson(Wmemcella*Wmemcellbscale,NCH,1);
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


double bitline_tag_delay(C,A,B,Ntwl,Ntbl,Ntspd,inrisetime,outrisetime)
int C,A,B,Ntbl,Ntspd;
double inrisetime,*outrisetime;
{
    double Tbit,Cline,Ccolmux,Rlineb,r1,r2,c1,c2,a,b,c;
    double m,tstep;
    double Cbitrow;    /* bitline capacitance due to access transistor */
    int rows,cols;

    Cbitrow = draincap(Wmemcella,NCH,1)/2.0; /* due to shared contact */
    rows = C/(B*A*Ntbl*Ntspd);
    cols = 8*B*A*Ntspd/Ntwl;
    if (Ntbl*Ntspd == 1)
    {
        Cline = rows*(Cbitrow+Cbitmetal)+2*draincap(Wbitpreequ,PCH,1);
        Ccolmux = 2*gatecap(WsenseQ1to4,10.0);
        Rlineb = Rbitmetal*rows/2.0;
        r1 = Rlineb;
    }
    else
    {
        Cline = rows*(Cbitrow+Cbitmetal) + 2*draincap(Wbitpreequ,PCH,1) +
                draincap(Wbitmuxn,NCH,1);
        Ccolmux = Ntspd*Ntbl*(draincap(Wbitmuxn,NCH,1))+2*gatecap(WsenseQ1to4,10.0);
        Rlineb = Rbitmetal*rows/2.0;
        r1 = Rlineb +
             transreson(Wbitmuxn,NCH,1);
    }
    r2 = transreson(Wmemcella,NCH,1) +
         transreson(Wmemcella*Wmemcellbscale,NCH,1);

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

double sense_amp_delay(inrisetime,outrisetime)
double inrisetime,*outrisetime;
{
    *outrisetime = tfalldata;
    return(tsensedata);
}

/*--------------------------------------------------------------*/

double sense_amp_tag_delay(inrisetime,outrisetime)
double inrisetime,*outrisetime;
{
    *outrisetime = tfalltag;
    return(tsensetag);
}

/*----------------------------------------------------------------------*/

/* Comparator Delay (see section 6.6) */


double compare_time(C,A,Ntbl,Ntspd,inputtime,outputtime)
int C,A,Ntbl,Ntspd;
double inputtime,*outputtime;
{
    double Req,Ceq,tf,st1del,st2del,st3del,nextinputtime,m;
    double c1,c2,r1,r2,tstep,a,b,c;
    double Tcomparatorni;
    int cols,tagbits;

    /* First Inverter */

    Ceq = gatecap(Wcompinvn2+Wcompinvp2,10.0) +
          draincap(Wcompinvp1,PCH,1) + draincap(Wcompinvn1,NCH,1);
    Req = transreson(Wcompinvp1,PCH,1);
    tf = Req*Ceq;
    st1del = horowitz(inputtime,tf,VTHCOMPINV,VTHCOMPINV,FALL);
    nextinputtime = st1del/VTHCOMPINV;

    /* Second Inverter */

    Ceq = gatecap(Wcompinvn3+Wcompinvp3,10.0) +
          draincap(Wcompinvp2,PCH,1) + draincap(Wcompinvn2,NCH,1);
    Req = transreson(Wcompinvn2,NCH,1);
    tf = Req*Ceq;
    st2del = horowitz(inputtime,tf,VTHCOMPINV,VTHCOMPINV,RISE);
    nextinputtime = st1del/(1.0-VTHCOMPINV);

    /* Third Inverter */

    Ceq = gatecap(Wevalinvn+Wevalinvp,10.0) +
          draincap(Wcompinvp3,PCH,1) + draincap(Wcompinvn3,NCH,1);
    Req = transreson(Wcompinvp3,PCH,1);
    tf = Req*Ceq;
    st3del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHEVALINV,FALL);
    nextinputtime = st1del/(VTHEVALINV);

    /* Final Inverter (virtual ground driver) discharging compare part */

    tagbits = ADDRESS_BITS - (int)logtwo((double)C) + (int)logtwo((double)A);
    cols = tagbits*Ntbl*Ntspd;

    r1 = transreson(Wcompn,NCH,2);
    r2 = transresswitch(Wevalinvn,NCH,1);
    c2 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
         draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);
    c1 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))
         +draincap(Wcompp,PCH,1) + gatecap(Wmuxdrv12n+Wmuxdrv12p,20.0) +
         cols*Cwordmetal;

    /* time to go to threshold of mux driver */

    tstep = (r2*c2+(r1+r2)*c1)*log(1.0/VTHMUXDRV1);

    /* take into account non-zero input rise time */

    m = Vdd/nextinputtime;

    if ((tstep) <= (0.5*(Vdd-Vt)/m))
    {
        a = m;
        b = 2*((Vdd*VTHEVALINV)-Vt);
        c = -2*(tstep)*(Vdd-Vt)+1/m*((Vdd*VTHEVALINV)-Vt)*((Vdd*VTHEVALINV)-Vt);
        Tcomparatorni = (-b+sqrt(b*b-4*a*c))/(2*a);
    }
    else
    {
        Tcomparatorni = (tstep) + (Vdd+Vt)/(2*m) - (Vdd*VTHEVALINV)/m;
    }
    *outputtime = Tcomparatorni/(1.0-VTHMUXDRV1);

    return(Tcomparatorni+st1del+st2del+st3del);
}




/*----------------------------------------------------------------------*/

/* Delay of the multiplexor Driver (see section 6.7) */


double mux_driver_delay(C,B,A,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,inputtime,outputtime)
int C,B,A,Ndbl,Nspd,Ndwl,Ntbl,Ntspd;
double inputtime,*outputtime;
{
    double Ceq,Req,tf,nextinputtime;
    double Tst1,Tst2,Tst3;

    /* first driver stage - Inverte "match" to produce "matchb" */
    /* the critical path is the DESELECTED case, so consider what
       happens when the address bit is true, but match goes low */

    Ceq = gatecap(WmuxdrvNORn+WmuxdrvNORp,15.0)*(8*B/BITOUT) +
          draincap(Wmuxdrv12n,NCH,1) + draincap(Wmuxdrv12p,PCH,1);
    Req = transreson(Wmuxdrv12p,PCH,1);
    tf = Ceq*Req;
    Tst1 = horowitz(inputtime,tf,VTHMUXDRV1,VTHMUXDRV2,FALL);
    nextinputtime = Tst1/VTHMUXDRV2;

    /* second driver stage - NOR "matchb" with address bits to produce sel */

    Ceq = gatecap(Wmuxdrv3n+Wmuxdrv3p,15.0) + 2*draincap(WmuxdrvNORn,NCH,1) +
          draincap(WmuxdrvNORp,PCH,2);
    Req = transreson(WmuxdrvNORn,NCH,1);
    tf = Ceq*Req;
    Tst2 = horowitz(nextinputtime,tf,VTHMUXDRV2,VTHMUXDRV3,RISE);
    nextinputtime = Tst2/(1-VTHMUXDRV3);

    /* third driver stage - invert "select" to produce "select bar" */

    Ceq = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0)+
          draincap(Wmuxdrv3p,PCH,1) + draincap(Wmuxdrv3n,NCH,1) +
          Cwordmetal*8*B*A*Nspd*Ndbl/2.0;
    Req = (Rwordmetal*8*B*A*Nspd*Ndbl/2)/2 + transreson(Wmuxdrv3p,PCH,1);
    tf = Ceq*Req;
    Tst3 = horowitz(nextinputtime,tf,VTHMUXDRV3,VTHOUTDRINV,FALL);
    *outputtime = Tst3/(VTHOUTDRINV);

    return(Tst1 + Tst2 + Tst3);

}



/*----------------------------------------------------------------------*/

/* Valid driver (see section 6.9 of tech report)
   Note that this will only be called for a direct mapped cache */

double valid_driver_delay(C,A,Ntbl,Ntspd,inputtime)
int C,A,Ntbl,Ntspd;
double inputtime;
{
    double Ceq,Tst1,tf;

    Ceq = draincap(Wmuxdrv12n,NCH,1)+draincap(Wmuxdrv12p,PCH,1)+Cout;
    tf = Ceq*transreson(Wmuxdrv12p,PCH,1);
    Tst1 = horowitz(inputtime,tf,VTHMUXDRV1,0.5,FALL);

    return(Tst1);
}


/*----------------------------------------------------------------------*/

/* Data output delay (data side) -- see section 6.8
   This is the time through the NAND/NOR gate and the final inverter
   assuming sel is already present */

double dataoutput_delay(C,B,A,Ndbl,Nspd,Ndwl,
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

    Ceq = 2*draincap(Woutdrvnorn,NCH,1)+draincap(Woutdrvnorp,PCH,2)+
          gatecap(Woutdrivern,10.0);
    tf = Ceq*transreson(Woutdrvnorp,PCH,2);
    nordel = horowitz(inrisetime,tf,VTHOUTDRNOR,VTHOUTDRIVE,FALL);
    nextinputtime = nordel/(VTHOUTDRIVE);

    /* Delay of final output driver */

    Ceq = (draincap(Woutdrivern,NCH,1)+draincap(Woutdriverp,PCH,1))*
          ((8*B*A)/BITOUT) +
          Cwordmetal*(8*B*A*Nspd* (vstack)) + Cout;
    Rwire = Rwordmetal*(8*B*A*Nspd* (vstack))/2;

    tf = Ceq*(transreson(Woutdriverp,PCH,1)+Rwire);
    outdel = horowitz(nextinputtime,tf,VTHOUTDRIVE,0.5,RISE);
    *outrisetime = outdel/0.5;
    return(outdel+nordel);
}

/*----------------------------------------------------------------------*/

/* Sel inverter delay (part of the output driver)  see section 6.8 */

double selb_delay_tag_path(inrisetime,outrisetime)
double inrisetime,*outrisetime;
{
    double Ceq,Tst1,tf;

    Ceq = draincap(Woutdrvseln,NCH,1)+draincap(Woutdrvselp,PCH,1)+
          gatecap(Woutdrvnandn+Woutdrvnandp,10.0);
    tf = Ceq*transreson(Woutdrvseln,NCH,1);
    Tst1 = horowitz(inrisetime,tf,VTHOUTDRINV,VTHOUTDRNAND,RISE);
    *outrisetime = Tst1/(1.0-VTHOUTDRNAND);

    return(Tst1);
}


/*----------------------------------------------------------------------*/

/* This routine calculates the extra time required after an access before
 * the next access can occur [ie.  it returns (cycle time-access time)].
 */

double precharge_delay(worddata)
double worddata;
{
    double Ceq,tf,pretime;

    /* as discussed in the tech report, the delay is the delay of
       4 inverter delays (each with fanout of 4) plus the delay of
       the wordline */

    Ceq = draincap(Wdecinvn,NCH,1)+draincap(Wdecinvp,PCH,1)+
          4*gatecap(Wdecinvn+Wdecinvp,0.0);
    tf = Ceq*transreson(Wdecinvn,NCH,1);
    pretime = 4*horowitz(0.0,tf,0.5,0.5,RISE) + worddata;

    return(pretime);
}



/*======================================================================*/


/* returns TRUE if the parameters make up a valid organization */
/* Layout concerns drive any restrictions you might add here */

int organizational_parameters_valid(rows,cols,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd)
int rows,cols,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd;
{
    /* don't want more than 8 subarrays for each of data/tag */

    if (Ndwl*Ndbl>MAXSUBARRAYS) return(FALSE);
    if (Ntwl*Ntbl>MAXSUBARRAYS) return(FALSE);

    /* add more constraints here as necessary */

    return(TRUE);
}


/*----------------------------------------------------------------------*/


void calculate_time(result,parameters)
result_type *result;
parameter_type *parameters;
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
    for (Nspd=1; Nspd<=MAXSPD; Nspd=Nspd*2)
    {
        for (Ndwl=1; Ndwl<=MAXN; Ndwl=Ndwl*2)
        {
            for (Ndbl=1; Ndbl<=MAXN; Ndbl=Ndbl*2)
            {
                for (Ntspd=1; Ntspd<=MAXSPD; Ntspd=Ntspd*2)
                {
                    for (Ntwl=1; Ntwl<=1; Ntwl=Ntwl*2)
                    {
                        for (Ntbl=1; Ntbl<=MAXN; Ntbl=Ntbl*2)
                        {

                            if (organizational_parameters_valid
                                    (rows,columns,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd))
                            {


                                /* Calculate data side of cache */


                                decoder_data = decoder_delay(parameters->cache_size,parameters->block_size,
                                                             parameters->associativity,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,
                                                             &decoder_data_driver,&decoder_data_3to8,
                                                             &decoder_data_inv,&outrisetime);

                                inrisetime = outrisetime;
                                wordline_data = wordline_delay(parameters->block_size,
                                                               parameters->associativity,Ndwl,Nspd,
                                                               inrisetime,&outrisetime);
                                inrisetime = outrisetime;
                                bitline_data = bitline_delay(parameters->cache_size,parameters->associativity,
                                                             parameters->block_size,Ndwl,Ndbl,Nspd,
                                                             inrisetime,&outrisetime);
                                inrisetime = outrisetime;
                                sense_amp_data = sense_amp_delay(inrisetime,&outrisetime);
                                inrisetime = outrisetime;
                                data_output = dataoutput_delay(parameters->cache_size,parameters->block_size,
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


                                decoder_tag = decoder_tag_delay(parameters->cache_size,
                                                                parameters->block_size,parameters->associativity,
                                                                Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,
                                                                &decoder_tag_driver,&decoder_tag_3to8,
                                                                &decoder_tag_inv,&outrisetime);
                                inrisetime = outrisetime;

                                wordline_tag = wordline_tag_delay(parameters->cache_size,
                                                                  parameters->associativity,Ntspd,Ntwl,
                                                                  inrisetime,&outrisetime);
                                inrisetime = outrisetime;

                                bitline_tag = bitline_tag_delay(parameters->cache_size,parameters->associativity,
                                                                parameters->block_size,Ntwl,Ntbl,Ntspd,
                                                                inrisetime,&outrisetime);
                                inrisetime = outrisetime;

                                sense_amp_tag = sense_amp_tag_delay(inrisetime,&outrisetime);
                                inrisetime = outrisetime;

                                compare_tag = compare_time(parameters->cache_size,parameters->associativity,
                                                           Ntbl,Ntspd,
                                                           inrisetime,&outrisetime);
                                inrisetime = outrisetime;

                                if (parameters->associativity == 1)
                                {
                                    mux_driver = 0;
                                    valid_driver = valid_driver_delay(parameters->cache_size,
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
                                    mux_driver = mux_driver_delay(parameters->cache_size,parameters->block_size,
                                                                  parameters->associativity,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,
                                                                  inrisetime,&outrisetime);

                                    selb = selb_delay_tag_path(inrisetime,&outrisetime);

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

                                precharge_del = precharge_delay(wordline_data);

                                cycle_time = access_time + precharge_del;

                                /*
                                 * The parameters are for a 0.8um process.  A quick way to
                                     * scale the results to another process is to divide all
                                     * the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
                                 */

                                if (result->cycle_time+1e-11*(result->best_Ndwl+result->best_Ndbl+result->best_Nspd+result->best_Ntwl+result->best_Ntbl+result->best_Ntspd) > cycle_time/FUDGEFACTOR+1e-11*(Ndwl+Ndbl+Nspd+Ntwl+Ntbl+Ntspd))
                                {
                                    result->cycle_time = cycle_time/FUDGEFACTOR;
                                    result->access_time = access_time/FUDGEFACTOR;
                                    result->best_Ndwl = Ndwl;
                                    result->best_Ndbl = Ndbl;
                                    result->best_Nspd = Nspd;
                                    result->best_Ntwl = Ntwl;
                                    result->best_Ntbl = Ntbl;
                                    result->best_Ntspd = Ntspd;
                                    result->decoder_delay_data = decoder_data/FUDGEFACTOR;
                                    result->decoder_delay_tag = decoder_tag/FUDGEFACTOR;
                                    result->dec_tag_driver = decoder_tag_driver/FUDGEFACTOR;
                                    result->dec_tag_3to8 = decoder_tag_3to8/FUDGEFACTOR;
                                    result->dec_tag_inv = decoder_tag_inv/FUDGEFACTOR;
                                    result->dec_data_driver = decoder_data_driver/FUDGEFACTOR;
                                    result->dec_data_3to8 = decoder_data_3to8/FUDGEFACTOR;
                                    result->dec_data_inv = decoder_data_inv/FUDGEFACTOR;
                                    result->wordline_delay_data = wordline_data/FUDGEFACTOR;
                                    result->wordline_delay_tag = wordline_tag/FUDGEFACTOR;
                                    result->bitline_delay_data = bitline_data/FUDGEFACTOR;
                                    result->bitline_delay_tag = bitline_tag/FUDGEFACTOR;
                                    result->sense_amp_delay_data = sense_amp_data/FUDGEFACTOR;
                                    result->sense_amp_delay_tag = sense_amp_tag/FUDGEFACTOR;
                                    result->compare_part_delay = compare_tag/FUDGEFACTOR;
                                    result->drive_mux_delay = mux_driver/FUDGEFACTOR;
                                    result->selb_delay = selb/FUDGEFACTOR;
                                    result->drive_valid_delay = valid_driver/FUDGEFACTOR;
                                    result->data_output_delay = data_output/FUDGEFACTOR;
                                    result->precharge_delay = precharge_del/FUDGEFACTOR;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}

