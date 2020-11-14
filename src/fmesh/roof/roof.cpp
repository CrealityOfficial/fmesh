#include "roof.h"
#include "fmesh/contour/polytree.h"

#include <boost/shared_ptr.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/create_straight_skeleton_from_polygon_with_holes_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef K::Point_2                    Point;
typedef CGAL::Polygon_2<K>            Polygon_2;
typedef CGAL::Polygon_with_holes_2<K> Polygon_with_holes;
typedef CGAL::Straight_skeleton_2<K>  Straight_skeleton;
typedef Straight_skeleton::Halfedge_const_iterator Halfedge_const_iterator;
typedef Straight_skeleton::Halfedge_const_handle   Halfedge_const_handle;

typedef boost::shared_ptr<Straight_skeleton> Straight_skeleton_ptr;

namespace fmesh
{
	Patch* buildRoof(ClipperLib::PolyTree* polyTree, double roofHeight)
	{
		return nullptr;
	}

    void fill_polygon(Polygon_2& polygon, ClipperLib::Path* path, bool inverse)
    {
        size_t size = path->size();
        if (!inverse)
        {
            for (size_t i = 0; i < size; ++i)
            {
                ClipperLib::IntPoint& p = path->at(i);
                polygon.push_back(Point(INT2MM(p.X), INT2MM(p.Y)));
            }
        }
        else
        {
            for (size_t i = 0; i < size; ++i)
            {
                ClipperLib::IntPoint& p = path->at(size - 1 - i);
                polygon.push_back(Point(INT2MM(p.X), INT2MM(p.Y)));
            }
        }
    }

    void build_polygon_with_holes(Polygon_with_holes* input, PolyPair* pair)
    {
        bool inverse = pair->clockwise;

        Polygon_2& poly = input->outer_boundary();
        ClipperLib::Path& path = pair->outer->Contour;
        size_t size = path.size();
        if (size < 3 || inverse)
            return;

        fill_polygon(input->outer_boundary(), &path, inverse);

        for (ClipperLib::PolyNode* n : pair->inner)
        {
            ClipperLib::Path& npath = n->Contour;
            size_t nsize = npath.size();
            if (nsize < 3)
                return;

            Polygon_2 hole;
            fill_polygon(hole, &npath, !inverse);

            input->add_hole(hole);
        }
    }

    bool test_simple_polygon(Polygon_with_holes& input)
    {
        //check the validity of the input and fix orientation
        if (!input.outer_boundary().is_simple())
        {
            std::cerr << "ERROR: outer boundary is not simple.";
            return false;
        }

        bool valid = true;
        for (Polygon_with_holes::Hole_iterator it = input.holes_begin();
            it != input.holes_end(); ++it)
        {
            if (!it->is_simple())
            {
                valid = false;
                break;
            }
        }

        return valid;
    }

	void buildRoofs(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches, double roofHeight, double thickness)
	{
        std::vector<PolyPair*> pairs;
        seperate1423(polyTree, pairs);

        for (PolyPair*  pair : pairs)
        {
            Polygon_with_holes input;
            build_polygon_with_holes(&input, pair);
            if (!test_simple_polygon(input))
                continue;

            Straight_skeleton_ptr ss = CGAL::create_interior_straight_skeleton_2(input);
            if (ss)
            {

            }
            else
            {
                std::cerr << "ERROR creating interior straight skeleton" << std::endl;
            }
        }

        for (PolyPair* pair : pairs)
        {
            delete pair;
        }
        pairs.clear();
	}

    ClipperLib::IntPoint cgal_to_point(const Point& point)
    {
        ClipperLib::IntPoint p;
        p.X = (ClipperLib::cInt)(1000.0 * point.x());
        p.Y = (ClipperLib::cInt)(1000.0 * point.y());

        return p;
    }

    void roofLine(ClipperLib::PolyTree* polyTree, ClipperLib::PolyTree* roof)
    {
        std::vector<PolyPair*> pairs;
        seperate1423(polyTree, pairs);

        for (PolyPair* pair : pairs)
        {
            Polygon_with_holes input;
            build_polygon_with_holes(&input, pair);
            if (!test_simple_polygon(input))
                continue;

            Straight_skeleton_ptr aSkeleton = CGAL::create_interior_straight_skeleton_2(input);
            if (aSkeleton)
            {
                for (Halfedge_const_iterator hit = aSkeleton->halfedges_begin();
                    hit != aSkeleton->halfedges_end(); ++hit)
                {
                    Halfedge_const_handle h = hit;

                    if (h->is_bisector() && ((h->id() % 2) == 0) 
                        && !h->has_infinite_time()
                        && !h->opposite()->has_infinite_time())
                    {
                        roof->Contour.push_back(cgal_to_point(h->vertex()->point()));
                        roof->Contour.push_back(cgal_to_point(h->opposite()->vertex()->point()));
                    }
                }
            }
            else
            {
                std::cerr << "ERROR creating interior straight skeleton" << std::endl;
            }
        }

        for (PolyPair* pair : pairs)
        {
            delete pair;
        }
        pairs.clear();
    }
}