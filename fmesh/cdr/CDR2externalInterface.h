#ifndef __CDR_2_EXTERNAL_INTERFACE_H__
#define __CDR_2_EXTERNAL_INTERFACE_H__
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>
#include <vector>
#include "clipperxyz/clipper.hpp"
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
		 ClipperLibXYZ::Paths  *m_Paths;
	public:	
	int loadCDR(char* filename);
	int extractCDRData();
	int jsonParseBaseCdr(char *Jsonbuff);
	ClipperLibXYZ::Path DrawBzier(ClipperLibXYZ::DoublePoint *pointValue, int pointsize, ClipperLibXYZ::Path& dPath);
};

}
#endif
