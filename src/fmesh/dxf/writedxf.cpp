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
	void writedxf(ClipperLib::PolyTree* tree, const std::string& file)
	{
		if (tree == nullptr)
			return;

		std::vector<ClipperLib::Path*> paths;
		auto func = [ &paths](ClipperLib::PolyNode* node) {
			int depth = fmesh::testPolyNodeDepth(node);
			if (depth == 2 || depth == 3)
				paths.push_back(&node->Contour);
		};
		mmesh::loopPolyTree(func, tree);

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
			std::string str = std::to_string(i);
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
		for (size_t i = 0; i < paths.size(); i++)
		{
			if (!paths.at(i)->size())
				continue;

			std::string str = std::to_string(i);
			dxf.writeBlock(*dw, DL_BlockData(str, 0, 0.0, 0.0, 0.0));
			DL_Attributes attributes(str, 256, -1, -1, "BYLAYER");
			dw->sectionEnd();

			// ENTITIES:
			dw->sectionEntities();
			for (size_t j = 0; j < paths.at(i)->size() - 1; j++)
			{
				DL_LineData lineData(paths.at(i)->at(j).X, paths.at(i)->at(j).Y, 0, paths.at(i)->at(j + 1).X, paths.at(i)->at(j + 1).Y, 0);
				dxf.writeLine(*dw, lineData, attributes);
			}
			if (paths.at(i)->size() > 0)
			{
				DL_LineData lineData(paths.at(i)->at(paths.at(i)->size() - 1).X, paths.at(i)->at(paths.at(i)->size() - 1).Y, 0, paths.at(i)->at(0).X, paths.at(i)->at(0).Y, 0);
				dxf.writeLine(*dw, lineData, attributes);
			}
			dxf.writeEndBlock(*dw, str);
		}

		dw->sectionEnd();
		dw->dxfEOF();
		dw->close();
		delete dw;
	}
}