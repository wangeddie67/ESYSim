/*
 Copyright @ UESTC

 ESY-series Many-core System-on-chip Simulation Environment is free software:
 you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3
 of the License, or (at your option) any later version.

 ESY-series Many-core System-on-chip Simulation Environment is distributed in
 the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License aint with
 this program. If not, see http://www.gnu.org/licenses/.
 */


#ifndef INTERFACE_SOC_CFG_H
#define INTERFACE_SOC_CFG_H

#include "esy_xmlerror.h"
#include "esy_netcfg.h"
#include "tinyxml.h"

using namespace std;

/** @defgroup soccfggroup SoC Configuration Interface. */

/**
 * @ingroup soccfggroup
 * @brief Structure of tile item.
 *
 * Holds parameters and value of one tile. Each item contains following
 * information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Meaning </th>
 *   <th> Default </th>
 *   <th> Access </th>
 * </tr>
 * </table>
 *
 * \sa EsySoCCfgNetwork, EsySoCCfg
 */
class EsySoCCfgTile
{
public:
	enum TileType {
		TILE_GPP,
		TILE_SPP,
		TILE_ASIC,
		TILE_COUNT
	};

	enum ISAType {
		ISA_MIPS,
        ISA_ALPHA,
		ISA_COUNT
	};

	enum NIPosition {
		NIPOS_NORTHWEST,
		NIPOS_NORTHEAST,
		NIPOS_SOUTHWEST,
		NIPOS_SOUTHEAST,
		NIPOS_COUNT
	};

protected:
	static string const_tile_type[ TILE_COUNT ];
	static string const_isa_type[ ISA_COUNT ];
	static string const_ni_position[ NIPOS_COUNT ];

	pair< double, double > m_pos;
    int m_ni_id;
    int m_ni_phy;
	NIPosition m_ni_pos;

    int m_tile_id;
	TileType m_tile_type;
	bool m_tile_enable;
    int m_tile_freq;

    int m_mpi_id;

	ISAType m_core_isa_type;
	string m_core_app_dir;
    int m_core_max_inst;
    int m_core_fetch_speed;

	string m_pred_update_type;
	string m_pred_predict_type;
    int m_pred_bimod_size;
    int m_pred_l1_size;
    int m_pred_l2_size;
    int m_pred_l2_hist_size;
	bool m_pred_l2_xor_enable;

    int m_meta_size;
    int m_return_stack_size;
    int m_btb_set;
    int m_btb_combination;

    int m_first_chunk_delay;
    int m_neiber_chunk_delay;
	string m_tlb_itlb_cfg;
	string m_tlb_dtlb_cfg;
    int m_tlb_miss_delay;
    int m_memory_bus_width;

    int m_ruu_ifq_size;
    int m_ruu_branch_penalt;
    int m_ruu_decoder_width;
    int m_ruu_issue_width;
    int m_ruu_inorder_issu;
    int m_ruu_include_spec;
    int m_ruu_commit_width;
    int m_ruu_size;
    int m_lsq_size;

	string m_cache_dl1_cfg;
    int m_cache_dl1_delay;
	string m_cache_dl2_cfg;
    int m_cache_dl2_delay;
	string m_cache_il1_cfg;
    int m_cache_il1_delay;
	string m_cache_il2_cfg;
    int m_cache_il2_delay;
	bool m_cache_i_compress_enable;

    int m_integar_adder_unit;
    int m_integar_multi_unit;
    int m_float_addr_unit;
    int m_float_multi_unit;
    int m_memory_port;

    string m_asic_name;
    int m_asic_type;
    int m_asic_delay;

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item with default value.
	 */
	EsySoCCfgTile();
	/*!
	 * \brief Constructs an item by copying an exist item.
	 * \param t  entity to copy.
	 */
	EsySoCCfgTile(const EsySoCCfgTile & t);
	///@}

	const pair< double, double > & pos() const { return m_pos; }
	void setPos( double first, double second )
		{ m_pos.first = first; m_pos.second = second; }
	void setPosFirst( double pos ) { m_pos.first = pos; }
	void setPosSecond( double pos ) { m_pos.second = pos; }

    int niId() const { return m_ni_id; }
    void setNiId( int id ) { m_ni_id = id; }

    int niPhy() const { return m_ni_phy; }
    void setNiPhy( int phy ) { m_ni_phy = phy; }

