/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA  02110-1301, USA.

---
Copyright (C) 2015, Junshi Wang <>
*/

#ifndef ESYNET_ECC_UNIT_H
#define ESYNET_ECC_UNIT_H

#include "esynet_fault_unit.h"
#include "esynet_flit.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace esynet;

/** @defgroup eccunit Error Correction Coding units */

/**
 * @ingroup eccunit
 * @brief Buffer unit for Error Correction Coding units.
 *
 * This class models the behavior of buffer in ECC. This buffer should add one
 * more cycle on the pipeline.
 */
class EsynetECCBufferUnit
{
protected:
    bool m_ecc_enable;  /**< \brief Enable ecc.

        Access function: #eccEnable(), #setECCEnable(), #clearECCEnable() */
    bool m_fault_injection_enable; /**< \brief Enable fault injection on ecc. */

    bool m_buf_state; /**< \brief Flag that one flit is in buffer or not.

        Access function: #bufferState() */
	EsynetFlit m_ecc_buffer; /**< \brief One flit buffer for ecc unit. */

    long m_ecc_word_width;  /**< \brief Width of ecc word after coding. */
    long m_ecc_correct_capacity; /**< \brief Correctable error bit of ECC. */
    long m_ecc_detect_capacity; /**< \brief Detectable error bit by ECC. */

    long m_vc; /**< \brief Virtual channel which the flit should be delivered
        to.

        Access function: #vcToGo(), #setVCToGo() */
    
    long m_data_width;  /**< \brief Width of data path after encoding. */

public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Construct an empty entity
     */
	EsynetECCBufferUnit() :
        m_ecc_enable( false ), m_fault_injection_enable( false ),
        m_buf_state( false ), m_ecc_buffer(),
        m_ecc_word_width( 1 ), 
        m_ecc_correct_capacity( 1 ), m_ecc_detect_capacity( 1 ), 
        m_vc( 0 ), m_data_width( 1 )
    {}
    /*!
     * \brief Construct an entity with specified configuration.
     * \param ecc_name   name of ecc coding scheme.
     * \param data_width  width of data path after coding.
     * \param ecc_enable  enable ecc.
     * \param fault_injection_enable enable fault injection in ecc unit.
     */
	EsynetECCBufferUnit(ECCMethod ecc_name, long data_width,
        bool ecc_enable, bool fault_injection_enable );
    ///@}
    ///
    /** @name Functions to access variables */
    /*!
     * \brief Access #m_ecc_enable.
     * \return #m_ecc_enable.
     */
    bool eccEnable() const { return m_ecc_enable; }
    /*!
     * \brief Enable ECC by seting #m_ecc_enable to true.
     */
    void setECCEnable() { m_ecc_enable = true; }
    /*!
     * \brief Disable ECC by seting #m_ecc_enable to false.
     */
    void clearECCEnable() { m_ecc_enable = false; }
    /*!
     * \brief Access #m_buf_state.
     * \return #m_buf_state.
     */
    bool bufferState() const { return m_buf_state; }
    /*!
     * \brief Access virtual channel of flit #m_vc.
     * \return #m_vc.
     */
    long vcToGo() const { return m_vc; }
    /*!
     * \brief Set virtual channel of flit #m_vc.
     * \param vc id of virtual channel.
     */
    void setVCToGo( long vc ) { m_vc = vc; } 
    ///@}

    /*!
     * \brief Correct faults in the flit according to ECC.
     *
     * The fault pattern of flit is stored in EsynetFlit::m_fault_pattern. In this
     * function, EsynetFlit::m_fault_pattern will be checked bit by bit.
     * - If the number of error bits in one group is within the correct
     * capacity, the error bits are cleared to show that errors are corrected.
     * - If the number of error bits in one group is beyond the correct
     * capacity, some bits are corrected while others are not. The number of
     * corrected bit equals to the correct capacity. At the same time,
     * EsynetFlit::FLIT_DROP is set to mark that the packet is error.
     *
     * \param flit  reference to the flit to decode.
     */
	void decode(EsynetFlit & flit);
    /*!
     * \brief Inject flit into the buffer and set the buffer full.
     * \param vc   virtual channel the flit should go.
     * \param flit flit to store.
     */
	void injectFlit(long vc, const EsynetFlit & flit)
        { m_buf_state = true; m_vc = vc; m_ecc_buffer = flit; }
    /*!
     * \brief Get flit from the buffer and clear the buffer empty.
     * \return flit in buffer.
     */
	const EsynetFlit & getFlit()
        { m_buf_state = false; return m_ecc_buffer; }
        
