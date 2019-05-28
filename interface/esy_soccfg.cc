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

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/.
 */

#include "esy_conv.h"
#include "esy_soccfg.h"

#define readItemLong( item, value, var ) \
if ( item->Value() == string( value ) ) \
{ \
	var = EsyConvert( item->GetText() ); \
}
#define readItemString( item, value, var ) \
if ( item->Value() == string( value ) ) \
{ \
	var = string( item->GetText() ); \
}
#define readItemEnum( item, value, var, type ) \
if ( item->Value() == string( value ) ) \
{ \
	long t = EsyConvert( item->GetText() ); \
	var = (type)t; \
}

#define writeItem( root, value, var ) \
TiXmlElement * t_##var##_item = new TiXmlElement( value );\
t_##var##_item->LinkEndChild( \
	new TiXmlText( EsyConvert( var ).c_str() ) ); \
root->LinkEndChild( t_##var##_item ); \

#define SOC_ARGU_DECLARATION_VERSION  "1.0"
#define SOC_ARGU_DECLARATION_ENCODING  "UTF-8"

#define SOC_ARGU_ROOT	"soccfg"
#define SOC_ARGU_TEMPLATE_TILE	"template_tile_cfg"
#define SOC_ARGU_TILE	"tile_cfg"
#define SOC_ARGU_NETWORK	"network_cfg"

#define SOC_ARGU_AX_X	"pos_x"
#define SOC_ARGU_AX_Y	"pos_y"
#define SOC_ARGU_NI_ID	"ni_id"
#define	SOC_ARGU_NI_PHY	"ni_phy"
#define SOC_ARGU_NI_POS	"ni_pos"

#define SOC_ARGU_TILE_ID	"tile_id"
#define SOC_ARGU_TILE_TYPE	"tile_type"
#define SOC_ARGU_TILE_ENABLE	"tile_enable"
#define SOC_ARGU_TILE_FREQ	"tile_freq"

#define SOC_ARGU_MPI_ID	"mpi_id"

#define SOC_ARGU_CORE_ISA_TYPE	"core_isa_type"
#define SOC_ARGU_CORE_APP_DIR	"core_app_dir"
#define SOC_ARGU_CORE_MAX_INST	"core_max_inst"
#define SOC_ARGU_CORE_FETCH_SPEED	"core_fetch_speed"

#define SOC_ARGU_PRED_UPDATE_TYPE	"pred_update_type"
#define SOC_ARGU_PRED_PREDICT_TYPE	"pred_predict_type"
#define SOC_ARGU_PRED_BIMOD_SIZE	"pred_bimod_size"
#define SOC_ARGU_PRED_L1_SIZE	"pred_l1_size"
#define SOC_ARGU_PRED_L2_SIZE	"pred_l2_size"
#define SOC_ARGU_PRED_L2_HIST_SIZE	"pred_l2_hist_size"
#define SOC_ARGU_PRED_L2_XOR_ENABLE	"pred_l2_xor_enable"

#define SOC_ARGU_META_SIZE	"meta_size"
#define SOC_ARGU_RETURN_STACK_SIZE	"return_stack_size"
#define SOC_ARGU_BTB_SET	"btb_set"
#define SOC_ARGU_BTB_COMBINATION	"btb_comb"

#define SOC_ARGU_FIRST_CHUNK_DELAY	"first_chunk_delay"
#define SOC_ARGU_NEIBER_CHUNK_DELAY	"neiber_chunk_delay"
#define SOC_ARGU_TLB_ITLB_CFG	"tlb_itlb_cfg"
#define SOC_ARGU_TLB_DTLB_CFG	"tlb_dtlb_cfg"
#define SOC_ARGU_TLB_MISS_DELAY	"tlb_miss_delay"
#define SOC_ARGU_MEMORY_BUS_WIDTH	"memory_bus_width"

#define SOC_ARGU_RUU_IFQ_SIZE	"ruu_ifq_size"
#define SOC_ARGU_RUU_BRANCH_PENALT	"ruu_branch_penalt"
#define SOC_ARGU_RUU_DECODER_WIDTH	"ruu_decoder_width"
#define SOC_ARGU_RUU_ISSUE_WIDTH	"ruu_issue_width"
#define SOC_ARGU_RUU_INORDER_ISSU	"ruu_inorder_issu"
#define SOC_ARGU_RUU_INCLUDE_SPEC	"ruu_include_spec"
#define SOC_ARGU_RUU_COMMIT_WIDTH	"ruu_commit_width"
#define SOC_ARGU_RUU_SIZE	"ruu_size"
#define SOC_ARGU_LSQ_SIZE	"lsq_size"

#define SOC_ARGU_CACHE_DL1_CFG	"cache_dl1_cfg"
#define SOC_ARGU_CACHE_DL1_DELAY	"cache_dl1_delay"
#define SOC_ARGU_CACHE_DL2_CFG	"cache_dl2_cfg"
#define SOC_ARGU_CACHE_DL2_DELAY	"cache_dl2_delay"
#define SOC_ARGU_CACHE_IL1_CFG	"cache_il1_cfg"
#define SOC_ARGU_CACHE_IL1_DELAY	"cache_il1_delay"
#define SOC_ARGU_CACHE_IL2_CFG	"cache_il2_cfg"
#define SOC_ARGU_CACHE_IL2_DELAY	"cache_il2_delay"
#define SOC_ARGU_CACHE_I_COMPRESS_ENABLE	"cache_i_compress_enable"

#define SOC_ARGU_INTEGAR_ADDER_UNIT	"integar_adder_unit"
#define SOC_ARGU_INTEGAR_MULTI_UNIT	"integar_multi_unit"
#define SOC_ARGU_FLOAT_ADDR_UNIT	"float_addr_unit"
#define SOC_ARGU_FLOAT_MULTI_UNIT	"float_multi_unit"
#define SOC_ARGU_MEMORY_PORT	"float_memory_port"

#define SOC_ARGU_ASIC_NAME  "asic_name"
#define SOC_ARGU_ASIC_TYPE  "asic_type"
#define SOC_ARGU_ASIC_DELAY "asic_delay"

#define SOC_ARGU_NETWORK_FREQ	"network_freq"

#define SOC_ARGU_DATA   "data"
#define SOC_ARGU_SIZE   "size"
#define SOC_ARGU_INDEX  "index"

#define DEFAULT_POSITION	0.0
#define DEFAULT_NI_ID	0
#define DEFAULT_NI_PHY	0
#define DEFAULT_NI_POS	NIPOS_SOUTHEAST

#define DEFAULT_TILE_ID	0
#define DEFAULT_TILE_TYPE	TILE_GPP
#define DEFAULT_TILE_ENABLE	false
#define DEFAULT_TILE_FREQ	1000

#define DEFAULT_MPI_ID	0

#define DEFAULT_CORE_ISA_TYPE	ISA_MIPS
#define DEFAULT_CORE_APP_DIR	"testmpi/cl.BNC"
#define DEFAULT_CORE_MAX_INST	0
#define DEFAULT_CORE_FETCH_SPEED	1

#define DEFAULT_PRED_UPDATE_TYPE	"ID"
#define DEFAULT_PRED_PREDICT_TYPE	"bimod"
#define DEFAULT_PRED_BIMOD_SIZE	2048
#define DEFAULT_PRED_L1_SIZE	1
#define DEFAULT_PRED_L2_SIZE	1024
#define DEFAULT_PRED_L2_HIST_SIZE	8
#define DEFAULT_PRED_L2_XOR_ENABLE	false

#define DEFAULT_META_SIZE	1024
#define DEFAULT_RETURN_STACK_SIZE	8
#define DEFAULT_BTB_SET	512
#define DEFAULT_BTB_COMBINATION	4

#define DEFAULT_FIRST_CHUNK_DELAY	18
#define DEFAULT_NEIBER_CHUNK_DELAY	2
#define DEFAULT_TLB_ITLB_CFG	"itlb:16:4096:4:l"
#define DEFAULT_TLB_DTLB_CFG	"dtlb:32:4096:4:l"
#define DEFAULT_TLB_MISS_DELAY	30
#define DEFAULT_MEMORY_BUS_WIDTH	4

#define DEFAULT_RUU_IFQ_SIZE	4
#define DEFAULT_RUU_BRANCH_PENALT	3
#define DEFAULT_RUU_DECODER_WIDTH	4
#define DEFAULT_RUU_ISSUE_WIDTH	4
#define DEFAULT_RUU_INORDER_ISSU	1
#define DEFAULT_RUU_INCLUDE_SPEC	1
#define DEFAULT_RUU_COMMIT_WIDTH	4
#define DEFAULT_RUU_SIZE	16
#define DEFAULT_LSQ_SIZE	8

#define DEFAULT_CACHE_DL1_CFG	"dl1:128:32:4:l"
#define DEFAULT_CACHE_DL1_DELAY	1
#define DEFAULT_CACHE_DL2_CFG	"ul2:1024:64:4:l"
#define DEFAULT_CACHE_DL2_DELAY	6
#define DEFAULT_CACHE_IL1_CFG	"il1:512:32:1:l"
#define DEFAULT_CACHE_IL1_DELAY	1
#define DEFAULT_CACHE_IL2_CFG	"dl2"
#define DEFAULT_CACHE_IL2_DELAY	6
#define DEFAULT_CACHE_I_COMPRESS_ENABLE	false

#define DEFAULT_INTEGAR_ADDER_UNIT	1
#define DEFAULT_INTEGAR_MULTI_UNIT	1
#define DEFAULT_FLOAT_ADDR_UNIT	1
#define DEFAULT_FLOAT_MULTI_UNIT	1
#define DEFAULT_MEMORY_PORT	1

#define DEFAULT_ASIC_NAME "asic_fft_128"
#define DEFAULT_ASIC_TYPE 0
#define DEFAULT_ASIC_DELAY  32

#define DEFAULT_NETWORK_FREQ	1000

string EsySoCCfgTile::const_tile_type[ TILE_COUNT ] =
	{ "GPP", "SPP", "ASIC" };

string EsySoCCfgTile::const_isa_type[ ISA_COUNT ] =
	{ "MIPS" };

string EsySoCCfgTile::const_ni_position[ NIPOS_COUNT ] =
	{ "NorthWest", "NorthEast", "SouthWest", "SouthEast" };

EsySoCCfgTile::EsySoCCfgTile():
	m_pos( DEFAULT_POSITION, DEFAULT_POSITION ),
	m_ni_id( DEFAULT_NI_ID ),
	m_ni_phy( DEFAULT_NI_PHY ),
	m_ni_pos( DEFAULT_NI_POS ),
	
	m_tile_id( DEFAULT_TILE_ID ),
	m_tile_type( DEFAULT_TILE_TYPE ),
	m_tile_enable( DEFAULT_TILE_ENABLE ),
	m_tile_freq( DEFAULT_TILE_FREQ ),

	m_mpi_id( DEFAULT_MPI_ID ),

	m_core_isa_type( DEFAULT_CORE_ISA_TYPE ),
	m_core_app_dir( DEFAULT_CORE_APP_DIR ),
	m_core_max_inst( DEFAULT_CORE_MAX_INST ),
	m_core_fetch_speed( DEFAULT_CORE_FETCH_SPEED ),

	m_pred_update_type( DEFAULT_PRED_UPDATE_TYPE ),
	m_pred_predict_type( DEFAULT_PRED_PREDICT_TYPE ),
	m_pred_bimod_size( DEFAULT_PRED_BIMOD_SIZE ),
	m_pred_l1_size( DEFAULT_PRED_L1_SIZE ),
	m_pred_l2_size( DEFAULT_PRED_L2_SIZE ),
	m_pred_l2_hist_size( DEFAULT_PRED_L2_HIST_SIZE ),
	m_pred_l2_xor_enable( DEFAULT_PRED_L2_XOR_ENABLE ),
	
	m_meta_size( DEFAULT_META_SIZE ),
	m_return_stack_size( DEFAULT_RETURN_STACK_SIZE ),
	m_btb_set( DEFAULT_BTB_SET ),
	m_btb_combination( DEFAULT_BTB_COMBINATION ),
	
	m_first_chunk_delay( DEFAULT_FIRST_CHUNK_DELAY ),
	m_neiber_chunk_delay( DEFAULT_NEIBER_CHUNK_DELAY ),
	m_tlb_itlb_cfg( DEFAULT_TLB_ITLB_CFG ),
	m_tlb_dtlb_cfg( DEFAULT_TLB_DTLB_CFG ),
	m_tlb_miss_delay( DEFAULT_TLB_MISS_DELAY ),
	m_memory_bus_width( DEFAULT_MEMORY_BUS_WIDTH ),
	
	m_ruu_ifq_size( DEFAULT_RUU_IFQ_SIZE ),
	m_ruu_branch_penalt( DEFAULT_RUU_BRANCH_PENALT ),
	m_ruu_decoder_width( DEFAULT_RUU_DECODER_WIDTH ),
	m_ruu_issue_width( DEFAULT_RUU_ISSUE_WIDTH ),
	m_ruu_inorder_issu( DEFAULT_RUU_INORDER_ISSU ),
	m_ruu_include_spec( DEFAULT_RUU_INCLUDE_SPEC ),
	m_ruu_commit_width( DEFAULT_RUU_COMMIT_WIDTH ),
	m_ruu_size( DEFAULT_RUU_SIZE ),
	m_lsq_size( DEFAULT_LSQ_SIZE ),
	
	m_cache_dl1_cfg( DEFAULT_CACHE_DL1_CFG ),
	m_cache_dl1_delay( DEFAULT_CACHE_DL1_DELAY ),
	m_cache_dl2_cfg( DEFAULT_CACHE_DL2_CFG ),
	m_cache_dl2_delay( DEFAULT_CACHE_DL2_DELAY ),
	m_cache_il1_cfg( DEFAULT_CACHE_IL1_CFG ),
	m_cache_il1_delay( DEFAULT_CACHE_IL1_DELAY ),
	m_cache_il2_cfg( DEFAULT_CACHE_IL2_CFG ),
	m_cache_il2_delay( DEFAULT_CACHE_IL2_DELAY ),
	m_cache_i_compress_enable( DEFAULT_CACHE_I_COMPRESS_ENABLE ),
	
	m_integar_adder_unit( DEFAULT_INTEGAR_ADDER_UNIT ),
	m_integar_multi_unit( DEFAULT_INTEGAR_MULTI_UNIT ),
	m_float_addr_unit( DEFAULT_FLOAT_ADDR_UNIT ),
	m_float_multi_unit( DEFAULT_FLOAT_MULTI_UNIT ),
    m_memory_port( DEFAULT_MEMORY_PORT ),

    m_asic_name( DEFAULT_ASIC_NAME ),
    m_asic_type( DEFAULT_ASIC_TYPE ),
    m_asic_delay( DEFAULT_ASIC_DELAY )
{}
EsySoCCfgTile::EsySoCCfgTile(const EsySoCCfgTile& t) :
	m_pos( t.pos() ),
	m_ni_id( t.niId() ),
	m_ni_phy( t.niPhy() ),
	m_ni_pos( t.niPos() ),
	
	m_tile_id( t.tileId() ),
	m_tile_type( t.tileType() ),
	m_tile_enable( t.tileEnable() ),
	m_tile_freq( t.tileFreq() ),

    m_mpi_id( t.mpiId() ),

    m_core_isa_type( t.coreIsaType() ),
	m_core_app_dir( t.coreAppDir() ),
	m_core_max_inst( t.coreMaxInst() ),
	m_core_fetch_speed( t.coreFetchSpeed() ),

	m_pred_update_type( t.predUpdateType() ),
	m_pred_predict_type( t.predPredictType() ),
	m_pred_bimod_size( t.predBimodSize() ),
	m_pred_l1_size( t.predL1Size() ),
	m_pred_l2_size( t.predL2Size() ),
	m_pred_l2_hist_size( t.predL2HistSize() ),
	m_pred_l2_xor_enable( t.predL2XorEnable() ),
	
	m_meta_size( t.metaSize() ),
	m_return_stack_size( t.returnStackSize() ),
	m_btb_set( t.btbSet() ),
	m_btb_combination( t.btbCombination() ),
	
	m_first_chunk_delay( t.firstChunkDelay() ),
	m_neiber_chunk_delay( t.neiberChunkDelay() ),
	m_tlb_itlb_cfg( t.tlbItlbCfg() ),
	m_tlb_dtlb_cfg( t.tlbDtlbCfg() ),
	m_tlb_miss_delay( t.tlbMissDelay() ),
	m_memory_bus_width( t.memoryBusWidth() ),
	
	m_ruu_ifq_size( t.ruuIfqSize() ),
	m_ruu_branch_penalt( t.ruuBranchPenalt() ),
	m_ruu_decoder_width( t.ruuDecoderWidth() ),
	m_ruu_issue_width( t.ruuIssueWidth() ),
	m_ruu_inorder_issu( t.ruuInorderIssu() ),
	m_ruu_include_spec( t.ruuIncludeSpec() ),
	m_ruu_commit_width( t.ruuCommitWidth() ),
	m_ruu_size( t.ruuSize() ),
	m_lsq_size( t.lsqSize() ),
	
	m_cache_dl1_cfg( t.cacheDl1Cfg() ),
	m_cache_dl1_delay( t.cacheDl1Delay() ),
	m_cache_dl2_cfg( t.cacheDl2Cfg() ),
	m_cache_dl2_delay( t.cacheDl2Delay() ),
	m_cache_il1_cfg( t.cacheIl1Cfg() ),
	m_cache_il1_delay( t.cacheIl1Delay() ),
	m_cache_il2_cfg( t.cacheIl2Cfg() ),
	m_cache_il2_delay( t.cacheIl2Delay() ),
	m_cache_i_compress_enable( t.cacheICompressEnable() ),
	
	m_integar_adder_unit( t.integarAdderUnit() ),
	m_integar_multi_unit( t.integarMultiUnit() ),
	m_float_addr_unit( t.floatAddrUnit() ),
	m_float_multi_unit( t.floatMultiUnit() ),
    m_memory_port( t.memoryPort() ),

    m_asic_name( t.asicName() ),
    m_asic_type( t.asicType() ),
    m_asic_delay( t.asicDelay() )
{}

void EsySoCCfgTile::readXml( TiXmlElement * root )
{
	for ( TiXmlNode * p_child = root->FirstChild(); p_child != 0;
		 p_child = p_child->NextSibling() )
	{
		TiXmlElement * t_element = p_child->ToElement();
		readItemLong( t_element, SOC_ARGU_TILE_ID, m_tile_id )
		readItemLong( t_element, SOC_ARGU_AX_X, m_pos.first )
		readItemLong( t_element, SOC_ARGU_AX_Y, m_pos.second )
		readItemLong( t_element, SOC_ARGU_NI_ID, m_ni_id )
		readItemLong( t_element, SOC_ARGU_NI_PHY, m_ni_phy )

		readItemEnum( t_element, SOC_ARGU_NI_POS, m_ni_pos, NIPosition )
		readItemEnum( t_element, SOC_ARGU_TILE_TYPE, m_tile_type, TileType )
		readItemLong( t_element, SOC_ARGU_TILE_ENABLE, m_tile_enable )
		readItemLong( t_element, SOC_ARGU_TILE_FREQ, m_tile_freq )

		readItemLong( t_element, SOC_ARGU_MPI_ID, m_mpi_id )

		readItemEnum( t_element, SOC_ARGU_CORE_ISA_TYPE, m_core_isa_type,
			ISAType )

		readItemString( t_element, SOC_ARGU_CORE_APP_DIR, m_core_app_dir )
		readItemLong( t_element, SOC_ARGU_CORE_MAX_INST, m_core_max_inst )
		readItemLong( t_element, SOC_ARGU_CORE_FETCH_SPEED, m_core_fetch_speed )

		readItemString( t_element, SOC_ARGU_PRED_UPDATE_TYPE,
			m_pred_update_type )
		readItemString( t_element, SOC_ARGU_PRED_PREDICT_TYPE,
			m_pred_predict_type )
		readItemLong( t_element, SOC_ARGU_PRED_BIMOD_SIZE, m_pred_bimod_size )
		readItemLong( t_element, SOC_ARGU_PRED_L1_SIZE, m_pred_l1_size )
		readItemLong( t_element, SOC_ARGU_PRED_L2_SIZE, m_pred_l2_size )
		readItemLong( t_element, SOC_ARGU_PRED_L2_HIST_SIZE,
			m_pred_l2_hist_size )
		readItemLong( t_element, SOC_ARGU_PRED_L2_XOR_ENABLE,
			m_pred_l2_xor_enable )

		readItemLong( t_element, SOC_ARGU_META_SIZE, m_meta_size )
		readItemLong( t_element, SOC_ARGU_RETURN_STACK_SIZE,
			m_return_stack_size )
		readItemLong( t_element, SOC_ARGU_BTB_SET, m_btb_set )
		readItemLong( t_element, SOC_ARGU_BTB_COMBINATION, m_btb_combination )

		readItemLong( t_element, SOC_ARGU_FIRST_CHUNK_DELAY,
			m_first_chunk_delay )
		readItemLong( t_element, SOC_ARGU_NEIBER_CHUNK_DELAY,
			m_neiber_chunk_delay )
		readItemString( t_element, SOC_ARGU_TLB_ITLB_CFG, m_tlb_itlb_cfg )
		readItemString( t_element, SOC_ARGU_TLB_DTLB_CFG, m_tlb_dtlb_cfg )
		readItemLong( t_element, SOC_ARGU_TLB_MISS_DELAY, m_tlb_miss_delay )
		readItemLong( t_element, SOC_ARGU_MEMORY_BUS_WIDTH, m_memory_bus_width )

		readItemLong( t_element, SOC_ARGU_RUU_IFQ_SIZE, m_ruu_ifq_size )
		readItemLong( t_element, SOC_ARGU_RUU_BRANCH_PENALT,
			m_ruu_branch_penalt )
		readItemLong( t_element, SOC_ARGU_RUU_DECODER_WIDTH,
			m_ruu_decoder_width )
		readItemLong( t_element, SOC_ARGU_RUU_ISSUE_WIDTH, m_ruu_issue_width )
		readItemLong( t_element, SOC_ARGU_RUU_INORDER_ISSU, m_ruu_inorder_issu )
		readItemLong( t_element, SOC_ARGU_RUU_INCLUDE_SPEC, m_ruu_include_spec )
		readItemLong( t_element, SOC_ARGU_RUU_COMMIT_WIDTH, m_ruu_commit_width )
		readItemLong( t_element, SOC_ARGU_RUU_SIZE, m_ruu_size )
		readItemLong( t_element, SOC_ARGU_LSQ_SIZE, m_lsq_size )

		readItemString( t_element, SOC_ARGU_CACHE_DL1_CFG, m_cache_dl1_cfg )
		readItemLong( t_element, SOC_ARGU_CACHE_DL1_DELAY, m_cache_dl1_delay )
		readItemString( t_element, SOC_ARGU_CACHE_DL2_CFG, m_cache_dl2_cfg )
		readItemLong( t_element, SOC_ARGU_CACHE_DL2_DELAY, m_cache_dl2_delay )
		readItemString( t_element, SOC_ARGU_CACHE_IL1_CFG, m_cache_il1_cfg )
		readItemLong( t_element, SOC_ARGU_CACHE_IL1_DELAY, m_cache_il1_delay )
		readItemString( t_element, SOC_ARGU_CACHE_IL2_CFG, m_cache_il2_cfg )
		readItemLong( t_element, SOC_ARGU_CACHE_IL2_DELAY, m_cache_il2_delay )
		readItemLong( t_element, SOC_ARGU_CACHE_I_COMPRESS_ENABLE,
			m_cache_i_compress_enable )

		readItemLong( t_element, SOC_ARGU_INTEGAR_ADDER_UNIT,
			m_integar_adder_unit )
		readItemLong( t_element, SOC_ARGU_INTEGAR_MULTI_UNIT,
			m_integar_multi_unit )
		readItemLong( t_element, SOC_ARGU_FLOAT_ADDR_UNIT, m_float_addr_unit )
		readItemLong( t_element, SOC_ARGU_FLOAT_MULTI_UNIT, m_float_multi_unit )
		readItemLong( t_element, SOC_ARGU_MEMORY_PORT, m_memory_port )

        readItemString( t_element, SOC_ARGU_ASIC_NAME, m_asic_name )
        readItemLong( t_element, SOC_ARGU_ASIC_TYPE, m_asic_type )
        readItemLong( t_element, SOC_ARGU_ASIC_DELAY, m_asic_delay )
	}
}

void EsySoCCfgTile::writeXml( TiXmlElement* root)
{
	writeItem( root, SOC_ARGU_TILE_ID, m_tile_id )
	TiXmlElement * t_pos_x_item = new TiXmlElement( SOC_ARGU_AX_X );
	t_pos_x_item->LinkEndChild( 
		new TiXmlText( EsyConvert( m_pos.first ).c_str() ) );
	root->LinkEndChild( t_pos_x_item );
	TiXmlElement * t_pos_y_item = new TiXmlElement( SOC_ARGU_AX_Y );
	t_pos_y_item->LinkEndChild( 
		new TiXmlText( EsyConvert( m_pos.second ).c_str() ) );
	root->LinkEndChild( t_pos_y_item );
	writeItem( root, SOC_ARGU_NI_ID, m_ni_id )
	writeItem( root, SOC_ARGU_NI_PHY, m_ni_phy )
	writeItem( root, SOC_ARGU_NI_POS, m_ni_pos )

	writeItem( root, SOC_ARGU_TILE_TYPE, m_tile_type )
	writeItem( root, SOC_ARGU_TILE_ENABLE, m_tile_enable )
	writeItem( root, SOC_ARGU_TILE_FREQ, m_tile_freq )

	writeItem( root, SOC_ARGU_MPI_ID, m_mpi_id )

	writeItem( root, SOC_ARGU_CORE_ISA_TYPE, m_core_isa_type )
	writeItem( root, SOC_ARGU_CORE_APP_DIR, m_core_app_dir )
	writeItem( root, SOC_ARGU_CORE_MAX_INST, m_core_max_inst )
	writeItem( root, SOC_ARGU_CORE_FETCH_SPEED, m_core_fetch_speed )

	writeItem( root, SOC_ARGU_PRED_UPDATE_TYPE, m_pred_update_type )
	writeItem( root, SOC_ARGU_PRED_PREDICT_TYPE, m_pred_predict_type )
	writeItem( root, SOC_ARGU_PRED_BIMOD_SIZE, m_pred_bimod_size )
	writeItem( root, SOC_ARGU_PRED_L1_SIZE, m_pred_l1_size )
	writeItem( root, SOC_ARGU_PRED_L2_SIZE, m_pred_l2_size )
	writeItem( root, SOC_ARGU_PRED_L2_HIST_SIZE, m_pred_l2_hist_size )
	writeItem( root, SOC_ARGU_PRED_L2_XOR_ENABLE, m_pred_l2_xor_enable )

	writeItem( root, SOC_ARGU_META_SIZE, m_meta_size )
	writeItem( root, SOC_ARGU_RETURN_STACK_SIZE, m_return_stack_size )
	writeItem( root, SOC_ARGU_BTB_SET, m_btb_set )
	writeItem( root, SOC_ARGU_BTB_COMBINATION, m_btb_combination )

	writeItem( root, SOC_ARGU_FIRST_CHUNK_DELAY, m_first_chunk_delay )
	writeItem( root, SOC_ARGU_NEIBER_CHUNK_DELAY, m_neiber_chunk_delay )
	writeItem( root, SOC_ARGU_TLB_ITLB_CFG, m_tlb_itlb_cfg )
	writeItem( root, SOC_ARGU_TLB_DTLB_CFG, m_tlb_dtlb_cfg )
	writeItem( root, SOC_ARGU_TLB_MISS_DELAY, m_tlb_miss_delay )
	writeItem( root, SOC_ARGU_MEMORY_BUS_WIDTH, m_memory_bus_width )

	writeItem( root, SOC_ARGU_RUU_IFQ_SIZE, m_ruu_ifq_size )
	writeItem( root, SOC_ARGU_RUU_BRANCH_PENALT, m_ruu_branch_penalt )
	writeItem( root, SOC_ARGU_RUU_DECODER_WIDTH, m_ruu_decoder_width )
	writeItem( root, SOC_ARGU_RUU_ISSUE_WIDTH, m_ruu_issue_width )
	writeItem( root, SOC_ARGU_RUU_INORDER_ISSU, m_ruu_inorder_issu )
	writeItem( root, SOC_ARGU_RUU_INCLUDE_SPEC, m_ruu_include_spec )
	writeItem( root, SOC_ARGU_RUU_COMMIT_WIDTH, m_ruu_commit_width )
	writeItem( root, SOC_ARGU_RUU_SIZE, m_ruu_size )
	writeItem( root, SOC_ARGU_LSQ_SIZE, m_lsq_size )

	writeItem( root, SOC_ARGU_CACHE_DL1_CFG, m_cache_dl1_cfg )
	writeItem( root, SOC_ARGU_CACHE_DL1_DELAY, m_cache_dl1_delay )
	writeItem( root, SOC_ARGU_CACHE_DL2_CFG, m_cache_dl2_cfg )
	writeItem( root, SOC_ARGU_CACHE_DL2_DELAY, m_cache_dl2_delay )
	writeItem( root, SOC_ARGU_CACHE_IL1_CFG, m_cache_il1_cfg )
	writeItem( root, SOC_ARGU_CACHE_IL1_DELAY, m_cache_il1_delay )
	writeItem( root, SOC_ARGU_CACHE_IL2_CFG, m_cache_il2_cfg )
	writeItem( root, SOC_ARGU_CACHE_IL2_DELAY, m_cache_il2_delay )
	writeItem( root, SOC_ARGU_CACHE_I_COMPRESS_ENABLE,
		m_cache_i_compress_enable )

	writeItem( root, SOC_ARGU_INTEGAR_ADDER_UNIT, m_integar_adder_unit )
	writeItem( root, SOC_ARGU_INTEGAR_MULTI_UNIT, m_integar_multi_unit )
	writeItem( root, SOC_ARGU_FLOAT_ADDR_UNIT, m_float_addr_unit )
	writeItem( root, SOC_ARGU_FLOAT_MULTI_UNIT, m_float_multi_unit )
	writeItem( root, SOC_ARGU_MEMORY_PORT, m_memory_port )

    writeItem( root, SOC_ARGU_ASIC_NAME, m_asic_name )
    writeItem( root, SOC_ARGU_ASIC_TYPE, m_asic_type )
    writeItem( root, SOC_ARGU_ASIC_DELAY, m_asic_delay )
}

EsySoCCfgNetwork::EsySoCCfgNetwork() :
	EsyNetworkCfg(), m_network_freq( DEFAULT_NETWORK_FREQ )
{}

EsySoCCfgNetwork::EsySoCCfgNetwork(const EsySoCCfgNetwork& t) :
	EsyNetworkCfg( t ), m_network_freq( t.networkFreq() )
{}

void EsySoCCfgNetwork::readXml(TiXmlElement* root)
{
	EsyNetworkCfg::readXml( root );
	
	for ( TiXmlNode * p_child = root->FirstChild(); p_child != 0;
		p_child = p_child->NextSibling() )
	{
		TiXmlElement * t_element = p_child->ToElement();
		readItemLong( t_element, SOC_ARGU_NETWORK_FREQ, m_network_freq )
	}
}

void EsySoCCfgNetwork::writeXml(TiXmlElement* root)
{
	EsyNetworkCfg::writeXml( root );
	writeItem( root, SOC_ARGU_NETWORK_FREQ, m_network_freq );
}

EsySoCCfg::EsySoCCfg() :
	m_tile( 0, EsySoCCfgTile() ), m_network()
{
	m_network.setTopology( EsyNetworkCfg::NT_IRREGULAR );
	m_network.setSize( 0, 1 );
	m_network.templateRouter().appendPort( EsyNetworkCfgPort() );
	m_network.templateRouter().port( 0 ).setNetworkInterface( true );
	m_network.templateRouter().port( 0 ).setPortDirection(
		EsyNetworkCfgPort::ROUTER_PORT_NORTHWEST );
}
EsySoCCfg::EsySoCCfg( const EsySoCCfg & t ) :
	m_tile( t.tile() ), m_network( t.network() )
{}

void EsySoCCfg::readXml( TiXmlElement * root )
{
	for ( TiXmlNode * p_child = root->FirstChild(); p_child != 0;
		 p_child = p_child->NextSibling() )
	{
		TiXmlElement * t_element = p_child->ToElement();

		if ( t_element->Value() == string( SOC_ARGU_TILE ) )
		{
			m_tile.resize( 
				EsyConvert( t_element->Attribute( SOC_ARGU_SIZE ) ) );
			for ( TiXmlNode * p_tile_child = t_element->FirstChild();
				 p_tile_child != 0;
				 p_tile_child = p_tile_child->NextSibling() )
			{
				TiXmlElement * t_tile_element = p_tile_child->ToElement();
				m_tile[ EsyConvert( 
					t_tile_element->Attribute( SOC_ARGU_INDEX ) ) ]
					.readXml( t_tile_element );
			}
		}
		else if ( t_element->Value() == string( SOC_ARGU_TEMPLATE_TILE ))
		{
			m_template_tile.readXml( t_element );
		}
		else if ( t_element->Value() == string( SOC_ARGU_NETWORK ) )
		{
			m_network.readXml( t_element );
		}
	}
}

void EsySoCCfg::writeXml( TiXmlElement * root )
{
	TiXmlElement * t_template_tile_item =
		new TiXmlElement( SOC_ARGU_TEMPLATE_TILE );
	m_template_tile.writeXml( t_template_tile_item );
	root->LinkEndChild( t_template_tile_item );

	TiXmlElement * t_tile_list_item = new TiXmlElement( SOC_ARGU_TILE );
	t_tile_list_item->SetAttribute( SOC_ARGU_SIZE, (int)m_tile.size() );
	for ( size_t i = 0; i < m_tile .size(); i ++ )
	{
		TiXmlElement * t_tile_item = new TiXmlElement( SOC_ARGU_DATA );
		t_tile_item->SetAttribute( SOC_ARGU_INDEX, (int)i );
		m_tile[ i ].writeXml( t_tile_item );
		t_tile_list_item->LinkEndChild( t_tile_item );
	}
	root->LinkEndChild( t_tile_list_item );

	TiXmlElement * t_network_item = new TiXmlElement( SOC_ARGU_NETWORK );
	m_network.writeXml( t_network_item );
	root->LinkEndChild( t_network_item );
}

EsyXmlError EsySoCCfg::readXml( const string & fname )
{
	string tname = fname + "." + SOCCFG_EXTENSION;
	TiXmlDocument t_doc( tname.c_str() );
	if ( !t_doc.LoadFile() )
	{
		return EsyXmlError( t_doc.ErrorDesc(), fname,
			EsyConvert( t_doc.ErrorRow() ), EsyConvert( t_doc.ErrorCol() ),
			"EsySoCCfg", "readXml" );
	}

	TiXmlElement * t_head = t_doc.RootElement();
	if ( t_head->Value() != string( SOC_ARGU_ROOT ) )
	{
		return EsyXmlError( t_head->Value(), fname, "EsySoCCfg",
			"readXml" );
	}

	readXml( t_head );
    
    t_doc.Clear();
	return EsyXmlError();
}

EsyXmlError EsySoCCfg::writeXml( const string & fname )
{
	TiXmlDocument t_doc;
	TiXmlDeclaration* t_decl = new TiXmlDeclaration(
		SOC_ARGU_DECLARATION_VERSION, SOC_ARGU_DECLARATION_ENCODING, "" );
	t_doc.LinkEndChild( t_decl );

	TiXmlElement * t_root = new TiXmlElement( SOC_ARGU_ROOT );
	t_doc.LinkEndChild( t_root );

	writeXml( t_root );

	string tname = fname + "." + SOCCFG_EXTENSION;
	t_doc.SaveFile( tname.c_str() );

    t_doc.Clear();
	return EsyXmlError();
}

void EsySoCCfg::updateSoC()
{
	m_network.updateRouter();

	m_tile.clear();
	updateTile();
}

void EsySoCCfg::updateTile()
{
	long tile_seq = 0;
	for ( int router = 0; router < m_network.routerCount(); router ++ )
	{
		for ( int phy = 0; phy < m_network.router( router ).portNum();
			phy ++ )
		{
			if ( m_network.router( router ).port( phy ).networkInterface() )
			{
				// update tile to this ni
				EsySoCCfgTile tile_t;
				// no this tile
				while( tile_seq < (long)m_tile.size() )
				{
					if ( tile_t.niId() > router )
					{
						tile_t = m_template_tile;
						break;
					}
					else if ( tile_t.niId() < router )
					{
						m_tile.erase( m_tile.begin() + tile_seq );
					}
					else if ( tile_t.niPhy() > phy )
					{
						tile_t = m_template_tile;
						break;
					}
					else if ( tile_t.niPhy() < phy )
					{
						m_tile.erase( m_tile.begin() + tile_seq );
					}
					else
					{
						tile_t = m_tile[ tile_seq ];
						break;
					}
				}
				if ( tile_seq >= (long)m_tile.size() )
				{
					tile_t = m_template_tile;
				}

				tile_t.setTileId( tile_seq );
				tile_t.setNiId( router );
				tile_t.setNiPhy( phy );

				EsySoCCfgTile::NIPosition nipos;
				switch ( m_network.router( router ).port( phy ).portDirection() )
				{
				case EsyNetworkCfgPort::ROUTER_PORT_NORTH:
					nipos = EsySoCCfgTile::NIPOS_SOUTHEAST; break;
				case EsyNetworkCfgPort::ROUTER_PORT_SOUTH:
					nipos = EsySoCCfgTile::NIPOS_NORTHWEST; break;
				case EsyNetworkCfgPort::ROUTER_PORT_EAST:
					nipos = EsySoCCfgTile::NIPOS_SOUTHWEST; break;
				case EsyNetworkCfgPort::ROUTER_PORT_WEST:
					nipos = EsySoCCfgTile::NIPOS_NORTHEAST; break;
				case EsyNetworkCfgPort::ROUTER_PORT_NORTHWEST:
					nipos = EsySoCCfgTile::NIPOS_SOUTHEAST; break;
				case EsyNetworkCfgPort::ROUTER_PORT_NORTHEAST:
					nipos = EsySoCCfgTile::NIPOS_SOUTHWEST; break;
				case EsyNetworkCfgPort::ROUTER_PORT_SOUTHWEST:
					nipos = EsySoCCfgTile::NIPOS_NORTHEAST; break;
				case EsyNetworkCfgPort::ROUTER_PORT_SOUTHEAST:
					nipos = EsySoCCfgTile::NIPOS_NORTHWEST; break;
				default:
					nipos = EsySoCCfgTile::NIPOS_SOUTHEAST; break;
				}

				tile_t.setNiPos( nipos );

				// no this tile
				if ( tile_seq >= (long)m_tile.size() )
				{
					m_tile.push_back( tile_t );
				}
				else if ( m_tile[ tile_seq ].niId() == router &&
						  m_tile[ tile_seq ].niPhy() == phy )
				{
					m_tile[ tile_seq ] = tile_t;
				}
				else
				{
					m_tile.insert( m_tile.begin() + tile_seq, tile_t );
				}
				tile_seq ++;
			}
		}
	}
}

#undef readItemLong
#undef readItemString
#undef readItemEnum
#undef writeItem
