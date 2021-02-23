#include "writedxf.h"
#include "fmesh/dxf/spline.h"

#include <iostream>
#include <numeric>

#include "dxf/dl_dxf.h"
#include "fmesh/generate/earpolygon.h"
#include "fmesh/generate/polyfiller.h"
#include "mmesh/clipper/circurlar.h"
#include "fmesh/contour/polytree.h"

namespace cdrdxf
{

	void writedxf(std::vector<ClipperLib::PolyTree*>& trees, const std::string& file)
	{
		std::vector<std::vector<ClipperLib::Path*>> paths;
		getDXFAllPolyPaths(trees, paths);
		processDXFData(paths,file);
	}

	void getDXFAllPolyPaths(std::vector<ClipperLib::PolyTree*>& trees, std::vector<std::vector<ClipperLib::Path*>>& paths)
	{
		if (!trees.size())
			return;
		std::vector<ClipperLib::Path*> path;
		auto func = [&path](ClipperLib::PolyNode* node) {
			if (node->Contour.size())
				path.push_back(&node->Contour);
		};
		for (size_t i = 0; i < trees.size(); i++)
		{
			path.clear();
			mmesh::loopPolyTree(func, trees.at(i));
			paths.push_back(path);
		}
	}

	void processDXFData(std::vector<std::vector<ClipperLib::Path*>>& paths, const std::string& file)
	{
		DL_Dxf dxf;
		DL_WriterA* dw = dxf.out(file.c_str(), DL_Codes::AC1015);
		// section header:
		dxf.writeHeader(*dw);
		dw->sectionEnd();
		// section tables:
		dw->sectionTables();
		// VPORT:
		dxf.writeVPort(*dw);
		// LTYPE:
		dw->tableLinetypes(1);
		dxf.writeLinetype(*dw, DL_LinetypeData("CONTINUOUS", "Continuous", 0, 0, 0.0));
		dxf.writeLinetype(*dw, DL_LinetypeData("BYLAYER", "", 0, 0, 0.0));
		dxf.writeLinetype(*dw, DL_LinetypeData("BYBLOCK", "", 0, 0, 0.0));
		dw->tableEnd();
		// LAYER:
		dw->tableLayers(paths.size());
		for (size_t i = 0; i < paths.size(); i++)
		{
			std::string str = "model_"+std::to_string(i);
			dxf.writeLayer(
				*dw,
				DL_LayerData(str, 0),
				DL_Attributes("", 1, 0x00ff0000, 15, "CONTINUOUS")
			);
		}
		dw->tableEnd();
		dxf.writeBlockRecord(*dw);
		dw->tableEnd();
		dw->sectionEnd();
		// BLOCK:
		dw->sectionBlocks();
		// LINE:
		for (size_t num = 0; num < paths.size(); num++)
		{
			for (size_t i = 0; i < paths.at(num).size(); i++)
			{
				ClipperLib::Path*& path = paths.at(num).at(i);
				if (!path->size())
					continue;
				std::string str = "model_" + std::to_string(num);
				dxf.writeBlock(*dw, DL_BlockData(str, 0, 0.0, 0.0, 0.0));
				DL_Attributes attributes(str, 256, -1, -1, "BYLAYER");
				dw->sectionEnd();
				// ENTITIES:
				dw->sectionEntities();
				for (size_t j = 0; j < path->size() - 1; j++)
				{
					DL_LineData lineData(INT2MM(path->at(j).X),
						INT2MM(path->at(j).Y),
						0,
						INT2MM(path->at(j + 1).X),
						INT2MM(path->at(j + 1).Y),
						0);
					dxf.writeLine(*dw, lineData, attributes);
				}
				if (path->size() > 0)
				{
					DL_LineData lineData(INT2MM(path->at(path->size() - 1).X),
						INT2MM(path->at(path->size() - 1).Y),
						0,
						INT2MM(path->at(0).X),
						INT2MM(path->at(0).Y),
						0);
					dxf.writeLine(*dw, lineData, attributes);
				}
				dxf.writeEndBlock(*dw, str);
			}
		}
		dw->sectionEnd();
		dw->dxfEOF();
		dw->close();
		delete dw;
	}

}