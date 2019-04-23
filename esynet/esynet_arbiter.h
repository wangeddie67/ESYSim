
#ifndef ESYNET_ARBITER_H
#define ESYNET_ARBITER_H

#include "esynet_global.h"
#include "esy_networkcfg.h"

#include <vector>
#include <map>

using namespace std;
using namespace esynet;

class EsynetArbiter
{
public:
	enum ArbiterType
	{
		ARBITER_MATRIX,
		ARBITER_RR,
		ARBITER_RANDOM
	};

private:
	ArbiterType m_type;
	int m_size;
	
	vector< vector< bool > > m_state_matrix;
	vector< bool > m_state_vector;

	map< int, VCType > m_vc_map;
	vector< bool > m_request;

public:
	EsynetArbiter( ArbiterType type, int size );
	EsynetArbiter( const EsynetArbiter & t );

	vector< bool > & request() { return m_request; }
	const vector< bool > & request() const { return m_request; }
	void setRequest( const vector< bool > & req ) { m_request = req; }

	void setHighestPriority( int port );
	void setHighestPriority( const VCType & port );
	void setLowestPriority( int port );
	void setLowestPriority( const VCType & port );
	
	void setInputPortMap( const EsyNetworkCfgRouter & router_cfg );
	void setOutputPortMap( const EsyNetworkCfgRouter & router_cfg );

	void setRequest( int a );
	void setRequest( const VCType & a );
	void clearRequest( int a );
	void clearRequest( const VCType & a );
	void clearRequest();
	
	bool reqestIsValid();
	
	int grant();
	VCType grantVc();
};

#endif