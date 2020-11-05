#ifndef BASE_H
#define BASE_H
#include <vector>
struct FloatDouble
{
	double X;
	double Y;
	FloatDouble(double x, double y)
	{
		X=x;Y=y;
	}
	FloatDouble()
	{
		X=0;
		Y=0;
	}
};
struct Box3Float
{
public:
	double Min3[3];
	double Max3[3];
	Box3Float()
	{
		Min3[0]=99999;
		Min3[1]=99999;
		Min3[2]=99999;
		Max3[0]=-99999;
		Max3[1]=-99999;
		Max3[2]=-99999;
	}
	~Box3Float()
	{
	}
	bool IsValid()
	{
		return Min3[0]<=Max3[0]&&Min3[1]<=Max3[1]&&Min3[2]<=Max3[2];
	}
	Box3Float(double minX, double minY, double minZ, double maxX, double maxY, double maxZ)
	{
		Min3[0] = minX;
		Min3[1] = minY;
		Min3[2] = minZ;
		Max3[0] = maxX;
		Max3[1] = maxY;
		Max3[2] = maxZ;
	}
	void UpdateRange(double x, double y, double z)
	{
		if (x < Min3[0])
			Min3[0] = x;
		if (y < Min3[1])
			Min3[1] = y;
		if (z < Min3[2])
			Min3[2] = z;
		if (x > Max3[0])
			Max3[0] = x;
		if (y > Max3[1])
			Max3[1] = y;
		if (z > Max3[2])
			Max3[2] = z;
	}
	float GetXLength()
	{
		return Max3[0]-Min3[0];
	}
	float GetYLength()
	{
		return Max3[1]-Min3[1];
	}
	float GetZLength()
	{
		return Max3[2]-Min3[2];
	}
};
struct Vector
{
	double X;
	double Y;
	double Z;
	Vector()
	{
		X = 0;
		Y = 0;
		Z = 0;
	}
	Vector(double x, double y, double z)
	{
		X = x;
		Y = y;
		Z = z;
	}
};
struct Point3d
{
public:
	double X;
	double Y;
	double Z;
	Point3d::Point3d() 
	{
		X = 0;
		Y = 0;
		Z = 0;
	}
	Point3d::Point3d(double x, double y, double z)
	{
		X = x;
		Y = y;
		Z = z;
	}
};
struct Triangle
{
public :
	int P0Index;
	int P1Index;
	int P2Index;
	Triangle::Triangle(int p0index, int p1index, int p2index)
	{
		P0Index=p0index;
		P1Index=p1index;
		P2Index=p2index;
	}
	Triangle::Triangle()
	{
		P0Index=-1;
		P1Index=-1;
		P2Index=-1;
	}
	static Vector CaculateNormal(Point3d& p0, Point3d& p1, Point3d& p2)
	{
		Vector Normal;
		double v1x = p1.X - p0.X;
		double v1y = p1.Y - p0.Y;
		double v1z = p1.Z - p0.Z;
		double v2x = p2.X - p1.X;
		double v2y = p2.Y - p1.Y;
		double v2z = p2.Z - p1.Z;
		Normal.X= v1y * v2z - v1z * v2y;
		Normal.Y = v1z * v2x - v1x * v2z;
		Normal.Z = v1x * v2y - v1y * v2x;
		float len = (float)sqrt(Normal.X * Normal.X + Normal.Y * Normal.Y + Normal.Z * Normal.Z);
		if (len > 0.00001f)
		{
			Normal.X /= len;
			Normal.Y /= len;
			Normal.Z /= len;
		}
		return Normal;
	}
};
class ContourLine
{
private:
	std::vector<FloatDouble> points;
	double z;
public:
	ContourLine():z(0) { }
	void AddPoint2d(double x, double y)
	{
		FloatDouble t(x,y);
		points.push_back(t);
	}
	inline std::vector<FloatDouble>& GetPointList()
	{
		return points;
	}
	inline int GetLinePointCount() const
	{
		return points.size();
	}
	inline void SetZ(double z)
	{
		this->z=z;
	}
	inline double GetZ() const
	{
		return z;
	}
	Box3Float GetBox()
	{
		Box3Float box;
		for (size_t i = 0; i < points.size(); i++)
		{
			box.UpdateRange(points[i].X, points[i].Y, 0);
		}
		return box;
	}	
};
class Mesh
{
public:
	std::vector<Point3d> Vertices;
	std::vector<Triangle> Faces;
public:
	Mesh(){}
	~Mesh(){}
	int AddVertex(Point3d& toAdd)
	{
		int index = (int)Vertices.size();
		Vertices.push_back(toAdd);
		return index;
	}
	int  AddFace(Triangle& triangle)
	{
		int  index = (int)Faces.size();
		Faces.push_back(triangle);
		return index;
	}
};
class PlyManager
{
public:
	static void Output(Mesh& mesh,const char* filename)
	{
		FILE * nfile = fopen(filename,"wb");
		fprintf(nfile,"ply\n");
		fprintf(nfile,"format ascii 1.0\n");
		fprintf(nfile,"comment VCGLIB generated\n");
		fprintf(nfile,"element vertex %d\n",mesh.Vertices.size());
		fprintf(nfile,"property float x\n");
		fprintf(nfile,"property float y\n");
		fprintf(nfile,"property float z\n");
		fprintf(nfile,"property uchar red\n");
		fprintf(nfile,"property uchar green\n");
		fprintf(nfile,"property uchar blue\n");
		fprintf(nfile,"element face %d\n",mesh.Faces.size());
		fprintf(nfile,"property list int int vertex_indices\n");
		fprintf(nfile,"end_header\n");
		for(size_t i=0;i<mesh.Vertices.size();i++)
		{
			AWriteV(nfile,mesh.Vertices[i].X,mesh.Vertices[i].Y,mesh.Vertices[i].Z,255,255,255);
		}
		for(size_t i=0;i<mesh.Faces.size();i++)
		{
			AWriteF(nfile,mesh.Faces[i].P0Index,mesh.Faces[i].P1Index,mesh.Faces[i].P2Index);
		}
		fclose(nfile);
	}
private:
	static void PlyManager::AWriteV(FILE* file, double v1, double v2, double v3,unsigned char r,unsigned char g,unsigned char b)
	{
		fprintf(file,"%.2f %.2f %.2f %d %d %d\n",v1,v2,v3,r,g,b);
	}
	static void PlyManager::AWriteF(FILE* file, int i1, int i2, int i3)
	{
		fprintf(file,"%d %d %d %d\n",3,i1,i2,i3);
	}
};
#endif