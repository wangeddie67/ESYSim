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

#include "esy_interdata.h"

EsyDataItemBenchmark::EsyDataItemBenchmark() :
    EsyDataListItem(),m_time( -1 ), m_src( -1 ), m_dst( -1 ), m_size( -1 )
{
    insertVariables();
}

EsyDataItemBenchmark::EsyDataItemBenchmark(
	double time, short src, short dst, short size) :
    EsyDataListItem(), m_time( time ), m_src( src ), m_dst( dst ), m_size( size )
{
    insertVariables();
}
    
EsyDataItemBenchmark::EsyDataItemBenchmark(const EsyDataItemBenchmark & t) :
    EsyDataListItem(),
    m_time( t.time() ), m_src( t.src() ), m_dst( t.dst() ),  m_size( t.size() )
{
    insertVariables();
}

void EsyDataItemBenchmark::insertVariables()
{
    insertDouble( &m_time );
    insertShort( &m_src );
    insertShort( &m_dst );
    insertShort( &m_size );
}

EsyDataItemEventtrace::EsyDataItemEventtrace() :
    EsyDataListItem(), m_time( 0 ), m_type( 0 ),
    m_src( -1 ), m_src_pc( -1 ), m_src_vc( -1 ),
    m_dst( -1 ), m_dst_pc( -1 ), m_dst_vc( -1 ),
    m_flit_id( 0 ), m_flit_size( 0 ), m_flit_type( 0 ),
    m_flit_src( -1 ),  m_flit_dst( -1 ), m_flit_flag( 0 ), m_start_time( 0 )
{ 
    insertVariables();
}

EsyDataItemEventtrace::EsyDataItemEventtrace(double time, char type,
        short src, char src_pc, char src_vc,
        short dst, char dst_pc, char dst_vc,
        long flit_id, short flit_size, char flit_type,
        short flit_src, short flit_dst, char flit_flag,
        double start_time ):
    EsyDataListItem(), m_time( time ), m_type( type ),
    m_src( src ), m_src_pc( src_pc ), m_src_vc( src_vc ),
    m_dst( dst ), m_dst_pc( dst_pc ), m_dst_vc( dst_vc ),
    m_flit_id( flit_id ), m_flit_size( flit_size ), m_flit_type( flit_type ),
    m_flit_src( flit_src ), m_flit_dst( flit_dst ),
    m_flit_flag( flit_flag ), m_start_time( start_time )
{
    insertVariables();
}

EsyDataItemEventtrace::EsyDataItemEventtrace(const EsyDataItemEventtrace& t) :
    EsyDataListItem(), m_time( t.time() ), m_type( t.type() ),
    m_src( t.src() ), m_src_pc( t.srcPc() ), m_src_vc( t.srcVc() ),
    m_dst( t.dst() ), m_dst_pc( t.dstPc() ), m_dst_vc( t.dstVc() ),
    m_flit_id( t.flitId() ),
    m_flit_size( t.flitSize() ), m_flit_type( t.flitType() ),
    m_flit_src( t.flitSrc() ), m_flit_dst( t.flitDst() ),
    m_flit_flag( t.flitFlag() ), m_start_time( t.flitStartTime() )
{
    insertVariables();
}

void EsyDataItemEventtrace::insertVariables()
{
    insertDouble( &m_time );
    insertNum8( &m_type );
    insertShort( &m_src );
    insertNum8( &m_src_pc );
    insertNum8( &m_src_vc );
    insertShort( &m_dst );
    insertNum8( &m_dst_pc );
    insertNum8( &m_dst_vc );
    insertLong( &m_flit_id );
    insertShort( &m_flit_size );
    insertNum8( &m_flit_type );
    insertShort( &m_flit_src );
    insertShort( &m_flit_dst );
    insertNum8( &m_flit_flag );
    insertDouble( &m_start_time );
}

EsyDataItemPathpair::EsyDataItemPathpair() :
    EsyDataListItem(), m_src( -1 ), m_dst( -1 ), m_hop( -1 )
{  
    insertVariables();
}

EsyDataItemPathpair::EsyDataItemPathpair(short src, short dst, short hop) :
    EsyDataListItem(), m_src( src ), m_dst( dst ), m_hop( hop )
{ 
    insertVariables();
}

EsyDataItemPathpair::EsyDataItemPathpair(const EsyDataItemPathpair & t) :
    EsyDataListItem(), m_src( t.src() ), m_dst( t.dst() ), m_hop( t.hop() )
{
    insertVariables();
}

