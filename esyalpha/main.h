//main.h
///////////////////////Ruke/////////////////
#define	SUPPORT_PREFETCH	//Support bypassing prefetching
//#define	SUPPORT_SQ_EMPTY	//Support bypassing SQ empty loads
//#define	SUPPORT_SQ_EMP_ENHANCE	//Support enhanced method of bypassing SQ empty loads
//#define	SUPPORT_LOAD_FREE	//Support bypassing free load identified by the parser

#define LSQ_LOAD_FAST_ISSUE		//Load is allow to issue even there are older
				//stores with unresolved address. Load replay is needed
				//when the load turns out to conflict with an older store.
				//If the store address overlaps the load address, but the
				//store data is not ready, the load will not issue.

				//If you don't allow this feature, comment this define.
				//Load will issue only when previous stores are resolved
				//(both address and store data are ready). No replay is needed.

#define	LSQ_FIX_PARTIAL_BUG	//Fix the bug of not considering addresses
				//partial overlapping.


typedef struct RES_DESC
{
	char *strName;				// name of functional unit
	int nCnt;				// total instances of this unit */

	int oplat;				// operation latency: cycles until
							//result is ready for use
	int issuelat;			// issue latency: number of cycles
							//before another operation can be
							//issued on this resource
	int Busy[16];			// non-zero if the unit is busy
							//At most 16 units are allowed.
}ResDesc, *PResDesc;

ResDesc m_SQPort;	//R/W ports of store queue.

//bool_t	GetSQPort();
//void	RelSQPort();

///////////////////////////////////////////////////////



