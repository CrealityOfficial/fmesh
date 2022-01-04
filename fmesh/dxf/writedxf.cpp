#include "writedxf.h"
#include "fmesh/dxf/spline.h"

#include <iostream>
#include <numeric>

#include "dxf/dl_dxf.h"
#include "fmesh/generate/earpolygon.h"
#include "fmesh/generate/polyfiller.h"
#include "fmesh/clipper/circurlar.h"
#include "fmesh/contour/polytree.h"

namespace cdrdxf
{
	void writedxf(std::vector<ClipperLibXYZ::PolyTree*>& trees, const std::string& file)
	{
		std::vector<std::vector<ClipperLibXYZ::Path*>> paths;
		getDXFAllPolyPaths(trees, paths);
		processDXFData(paths,file);
	}

	void getDXFAllPolyPaths(std::vector<ClipperLibXYZ::PolyTree*>& trees, std::vector<std::vector<ClipperLibXYZ::Path*>>& paths)
	{
		if (!trees.size())
			return;
		std::vector<ClipperLibXYZ::Path*> path;
		auto func = [&path](ClipperLibXYZ::PolyNode* node) {
			if (node->Contour.size())
				path.push_back(&node->Contour);
		};
		for (size_t i = 0; i < trees.size(); i++)
		{
			path.clear();
			fmesh::loopPolyTree(func, trees.at(i));
			paths.push_back(path);
		}
	}

	void processDXFData(std::vector<std::vector<ClipperLibXYZ::Path*>>& paths, const std::string& file)
	{
		DL_Dxf dxf;
		DL_WriterA* dw = dxf.out(file.c_str(), DL_Codes::AC1009);
		if (dw->openFailed())
			return;
		// section header:
		dxf.writeHeader(*dw);
		dw->sectionEnd(); //end DXF header
		// section tables:
		dw->sectionTables();  // open tables section
		// VPORT:
		dxf.writeVPort(*dw);   //write Viewports
		// LTYPE:
		dw->tableLinetypes(3);
		dxf.writeLinetype(*dw, DL_LinetypeData("CONTINUOUS", "Continuous", 0, 0, 0.0));
		dxf.writeLinetype(*dw, DL_LinetypeData("BYLAYER", "", 0, 0, 0.0));
		dxf.writeLinetype(*dw, DL_LinetypeData("BYBLOCK", "", 0, 0, 0.0));
		dw->tableEnd();   // end Linetypes
		// LAYER:
		dw->tableLayers(paths.size());  //write Layers
		for (size_t i = 0; i < paths.size(); i++)
		{
			std::string str = "lmodel_"+std::to_string(i);
			dxf.writeLayer(
				*dw,
				DL_LayerData(str, 0),
				DL_Attributes("", 1, 0x00ff0000, 15, "CONTINUOUS")
			);
		}
		dw->tableEnd();  //end layers

		dxf.writeBlockRecord(*dw);  //write Block Records
		//
		for (size_t num = 0; num < paths.size(); num++)
		{
			std::string str = "bsmodel_" + std::to_string(num);
			dxf.writeBlockRecord(*dw, str);
		}
		dw->tableEnd();  //end Block Records

		dw->sectionEnd();  //end Tables Section
		// BLOCK:
		//dw->sectionBlocks();  //write Blocks Section
		//for (size_t num = 0; num < paths.size(); num++)
		//{
		//	for (size_t i = 0; i < paths.at(num).size(); i++)
		//	{
		//		std::string str = "bmodel_" + std::to_string(num) + std::to_string(i);;
		//		dxf.writeBlock(*dw,
		//			DL_BlockData(str, 0, 0.0, 0.0, 0.0));
		//		dxf.writeEndBlock(*dw, str);
		//	}

		//}
		//dw->sectionEnd();  //end Tables Section
		
		// LINE:
		dw->sectionEntities();
		for (size_t num = 0; num < paths.size(); num++)
		{		
			//dw->sectionEntities();  //laser soft not support this
			for (size_t i = 0; i < paths.at(num).size(); i++)
			{
				std::string str = "model_" + std::to_string(num) + std::to_string(i);
				DL_Attributes attributes(str, 256, -1, -1, "BYLAYER");

				ClipperLibXYZ::Path*& path = paths.at(num).at(i);
				if (!path->size())
					continue;

				DL_PolylineData ployData(path->size(),0,1,1);
				dxf.writePolyline(*dw, ployData, attributes);

				for (size_t j = 0; j < path->size(); j++)
				{
					DL_VertexData vertex(INT2MM(path->at(j).X), INT2MM(path->at(j).Y), 0);
					dxf.writeVertex(*dw, vertex);
				}
				dxf.writePolylineEnd(*dw);
			}		
		}
		dw->sectionEnd(); //end Entities
		
		dw->dxfEOF();
		dw->close();
		delete dw;
	}

}