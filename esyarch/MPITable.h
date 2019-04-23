/*
 * MPITable.h
 *
 *  Created on: 2016年5月9日
 *      Author: root
 */

#ifndef MPITABLE_H_
#define MPITABLE_H_
#include "define.h"
#define ID_TYPE 4//0:using or not; 1:id; 2:master_id;  3:actual_id;
class MPITable
{
public:
	int ID_TABLE[MAXTHREADS][ID_TYPE];
	MPITable();
	virtual ~MPITable();
	void Id_table_init();
	int Id_table_assign(int id,int master_id,int actual_id);
	int Id_table_get(int master_id,int id);
};




#endif /* MPITABLE_H_ */
