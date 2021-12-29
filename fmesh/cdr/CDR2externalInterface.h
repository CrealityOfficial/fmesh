#ifndef __CDR_2_EXTERNAL_INTERFACE_H__
#define __CDR_2_EXTERNAL_INTERFACE_H__
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>
#include <vector>
#include <clipper/clipper.hpp>
namespace CDRUSERINTERFACE
{

class CDR_USER_OBJ
{
	private:
		bool m_initedflg;
	public:
		CDR_USER_OBJ();
		~CDR_USER_OBJ();
	public:
		 ClipperLib::Paths  *m_Paths;
	public:	
	int loadCDR(char* filename);
	int extractCDRData();
	int jsonParseBaseCdr(char *Jsonbuff);
	ClipperLib::Path DrawBzier(ClipperLib::DoublePoint *pointValue, int pointsize, ClipperLib::Path& dPath);
};

}
#endif
