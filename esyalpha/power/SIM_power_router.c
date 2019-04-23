/*
 * TODO:  (2) add routing table
 *
 * FIXME: (1) ignore internal nodes of tri-state buffer till we find a good solution
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "LIB_defines.h"
#include "SIM_power.h"
#include "SIM_power_array.h"
#include "SIM_power_misc.h"
#include "SIM_power_router.h"
#include "SIM_power_router_internal.h"

double EnergyFactor = Energyfactor;
double SenseEnergyFactor = SenseEnergyfactor;
double EnergyBack = Energyfactor;
double SenseEnergyBack = SenseEnergyfactor;
/*============================== crossbar ==============================*/

static double SIM_crossbar_in_cap(double wire_cap, u_int n_out, int connect_type, int trans_type, double *Nsize)
{
    double Ctotal = 0, Ctrans, psize, nsize;

    /* part 1: wire cap */
    Ctotal = wire_cap;

    /* part 2: drain cap of transmission gate or gate cap of tri-state gate */
    if (connect_type == TRANS_GATE)
    {
        /* FIXME: resizing strategy */
        nsize = Nsize ? *Nsize : Wmemcellr;
        psize = nsize * Wdecinvp / Wdecinvn;
        Ctrans = SIM_power_draincap(nsize, NCH, 1);
        if (trans_type == NP_GATE)
            Ctrans += SIM_power_draincap(psize, PCH, 1);
    }
    else if (connect_type == TRISTATE_GATE)
    {
        Ctrans = SIM_power_gatecap(Woutdrvnandn + Woutdrvnandp, 0) +
                 SIM_power_gatecap(Woutdrvnorn + Woutdrvnorp, 0);
    }
    else
    {
        /* some error handler */
    }

    Ctotal += n_out * Ctrans;

    /* part 3: input driver */
    /* FIXME: how to specify timing? */
    psize = SIM_power_driver_size(Ctotal, Period / 3);
    nsize = psize * Wdecinvn / Wdecinvp;
    Ctotal += SIM_power_draincap(nsize, NCH, 1) + SIM_power_draincap(psize, PCH, 1) +
              SIM_power_gatecap(nsize + psize, 0);

    return Ctotal / 2;
}


static double SIM_crossbar_out_cap(double length, u_int n_in, int connect_type, int trans_type, double *Nsize)
{
    double Ctotal = 0, Ctrans, psize, nsize;

    /* part 1: wire cap */
    Ctotal = CC3metal * length;

    /* part 2: output driver */
    Ctotal += SIM_power_draincap(Woutdrivern, NCH, 1) + SIM_power_draincap(Woutdriverp, PCH, 1) +
              SIM_power_gatecap(Woutdrivern + Woutdriverp, 0);

    /* part 3: drain cap of transmission gate or tri-state gate */
    if (connect_type == TRANS_GATE)
    {
        /* FIXME: resizing strategy */
        if (Nsize)
        {
            /* FIXME: how to specify timing? */
            psize = SIM_power_driver_size(Ctotal, Period / 3);
            *Nsize = nsize = psize * Wdecinvn / Wdecinvp;
        }
        else
        {
            nsize = Wmemcellr;
            psize = nsize * Wdecinvp / Wdecinvn;
        }
        Ctrans = SIM_power_draincap(nsize, NCH, 1);
        if (trans_type == NP_GATE)
            Ctrans += SIM_power_draincap(psize, PCH, 1);
    }
    else if (connect_type == TRISTATE_GATE)
    {
        Ctrans = SIM_power_draincap(Woutdrivern, NCH, 1) + SIM_power_draincap(Woutdriverp, PCH, 1);
    }
    else
    {
        /* some error handler */
    }

    Ctotal += n_in * Ctrans;

    return Ctotal / 2;
}


static double SIM_crossbar_int_cap(u_int degree, int connect_type, int trans_type)
{
    double Ctotal = 0, Ctrans;

    if (connect_type == TRANS_GATE)
    {
        /* part 1: drain cap of transmission gate */
        /* FIXME: Wmemcellr and resize */
        Ctrans = SIM_power_draincap(Wmemcellr, NCH, 1);
        if (trans_type == NP_GATE)
            Ctrans += SIM_power_draincap(Wmemcellr * Wdecinvp / Wdecinvn, PCH, 1);
        Ctotal += (degree + 1) * Ctrans;
    }
    else if (connect_type == TRISTATE_GATE)
    {
        /* part 1: drain cap of tri-state gate */
        Ctotal += degree * (SIM_power_draincap(Woutdrivern, NCH, 1) + SIM_power_draincap(Woutdriverp, PCH, 1));

        /* part 2: gate cap of tri-state gate */
        Ctotal += SIM_power_gatecap(Woutdrvnandn + Woutdrvnandp, 0) +
                  SIM_power_gatecap(Woutdrvnorn + Woutdrvnorp, 0);
    }
    else
    {
        /* some error handler */
    }

    return Ctotal / 2;
}


static double SIM_crossbar_ctr_cap(double length, u_int data_width, int prev_ctr, int next_ctr, u_int degree, int connect_type, int trans_type)
{
    double Ctotal = 0, Cgate;

    /* part 1: wire cap */
    Ctotal = Cmetal * length;

    /* part 2: gate cap of transmission gate or tri-state gate */
    if (connect_type == TRANS_GATE)
    {
        /* FIXME: Wmemcellr and resize */
        Cgate = SIM_power_gatecap(Wmemcellr, 0);
        if (trans_type == NP_GATE)
            Cgate += SIM_power_gatecap(Wmemcellr * Wdecinvp / Wdecinvn, 0);
    }
    else if (connect_type == TRISTATE_GATE)
    {
        Cgate = SIM_power_gatecap(Woutdrvnandn + Woutdrvnandp, 0) +
                SIM_power_gatecap(Woutdrvnorn + Woutdrvnorp, 0);
    }
    else
    {
        /* some error handler */
    }

    Ctotal += data_width * Cgate;

    /* part 3: inverter */
    if (!(connect_type == TRANS_GATE && trans_type == N_GATE && !prev_ctr))
        /* FIXME: need accurate size, use minimal size for now */
        Ctotal += SIM_power_draincap(Wdecinvn, NCH, 1) + SIM_power_draincap(Wdecinvp, PCH, 1) +
                  SIM_power_gatecap(Wdecinvn + Wdecinvp, 0);

    /* part 4: drain cap of previous level control signal */
    if (prev_ctr)
        /* FIXME: need actual size, use decoder data for now */
        Ctotal += degree * SIM_power_draincap(WdecNORn, NCH, 1) + SIM_power_draincap(WdecNORp, PCH, degree);

    /* part 5: gate cap of next level control signal */
    if (next_ctr)
        /* FIXME: need actual size, use decoder data for now */
        Ctotal += SIM_power_gatecap(WdecNORn + WdecNORp, degree * 40 + 20);

    return Ctotal;
}


