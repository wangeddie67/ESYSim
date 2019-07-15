/*
 * File name : esynet_random_unit.cc
 * Function : Implement random number generator.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Copyright (C) 2017, Junshi Wang <wangeddie67@gmail.com>
 */

/**
 * @ingroup ESYNET_RANDOM
 * @file esynet_random_unit.cc
 * @brief Implement random number generator.
 */

#include "esynet_random_unit.h"

EsynetSRGen * EsynetSRGen::mp_global_pointer = 0;

double EsynetSRGen::PI = 3.141592658979323846;

EsynetSRGen::EsynetSRGen()
    : m_idum( 1 )
{
    mp_global_pointer = this;
}

EsynetSRGen::EsynetSRGen( long a )
    : m_idum( a )
{
    mp_global_pointer = this;
}

void EsynetSRGen::setSeed( long a )
{
    m_idum = a;
}

double EsynetSRGen::sflat01()
{
    const long IM1 = 2147483563, IM2 = 2147483399;
    const long IA1 = 40014, IA2 = 40692, IQ1 = 53668, IQ2 = 52774;
    const long IR1 = 12211, IR2 = 3791, NTAB = 32, IMM1 = IM1 - 1;
    const long NDIV = 1 + IMM1 / NTAB;
    const double EPS = 3.0e-16, RNMX = 1.0 - EPS, AM = 1.0 / double( IM1 );
    static int idum2 = 123456789, iy = 0;
    static std::vector< int > iv( NTAB );
    int j, k;
    double temp;

    if ( m_idum <= 0 )
    {
        m_idum = (m_idum == 0 ? 1 : - m_idum );
        idum2 = m_idum;
        for ( j = NTAB + 7; j >= 0; j -- )
        {
            k = m_idum / IQ1;
            m_idum = IA1 * ( m_idum - k * IQ1 ) - k * IR1;
            if ( m_idum < 0 )
            {
                m_idum += IM1;
            }
            if ( j < NTAB )
            {
                iv[ j ] = m_idum;
            }
        }
        iy = iv[ 0 ];
    }
    k = m_idum / IQ1;
    m_idum = IA1 * ( m_idum - k * IQ1 ) - k * IR1;
    if ( m_idum < 0 )
    {
        m_idum += IM1;
    }
    k = idum2 / IQ2;
    idum2 = IA2 * ( idum2 - k * IQ2 ) - k * IR2;
    if ( idum2 < 0 )
    {
        idum2 += IM2;
    }
    j = iy / NDIV;
    iy = iv[ j ] - idum2;
    iv[ j ] = m_idum;
    if ( iy < 1 )
    {
        iy += IMM1;
    }
    if ( (temp = AM * iy ) > RNMX )
    {
        return RNMX;
    }
    else
    {
        return temp;
    }
}

double EsynetSRGen::flatDouble( double low, double high )
{
    assert( low < high );
    double val = ( high - low ) * sflat01() + low;
    assert( val >= low && val < high );
    return val;
}

long EsynetSRGen::flatLong( long low, long high )
{
    assert( low < high );
    long val = (long)( ( high - low ) * sflat01() + low );
    assert( val >= low && val < high );
    return val;
}

unsigned long EsynetSRGen::flatUnsignedLong( unsigned long low, unsigned long high )
{
    assert( low < high );
    unsigned long val = (unsigned long)( ( high - low ) * sflat01() + low );
    assert( val >= low && val < high );
    return val;
}

unsigned long long EsynetSRGen::flatUnsignedLongLong( unsigned long long low, unsigned long long high )
{
    assert( low < high );
    unsigned long long val = (unsigned long long)( ( high - low ) * sflat01() + low );
    assert( val >= low && val < high );
    return val;
}

double EsynetSRGen::gauss01()
{
    static int recalc = 1;
    double in_a, in_b;
    double out_a;
    static double out_b;
    double modifier;
    double compile_b;

    if( recalc )
    {
        in_a = 1.0 - sflat01();
        in_b = sflat01();

        modifier = sqrt( -2.0 * log( in_a ) );
        compile_b = 2.0 * PI * in_b;

        out_a = modifier * cos( compile_b );
        out_b = modifier * sin( compile_b );

        recalc = 0;
        return out_b;
    }

    recalc = 1;
    return out_b;
}

double EsynetSRGen::gaussDouble( double mean, double variance )
{
    double ret = gauss01() * variance + mean;
    return ret;
}

long EsynetSRGen::gaussLong( long mean, double variance )
{
    return (long)( gauss01() * variance + mean );
}

unsigned long EsynetSRGen::gaussUnsignedLong( unsigned long mean, double variance )
{
    return (unsigned long)( gauss01() * variance + mean );
}

unsigned long long EsynetSRGen::gaussUnsignedLongLong( unsigned long long mean, double variance )
{
    return (unsigned long long)( gauss01() * variance + mean );
}

#define EsynetSRGenFlatDefine( Type, Fun) \
Type EsynetSRGenFlat##Fun( Type low, Type high ) \
{ \
    return EsynetSRGen::globalPointer().flat##Fun( low, high ); \
}
EsynetSRGenFlatDefine( double, Double )
EsynetSRGenFlatDefine( long, Long )
EsynetSRGenFlatDefine( unsigned long, UnsignedLong )
EsynetSRGenFlatDefine( unsigned long long, UnsignedLongLong )
#undef EsynetSRGenFlatDefine

#define EsynetSRGenGaussMeanDefine( Type, Fun ) \
Type EsynetSRGenGaussMean##Fun( Type mean, double variance ) \
{ \
    return EsynetSRGen::globalPointer().gauss##Fun( mean, variance ); \
}
EsynetSRGenGaussMeanDefine( double, Double )
EsynetSRGenGaussMeanDefine( long, Long )
EsynetSRGenGaussMeanDefine( unsigned long, UnsignedLong )
EsynetSRGenGaussMeanDefine( unsigned long long, UnsignedLongLong )
#undef EsynetSRGenGaussMeanDefine