	NIPosition niPos() const { return m_ni_pos; }
	void setNiPos( NIPosition pos ) { m_ni_pos = pos; }
	const string & niPosStr() const { return const_ni_position[ m_ni_pos ]; }
	static string niPosStrVector( int i ) { return const_ni_position[ i ]; }

    int tileId() const { return m_tile_id; }
    void setTileId( int id ) { m_tile_id = id; }

	bool tileEnable() const { return m_tile_enable; }
	void setTileEnable( bool enable ) { m_tile_enable = enable; }

	TileType tileType() const { return m_tile_type; }
	void setTileType( TileType type ) { m_tile_type = type; }
	const string & tileTypeStr() const
		{ return const_tile_type[ m_tile_type ]; }
	static string tileTypeStrVector( int i ) { return const_tile_type[ i ]; }

    int tileFreq() const { return m_tile_freq; }
    void setTileFreq( int freq ) { m_tile_freq = freq; }

    int mpiId() const { return m_mpi_id; }
    void setMpiId( int id ) { m_mpi_id = id; }

	ISAType coreIsaType() const { return m_core_isa_type; }
	void setCoreIsaType( ISAType type ) { m_core_isa_type = type; }
	const string & coreIsaTypeStr() const
		{ return const_isa_type[ m_core_isa_type ]; }
	static string coreIsaTypeStrVector( int i ) { return const_isa_type[ i ]; }

	const string & coreAppDir() const { return m_core_app_dir; }
	void setCoreAppDir( const string & dir ) { m_core_app_dir = dir; }

    int coreMaxInst() const { return m_core_max_inst; }
    void setCoreMaxInst( int inst ) { m_core_max_inst = inst; }

    int coreFetchSpeed() const { return m_core_fetch_speed; }
    void setCoreFetchSpeed( int speed ) { m_core_fetch_speed = speed; }

	const string & predUpdateType() const { return m_pred_update_type; }
	void setPredUpdateType( const string & type ) { m_pred_update_type = type; }

	const string & predPredictType() const { return m_pred_predict_type; }
	void setPredPredictType( const string & type ) { m_pred_predict_type = type; }

    int predBimodSize() const { return m_pred_bimod_size; }
    void setPredBimodSize( int size ) { m_pred_bimod_size = size; }

    int predL1Size() const { return m_pred_l1_size; }
    void setPredL1Size( int size ) { m_pred_l1_size = size; }

    int predL2Size() const { return m_pred_l2_size; }
    void setPredL2Size( int size ) { m_pred_l2_size = size; }

    int predL2HistSize() const { return m_pred_l2_hist_size; }
    void setPredL2HistSize( int size ) { m_pred_l2_hist_size = size; }

	bool predL2XorEnable() const { return m_pred_l2_xor_enable; }
	void setPredL2XorEnable( bool enable ) { m_pred_l2_xor_enable = enable; }

    int metaSize() const { return m_meta_size; }
    void setMetaSize( int size ) { m_meta_size = size; }

    int returnStackSize() const { return m_return_stack_size; }
    void setReturnStackSize( int size ) { m_return_stack_size = size; }

    int btbSet() const { return m_btb_set; }
    void setBtbSet( int size ) { m_btb_set = size; }

    int btbCombination() const { return m_btb_combination; }
    void setBtbCombination( int size ) { m_btb_combination = size; }

    int firstChunkDelay() const { return m_first_chunk_delay; }
    void setFirstChunkDelay( int delay ) { m_first_chunk_delay = delay; }

    int neiberChunkDelay() const { return m_neiber_chunk_delay; }
    void setNeiberChunkDelay( int delay ) { m_neiber_chunk_delay = delay; }

	const string & tlbItlbCfg() const { return m_tlb_itlb_cfg; }
	void setTlbItlbCfg( const string & cfg ) { m_tlb_itlb_cfg = cfg; }

	const string & tlbDtlbCfg() const { return m_tlb_dtlb_cfg; }
	void setTlbDtlbCfg( const string & cfg ) { m_tlb_dtlb_cfg = cfg; }

    int tlbMissDelay() const { return m_tlb_miss_delay; }
    void settlbMissDelay( int delay ) { m_tlb_miss_delay = delay; }

    int memoryBusWidth() const { return m_memory_bus_width; }
    void setMemoryBusWidth( int size ) { m_memory_bus_width = size; }