int SIM_crossbar_init(SIM_power_crossbar_t *crsbar, int model, u_int n_in, u_int n_out, u_int data_width, u_int degree, int connect_type, int trans_type, double in_len, double out_len, double *req_len)
{
    double in_length, out_length, ctr_length, Nsize, in_wire_cap;

    if ((crsbar->model = model) && model < CROSSBAR_MAX_MODEL)
    {
        crsbar->n_in = n_in;
        crsbar->n_out = n_out;
        crsbar->data_width = data_width;
        crsbar->degree = degree;
        crsbar->connect_type = connect_type;
        crsbar->trans_type = trans_type;
        /* redundant field */
        crsbar->mask = HAMM_MASK(data_width);

        crsbar->n_chg_in = crsbar->n_chg_int = crsbar->n_chg_out = crsbar->n_chg_ctr = 0;

        switch (model)
        {
        case MATRIX_CROSSBAR:
            /* FIXME: need accurate spacing */
            in_length = n_out * data_width * CrsbarCellWidth;
            out_length = n_in * data_width * CrsbarCellHeight;
            if (in_length < in_len) in_length = in_len;
            if (out_length < out_len) out_length = out_len;
            ctr_length = in_length / 2;
            if (req_len) *req_len = in_length;

            in_wire_cap = in_length * CC3metal;

            crsbar->e_chg_out = SIM_crossbar_out_cap(out_length, n_in, connect_type, trans_type, &Nsize) * EnergyFactor;
            crsbar->e_chg_in = SIM_crossbar_in_cap(in_wire_cap, n_out, connect_type, trans_type, &Nsize) * EnergyFactor;
            //crsbar->e_chg_out = SIM_crossbar_out_cap(out_length, n_in, connect_type, trans_type, NULL) * EnergyFactor;
            //crsbar->e_chg_in = SIM_crossbar_in_cap(in_length, n_out, connect_type, trans_type, NULL) * EnergyFactor;
            /* FIXME: wire length estimation, really reset? */
            /* control signal should reset after transmission is done, so no 1/2 */
            crsbar->e_chg_ctr = SIM_crossbar_ctr_cap(ctr_length, data_width, 0, 0, 0, connect_type, trans_type) * EnergyFactor;
            crsbar->e_chg_int = 0;
            break;

        case MULTREE_CROSSBAR:
            /* input wire horizontal segment length */
            in_length = n_in * data_width * CrsbarCellWidth * (n_out / 2);
            in_wire_cap = in_length * CCmetal;
            /* input wire vertical segment length */
            in_length = n_in * data_width * (5 * Lamda) * (n_out / 2);
            in_wire_cap += in_length * CC3metal;

            ctr_length = n_in * data_width * CrsbarCellWidth * (n_out / 2) / 2;

            crsbar->e_chg_out = SIM_crossbar_out_cap(0, degree, connect_type, trans_type, NULL) * EnergyFactor;
            crsbar->e_chg_in = SIM_crossbar_in_cap(in_wire_cap, n_out, connect_type, trans_type, NULL) * EnergyFactor;
            crsbar->e_chg_int = SIM_crossbar_int_cap(degree, connect_type, trans_type) * EnergyFactor;

            /* redundant field */
            crsbar->depth = (u_int)ceil(log(n_in) / log(degree));

            /* control signal should reset after transmission is done, so no 1/2 */
            if (crsbar->depth == 1)
                /* only one level of control signal */
                crsbar->e_chg_ctr = SIM_crossbar_ctr_cap(ctr_length, data_width, 0, 0, degree, connect_type, trans_type) * EnergyFactor;
            else
            {
                /* first level and last level control signals */
                crsbar->e_chg_ctr = SIM_crossbar_ctr_cap(ctr_length, data_width, 0, 1, degree, connect_type, trans_type) * EnergyFactor +
                                    SIM_crossbar_ctr_cap(0, data_width, 1, 0, degree, connect_type, trans_type) * EnergyFactor;
                /* intermediate control signals */
                if (crsbar->depth > 2)
                    crsbar->e_chg_ctr += (crsbar->depth - 2) * SIM_crossbar_ctr_cap(0, data_width, 1, 1, degree, connect_type, trans_type) * EnergyFactor;
            }
            break;

        default:	/* some error handler */
            break;
        }

        return 0;
    }
    else
        return -1;
}


inline int SIM_crossbar_record(SIM_power_crossbar_t *xb, int io, LIB_Type_max_uint new_data, LIB_Type_max_uint old_data, u_int new_port, u_int old_port)
{
    switch (xb->model)
    {
    case MATRIX_CROSSBAR:
        if (io)	/* input port */
            xb->n_chg_in += SIM_power_Hamming(new_data, old_data, xb->mask);
        else  		/* output port */
        {
            xb->n_chg_out += SIM_power_Hamming(new_data, old_data, xb->mask);
            xb->n_chg_ctr += new_port != old_port;
        }
        break;

    case MULTREE_CROSSBAR:
        break;

    default:	/* some error handler */
        break;
    }

    return 0;
}