void EsyDataItemPathpair::insertVariables()
{
    insertShort( &m_src );
    insertShort( &m_dst );
    insertShort( &m_hop );
}

EsyDataItemHop::EsyDataItemHop() :
    m_addr( -1 ), m_in_pc( -1 ), m_in_vc( -1 ), m_out_pc( -1 ), m_out_vc( -1 )
{}
EsyDataItemHop::EsyDataItemHop(
    long addr, long in_pc, long in_vc, long out_pc, long out_vc
):
    m_addr( addr ), m_in_pc( in_pc ), m_in_vc( in_vc ),
    m_out_pc( out_pc ),  m_out_vc( out_vc )
{}

EsyDataItemHop::EsyDataItemHop(const EsyDataItemHop & t) :
    m_addr( t.addr() ), m_in_pc( t.inputPc() ), m_in_vc( t.inputVc() ),
    m_out_pc( t.outputPc() ), m_out_vc( t.outputVc() )
{}

EsyDataItemTraffic::EsyDataItemTraffic() :
	EsyDataItemHop(), m_traffic(-1), m_faulty(false)
{}

EsyDataItemTraffic::EsyDataItemTraffic(
    long id, long in_pc, long in_vc, long out_pc, long out_vc, long count
) :
	EsyDataItemHop(id, in_pc, in_vc, out_pc, out_vc),
    m_traffic( count ), m_faulty( false )
{}

EsyDataItemTraffic::EsyDataItemTraffic(const EsyDataItemTraffic & t) :
	EsyDataItemHop(t.addr(), t.inputPc(), t.inputVc(),
                  t.outputPc(), t.outputVc() ),
    m_traffic( t.traffic() ), m_faulty( false )
{}

EsyDataItemPathtrace::EsyDataItemPathtrace() :
    EsyDataListItem(), m_src( -1 ), m_dst( -1 ), m_trace()
{  
    insertVariables();
}

EsyDataItemPathtrace::EsyDataItemPathtrace(
    long src, long dst, const vector< long > & trace
) :
    EsyDataListItem(), m_src( src ), m_dst( dst ), m_trace( trace )
{ 
    insertVariables();
}

EsyDataItemPathtrace::EsyDataItemPathtrace(
	long src, long dst, const vector< EsyDataItemHop > & trace
) :
    EsyDataListItem(), m_src( src ), m_dst( dst ), m_trace()
{ 
    insertVariables();
    for ( size_t i = 0; i < trace .size(); i ++ )
    {
        m_trace.push_back( trace[ i ].addr() );
        m_trace.push_back( trace[ i ].inputPc() );
        m_trace.push_back( trace[ i ].inputVc() );
        m_trace.push_back( trace[ i ].outputPc() );
        m_trace.push_back( trace[ i ].outputVc() );
    }
}
    
EsyDataItemPathtrace::EsyDataItemPathtrace(const EsyDataItemPathtrace& t) :
    EsyDataListItem(),
    m_src( t.src() ), m_dst( t.dst() ), m_trace( t.trace() )
{
     insertVariables();
}

vector< EsyDataItemHop > EsyDataItemPathtrace::pathhoplist() const
{
	vector< EsyDataItemHop > hop_list;
    for ( size_t i = 0; i < m_trace .size(); i = i + 5 )
    {
		hop_list.push_back(EsyDataItemHop(
            m_trace[ i ], m_trace[ i + 1 ], m_trace[ i + 2 ],
            m_trace[ i + 3 ], m_trace[ i + 4 ] ) );
    }
    return hop_list;
}

void EsyDataItemPathtrace::insertVariables()
{
    insertLong( &m_src );
    insertLong( &m_dst );
    insertLongVector( &m_trace, 'e' );
}

EsyDataItemTrafficWindow::EsyDataItemTrafficWindow() :
    m_start( -1 ), m_end( -1 ), 
    m_inject(), m_accept(), m_through()
{
    insertVariables();
}

EsyDataItemTrafficWindow::EsyDataItemTrafficWindow( 
    double start, double end, long size ) :
    m_start( start ), m_end( end ), 
    m_inject( size ), m_accept( size ), m_through( size )
{
    insertVariables();
}

EsyDataItemTrafficWindow::EsyDataItemTrafficWindow(
    const EsyDataItemTrafficWindow& t ):
    m_start( t.start() ), m_end( t.end() ),
    m_inject( t.injectTraffic() ), m_accept( t.acceptTraffic() ), 
    m_through( t.throughTraffic() )
{
    insertVariables();
}