    static long dataPathWithECC(ECCMethod name, long width);
};

/**
 * @ingroup eccunit
 * @ingroup fip
 * @brief Encoder of Error Correction Coding units.
 *
 * The fault injection point of one encoder is divided into N single signal FIPs
 * and 1 multi signal FIP, where N is the width of encoded codes. If one signle
 * signal FIP is faulty, the corresponding bit is error. If one multi signal
 * FIP is faulty, the entire flit is error and the flit is dropped.
 *
 * The number of FIPs equals to the width of data path plus the number of
 * encoder group. The first part of FIPs (width of data path) describe the
 * signal FIPs for every bit in data path. The second part of FIPs (number of
 * encoder group describe the multi FIPs for every group in data path.
 */
class EsynetECCEncoder : public EsynetECCBufferUnit, public EsynetLinkFaultUnit
{
public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Construct an empty encoder unit.
     */
	EsynetECCEncoder() :
		EsynetLinkFaultUnit(EsyFaultConfigItem::FAULT_ECCENC, -1, -1, 32),
		EsynetECCBufferUnit()
    {}
    /*!
     * \brief Construct an encoder unit with specified configuration.
     * \param id  id of router.
     * \param port  id of physical port.
     * \param width  width of data path, after coding.
     * \param ecc_enable  enable ecc.
     * \param ecc_name    name of ecc scheme.
     * \param fip_enable  enable fault injection in ecc.
     */
	EsynetECCEncoder(long id, long port, int width,
        bool ecc_enable, ECCMethod ecc_name, bool fip_enable 
    ):
		EsynetLinkFaultUnit(EsyFaultConfigItem::FAULT_ECCENC, id, port,
            width + ( width / m_ecc_word_width ) ),
			EsynetECCBufferUnit(ecc_name, width, ecc_enable, fip_enable)
    {}
    ///@}

    /*!
     * \brief Operation of encoder.
     *
     * Inject the flit into the buffer.
     *
     * If the fault injection is enable, check the fault injection points.
     * - First part (width of data path): if FIP is error, set corresponding
     * bit error in EsynetFlit::m_fault_pattern. These faults will be corrected in
     * next decoder.
     * - Second part (number of decoder group): if FIP is error, the errors in
     * outputs cannot be corrected so that the flit is dropped directly by set
     * EsynetFlit::FLIT_DROP.
     *
     * \param vc  virtual channel where the flit to go.
     * \param flit flit to inject into encoder.
     * \sa EsynetECCBufferUnit::decode();
     */
	void encoder(long vc, const EsynetFlit & flit);
};

/**
 * @ingroup eccunit
 * @ingroup fip
 * @brief Inter decoder of Error Correction Coding units.
 *
 * Outputs of inter decoders are still the code word rather than orignal
 * information. They are used at the ports of routers.
 *
 * The fault injection point of one encoder is divided into N single signal FIPs
 * and 1 multi signal FIP, where N is the width of encoded codes. If one signle
 * signal FIP is faulty, the corresponding bit is error. If one multi signal
 * FIP is faulty, the entire flit is error and the flit is dropped.
 *
 * The number of FIPs equals to the width of data path plus the number of
 * decoder group. The first part of FIPs (width of data path) describe the
 * signal FIPs for every bit in data path. The second part of FIPs (number of
 * decoder group describe the multi FIPs for every group in data path.
 */