    int ruuIfqSize() const { return m_ruu_ifq_size; }
    void setRuuIfqSize( int size ) { m_ruu_ifq_size = size; }

    int ruuBranchPenalt() const { return m_ruu_branch_penalt; }
    void setRuuBranchPenalt( int size ) { m_ruu_branch_penalt = size; }

    int ruuDecoderWidth() const { return m_ruu_decoder_width; }
    void setRuuDecoderWidth( int size ) { m_ruu_decoder_width = size; }

    int ruuIssueWidth() const { return m_ruu_issue_width; }
    void setRuuIssueWidth( int size ) { m_ruu_issue_width = size; }

    int ruuInorderIssu() const { return m_ruu_inorder_issu; }
    void setRuuInorderIssu( int size ) { m_ruu_inorder_issu = size; }

    int ruuIncludeSpec() const { return m_ruu_include_spec; }
    void setRuuIncludeSpec( int size ) { m_ruu_include_spec = size; }

    int ruuCommitWidth() const { return m_ruu_commit_width; }
    void setRuuCommitWidth( int size ) { m_ruu_commit_width = size; }

    int ruuSize() const { return m_ruu_size; }
    void setRuuSize( int size ) { m_ruu_size = size; }

    int lsqSize() const { return m_lsq_size; }
    void setLsqSize( int size ) { m_lsq_size = size; }

	const string & cacheDl1Cfg() const { return m_cache_dl1_cfg; }
	void setCacheDl1Cfg( const string & cfg ) { m_cache_dl1_cfg = cfg; }

    int cacheDl1Delay() const { return m_cache_dl1_delay; }
    void setCacheDl1Delay( int delay ) { m_cache_dl1_delay = delay; }

	const string & cacheDl2Cfg() const { return m_cache_dl2_cfg; }
	void setCacheDl2Cfg( const string & cfg ) { m_cache_dl2_cfg = cfg; }

    int cacheDl2Delay() const { return m_cache_dl2_delay; }
    void setCacheDl2Delay( int delay ) { m_cache_dl2_delay = delay; }

	const string & cacheIl1Cfg() const { return m_cache_il1_cfg; }
	void setCacheIl1Cfg( const string & cfg ) { m_cache_il1_cfg = cfg; }

    int cacheIl1Delay() const { return m_cache_il1_delay; }
    void setCacheIl1Delay( int delay ) { m_cache_il1_delay = delay; }

	const string & cacheIl2Cfg() const { return m_cache_il2_cfg; }
	void setCacheIl2Cfg( const string & cfg ) { m_cache_il2_cfg = cfg; }

    int cacheIl2Delay() const { return m_cache_il2_delay; }
    void setCacheIl2Delay( int delay ) { m_cache_il2_delay = delay; }

	bool cacheICompressEnable() const
		{ return m_cache_i_compress_enable; }
	void setCacheICompressEnable( bool enable )
		{ m_cache_i_compress_enable = enable; }

    int integarAdderUnit() const { return m_integar_adder_unit; }
    void setIntegarAdderUnit( int unit ) { m_integar_adder_unit = unit; }

    int integarMultiUnit() const { return m_integar_multi_unit; }
    void setIntegarMultiUnit( int unit ) { m_integar_multi_unit = unit; }

    int floatAddrUnit() const { return m_float_addr_unit; }
    void setFloatAddrUnit( int unit ) { m_float_addr_unit = unit; }

    int floatMultiUnit() const { return m_float_multi_unit; }
    void setFloatMultiUnit( int unit ) { m_float_multi_unit = unit; }

    int memoryPort() const { return m_memory_port; }
    void setMemoryPort( int unit ) { m_memory_port = unit; }

    const string & asicName() const { return m_asic_name; }
    void setAsicName( const string & name ) { m_asic_name = name; }

    int asicType() const { return m_asic_type; }
    void setAsicType( int type ) { m_asic_type = type; }

    int asicDelay() const { return m_asic_delay; }
    void setAsicDelay( int delay ) { m_asic_delay = delay; }

	/** @name Functions to access network configuration file in XML. */
	/*!
	 * \brief Read port configuration from XML file.
	 * \param root  root of XML structure.
	 */
	void readXml( TiXmlElement * root );
	/*!
	 * \brief Write port configuration to XML file.
	 *
	 * If full is true, print out all fields. Otherwise, #m_neighbor_router and
	 * #m_neighbor_port are not printed.
	 *
	 * \param full  Full configuraion flag.
	 * \param root  root of XML structure.
	 */
	void writeXml( TiXmlElement * root );
	///@}