double SIM_crossbar_report(SIM_power_crossbar_t *crsbar, int telem)
{
    extern unsigned long long sim_cycle;
    return (telem*sim_cycle * turnoff_factor * (crsbar->e_chg_in + crsbar->e_chg_out + crsbar->e_chg_int + crsbar->e_chg_ctr) +
            crsbar->n_chg_in * crsbar->e_chg_in + crsbar->n_chg_out * crsbar->e_chg_out +
            crsbar->n_chg_int * crsbar->e_chg_int + crsbar->n_chg_ctr * crsbar->e_chg_ctr);
}

double SIM_Crossbar_Report(SIM_power_crossbar_t *crsbar)
{
    double crossbar_power;

    crossbar_power = turnoff_factor * (crsbar->e_chg_in + crsbar->e_chg_out + crsbar->e_chg_int + crsbar->e_chg_ctr) +
                     crsbar->n_chg_in * crsbar->e_chg_in + crsbar->n_chg_out * crsbar->e_chg_out +
                     crsbar->n_chg_int * crsbar->e_chg_int + crsbar->n_chg_ctr * crsbar->e_chg_ctr;
    crsbar->n_chg_in = crsbar->n_chg_out = crsbar->n_chg_int = crsbar->n_chg_ctr = 0;

    return (crossbar_power);
}