void EsyDataItemTrafficWindow::insertVariables()
{
    insertDouble( &m_start );
    insertDouble( &m_end );
    insertLongVector( &m_inject, 'e' );
    insertLongVector( &m_accept, 'e' );
    insertLongVector( &m_through, 'e' );
}

EsyDataItemSoCRecord::EsyDataItemSoCRecord() :
	m_valid( 0 ), m_hop( 0 ), m_actual_id( -1 ), m_sim_cycle( 0 ),
	m_il1_access( 0 ), m_il1_hit( 0 ), m_il1_miss( 0 ),
	m_dl1_access( 0 ), m_dl1_hit( 0 ), m_dl1_miss( 0 ),
	m_il2_access( 0 ), m_il2_hit( 0 ), m_il2_miss( 0 ),
	m_dl2_access( 0 ), m_dl2_hit( 0 ), m_dl2_miss( 0 ),
	m_mem_access( 0 ), m_msg_send( 0 ), m_msg_recv( 0 ), m_msg_probe( 0 ),
	m_add_integer( 0 ), m_multi_integar( 0 ), m_add_float( 0 ), m_multi_float( 0 )
{
	insertVariables();
}

EsyDataItemSoCRecord::EsyDataItemSoCRecord(
	char valid, double hop, short actual_id, double sim_cycle,
	char il1_access, char il1_hit, char il1_miss,
	char dl1_access, char dl1_hit, char dl1_miss,
	char il2_access, char il2_hit, char il2_miss,
	char dl2_access, char dl2_hit, char dl2_miss,
	char mem_access, char msg_send, char msg_recv, char msg_probe,
	char add_integar, char multi_integar, char add_float, char multi_float
) :
	m_valid( valid ),
	m_hop( hop ), m_actual_id( actual_id ), m_sim_cycle( sim_cycle ),
	m_il1_access( il1_access ), m_il1_hit( il1_hit ), m_il1_miss( il1_miss ),
	m_dl1_access( dl1_access ), m_dl1_hit( dl1_hit ), m_dl1_miss( dl1_miss ),
	m_il2_access( il2_access ), m_il2_hit( il2_hit ), m_il2_miss( il2_miss ),
	m_dl2_access( dl2_access ), m_dl2_hit( dl2_hit ), m_dl2_miss( dl2_miss ),
	m_mem_access( mem_access ), m_msg_send( msg_send ), m_msg_recv( msg_recv ),
	m_msg_probe( msg_probe ),
	m_add_integer( add_integar ), m_multi_integar( multi_integar ),
	m_add_float( add_float ), m_multi_float( multi_float )
{
	insertVariables();
}

EsyDataItemSoCRecord::EsyDataItemSoCRecord(const EsyDataItemSoCRecord & t) :
	m_valid( t.valid() ),
	m_hop( t.hop() ), m_actual_id( t.actualId() ), m_sim_cycle( t.simCycle() ),
	m_il1_access( t.il1Access() ), m_il1_hit( t.il1Hit() ),
	m_il1_miss( t.il1Miss() ),
	m_dl1_access( t.dl1Access() ), m_dl1_hit( t.dl1Hit() ),
	m_dl1_miss( t.dl1Miss() ),
	m_il2_access( t.il2Access() ), m_il2_hit( t.il2Hit() ),
	m_il2_miss( t.il2Miss() ),
	m_dl2_access( t.dl2Access() ), m_dl2_hit( t.dl2Hit() ),
	m_dl2_miss( t.dl2Miss() ),
	m_mem_access( t.memAccess() ), m_msg_send( t.msgSend() ),
	m_msg_recv( t.msgRecv() ), m_msg_probe( t.msgProbe() ),
	m_add_integer( t.addInteger() ), m_multi_integar( t.multiIntegar() ),
	m_add_float( t.addFloat() ), m_multi_float( t.multiFloat() )
{
	insertVariables();
}

