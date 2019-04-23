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

#ifndef ESYNET_PORT_UNIT_H
#define ESYNET_RORT_UNIT_H

/* including head file */
#include "esy_networkcfg.h"
#include "esynet_flit.h"


class EsynetInputVirtualChannel
{
/* Properties */
private:
    /* input buffers: [unit] */
	vector< EsynetFlit > m_input_buffer;
    /* the state of each input vc */
    VCStateType m_vc_state;
    /* routing requirement, insert by routing determine: [require] */
    vector< VCType > m_routing;
    /* the chosen routing vc */
    VCType m_connection;
    
/* Public functions */
public:
    /* constructor */
	EsynetInputVirtualChannel();
	EsynetInputVirtualChannel(EsyNetworkCfgPort * port_cfg);

    /* function to access m_input_buffer */
    long flitCount() const { return (long)m_input_buffer.size(); }
    /* return the first flit in physical port phy and virtual channel vc */
	const EsynetFlit & getFlit() const { return m_input_buffer[0]; }
	EsynetFlit & getFlit() { return m_input_buffer[0]; }
    /* add a flit flit to phyiscal port phy virtual channel vc */
	void addFlit(const EsynetFlit & flit) { m_input_buffer.push_back(flit); }
    /* remove first flit from phyiscal port phy virtual channel vc */
    void removeFlit() { m_input_buffer.erase( m_input_buffer.begin() ); }

    /* function to access states_ */
    /* return states for all virtual channel */
    VCStateType state() const { return m_vc_state; }
    /* set state of physical port phy and virtual channel vc */
    void updateState( VCStateType state ) { m_vc_state = state; }

    /* function to access crouting_ */
    /* return crouitng for physical port phy virtual channel vc */
    const VCType & connection() const { return m_connection; }
    /* set crouting for physical port phy and virtual channel vc */
    void assignConnection( VCType crouting ) { m_connection = crouting; }
    /* clear crouting for physical port phy and virtual channel vc */
    void clearConnection() { m_connection = VC_NULL; }

    /* function to access routing_ */
    /* return routing requirement for physical port phy virtual channel vc */
    const vector< VCType > & routing() const { return m_routing; }
    /* insert routing requirement routing to physical port phy virtual channel
     * vc */
    void addRouting( const VCType& routing ) { m_routing.push_back( routing ); }
    /* clear routing requirement for physical port phy and virtual channel vc */
    void clearRouting() { m_routing.clear(); }
};

class EsynetInputPort : public vector< EsynetInputVirtualChannel >
{
/* Properties */
private:
    /* input neithbor */
    VCType m_neighbor_addr;
    /* each vc buffer size */
    long m_input_buffer_size; 
    
/* Public functions */
public:
    /* constructor */
	EsynetInputPort();
	EsynetInputPort(EsyNetworkCfgPort * port_cfg);
    
    long vcNumber() const { return (long)size(); }

    /* function to access inputNeighborAddr */
    const VCType & inputNeighborAddr() const { return m_neighbor_addr; }
    long inputNeighborAddrId() const { return m_neighbor_addr.first; }
    long inputNeighborAddrPort() const { return m_neighbor_addr.second; }
    void setInputNeighborAddr( const VCType & addr ) { m_neighbor_addr = addr; }
        
    long inputBufferSize() { return m_input_buffer_size; }
   
    bool isEmpty();
    
/* Related Non-Members */
public:
    /* print input model information to stream */
	friend ostream& operator<<(ostream& os, const EsynetInputPort & Ri);
};

class EsynetOutputVirtualChannel
{
/* Properties */
private:
    /* assigned for the input [vc], default is VC_NULL */
    VCType m_vc_assign;
    /* virtual channel usage type, USED_ or FREE_ [vc], default is FREE_ */
    VCUsageType m_vc_usage;
    /* credit counter */
    long m_credit_counter;
    
/* Public Functions */
public:
    /* constructor */
	EsynetOutputVirtualChannel();
	EsynetOutputVirtualChannel(EsyNetworkCfgPort * port_cfg);

    long creditCounter() const { return m_credit_counter; }
    void incCreditCounter() { m_credit_counter ++; }
    void decCreditCounter() { m_credit_counter --; }
    void setCreditCounter( long credit ) { m_credit_counter = credit; }

    /* function to access usage_ */
    /* return usage state for physical port phy virtual channel vc */
    VCUsageType usage() const { return m_vc_usage; }

    /* function to access usage_ */
    /* return assign for physical port phy virtual channel vc */
    const VCType & assign() const { return m_vc_assign; }

    /* assign output port a and virual channel b with channel c */
    void acquire( const VCType & channel )
        { m_vc_usage = VC_USED; m_vc_assign = channel; }
    /* release assignment of output port a and virual channel b */
    void release()
        { m_vc_usage = VC_FREE; m_vc_assign = VC_NULL; }
};

class EsynetOutputPort : public vector< EsynetOutputVirtualChannel >
{
/* Properties */
private:
    /* local output buffers [unit]*/
	vector< EsynetFlit > m_output_buffer;
    /* output address [unit] */
    vector< VCType > m_out_add;
    /* output neighbor address */
    VCType m_neighbor_addr;
    /* each vc buffer size */
    long m_output_buffer_size;    
    /* one flit on the link */
    bool m_flit_on_link;
    
/* Public Functions */
public:
    /* constructor */
	EsynetOutputPort();
	EsynetOutputPort(EsyNetworkCfgPort * port_cfg);

    long vcNumber() const { return size(); }

    /* function to access outbuffers_ */
    long flitCount() const { return (long)m_output_buffer.size(); }
    /* return the first flit in buffer of physical port phy */
	const EsynetFlit & getFlit() const { return m_output_buffer[0]; }
    /* add flit into buffer of physical port phy */
	void addFlit(const EsynetFlit& flit) { m_output_buffer.push_back(flit); }
    /* remove the first flit in buffer of physical port phy */
    void removeFlit() { m_output_buffer.erase( m_output_buffer.begin() ); }

    /* function to access outadd_ */
    /* return the first address of port phy */
    const VCType & getAdd() const { return m_out_add[ 0 ]; }
    /* remove the first address of port phy */
    void removeAdd() { m_out_add.erase( m_out_add.begin() ); }
    /* insert address vc to queue of port phy */
    void addAdd( const VCType & vc ) { m_out_add.push_back( vc ); }

    /* function to access outputNeighborAddr */
    const VCType & outputNeighborAddr() const { return m_neighbor_addr; }
    long outputNeighborAddrId() const { return m_neighbor_addr.first; }
    long outputNeighborAddrPort() const { return m_neighbor_addr.second; }
    void setOutputNeighborAddr( const VCType & addr ) 
        { m_neighbor_addr = addr; }

    /* function to access m_flit_on_link */
    bool flitOnLink() { return m_flit_on_link; }
    void setFlitOnLink() { m_flit_on_link = true; }
    void clearFlitOnLink() { m_flit_on_link = false; }
    
    bool isEmpty();

/* Related Non-Members */
public:
    /* print input model information to stream */
	friend ostream& operator<<(ostream& os, const EsynetOutputPort & Ri);
};

#endif