	/*!
	 * \brief Overload operator == function. Compare value of two items.
	 * \param t  item to compare.
	 * \return If t has the same value as this item at all field, return TRUE.
	 * Otherwise, return FALSE.
	 */
	bool operator ==(const EsySoCCfgTile & t) const;
};

/**
 * @ingroup soccfggroup
 * @brief Structure of network configuration item.
 *
 * Holds parameters and value of one network. Each item contains following
 * information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Meaning </th>
 *   <th> Default </th>
 *   <th> Access </th>
 * </tr>
 * </table>
 *
 * \sa EsySoCCfgTile, EsySoCCfg
 */
class EsySoCCfgNetwork : public EsyNetworkCfg
{

protected:
    int m_network_freq;

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item with default value.
	 */
	EsySoCCfgNetwork();
	/*!
	 * \brief Constructs an item by copying an exist item.
	 * \param t  entity to copy.
	 */
	EsySoCCfgNetwork( const EsySoCCfgNetwork & t );
	///@}

    int networkFreq() const { return m_network_freq; }
    void setNetworkFreq( int freq ) { m_network_freq = freq; }

	/** @name Functions to access network configuration file in XML. */
	///@{
	/*!
	 * \brief Read network configuration from XML file.
	 * \param root  root of XML structure.
	 */
	void readXml( TiXmlElement * root );
	/*!
	 * \brief Write network configuration to XML file.
	 * \param root  root of XML structure.
	 */
	void writeXml( TiXmlElement * root );
	///@}
};

/**
 * @ingroup soccfggroup
 * @brief Structure of SoC configuration item.
 *
 * Holds parameters and value of one network. Each item contains following
 * information:
 * <table>
 * <tr>
 *   <th> Variable </th>
 *   <th> Meaning </th>
 *   <th> Default </th>
 *   <th> Access </th>
 * </tr>
 * </table>
 *
 * \sa EsySoCCfgTile, EsySoCCfgNetwork
 */
class EsySoCCfg
{
public:

    #define SOCCFG_EXTENSION "soccfg"

protected:
	EsySoCCfgTile m_template_tile;
	vector< EsySoCCfgTile > m_tile;
	EsySoCCfgNetwork m_network;

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item with default value.
	 */
	EsySoCCfg();
	/*!
	 * \brief Constructs an item by copying an exist item.
	 * \param t  entity to copy.
	 */
	EsySoCCfg( const EsySoCCfg & t );
	///@}
	const EsySoCCfgTile & templateTile() const { return m_template_tile; }
	EsySoCCfgTile & templateTile() { return m_template_tile; }

	const vector< EsySoCCfgTile > & tile() const { return m_tile; }
    const EsySoCCfgTile & tile( long i ) const { return m_tile[ i ]; }
    EsySoCCfgTile & tile( long i ) { return m_tile[ i ]; }
	void appendTile(const EsySoCCfgTile & tile) { m_tile.push_back( tile ); }
	size_t tileCount() const { return m_tile.size(); }

	const EsySoCCfgNetwork & network() const { return m_network; }
	EsySoCCfgNetwork & network() { return m_network; }
	///@}

	void updateSoC();
	void updateTile();

	/** @name Functions to access network configuration file in XML. */
	///@{
	/*!
	 * \brief Read network configuration from XML file.
	 * \param root  root of XML structure.
	 */
	void readXml( TiXmlElement * root );
	/*!
	 * \brief Open XML network configuration file for reading.
	 * \param fname  direction of network configuration file.
	 * \return  XML error handler.
	 * \sa readXml(TiXmlElement * root), EsyNetworkCfgRouter::readXml(),
	 * EsyNetworkCfgPort::readXml()
	 */
	EsyXmlError readXml( const string & fname );
	/*!
	 * \brief Write network configuration to XML file.
	 * \param root  root of XML structure.
	 */
	void writeXml( TiXmlElement * root );
	/*!
	 * \brief Open XML network configuration file for writing.
	 * \param fname  direction of network configuration file.
	 * \return  XML error handler.
	 * \sa writeXml(TiXmlElement * root), EsyNetworkCfgRouter::writeXml(),
	 * EsyNetworkCfgPort::writeXml()
	 */
	EsyXmlError writeXml( const string & fname );
	///@}
};

#endif
