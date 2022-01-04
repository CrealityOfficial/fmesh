#include "circurlar.h"

namespace fmesh
{
	void loopPolyTree(polyNodeFunc func, ClipperLibXYZ::PolyNode* polyNode)
	{
		if (!polyNode) return;

		polyNodeFunc cFunc = [&cFunc, &func](ClipperLibXYZ::PolyNode* node) {
			func(node);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				cFunc(n);
		};

		cFunc(polyNode);
	}

	void level2PolyNode(polyNodeFunc func, ClipperLibXYZ::PolyNode* polyNode)
	{
		if (!polyNode) return;

		int index = 0;
		polyNodeFunc cFunc = [&cFunc, &func](ClipperLibXYZ::PolyNode* node) {
			func(node);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				cFunc(n);
		};

		cFunc(polyNode);
	}

	void seperatePolyTree(ClipperLibXYZ::PolyTree* polyTree,
		std::vector<ClipperLibXYZ::Path*>& exterior, std::vector<ClipperLibXYZ::Path*>& interior)
	{
		polyNodeFunc func = [&func, &exterior, &interior](ClipperLibXYZ::PolyNode* node) {
			if (!node->IsHole())
				exterior.push_back(&node->Contour);
			if (node->IsHole() && node->Parent)
				interior.push_back(&node->Contour);
		};

		fmesh::loopPolyTree(func, polyTree);
	}

	void seperatePolyTree(ClipperLibXYZ::PolyTree* polyTree, std::vector<ClipperLibXYZ::Path*>& exterior, std::vector<ClipperLibXYZ::Path*>& interior, std::vector<int>& iexterior, std::vector<int>& iinterior)
	{
		polyNodeFunc func = [&func, &exterior, &interior, &iexterior,&iinterior](ClipperLibXYZ::PolyNode* node) {
			if (!node->IsHole())
			{
				exterior.push_back(&node->Contour);
				if (node->Parent)
				{
					if (node->Parent->Contour.size())
					{
						iexterior.push_back(exterior.size());
					}
				}
			}
			if (node->IsHole() && node->Parent)
			{
				interior.push_back(&node->Contour);

				if (iexterior.size()>0)
				{
					if (iexterior.back() == exterior.size())
					{
						iinterior.push_back(interior.size());
					}
				}
			}
		};

		fmesh::loopPolyTree(func, polyTree);
	}

}