void EsyDataItemSoCRecord::insertVariables()
{
	insertNum8( &m_valid );

	insertDouble( &m_hop );
	insertShort( &m_actual_id );
	insertDouble( &m_sim_cycle );

	insertNum8( &m_il1_access );
	insertNum8( &m_il1_hit );
	insertNum8( &m_il1_miss );

	insertNum8( &m_dl1_access );
	insertNum8( &m_dl1_hit );
	insertNum8( &m_dl1_miss );

	insertNum8( &m_il2_access );
	insertNum8( &m_il2_hit );
	insertNum8( &m_il2_miss );

	insertNum8( &m_dl2_access );
	insertNum8( &m_dl2_hit );
	insertNum8( &m_dl2_miss );

	insertNum8( &m_mem_access );

    insertNum8( &m_msg_send );
    insertNum8( &m_msg_recv );
    insertNum8( &m_msg_probe );

    insertNum8( &m_add_integer );
    insertNum8( &m_multi_integar );
    insertNum8( &m_add_float );
    insertNum8( &m_multi_float );
}

EsyDataItemRecordWindow::EsyDataItemRecordWindow() :
    m_start( -1 ), m_end( -1 ), 
    m_il1_access(), m_il1_hit(), m_il1_miss(),
    m_dl1_access(), m_dl1_hit(), m_dl1_miss(),
    m_il2_access(), m_il2_hit(), m_il2_miss(),
    m_dl2_access(), m_dl2_hit(), m_dl2_miss(),
    m_mem_access(), m_msg_send(), m_msg_recv(), m_msg_probe(),
    m_add_integer(), m_multi_integar(), m_add_float(), m_multi_float()
{
    insertVariables();
}

EsyDataItemRecordWindow::EsyDataItemRecordWindow( 
    double start, double end, long size ) :
    m_start( start ), m_end( end ), 
    m_il1_access( size ), m_il1_hit( size ), m_il1_miss( size ),
    m_dl1_access( size ), m_dl1_hit( size ), m_dl1_miss( size ),
    m_il2_access( size ), m_il2_hit( size ), m_il2_miss( size ),
    m_dl2_access( size ), m_dl2_hit( size ), m_dl2_miss( size ),
    m_mem_access( size ), m_msg_send( size ), m_msg_recv( size ),
    m_msg_probe( size ),
    m_add_integer( size ), m_multi_integar( size ),
    m_add_float( size ), m_multi_float( size )
{
    insertVariables();
}

EsyDataItemRecordWindow::EsyDataItemRecordWindow(
    const EsyDataItemRecordWindow& t ):
    m_start( t.start() ), m_end( t.end() ),
    m_il1_access( t.il1Access() ), m_il1_hit( t.il1Hit() ),
    m_il1_miss( t.il1Miss() ),
    m_dl1_access( t.dl1Access() ), m_dl1_hit( t.dl1Hit() ),
    m_dl1_miss( t.dl1Miss() ),
    m_il2_access( t.il2Access() ), m_il2_hit( t.il2Hit() ),
    m_il2_miss( t.il2Miss() ),
    m_dl2_access( t.dl2Access() ), m_dl2_hit( t.dl2Hit() ),
    m_dl2_miss( t.dl2Miss() ),
    m_mem_access( t.memAccess() ), m_msg_send( t.msgSend() ),
    m_msg_recv( t.msgRecv() ), m_msg_probe( t.msgProbe() ),
    m_add_integer( t.addInteger() ), m_multi_integar( t.multiIntegar() ),
    m_add_float( t.addFloat() ), m_multi_float( t.multiFloat() )
{
    insertVariables();
}

void EsyDataItemRecordWindow::insertVariables()
{
    insertDouble( &m_start );
    insertDouble( &m_end );

    insertLongVector( &m_il1_access, 'e' );
    insertLongVector( &m_il1_hit, 'e' );
    insertLongVector( &m_il1_miss, 'e' );

    insertLongVector( &m_dl1_access, 'e' );
    insertLongVector( &m_dl1_hit, 'e' );
    insertLongVector( &m_dl1_miss, 'e' );

    insertLongVector( &m_il2_access, 'e' );
    insertLongVector( &m_il2_hit, 'e' );
    insertLongVector( &m_il2_miss, 'e' );

    insertLongVector( &m_dl2_access, 'e' );
    insertLongVector( &m_dl2_hit, 'e' );
    insertLongVector( &m_dl2_miss, 'e' );

    insertLongVector( &m_mem_access, 'e' );

    insertLongVector( &m_msg_send, 'e' );
    insertLongVector( &m_msg_recv, 'e' );
    insertLongVector( &m_msg_probe, 'e' );

    insertLongVector( &m_add_integer, 'e' );
    insertLongVector( &m_multi_integar, 'e' );
    insertLongVector( &m_add_float, 'e' );
    insertLongVector( &m_multi_float, 'e' );
}
