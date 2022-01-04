#include "load.h"
#include "dxf/dl_dxf.h"
#include "fmesh/dxf/test_creationclass.h"
#include "fmesh/dxf/builder.h"

#include <memory>

namespace cdrdxf
{
	Test_CreationClass* _loadDXF(const char* fileName)
	{
		Test_CreationClass* creationClass = new Test_CreationClass();
		DL_Dxf dxf;
		if (!dxf.in(fileName, creationClass))// if file open failed
		{
			std::cerr << fileName << "DXF could not be opened.\n";
			delete creationClass;
			return nullptr;
		}

		return creationClass;
	}

	ClipperLibXYZ::Paths* loadDXFFile(const char* fileName)
	{
		Test_CreationClass* creationClass = new Test_CreationClass();
		std::unique_ptr<DL_Dxf> dxf(new DL_Dxf());
		if (!dxf->in(fileName, creationClass))// if file open failed
		{
			std::cerr << fileName << "DXF could not be opened.\n";
			return nullptr;
		}

		ClipperLibXYZ::Paths* paths = new ClipperLibXYZ::Paths();
		creationClass->myblock2Paths(paths);
		return paths;
	}

	ClipperLibXYZ::Paths* loadSplineFromDXFFile(const char* fileName)
	{
		std::unique_ptr<Test_CreationClass> creation(_loadDXF(fileName));
		ClipperLibXYZ::Paths* paths = nullptr;
		if (creation.get())
		{
			std::vector<DXFSpline*> splines = creation->splines();
			size_t size = splines.size();
			if (size > 0)
			{
				paths = new ClipperLibXYZ::Paths(size);
				for (size_t i = 0; i < size; ++i)
					convert(splines.at(i), &paths->at(i));
			}
		}

		return paths;
	}

	std::vector<ClipperLibXYZ::Paths*> loadMultiDXFFile(const char* fileName)
	{
		std::vector<ClipperLibXYZ::Paths*> tmp;

		Test_CreationClass* creationClass = new Test_CreationClass();
		std::unique_ptr<DL_Dxf> dxf(new DL_Dxf());
		if (!dxf->in(fileName, creationClass))// if file open failed
		{
			std::cerr << fileName << "DXF could not be opened.\n";
			return tmp;
		}

		creationClass->myblock2MultiPaths(tmp);
		return tmp;
	}
}