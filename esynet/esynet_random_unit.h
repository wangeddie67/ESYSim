/*
 * File name : esynet_random_unit.h
 * Function : Declaire random number generator.
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

#ifndef ESYNET_RANDOM_UNIT_H
#define ESYNET_RANDOM_UNIT_H

#include <cmath>
#include <cstdio>
#include <cassert>
#include <vector>

/** @defgroup randomnumber Generation random number.
 * @{
 */
/*!
 * @brief Random number generator.
 * 
 * Generate random number by algorithm. Which is much uniform than rand() 
 * function provide by C library.
 * 
 * For one program, there should be only one entity of SRGen. Program should 
 * access this entity by global pointer #globalPointer().
 * 
 * Some global functions are provided to access the random number directly.
 */
class EsynetSRGen
{
protected:
    long m_idum;        /*!< @brief Last value of state machine. */
    static double PI;   /*!< @brief Static value of \f$\pi\f$ */
    static EsynetSRGen * mp_global_pointer; /*!< @brief Global pointer to this entity. */

public:
    /**
     * @name Constructor and destructor
     * @{
     */
    /**
     * @brief Generate an entity with specified seed.
     * @param a  Specified seed.
     */
    EsynetSRGen( long a );
    /**
     * @brief Generate an entity with default seed.
     */
    EsynetSRGen();
    /**
     * @brief Destructor
     */
    ~EsynetSRGen() { mp_global_pointer = 0; }
    /**
     * @}
     */

    /**
     * @name Function to access variables
     * @{
     */
    /**
     * @brief Access global pointer to this entity #mp_global_pointer
     * @return reference of this entity.
     */
    static EsynetSRGen & globalPointer() { return *mp_global_pointer; }
    /**
     * @brief Set seed of random generator #m_seed.
     * @param a  seed of random generator #m_seed.
     */
    void setSeed( long a );
    /**
     * @}
     */

    /**
     * @name Functions to generate flat/uniform distribution number
     * @{
     */
    /**
     * @brief Generate random number following fault/unifom distribution in range [low, high) in double format.
     * @param low  low boundary of random number, included.
     * @param high high boundary of random number, not included.
     * @return  random number in double format.
     */
    double flatDouble( double low, double high );
    /**
     * @brief Generate random number following fault/unifom distribution in range [low, high) in long format.
     * @param low  low boundary of random number, included.
     * @param high high boundary of random number, not included.
     * @return  random number in long format.
     */
    long flatLong( long low, long high );
    /**
     * @brief Generate random number following fault/unifom distribution in range [low, high) in unsigned long format.
     * @param low  low boundary of random number, included.
     * @param high high boundary of random number, not included.
     * @return  random number in unsigned long format.
     */
    unsigned long flatUnsignedLong( unsigned long low, unsigned long high );
    /**
     * @brief Generate random number following fault/unifom distribution in range [low, high) in unsigned long long format.
     * @param low  low boundary of random number, included.
     * @param high high boundary of random number, not included.
     * @return  random number in unsigned long long format.
     */
    unsigned long long flatUnsignedLongLong( unsigned long long low, unsigned long long high);
    /**
     * @}
     */

    /**
     * @name Functions to generate gauss distribution number
     * @{
     */
    /**
     * @brief Generate random number following gauss distribution with specified mean and variance in double format.
     * @param mean     mean of gauss distribution.
     * @param variance variance of gaussdistribution.
     * @return  random number in double format.
     */
    double gaussDouble( double mean, double variance );
    /**
     * @brief Generate random number following gauss distribution with specified mean and variance in long format.
     * @param mean     mean of gauss distribution.
     * @param variance variance of gaussdistribution.
     * @return  random number in long format.
     */
    long gaussLong( long mean, double variance );
    /**
     * @brief Generate random number following gauss distribution with specified mean and variance in unsigned long format.
     * @param mean     mean of gauss distribution.
     * @param variance variance of gaussdistribution.
     * @return  random number in unsigned long format.
     */
    unsigned long gaussUnsignedLong( unsigned long mean, double variance );
    /**
     * @brief Generate random number following gauss distribution with specified mean and variance in unsigned long long format.
     * @param mean     mean of gauss distribution.
     * @param variance variance of gaussdistribution.
     * @return  random number in unsigned long long format.
     */
    unsigned long long gaussUnsignedLongLong( unsigned long long mean, double variance );
    /**
     * @}
     */

protected:
    /**
     * @brief Generate random number following fault/unifom distribution in range [0, 1) in double format.
     * @return  random number.
     */
    double sflat01();
    /**
     * @brief Generate random number following gauss distribution with mean of 0 and variance of 1 in double format.
     * @return  random number.
     */
    double gauss01();
};

/**
 * @name Global functions to get flat/uniform distribution number
 * @{
 */
/**
 * @brief Generate random number following fault/unifom distribution in range [low, high) in double format.
 * @param low  low boundary of random number, included.
 * @param high high boundary of random number, not included.
 * @return  random number in double format.
 */
double EsynetSRGenFlatDouble(double low, double high);
/**
 * @brief Generate random number following fault/unifom distribution in range [low, high) in long format.
 * @param low  low boundary of random number, included.
 * @param high high boundary of random number, not included.
 * @return  random number in long format.
 */
long EsynetSRGenFlatLong(long low, long high);
/**
 * @brief Generate random number following fault/unifom distribution in range [low, high) in unsigned long format.
 * @param low  low boundary of random number, included.
 * @param high high boundary of random number, not included.
 * @return  random number in unsigned long format.
 */
unsigned long EsynetSRGenFlatUnsignedLong( unsigned long low, unsigned long high );
/**
 * @brief Generate random number following fault/unifom distribution in range [low, high) in unsigned long long format.
 * @param low  low boundary of random number, included.
 * @param high high boundary of random number, not included.
 * @return  random number in unsigned long long format.
 */
unsigned long long EsynetSRGenFlatUnsignedLongLong( unsigned long long low, unsigned long long high );
/**
 * @}
 */

/**
 * @name Global functions to generate gauss distribution number
 * @{
 */
/**
 * @brief Generate random number following gauss distribution with specified mean and variance in double format.
 * @param mean     mean of gauss distribution.
 * @param variance variance of gaussdistribution.
 * @return  random number in double format.
 */
double EsynetSRGenGaussDouble( double mean, double variance );
/**
 * @brief Generate random number following gauss distribution with specified mean and variance in long format.
 * @param mean     mean of gauss distribution.
 * @param variance variance of gaussdistribution.
 * @return  random number in long format.
 */
long EsynetSRGenGaussLong( long mean, double variance );
/**
 * @brief Generate random number following gauss distribution with specified mean and variance in unsigned long format.
 * @param mean     mean of gauss distribution.
 * @param variance variance of gaussdistribution.
 * @return  random number in unsigned long format.
 */
unsigned long EsynetSRGenGaussUnsignedLong( unsigned long mean, double variance );
/**
 * @brief Generate random number following gauss distribution with specified mean and variance in unsigned long long format.
 * @param mean     mean of gauss distribution.
 * @param variance variance of gaussdistribution.
 * @return  random number in unsigned long long format.
 */
unsigned long long EsynetSRGenGaussUnsignedLongLong( unsigned long long mean, double variance );
/**
 * @}
 */

#endif