static double SIM_crossbar_stat_energy(SIM_power_crossbar_t *crsbar, int print_depth, char *path, int max_avg, double n_data)
{
    double Eavg = 0, Eatomic;
    int next_depth;
    u_int path_len;

    if (n_data > crsbar->n_out)
    {
        fprintf(stderr, "%s: overflow\n", path);
        n_data = crsbar->n_out;
    }

    next_depth = NEXT_DEPTH(print_depth);
    path_len = SIM_power_strlen(path);

    switch (crsbar->model)
    {
    case MATRIX_CROSSBAR:
    case MULTREE_CROSSBAR:
        /* assume 0.5 data switch probability */
        Eatomic = crsbar->e_chg_in * crsbar->data_width * (max_avg ? 1 : 0.5) * n_data;
        SIM_print_stat_energy(SIM_power_strcat(path, "input"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;

        Eatomic = crsbar->e_chg_out * crsbar->data_width * (max_avg ? 1 : 0.5) * n_data;
        SIM_print_stat_energy(SIM_power_strcat(path, "output"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;

        Eatomic = crsbar->e_chg_ctr * n_data;
        SIM_print_stat_energy(SIM_power_strcat(path, "control"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;

        if (crsbar->model == MULTREE_CROSSBAR && crsbar->depth > 1)
        {
            Eatomic = crsbar->e_chg_int * crsbar->data_width * (crsbar->depth - 1) * (max_avg ? 1 : 0.5) * n_data;
            SIM_print_stat_energy(SIM_power_strcat(path, "internal node"), Eatomic, next_depth);
            SIM_power_res_path(path, path_len);
            Eavg += Eatomic;
        }
        break;

    default:	/* some error handler */
        break;
    }

    SIM_print_stat_energy(path, Eavg, print_depth);

    return Eavg;
}

/*============================== crossbar ==============================*/



/*============================== arbiter ==============================*/

/* switch cap of request signal (round robin arbiter) */
static double SIM_rr_arbiter_req_cap(double length)
{
    double Ctotal = 0;

    /* part 1: gate cap of 2 NOR gates */
    /* FIXME: need actual size */
    Ctotal += 2 * SIM_power_gatecap(WdecNORn + WdecNORp, 0);

    /* part 2: inverter */
    /* FIXME: need actual size */
    Ctotal += SIM_power_draincap(Wdecinvn, NCH, 1) + SIM_power_draincap(Wdecinvp, PCH, 1) +
              SIM_power_gatecap(Wdecinvn + Wdecinvp, 0);

    /* part 3: wire cap */
    Ctotal += length * Cmetal;

    return Ctotal;
}


/* switch cap of priority signal (round robin arbiter) */
static double SIM_rr_arbiter_pri_cap()
{
    double Ctotal = 0;

    /* part 1: gate cap of NOR gate */
    /* FIXME: need actual size */
    Ctotal += SIM_power_gatecap(WdecNORn + WdecNORp, 0);

    return Ctotal;
}


/* switch cap of grant signal (round robin arbiter) */
static double SIM_rr_arbiter_grant_cap()
{
    double Ctotal = 0;

    /* part 1: drain cap of NOR gate */
    /* FIXME: need actual size */
    Ctotal += 2 * SIM_power_draincap(WdecNORn, NCH, 1) + SIM_power_draincap(WdecNORp, PCH, 2);

    return Ctotal;
}


/* switch cap of carry signal (round robin arbiter) */
static double SIM_rr_arbiter_carry_cap()
{
    double Ctotal = 0;

    /* part 1: drain cap of NOR gate (this block) */
    /* FIXME: need actual size */
    Ctotal += 2 * SIM_power_draincap(WdecNORn, NCH, 1) + SIM_power_draincap(WdecNORp, PCH, 2);

    /* part 2: gate cap of NOR gate (next block) */
    /* FIXME: need actual size */
    Ctotal += SIM_power_gatecap(WdecNORn + WdecNORp, 0);

    return Ctotal;
}


/* switch cap of internal carry node (round robin arbiter) */
static double SIM_rr_arbiter_carry_in_cap()
{
    double Ctotal = 0;

    /* part 1: gate cap of 2 NOR gates */
    /* FIXME: need actual size */
    Ctotal += 2 * SIM_power_gatecap(WdecNORn + WdecNORp, 0);

    /* part 2: drain cap of NOR gate */
    /* FIXME: need actual size */
    Ctotal += 2 * SIM_power_draincap(WdecNORn, NCH, 1) + SIM_power_draincap(WdecNORp, PCH, 2);

    return Ctotal;
}


/* the "huge" NOR gate in matrix arbiter model is an approximation */
/* switch cap of request signal (matrix arbiter) */
static double SIM_matrix_arbiter_req_cap(u_int req_width, double length)
{
    double Ctotal = 0;

    /* FIXME: all need actual sizes */
    /* part 1: gate cap of NOR gates */
    Ctotal += (req_width - 1) * SIM_power_gatecap(WdecNORn + WdecNORp, 0);

    /* part 2: inverter */
    Ctotal += SIM_power_draincap(Wdecinvn, NCH, 1) + SIM_power_draincap(Wdecinvp, PCH, 1) +
              SIM_power_gatecap(Wdecinvn + Wdecinvp, 0);

    /* part 3: gate cap of the "huge" NOR gate */
    Ctotal += SIM_power_gatecap(WdecNORn + WdecNORp, 0);

    /* part 4: wire cap */
    Ctotal += length * Cmetal;

    return Ctotal;
}


/* switch cap of priority signal (matrix arbiter) */
static double SIM_matrix_arbiter_pri_cap(u_int req_width)
{
    double Ctotal = 0;

    /* part 1: gate cap of NOR gates (2 groups) */
    Ctotal += 2 * SIM_power_gatecap(WdecNORn + WdecNORp, 0);

    /* no inverter because priority signal is kept by a flip flop */
    return Ctotal;
}


/* switch cap of grant signal (matrix arbiter) */
static double SIM_matrix_arbiter_grant_cap(u_int req_width)
{
    /* drain cap of the "huge" NOR gate */
    return (req_width * SIM_power_draincap(WdecNORn, NCH, 1) + SIM_power_draincap(WdecNORp, PCH, req_width));
}


/* switch cap of internal node (matrix arbiter) */
static double SIM_matrix_arbiter_int_cap()
{
    double Ctotal = 0;

    /* part 1: drain cap of NOR gate */
    Ctotal += 2 * SIM_power_draincap(WdecNORn, NCH, 1) + SIM_power_draincap(WdecNORp, PCH, 2);

    /* part 2: gate cap of the "huge" NOR gate */
    Ctotal += SIM_power_gatecap(WdecNORn + WdecNORp, 0);

    return Ctotal;
}


static int SIM_arbiter_clear_stat(SIM_power_arbiter_t *arb)
{
    arb->n_chg_req = arb->n_chg_grant = arb->n_chg_mint = 0;
    arb->n_chg_carry = arb->n_chg_carry_in = 0;

    SIM_array_clear_stat(&arb->queue);
    SIM_fpfp_clear_stat(&arb->pri_ff);

    return 0;
}


int SIM_arbiter_init(SIM_power_arbiter_t *arb, int arbiter_model, int ff_model, u_int req_width, double length, SIM_power_array_info_t *info)
{
    if ((arb->model = arbiter_model) && arbiter_model < ARBITER_MAX_MODEL)
    {
        arb->req_width = req_width;
        SIM_arbiter_clear_stat(arb);
        /* redundant field */
        arb->mask = HAMM_MASK(req_width);

        switch (arbiter_model)
        {
        case RR_ARBITER:
            arb->e_chg_req = SIM_rr_arbiter_req_cap(length) / 2 * EnergyFactor;
            /* two grant signals switch together, so no 1/2 */
            arb->e_chg_grant = SIM_rr_arbiter_grant_cap() * EnergyFactor;
            arb->e_chg_carry = SIM_rr_arbiter_carry_cap() / 2 * EnergyFactor;
            arb->e_chg_carry_in = SIM_rr_arbiter_carry_in_cap() / 2 * EnergyFactor;
            arb->e_chg_mint = 0;

            if (SIM_fpfp_init(&arb->pri_ff, ff_model, SIM_rr_arbiter_pri_cap()))
                return -1;
            break;

        case MATRIX_ARBITER:
            arb->e_chg_req = SIM_matrix_arbiter_req_cap(req_width, length) / 2 * EnergyFactor;
            /* 2 grant signals switch together, so no 1/2 */
            arb->e_chg_grant = SIM_matrix_arbiter_grant_cap(req_width) * EnergyFactor;
            arb->e_chg_mint = SIM_matrix_arbiter_int_cap() / 2 * EnergyFactor;
            arb->e_chg_carry = arb->e_chg_carry_in = 0;

            if (SIM_fpfp_init(&arb->pri_ff, ff_model, SIM_matrix_arbiter_pri_cap(req_width)))
                return -1;
            break;

        case QUEUE_ARBITER:
            arb->e_chg_req = arb->e_chg_grant = arb->e_chg_mint = 0;
            arb->e_chg_carry = arb->e_chg_carry_in = 0;

            return SIM_array_power_init(info, &arb->queue);
            break;

        default:	/* some error handler */
            break;
        }

        return 0;
    }
    else
        return -1;
}


inline int SIM_arbiter_record(SIM_power_arbiter_t *arb, LIB_Type_max_uint new_req, LIB_Type_max_uint old_req, u_int new_grant, u_int old_grant)
{
    switch (arb->model)
    {
    case MATRIX_ARBITER:
        arb->n_chg_req += SIM_power_Hamming(new_req, old_req, arb->mask);
        arb->n_chg_grant += new_grant != old_grant;
        /* FIXME: approximation */
        arb->n_chg_mint += (arb->req_width - 1) * arb->req_width / 2;
        /* priority registers */
        /* FIXME: use average instead */
        arb->pri_ff.n_switch += (arb->req_width - 1) / 2;
        break;

    case RR_ARBITER:
        arb->n_chg_req += SIM_power_Hamming(new_req, old_req, arb->mask);
        arb->n_chg_grant += new_grant != old_grant;
        /* FIXME: use average instead */
        arb->n_chg_carry += arb->req_width / 2;
        arb->n_chg_carry_in += arb->req_width / 2 - 1;
        /* priority registers */
        arb->pri_ff.n_switch += 2;
        break;

    case QUEUE_ARBITER:
        break;

    default:	/* some error handler */
        break;
    }

    return 0;
}


double SIM_Arbiter_Report(SIM_power_arbiter_t *arb)
{

    double arbiter_power;

    switch (arb->model)
    {
    case MATRIX_ARBITER:
    {
        arbiter_power = turnoff_factor * (arb->e_chg_req + arb->e_chg_grant +
                                          arb->e_chg_mint + arb->pri_ff.e_switch + arb->pri_ff.e_keep_1 +
                                          arb->pri_ff.e_keep_0 + arb->pri_ff.e_clock) +

                        arb->n_chg_req * arb->e_chg_req + arb->n_chg_grant * arb->e_chg_grant +
                        arb->n_chg_mint * arb->e_chg_mint +
                        arb->pri_ff.n_switch * arb->pri_ff.e_switch +
                        arb->pri_ff.n_keep_1 * arb->pri_ff.e_keep_1 +
                        arb->pri_ff.n_keep_0 * arb->pri_ff.e_keep_0 +
                        arb->pri_ff.n_clock * arb->pri_ff.e_clock;

        arb->n_chg_req = arb->n_chg_grant = arb->n_chg_mint = 0;
        arb->pri_ff.n_switch = arb->pri_ff.n_keep_1 = arb->pri_ff.n_keep_0 = arb->pri_ff.n_clock = 0;
        break;
    }

    case RR_ARBITER:
    {
        arbiter_power =  turnoff_factor * (arb->e_chg_req + arb->e_chg_grant +
                                           arb->e_chg_carry + arb->e_chg_carry_in +
                                           arb->pri_ff.e_switch + arb->pri_ff.e_keep_1 +
                                           arb->pri_ff.e_keep_0 + arb->pri_ff.e_clock) +

                         arb->n_chg_req * arb->e_chg_req + arb->n_chg_grant * arb->e_chg_grant +
                         arb->n_chg_carry * arb->e_chg_carry + arb->n_chg_carry_in * arb->e_chg_carry_in +
                         arb->pri_ff.n_switch * arb->pri_ff.e_switch +
                         arb->pri_ff.n_keep_1 * arb->pri_ff.e_keep_1 +
                         arb->pri_ff.n_keep_0 * arb->pri_ff.e_keep_0 +
                         arb->pri_ff.n_clock * arb->pri_ff.e_clock;

        arb->n_chg_req = arb->n_chg_grant = arb->n_chg_carry = arb->n_chg_carry_in = 0;
        arb->pri_ff.n_switch = arb->pri_ff.n_keep_1 = arb->pri_ff.n_keep_0 = arb->pri_ff.n_clock = 0;
        break;
    }

    default:
    {
        arbiter_power = 0.0;
        break;
    }

    }

    return (arbiter_power);
}

double SIM_arbiter_report(SIM_power_arbiter_t *arb, int telem)
{
    extern unsigned long long sim_cycle;

    switch (arb->model)
    {
    case MATRIX_ARBITER:
        return (telem*sim_cycle * turnoff_factor * (arb->e_chg_req + arb->e_chg_grant +
                arb->e_chg_mint + arb->pri_ff.e_switch + arb->pri_ff.e_keep_1 +
                arb->pri_ff.e_keep_0 + arb->pri_ff.e_clock) +

                arb->n_chg_req * arb->e_chg_req + arb->n_chg_grant * arb->e_chg_grant +
                arb->n_chg_mint * arb->e_chg_mint +
                arb->pri_ff.n_switch * arb->pri_ff.e_switch +
                arb->pri_ff.n_keep_1 * arb->pri_ff.e_keep_1 +
                arb->pri_ff.n_keep_0 * arb->pri_ff.e_keep_0 +
                arb->pri_ff.n_clock * arb->pri_ff.e_clock);

    case RR_ARBITER:
        return (telem*sim_cycle * turnoff_factor * (arb->e_chg_req + arb->e_chg_grant +
                arb->e_chg_carry + arb->e_chg_carry_in +
                arb->pri_ff.e_switch + arb->pri_ff.e_keep_1 +
                arb->pri_ff.e_keep_0 + arb->pri_ff.e_clock) +

                arb->n_chg_req * arb->e_chg_req + arb->n_chg_grant * arb->e_chg_grant +
                arb->n_chg_carry * arb->e_chg_carry + arb->n_chg_carry_in * arb->e_chg_carry_in +
                arb->pri_ff.n_switch * arb->pri_ff.e_switch +
                arb->pri_ff.n_keep_1 * arb->pri_ff.e_keep_1 +
                arb->pri_ff.n_keep_0 * arb->pri_ff.e_keep_0 +
                arb->pri_ff.n_clock * arb->pri_ff.e_clock);

    default:
        return -1;
    }
}

/* stat over one cycle */
/* info is only used by queuing arbiter */
static double SIM_arbiter_stat_energy(SIM_power_arbiter_t *arb, SIM_power_array_info_t *info, double n_req, int print_depth, char *path, int max_avg)
{
    double Eavg = 0, Estruct, Eatomic;
    int next_depth, next_next_depth;
    double total_pri, n_chg_pri, n_grant;
    u_int path_len, next_path_len;

    next_depth = NEXT_DEPTH(print_depth);
    next_next_depth = NEXT_DEPTH(next_depth);
    path_len = SIM_power_strlen(path);

    /* energy cycle distribution */
    if (n_req > arb->req_width)
    {
        fprintf(stderr, "arbiter overflow\n");
        n_req = arb->req_width;
    }
    if (n_req >= 1) n_grant = 1;
    else n_grant = 1.0 / ceil(1.0 / n_req);

    switch (arb->model)
    {
    case RR_ARBITER:
        /* FIXME: we may overestimate request switch */
        Eatomic = arb->e_chg_req * n_req;
        SIM_print_stat_energy(SIM_power_strcat(path, "request"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;

        Eatomic = arb->e_chg_grant * n_grant;
        SIM_print_stat_energy(SIM_power_strcat(path, "grant"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;

        /* assume carry signal propagates half length in average case */
        /* carry does not propagate in maximum case, i.e. all carrys go down */
        Eatomic = arb->e_chg_carry * arb->req_width * (max_avg ? 1 : 0.5) * n_grant;
        SIM_print_stat_energy(SIM_power_strcat(path, "carry"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;

        Eatomic = arb->e_chg_carry_in * (arb->req_width * (max_avg ? 1 : 0.5) - 1) * n_grant;
        SIM_print_stat_energy(SIM_power_strcat(path, "internal carry"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;

        /* priority registers */
        Estruct = 0;
        SIM_power_strcat(path, "priority");
        next_path_len = SIM_power_strlen(path);

        Eatomic = arb->pri_ff.e_switch * 2 * n_grant;
        SIM_print_stat_energy(SIM_power_strcat(path, "switch"), Eatomic, next_next_depth);
        SIM_power_res_path(path, next_path_len);
        Estruct += Eatomic;

        Eatomic = arb->pri_ff.e_keep_0 * (arb->req_width - 2 * n_grant);
        SIM_print_stat_energy(SIM_power_strcat(path, "keep 0"), Eatomic, next_next_depth);
        SIM_power_res_path(path, next_path_len);
        Estruct += Eatomic;

        Eatomic = arb->pri_ff.e_clock * arb->req_width;
        SIM_print_stat_energy(SIM_power_strcat(path, "clock"), Eatomic, next_next_depth);
        SIM_power_res_path(path, next_path_len);
        Estruct += Eatomic;

        SIM_print_stat_energy(path, Estruct, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Estruct;
        break;

    case MATRIX_ARBITER:
        total_pri = arb->req_width * (arb->req_width - 1) * 0.5;
        /* assume switch probability 0.5 for priorities */
        n_chg_pri = (arb->req_width - 1) * (max_avg ? 1 : 0.5);

        /* FIXME: we may overestimate request switch */
        Eatomic = arb->e_chg_req * n_req;
        SIM_print_stat_energy(SIM_power_strcat(path, "request"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;

        Eatomic = arb->e_chg_grant * n_grant;
        SIM_print_stat_energy(SIM_power_strcat(path, "grant"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;

        /* priority registers */
        Estruct = 0;
        SIM_power_strcat(path, "priority");
        next_path_len = SIM_power_strlen(path);

        Eatomic = arb->pri_ff.e_switch * n_chg_pri * n_grant;
        SIM_print_stat_energy(SIM_power_strcat(path, "switch"), Eatomic, next_next_depth);
        SIM_power_res_path(path, next_path_len);
        Estruct += Eatomic;

        /* assume 1 and 0 are uniformly distributed */
        if (arb->pri_ff.e_keep_0 >= arb->pri_ff.e_keep_1 || !max_avg)
        {
            Eatomic = arb->pri_ff.e_keep_0 * (total_pri - n_chg_pri * n_grant) * (max_avg ? 1 : 0.5);
            SIM_print_stat_energy(SIM_power_strcat(path, "keep 0"), Eatomic, next_next_depth);
            SIM_power_res_path(path, next_path_len);
            Estruct += Eatomic;
        }

        if (arb->pri_ff.e_keep_0 < arb->pri_ff.e_keep_1 || !max_avg)
        {
            Eatomic = arb->pri_ff.e_keep_1 * (total_pri - n_chg_pri * n_grant) * (max_avg ? 1 : 0.5);
            SIM_print_stat_energy(SIM_power_strcat(path, "keep 1"), Eatomic, next_next_depth);
            SIM_power_res_path(path, next_path_len);
            Estruct += Eatomic;
        }

        Eatomic = arb->pri_ff.e_clock * total_pri;
        SIM_print_stat_energy(SIM_power_strcat(path, "clock"), Eatomic, next_next_depth);
        SIM_power_res_path(path, next_path_len);
        Estruct += Eatomic;

        SIM_print_stat_energy(path, Estruct, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Estruct;

        /* based on above assumptions */
        if (max_avg)
            /* min(p,n/2)(n-1) + 2(n-1) */
            Eatomic = arb->e_chg_mint * (MIN(n_req, arb->req_width * 0.5) + 2) * (arb->req_width - 1);
        else
            /* p(n-1)/2 + (n-1)/2 */
            Eatomic = arb->e_chg_mint * (n_req + 1) * (arb->req_width - 1) * 0.5;
        SIM_print_stat_energy(SIM_power_strcat(path, "internal node"), Eatomic, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Eatomic;
        break;

    case QUEUE_ARBITER:
        /* FIXME: what if n_req > 1? */
        Eavg = SIM_reg_stat_energy(info, &arb->queue, n_req, n_grant, next_depth, SIM_power_strcat(path, "queue"), max_avg);
        SIM_power_res_path(path, path_len);
        break;

    default:	/* some error handler */
        break;
    }

    SIM_print_stat_energy(path, Eavg, print_depth);

    return Eavg;
}

/*============================== arbiter ==============================*/



/* FIXME: request wire length estimation is ad hoc */
int SIM_router_power_init(SIM_power_router_info_t *info, SIM_power_router_t *router)
{
    double cbuf_width, req_len = 0;

    /* initialize crossbar */
    if (info->crossbar_model)
        SIM_crossbar_init(&router->crossbar, info->crossbar_model, info->n_switch_in, info->n_switch_out, info->flit_width, info->degree, info->connect_type, info->trans_type, info->crossbar_in_len, info->crossbar_out_len, &req_len);

    /* initialize various buffers */
    if (info->in_buf)
        SIM_array_power_init(&info->in_buf_info, &router->in_buf);
    if (info->cache_in_buf)
        SIM_array_power_init(&info->cache_in_buf_info, &router->cache_in_buf);
    if (info->mc_in_buf)
        SIM_array_power_init(&info->mc_in_buf_info, &router->mc_in_buf);
    if (info->io_in_buf)
        SIM_array_power_init(&info->io_in_buf_info, &router->io_in_buf);
    if (info->out_buf)
        SIM_array_power_init(&info->out_buf_info, &router->out_buf);
    if (info->central_buf)
    {
        /* MUST initialize array before crossbar because the latter needs array width */
        SIM_array_power_init(&info->central_buf_info, &router->central_buf);

        /* WHS: use MATRIX_CROSSBAR, info->connect_type, info->trans_type rather than specifying new parameters */
        cbuf_width = info->central_buf_info.data_arr_width + info->central_buf_info.tag_arr_width;
        req_len = info->central_buf_info.data_arr_height;

        SIM_crossbar_init(&router->in_cbuf_crsbar, MATRIX_CROSSBAR, info->n_switch_in, info->pipe_depth * info->central_buf_info.write_ports, info->flit_width, 0, info->connect_type, info->trans_type, cbuf_width, 0, NULL);
        SIM_crossbar_init(&router->out_cbuf_crsbar, MATRIX_CROSSBAR, info->pipe_depth * info->central_buf_info.read_ports, info->n_switch_out, info->flit_width, 0, info->connect_type, info->trans_type, 0, cbuf_width, NULL);

        /* dirty hack */
        SIM_fpfp_init(&router->cbuf_ff, info->cbuf_ff_model, 0);
    }

    /* initialize arbiter */
    if (info->in_arb_model)
    {
        SIM_arbiter_init(&router->in_arb, info->in_arb_model, info->in_arb_ff_model, info->n_v_class, 0, &info->in_arb_queue_info);
        if (info->n_cache_in)
            SIM_arbiter_init(&router->cache_in_arb, info->in_arb_model, info->in_arb_ff_model, info->cache_class, 0, &info->cache_in_arb_queue_info);
        if (info->n_mc_in)
            SIM_arbiter_init(&router->mc_in_arb, info->in_arb_model, info->in_arb_ff_model, info->mc_class, 0, &info->mc_in_arb_queue_info);
        if (info->n_io_in)
            SIM_arbiter_init(&router->io_in_arb, info->in_arb_model, info->in_arb_ff_model, info->io_class, 0, &info->io_in_arb_queue_info);
    }

    /* WHS: must after switch initialization */
    if (info->out_arb_model)
    {
        SIM_arbiter_init(&router->out_arb, info->out_arb_model, info->out_arb_ff_model, info->n_total_in - 1, req_len, &info->out_arb_queue_info);
    }

    return 0;
}


/* THIS FUNCTION IS OBSOLETE */
int SIM_router_power_report(SIM_power_router_info_t *info, SIM_power_router_t *router)
{
    double epart, etotal = 0;

    if (info->crossbar_model)
    {
        epart = SIM_crossbar_report(&router->crossbar, 1);
        fprintf(stderr, "switch: %g\n", epart);
        etotal += epart;
    }

    fprintf(stderr, "total energy: %g\n", etotal);

    return 0;
}


/*
 * time unit:	1 cycle
 * e_fin:	average # of flits received by one input port during unit time
 * 		  (at most 0.5 for InfiniBand router)
 * e_buf_wrt:	average # of input buffer writes of all ports during unit time
 * 		e_buf_wrt = e_fin * n_buf_in
 * e_buf_rd:	average # of input buffer reads of all ports during unit time
 * 		e_buf_rd = e_buf_wrt
 * 		  (splitted into different input ports in program)
 * e_cbuf_fin:	average # of flits passing through the switch during unit time
 * 		e_cbuf_fin = e_fin * n_total_in
 * e_cbuf_wrt:	average # of central buffer writes during unit time
 * 		e_cbuf_wrt = e_cbuf_fin / (pipe_depth * pipe_width)
 * e_cbuf_rd:	average # of central buffer reads during unit time
 * 		e_cbuf_rd = e_cbuf_wrt
 * e_arb:	average # of arbitrations per arbiter during unit time
 * 		assume e_arb = 1
 *
 * NOTES: (1) negative print_depth means infinite print depth
 *
 * FIXME: (1) hack: SIM_reg_stat_energy cannot be used for shared buffer,
 *            we use it now anyway
 */
double SIM_router_stat_energy(SIM_power_router_info_t *info, SIM_power_router_t *router, int print_depth, char *path, int max_avg, double e_fin, int plot_flag, double freq)
{
    double Eavg = 0, Eatomic, Estruct;
    double Pbuf, Pswitch, Parbiter, Ptotal;
    double e_in_buf_rw, e_cache_in_buf_rw, e_mc_in_buf_rw, e_io_in_buf_rw;
    double e_cbuf_fin, e_cbuf_rw, e_out_buf_rw;
    int next_depth;
    u_int path_len, n_regs;

    /* expected value computation */
    e_in_buf_rw       = e_fin * info->n_in;
    e_cache_in_buf_rw = e_fin * info->n_cache_in;
    e_mc_in_buf_rw    = e_fin * info->n_mc_in;
    e_io_in_buf_rw    = e_fin * info->n_io_in;
    e_cbuf_fin        = e_fin * info->n_total_in;
    e_out_buf_rw      = e_cbuf_fin / info->n_total_out * info->n_out;
    e_cbuf_rw         = e_cbuf_fin * info->flit_width / info->central_buf_info.blk_bits;

    next_depth = NEXT_DEPTH(print_depth);
    path_len = SIM_power_strlen(path);

    /* input buffers */
    if (info->in_buf)
    {
        Eavg += SIM_reg_stat_energy(&info->in_buf_info, &router->in_buf, e_in_buf_rw, e_in_buf_rw, next_depth, SIM_power_strcat(path, "input buffer"), max_avg);
        SIM_power_res_path(path, path_len);
    }
    if (info->cache_in_buf)
    {
        Eavg += SIM_reg_stat_energy(&info->cache_in_buf_info, &router->cache_in_buf, e_cache_in_buf_rw, e_cache_in_buf_rw, next_depth, SIM_power_strcat(path, "cache input buffer"), max_avg);
        SIM_power_res_path(path, path_len);
    }
    if (info->mc_in_buf)
    {
        Eavg += SIM_reg_stat_energy(&info->mc_in_buf_info, &router->mc_in_buf, e_mc_in_buf_rw, e_mc_in_buf_rw, next_depth, SIM_power_strcat(path, "memory controller input buffer"), max_avg);
        SIM_power_res_path(path, path_len);
    }
    if (info->io_in_buf)
    {
        Eavg += SIM_reg_stat_energy(&info->io_in_buf_info, &router->io_in_buf, e_io_in_buf_rw, e_io_in_buf_rw, next_depth, SIM_power_strcat(path, "I/O input buffer"), max_avg);
        SIM_power_res_path(path, path_len);
    }

    /* output buffers */
    if (info->out_buf)
    {
        /* local output ports don't use router buffers */
        Eavg += SIM_reg_stat_energy(&info->out_buf_info, &router->out_buf, e_out_buf_rw, e_out_buf_rw, next_depth, SIM_power_strcat(path, "output buffer"), max_avg);
        SIM_power_res_path(path, path_len);
    }

    Pbuf = Eavg * freq;

    /* main crossbar */
    if (info->crossbar_model)
    {
        Eavg += SIM_crossbar_stat_energy(&router->crossbar, next_depth, SIM_power_strcat(path, "crossbar"), max_avg, e_cbuf_fin);
        SIM_power_res_path(path, path_len);
    }

    /* central buffer */
    if (info->central_buf)
    {
        Eavg += SIM_reg_stat_energy(&info->central_buf_info, &router->central_buf, e_cbuf_rw, e_cbuf_rw, next_depth, SIM_power_strcat(path, "central buffer"), max_avg);
        SIM_power_res_path(path, path_len);

        Eavg += SIM_crossbar_stat_energy(&router->in_cbuf_crsbar, next_depth, SIM_power_strcat(path, "central buffer input crossbar"), max_avg, e_cbuf_fin);
        SIM_power_res_path(path, path_len);

        Eavg += SIM_crossbar_stat_energy(&router->out_cbuf_crsbar, next_depth, SIM_power_strcat(path, "central buffer output crossbar"), max_avg, e_cbuf_fin);
        SIM_power_res_path(path, path_len);

        /* dirty hack, REMEMBER to REMOVE Estruct and Eatomic */
        Estruct = 0;
        n_regs = info->central_buf_info.n_set * (info->central_buf_info.read_ports + info->central_buf_info.write_ports);

        /* ignore e_switch for now because we overestimate wordline driver cap */

        Eatomic = router->cbuf_ff.e_keep_0 * (info->pipe_depth - 1) * (n_regs - 2 * (e_cbuf_rw + e_cbuf_rw));
        SIM_print_stat_energy(SIM_power_strcat(path, "central buffer pipeline registers/keep 0"), Eatomic, NEXT_DEPTH(next_depth));
        SIM_power_res_path(path, path_len);
        Estruct += Eatomic;

        Eatomic = router->cbuf_ff.e_clock * (info->pipe_depth - 1) * n_regs;
        SIM_print_stat_energy(SIM_power_strcat(path, "central buffer pipeline registers/clock"), Eatomic, NEXT_DEPTH(next_depth));
        SIM_power_res_path(path, path_len);
        Estruct += Eatomic;

        SIM_print_stat_energy(SIM_power_strcat(path, "central buffer pipeline registers"), Estruct, next_depth);
        SIM_power_res_path(path, path_len);
        Eavg += Estruct;
    }

    Pswitch = Eavg * freq - Pbuf;

    /* input (local) arbiter */
    if (info->in_arb_model)
    {
        Eavg += SIM_arbiter_stat_energy(&router->in_arb, &info->in_arb_queue_info, e_fin / info->in_n_switch, next_depth, SIM_power_strcat(path, "input arbiter"), max_avg) * info->in_n_switch * info->n_in;
        SIM_power_res_path(path, path_len);

        if (info->n_cache_in)
        {
            Eavg += SIM_arbiter_stat_energy(&router->cache_in_arb, &info->cache_in_arb_queue_info, e_fin / info->cache_n_switch, next_depth, SIM_power_strcat(path, "cache input arbiter"), max_avg) * info->cache_n_switch * info->n_cache_in;
            SIM_power_res_path(path, path_len);
        }

        if (info->n_mc_in)
        {
            Eavg += SIM_arbiter_stat_energy(&router->mc_in_arb, &info->mc_in_arb_queue_info, e_fin / info->mc_n_switch, next_depth, SIM_power_strcat(path, "memory controller input arbiter"), max_avg) * info->mc_n_switch * info->n_mc_in;
            SIM_power_res_path(path, path_len);
        }

        if (info->n_io_in)
        {
            Eavg += SIM_arbiter_stat_energy(&router->io_in_arb, &info->io_in_arb_queue_info, e_fin / info->io_n_switch, next_depth, SIM_power_strcat(path, "I/O input arbiter"), max_avg) * info->io_n_switch * info->n_io_in;
            SIM_power_res_path(path, path_len);
        }
    }

    /* output (global) arbiter */
    if (info->out_arb_model)
    {
        Eavg += SIM_arbiter_stat_energy(&router->out_arb, &info->out_arb_queue_info, e_cbuf_fin / info->n_switch_out, next_depth, SIM_power_strcat(path, "output arbiter"), max_avg) * info->n_switch_out;
        SIM_power_res_path(path, path_len);
    }

    Ptotal = Eavg * freq;
    Parbiter = Ptotal - Pbuf - Pswitch;

    SIM_print_stat_energy(path, Eavg, print_depth);

    if (plot_flag)
        fprintf(stderr, "%g\t%g\t%g\t%g\t%g\t%g\t%g\n", Pbuf, Pswitch, Parbiter, Ptotal, Pbuf / Ptotal, Pswitch / Ptotal, Parbiter / Ptotal);

    return Eavg;
}