class EsynetECCInterDecoder : public EsynetECCBufferUnit, public EsynetLinkFaultUnit
{
public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Construct an empty decoder unit.
     */
	EsynetECCInterDecoder() :
		EsynetLinkFaultUnit(EsyFaultConfigItem::FAULT_ECCINTERDEC, -1, -1, 32),
		EsynetECCBufferUnit()
    {}
    /*!
     * \brief Construct an decoder unit with specified configuration.
     * \param id  id of router.
     * \param port  id of physical port.
     * \param width  width of data path, after coding.
     * \param ecc_enable  enable ecc.
     * \param ecc_name    name of ecc scheme.
     * \param fip_enable  enable fault injection in ecc.
     */
	EsynetECCInterDecoder(long id, long port, int width,
		bool ecc_enable, ECCMethod ecc_name, bool fip_enable
    ):
		EsynetLinkFaultUnit(EsyFaultConfigItem::FAULT_ECCINTERDEC, id, port,
            width + ( width / m_ecc_word_width ) ),
		EsynetECCBufferUnit(ecc_name, width, ecc_enable, fip_enable)
    {}
    ///@}

    /*!
     * \brief Operation of decoder.
     *
     * Correct the faults in the flit and inject the flit into the buffer.
     *
     * If the fault injection is enable, check the fault injection points.
     * - First part (width of data path): if FIP is error, set corresponding
     * bit error in EsynetFlit::m_fault_pattern. These faults will be corrected in
     * next decoder.
     * - Second part (number of decoder group): if FIP is error, the errors in
     * outputs cannot be corrected so that the flit is dropped directly by set
     * EsynetFlit::FLIT_DROP.
     *
     * \param vc  virtual channel where the flit to go.
     * \param flit flit to inject into encoder.
     * \sa EsynetECCBufferUnit::decode();
     */
	void decoder(long vc, const EsynetFlit & flit);
};

/**
 * @ingroup eccunit
 * @ingroup fip
 * @brief Final decoder of Error Correction Coding units.
 *
 * Outputs of final decoders are orignalinformation. They are used at the
 * network interfaces.
 *
 * For one decoder, only one fip is implied. So, the number of fips equals to
 * the number of decoder groups. If one fip is error, the output of
 * corresponding decoder group is error and the flit is dropped.
 */
class EsynetECCFinalDecoder : public EsynetECCBufferUnit, public EsynetLinkFaultUnit
{
public:
    /** @name Constructor */
    ///@{
    /*!
     * \brief Construct an empty decoder unit.
     */
	EsynetECCFinalDecoder() :
		EsynetLinkFaultUnit(EsyFaultConfigItem::FAULT_ECCFINALDEC, -1, -1, 32),
		EsynetECCBufferUnit()
    {}
    /*!
     * \brief Construct an decoder unit with specified configuration.
     * \param id  id of router.
     * \param port  id of physical port.
     * \param width  width of data path, after coding.
     * \param ecc_enable  enable ecc.
     * \param ecc_name    name of ecc scheme.
     * \param fip_enable  enable fault injection in ecc.
     */
	EsynetECCFinalDecoder(long id, long port, int width,
		bool ecc_enable, ECCMethod ecc_name, bool fip_enable
    ):
		EsynetLinkFaultUnit(EsyFaultConfigItem::FAULT_ECCFINALDEC, id, port,
            width / m_ecc_word_width ),
		EsynetECCBufferUnit(ecc_name, width, ecc_enable, fip_enable)
    {}
    ///@}

    /*!
     * \brief Operation of decoder.
     *
     * Correct the faults in the flit and inject the flit into the buffer.
     *
     * If the fault injection is enable, check the fault injection points. Each
     * fault injection points corresponding to one decoder group. If any of them
     * is faulty, the flit is dropped, as the decoder must be correct.
     *
     * \param vc  virtual channel where the flit to go.
     * \param flit flit to inject into encoder.
     * \sa EsynetECCBufferUnit::decode();
     */
	void decoder(long vc, const EsynetFlit & flit);
};

#endif
