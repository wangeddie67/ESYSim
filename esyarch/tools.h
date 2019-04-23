/*
 * tools.h
 *
 *  Created on: 2016年10月24日
 *      Author: root
 */

#ifndef TOOLS_H_
#define TOOLS_H_
#include <string.h>
typedef struct Tool_Struct
{
	int operator()(const char* a, const char* b)
    {
		return strcmp(a, b);
	}

}Toolstruct;



#endif /* TOOLS_H_ */
