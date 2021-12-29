#include "load.h"
#include "libcdr/CDRDocument.h"
#include "libcdr/CMXDocument.h"
#include "CDR2externalInterface.h"

#include <iostream>

namespace cdrdxf
{
    static CDRUSERINTERFACE::CDR_USER_OBJ *cdrUserObjPtr;

	ClipperLib::Paths* loadCDRFile(const char* fileName)
	{
        //CDRUSERINTERFACE::CDR_USER_OBJ testhyt;
        //testhyt.loadCDR("tst");
        //return NULL;
        librevenge::RVNGFileStream input(fileName);
        librevenge::RVNGStringVector output;
        librevenge::RVNGSVGDrawingGenerator painter(output, "svg");

        if (!libcdr::CDRDocument::isSupported(&input))
        {
            if (!libcdr::CMXDocument::isSupported(&input))
            {
                fprintf(stderr, "ERROR: Unsupported file format (unsupported version) or file is encrypted!\n");
                return nullptr;
            }
            else if (!libcdr::CMXDocument::parse(&input, &painter))
            {
                fprintf(stderr, "ERROR: Parsing of document failed!\n");
                return nullptr;
            }
        }
        else if (!libcdr::CDRDocument::parse(&input, &painter))
        {
            fprintf(stderr, "ERROR: Parsing of document failed!\n");
            return nullptr;
        }

        if (output.empty())
        {
            std::cerr << "ERROR: No SVG document generated!" << std::endl;
            return nullptr;
        }
        cdrUserObjPtr=new CDRUSERINTERFACE::CDR_USER_OBJ;
        for (unsigned k = 0; k < output.size(); ++k)
        {
            std::cout << output[k].cstr() << std::endl;
            std::cout <<"size=="<< output[k].size() << std::endl;
            cdrUserObjPtr->jsonParseBaseCdr(( char *)output[k].cstr());

        }

        return cdrUserObjPtr->m_Paths;
	}
}