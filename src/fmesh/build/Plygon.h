#ifndef POLYGON_H
#define POLYGON_H
#include "Base.h"

class PolyTriangulator
{
public:
	static bool Process(std::vector<FloatDouble>& contour, std::vector<Triangle>& result)
	{
		int n = contour.size();
		if (n < 3) return false;
		int* V = new int[n];
		if (0.0f < Area(contour))
			for (int v = 0; v < n; v++) V[v] = v;
		else
			for (int v = 0; v < n; v++) V[v] = (n - 1) - v;
		int nv = n;
		int count = 2 * nv;   /* error detection */
		for (int m = 0, v = nv - 1; nv > 2; )
		{
			if (0 >= (count--))
			{
				return false;
			}
			int u = v; if (nv <= u) u = 0;     /* previous */
			v = u + 1; if (nv <= v) v = 0;     /* new v    */
			int w = v + 1; if (nv <= w) w = 0;     /* next     */
			if (Snip(contour, u, v, w, nv, V))
			{
				int a, b, c, s, t;
				a = V[u]; b = V[v]; c = V[w];
				Triangle tri(a, b, c);
				result.push_back(tri);
				m++;
				for (s = v, t = v + 1; t < nv; s++, t++) V[s] = V[t]; nv--;
				count = 2 * nv;
			}
		}
		delete []V;
		return true;
	}
	static float Area(std::vector<FloatDouble>& contour)
	{
		int n = contour.size();
		float A = 0.0f;
		for (int p = n - 1, q = 0; q < n; p = q++)
		{
			A += contour[p].X * contour[q].Y - contour[q].X * contour[p].Y;
		}
		return A * 0.5f;
	}
private:
	static bool InsideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py)
	{
		float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
		float cCROSSap, bCROSScp, aCROSSbp;

		ax = Cx - Bx; ay = Cy - By;
		bx = Ax - Cx; by = Ay - Cy;
		cx = Bx - Ax; cy = By - Ay;
		apx = Px - Ax; apy = Py - Ay;
		bpx = Px - Bx; bpy = Py - By;
		cpx = Px - Cx; cpy = Py - Cy;

		aCROSSbp = ax * bpy - ay * bpx;
		cCROSSap = cx * apy - cy * apx;
		bCROSScp = bx * cpy - by * cpx;

		return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
	}
	static bool Snip(std::vector<FloatDouble>& contour, int u, int v, int w, int n, int* V)
	{
		int p;
		float Ax, Ay, Bx, By, Cx, Cy, Px, Py;
		Ax = contour[V[u]].X;
		Ay = contour[V[u]].Y;
		Bx = contour[V[v]].X;
		By = contour[V[v]].Y;
		Cx = contour[V[w]].X;
		Cy = contour[V[w]].Y;
		if (0.00001f > (((Bx - Ax) * (Cy - Ay)) - ((By - Ay) * (Cx - Ax)))) return false;
		for (p = 0; p < n; p++)
		{
			if ((p == u) || (p == v) || (p == w)) continue;
			Px = contour[V[p]].X;
			Py = contour[V[p]].Y;
			if (InsideTriangle(Ax, Ay, Bx, By, Cx, Cy, Px, Py)) return false;
		}
		return true;
	}
};
#endif