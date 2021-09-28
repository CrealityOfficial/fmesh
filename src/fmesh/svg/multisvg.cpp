#include "multisvg.h"
#include <math.h>

double const _PI = 3.141592653589793238;

namespace fmesh
{
	MultiSVG::MultiSVG()
	{
		m_isBigchars = false;
		m_BezierPoint.first = false;
		m_BezierPoint.second = ClipperLib::DoublePoint(0, 0);
	}

	MultiSVG::~MultiSVG()
	{

	}

	void MultiSVG::addRect(float width, float height,
		float leftTopX, float leftTopY)
	{
		//ClipperLib::Paths* paths = new ClipperLib::Paths(1);
		ClipperLib::Path* path = new ClipperLib::Path;

		path->push_back(CLIPPERPOINT(leftTopX, leftTopY));
		path->push_back(CLIPPERPOINT(leftTopX, leftTopY + height));
		path->push_back(CLIPPERPOINT(leftTopX + width, leftTopY + height));
		path->push_back(CLIPPERPOINT(leftTopX + width, leftTopY));

		m_path.push_back(path);
	}

	void MultiSVG::addLine(float start_x, float start_y, float end_x, float end_y)
	{
		//ClipperLib::Paths* paths = new ClipperLib::Paths(1);
		ClipperLib::Path* path = new ClipperLib::Path;

		path->push_back(CLIPPERPOINT(start_x, start_y));
		path->push_back(CLIPPERPOINT(end_x, end_y));
		m_path.push_back(path);
	}

	void MultiSVG::addPolyLine(const std::string& polygon)
	{
		//ClipperLib::Paths* paths = new ClipperLib::Paths(1);
		ClipperLib::Path* path = new ClipperLib::Path;

		std::vector<std::string> Vctdest;
		SplitString(polygon, Vctdest, " ");
		for (std::string strTemp : Vctdest)
		{
			std::vector<std::string> VctPoint;
			SplitString(strTemp, VctPoint, ",");
			if (2 == VctPoint.size())
			{
				float x = atof(VctPoint[0].data());
				float y = atof(VctPoint[1].data());
				path->push_back(CLIPPERPOINT(x, y));
			}
		}
		m_path.push_back(path);
	}

	void MultiSVG::addEllipse(float cx, float cy, float rx, float ry, int precision)
	{
		//ClipperLib::Paths* paths = new ClipperLib::Paths(1);
		ClipperLib::Path* path = new ClipperLib::Path;

		double delta = 2.0 * _PI / (double)precision;
		for (unsigned int i = 0; i < precision; i++)//顺时针取点
		{
			double Angle = (double)i * delta;
			float x = (cx + rx * cos(Angle));
			float y = (cy + ry * sin(Angle));
			path->push_back(CLIPPERPOINT(x, y));
		}

		m_path.push_back(path);
	}

	void MultiSVG::addPolygonS(const std::string& polygon)
	{
		//ClipperLib::Paths* paths = new ClipperLib::Paths(1);
		ClipperLib::Path* path = new ClipperLib::Path;

		std::vector<std::string> Vctdest;
		SplitString(polygon, Vctdest, " ");
		for (std::string strTemp : Vctdest)
		{
			std::vector<std::string> VctPoint;
			SplitString(strTemp, VctPoint, ",");
			if (2 == VctPoint.size())
			{
				float x = atof(VctPoint[0].data());
				float y = atof(VctPoint[1].data());
				path->push_back(CLIPPERPOINT(x, y));
			}
		}

		m_path.push_back(path);
	}

	void MultiSVG::addPathS(const std::string& strPath)
	{
		ClipperLib::Paths* paths = new ClipperLib::Paths();

		std::vector<std::string> vctString;
		bool flagBegin = false;
		std::string strTemp;

		paths->reserve(strPath.size());
		for (int n = 0; n < strPath.size(); n++)
		{
			if (isSpechars(strPath[n]))
			{
				if (flagBegin == false)
				{
					strTemp += strPath[n];
					flagBegin = true;
				}
				else
				{
					vctString.push_back(strTemp);
					strTemp.clear();
					flagBegin = false;
					n--;
				}
			}
			else
			{
				if (flagBegin) strTemp += strPath[n];
			}

			//最后一个
			if (n == strPath.size() - 1)
			{
				vctString.push_back(strTemp);
			}
		}

		for (std::string& stringPoint : vctString)
		{
			switch (stringPoint[0])
			{
			case 'm':
			case 'M':
			{
				pausePathMm(stringPoint);
				break;
			}
			case 'L':
			case 'l':
			{
				pausePathLl(stringPoint);
				break;
			}
			case 'H':
			case 'h':
			{
				pausePathHh(stringPoint);
				break;
			}
			case 'v':
			case 'V':
			{
				pausePathVv(stringPoint);
				break;
			}
			case 'C':
			case 'c':
			{
				pausePathCc(stringPoint);
				break;
			}
			case 'Q':
			case 'q':
			{
				pausePathQq(stringPoint);
				break;
			}
			case 'T':
			case 't':
			{
				pausePathTt(stringPoint);
				break;
			}
			case 'S':
			case 's':
			{
				pausePathSs(stringPoint);
				break;
			}
			case 'A':
			case 'a':
			{
				pausePathAa(stringPoint);
				break;
			}
			case 'Z':
			case 'z':
			{
				paths->emplace_back();
				paths->back().swap(m_curPath);
				m_curPath.clear();
				break;
			}
			default:
			{
				savePenultPoint();
				break;
			}
			}//end switch
		}

		if (m_curPath.size())
		{
			paths->emplace_back();
			paths->back().swap(m_curPath);
			m_curPath.clear();
		}
		m_pathses.push_back(paths);
	}

	std::vector<ClipperLib::Paths*> MultiSVG::take()
	{
		std::vector<ClipperLib::Paths*> tmp;

		for (size_t i = 0; i < m_pathses.size(); i++)
		{
			for (size_t j = 0; j < m_pathses[i]->size(); j++)
			{
				m_path.push_back(&m_pathses[i]->at(j));
			}
			//m_path.insert(m_path.end(),&m_pathses[i]->begin(),m_pathses[i]->end());
		}

		std::vector<ClipperLib::Paths*> pathses;
		for (size_t i = 0; i < m_path.size(); i++)
		{
			ClipperLib::Paths* paths = nullptr;
			if (pathses.size())
			{
				aabb old = getAABB(&pathses.back()->back());
				aabb cur = getAABB(m_path[i]);
				if ((old.pMmin.X >= cur.pMmin.X
					&& old.pMax.X <= cur.pMax.X
					&& old.pMmin.Y >= cur.pMmin.Y
					&& old.pMax.Y <= cur.pMax.Y)
					|| (old.pMmin.X <= cur.pMmin.X
					&& old.pMax.X >= cur.pMax.X
					&& old.pMmin.Y <= cur.pMmin.Y
					&& old.pMax.Y >= cur.pMax.Y))
				{
					paths = pathses.back();
					paths->push_back(*m_path[i]);
					continue;
				}
			}

			paths = new ClipperLib::Paths;
			paths->push_back(*m_path[i]);
			pathses.push_back(paths);
		}

		//m_pathses.insert(m_pathses.end(), pathses.begin(), pathses.end());

		tmp.swap(pathses);
		m_pathses.clear();
		return tmp;
	}

	void MultiSVG::SplitString(const std::string& Src, std::vector<std::string>& Vctdest, const std::string& c)
	{
		std::string::size_type pos1, pos2;
		pos2 = Src.find(c);
		pos1 = 0;
		while (std::string::npos != pos2)
		{
			if (pos2 != 0)
			{
				Vctdest.push_back(Src.substr(pos1, pos2 - pos1));
			}
			pos1 = pos2 + c.size();
			pos2 = Src.find(c, pos1);
		}
		if (pos1 != Src.length())
		{
			Vctdest.push_back(Src.substr(pos1));
		}
	}

	void MultiSVG::SplitStringS(const std::string& Src, std::vector<std::string>& Vctdest, std::vector<std::string>& VctS)
	{
		std::string::size_type pos1, pos2, pos2Mini = std::string::npos, pos3Mini = std::string::npos;
		for (std::string strTemp : VctS)
		{
			pos2 = Src.find(strTemp);
			if (std::string::npos == pos2)
			{
				continue;
			}
			pos2Mini = std::min(pos2Mini, pos2);
		}
		pos1 = 0;
		while (std::string::npos != pos2Mini)
		{
			if (pos2Mini != 0 && (pos2Mini - pos1 > 0))
			{
				Vctdest.push_back(Src.substr(pos1, pos2Mini - pos1));
			}
			pos1 = pos2Mini + 1;
			for (std::string strTemp : VctS)
			{
				pos2 = Src.find(strTemp, pos1);
				if (std::string::npos == pos2)
				{
					continue;
				}
				pos3Mini = std::min(pos3Mini, pos2);
			}
			pos2Mini = pos3Mini;
			pos3Mini = std::string::npos;
		}
		if (pos1 != Src.length())
		{
			Vctdest.push_back(Src.substr(pos1));
		}
	}

	bool MultiSVG::isSpechars(char chars)
	{
		char ch = toupper(chars);
		if (ch == 'M' || ch == 'L' || ch == 'H' || ch == 'V' || ch == 'S' || ch == 'Q' || ch == 'T' || ch == 'A' || ch == 'Z' || ch == 'C')
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void MultiSVG::pausePathMm(std::string stringPoint)
	{
		if (stringPoint.substr(0, 1) == "M")
		{
			m_isBigchars = true;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}
		else
		{
			m_isBigchars = false;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}

		std::vector<std::string> VctPoint;
		std::vector<std::string> VctS = { " ","," };
		SplitStringS(stringPoint, VctPoint, VctS);
		for (int n = 1; n < VctPoint.size(); n += 2)
		{
			if (m_isBigchars)
			{
				m_currentPosition.X = atof(VctPoint[n - 1].data());
				m_currentPosition.Y = -atof(VctPoint[n].data());
			}
			else
			{
				m_currentPosition.X = m_currentPosition.X + atof(VctPoint[n - 1].data());
				m_currentPosition.Y = m_currentPosition.Y - atof(VctPoint[n].data());
			}
			m_curPath.push_back(CLIPPERPOINT(m_currentPosition.X, m_currentPosition.Y));
		}
		savePenultPoint();
	}

	void MultiSVG::pausePathLl(std::string stringPoint)
	{
		if (stringPoint.substr(0, 1) == "L")
		{
			m_isBigchars = true;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}
		else
		{
			m_isBigchars = false;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}

		std::vector<std::string> VctPoint;
		std::vector<std::string> VctS = { " ","," };
		SplitStringS(stringPoint, VctPoint, VctS);
		for (int n = 1; n < VctPoint.size(); n += 2)
		{
			if (m_isBigchars)
			{
				m_currentPosition.X = atof(VctPoint[n - 1].data());
				m_currentPosition.Y = -atof(VctPoint[n].data());
			}
			else
			{
				m_currentPosition.X = m_currentPosition.X + atof(VctPoint[n - 1].data());
				m_currentPosition.Y = m_currentPosition.Y - atof(VctPoint[n].data());
			}
			m_curPath.push_back(CLIPPERPOINT(m_currentPosition.X, m_currentPosition.Y));
		}
		savePenultPoint();
	}

	void MultiSVG::pausePathHh(std::string stringPoint)
	{
		if (stringPoint.substr(0, 1) == "H")
		{
			m_isBigchars = true;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}
		else
		{
			m_isBigchars = false;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}

		std::vector<std::string> VctPoint;
		SplitString(stringPoint, VctPoint, " ");
		if (1 == VctPoint.size())
		{
			if (m_isBigchars)
			{
				m_currentPosition.X = atof(VctPoint[0].data());
			}
			else
			{
				m_currentPosition.X = m_currentPosition.X + atof(VctPoint[0].data());
			}
			m_curPath.push_back(CLIPPERPOINT(m_currentPosition.X, m_currentPosition.Y));
		}
		savePenultPoint();
	}

	void MultiSVG::pausePathVv(std::string stringPoint)
	{
		if (stringPoint.substr(0, 1) == "V")
		{
			m_isBigchars = true;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}
		else
		{
			m_isBigchars = false;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}

		std::vector<std::string> VctPoint;
		SplitString(stringPoint, VctPoint, " ");
		if (1 == VctPoint.size())
		{
			if (m_isBigchars)
			{
				m_currentPosition.Y = atof(VctPoint[0].data());
			}
			else
			{
				m_currentPosition.Y = m_currentPosition.Y + atof(VctPoint[0].data());
			}
			m_curPath.push_back(CLIPPERPOINT(m_currentPosition.X, m_currentPosition.Y));
		}
		savePenultPoint();
	}

	void MultiSVG::pausePathCc(std::string stringPoint)
	{
		if (stringPoint.substr(0, 1) == "C")
		{
			m_isBigchars = true;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}
		else
		{
			m_isBigchars = false;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}

		std::vector<std::string> VctPoint;
		std::vector<std::string> VctS = { " ","," };
		SplitStringS(stringPoint, VctPoint, VctS);
		for (int n = 5; n < VctPoint.size(); n += 6)
		{
			ClipperLib::DoublePoint bezierStart = m_currentPosition;
			if (m_isBigchars)
			{
				std::vector<ClipperLib::DoublePoint> vctBasePoint;
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = atof(VctPoint[n - 5].data());
				m_currentPosition.Y = -atof(VctPoint[n - 4].data());
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = atof(VctPoint[n - 3].data());
				m_currentPosition.Y = -atof(VctPoint[n - 2].data());
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = atof(VctPoint[n - 1].data());
				m_currentPosition.Y = -atof(VctPoint[n].data());
				vctBasePoint.push_back(m_currentPosition);
				for (int tm = 1; tm <= 50; tm++)
				{
					ClipperLib::DoublePoint FT_Temp;
					float t = tm / 50.0;
					FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 3) + vctBasePoint[1].X * t * pow(1 - t, 2) * 3 + vctBasePoint[2].X * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].X * pow(t, 3);
					FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 3) + vctBasePoint[1].Y * t * pow(1 - t, 2) * 3 + vctBasePoint[2].Y * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].Y * pow(t, 3);
					m_curPath.push_back(CLIPPERPOINT(FT_Temp.X, FT_Temp.Y));
				}
				savePenultPoint(vctBasePoint[2], vctBasePoint[3]);
				m_currentPosition = vctBasePoint.back();
				vctBasePoint.clear();
			}
			else
			{
				std::vector<ClipperLib::DoublePoint> vctBasePoint;
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 5].data());
				m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n - 4].data());
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 3].data());
				m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n - 2].data());
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data());
				m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n].data());
				vctBasePoint.push_back(m_currentPosition);
				for (int tm = 1; tm <= 50; tm++)
				{
					ClipperLib::DoublePoint FT_Temp;
					float t = tm / 50.0;
					FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 3) + vctBasePoint[1].X * t * pow(1 - t, 2) * 3 + vctBasePoint[2].X * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].X * pow(t, 3);
					FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 3) + vctBasePoint[1].Y * t * pow(1 - t, 2) * 3 + vctBasePoint[2].Y * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].Y * pow(t, 3);
					m_curPath.push_back(CLIPPERPOINT(FT_Temp.X, FT_Temp.Y));
				}
				savePenultPoint(vctBasePoint[2], vctBasePoint[3]);
				m_currentPosition = vctBasePoint.back();
				vctBasePoint.clear();
			}
		}
	}

	void MultiSVG::pausePathQq(std::string stringPoint)
	{
		if (stringPoint.substr(0, 1) == "Q")
		{
			m_isBigchars = true;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}
		else
		{
			m_isBigchars = false;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}

		std::vector<std::string> VctPoint;
		std::vector<std::string> VctS = { " ","," };
		SplitStringS(stringPoint, VctPoint, VctS);

		for (int n = 3; n < VctPoint.size(); n += 4)
		{
			if (m_isBigchars)
			{
				std::vector<ClipperLib::DoublePoint> vctBasePoint;
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = atof(VctPoint[n - 3].data());
				m_currentPosition.Y = -atof(VctPoint[n - 2].data());
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = atof(VctPoint[n - 1].data());
				m_currentPosition.Y = -atof(VctPoint[n].data());
				vctBasePoint.push_back(m_currentPosition);
				for (int tm = 1; tm <= 50; tm++)
				{
					ClipperLib::DoublePoint FT_Temp;
					float t = tm / 50.0;
					FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].X + pow(t, 2) * vctBasePoint[2].X;
					FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].Y + pow(t, 2) * vctBasePoint[2].Y;
					m_curPath.push_back(CLIPPERPOINT(FT_Temp.X, FT_Temp.Y));
				}
				savePenultPoint(vctBasePoint[1], vctBasePoint[2]);
				m_currentPosition = vctBasePoint.back();
				vctBasePoint.clear();
			}
			else
			{
				ClipperLib::DoublePoint bezierStart = m_currentPosition;
				std::vector<ClipperLib::DoublePoint> vctBasePoint;
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 3].data());
				m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n - 2].data());
				vctBasePoint.push_back(m_currentPosition);
				m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data());
				m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n].data());
				vctBasePoint.push_back(m_currentPosition);
				for (int tm = 1; tm <= 50; tm++)
				{
					ClipperLib::DoublePoint FT_Temp;
					float t = tm / 50.0;
					FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].X + pow(t, 2) * vctBasePoint[2].X;
					FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].Y + pow(t, 2) * vctBasePoint[2].Y;
					m_curPath.push_back(CLIPPERPOINT(FT_Temp.X, FT_Temp.Y));
				}
				savePenultPoint(vctBasePoint[1], vctBasePoint[2]);
				m_currentPosition = vctBasePoint.back();
				vctBasePoint.clear();
			}
		}
	}

	void MultiSVG::pausePathTt(std::string stringPoint)
	{
		if (stringPoint.substr(0, 1) == "T")
		{
			m_isBigchars = true;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}
		else
		{
			m_isBigchars = false;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}

		std::vector<std::string> VctPoint;
		std::vector<std::string> VctS = { " ","," };
		SplitStringS(stringPoint, VctPoint, VctS);
		for (int n = 1; n < VctPoint.size(); n += 2)
		{
			if (m_isBigchars)
			{
				ClipperLib::DoublePoint bezierStart = m_currentPosition;
				std::vector<ClipperLib::DoublePoint> vctBasePoint;
				if (m_BezierPoint.first)
				{
					vctBasePoint.push_back(m_currentPosition);
					vctBasePoint.push_back(m_BezierPoint.second);
					m_currentPosition.X = atof(VctPoint[n - 1].data());
					m_currentPosition.Y = -atof(VctPoint[n].data());
					vctBasePoint.push_back(m_currentPosition);
				}
				else
				{
					vctBasePoint.push_back(m_currentPosition);
					m_currentPosition.X = atof(VctPoint[n - 1].data());
					m_currentPosition.Y = -atof(VctPoint[n].data());
					vctBasePoint.push_back(m_currentPosition);
					vctBasePoint.push_back(m_currentPosition);
				}
				for (int tm = 1; tm <= 50; tm++)
				{
					ClipperLib::DoublePoint FT_Temp;
					float t = tm / 50.0;
					FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].X + pow(t, 2) * vctBasePoint[2].X;
					FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].Y + pow(t, 2) * vctBasePoint[2].Y;
					m_curPath.push_back(CLIPPERPOINT(FT_Temp.X, FT_Temp.Y));
				}
				savePenultPoint(vctBasePoint[1], vctBasePoint[2]);
				m_currentPosition = vctBasePoint.back();
				vctBasePoint.clear();
			}
			else
			{
				ClipperLib::DoublePoint bezierStart = m_currentPosition;
				std::vector<ClipperLib::DoublePoint> vctBasePoint;
				if (m_BezierPoint.first)
				{
					vctBasePoint.push_back(m_currentPosition);
					vctBasePoint.push_back(m_BezierPoint.second);
					m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data());
					m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n].data());
					vctBasePoint.push_back(m_currentPosition);
				}
				else
				{
					vctBasePoint.push_back(m_currentPosition);
					m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data());
					m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n].data());
					vctBasePoint.push_back(m_currentPosition);
					vctBasePoint.push_back(m_currentPosition);
				}
				for (int tm = 1; tm <= 50; tm++)
				{
					ClipperLib::DoublePoint FT_Temp;
					float t = tm / 50.0;
					FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].X + pow(t, 2) * vctBasePoint[2].X;
					FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 2) + 2 * t * (1 - t) * vctBasePoint[1].Y + pow(t, 2) * vctBasePoint[2].Y;
					m_curPath.push_back(CLIPPERPOINT(FT_Temp.X, FT_Temp.Y));
				}
				savePenultPoint(vctBasePoint[1], vctBasePoint[2]);
				m_currentPosition = vctBasePoint.back();
				vctBasePoint.clear();
			}
		}
	}

	void MultiSVG::pausePathSs(std::string stringPoint)
	{
		if (stringPoint.substr(0, 1) == "S")
		{
			m_isBigchars = true;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}
		else
		{
			m_isBigchars = false;
			stringPoint = stringPoint.substr(1, stringPoint.size() - 1);
		}

		std::vector<std::string> VctPoint;
		std::vector<std::string> VctS = { " ","," };
		SplitStringS(stringPoint, VctPoint, VctS);
		for (int n = 3; n < VctPoint.size(); n += 4)
		{
			if (m_isBigchars)
			{
				ClipperLib::DoublePoint bezierStart = m_currentPosition;
				std::vector<ClipperLib::DoublePoint> vctBasePoint;
				if (m_BezierPoint.first)
				{
					vctBasePoint.push_back(m_currentPosition);
					vctBasePoint.push_back(m_BezierPoint.second);
					m_currentPosition.X = atof(VctPoint[n - 3].data());
					m_currentPosition.Y = -atof(VctPoint[n - 2].data());
					vctBasePoint.push_back(m_currentPosition);
					m_currentPosition.X = atof(VctPoint[n - 1].data());
					m_currentPosition.Y = -atof(VctPoint[n].data());
					vctBasePoint.push_back(m_currentPosition);
				}
				else
				{
					vctBasePoint.push_back(m_currentPosition);
					m_currentPosition.X = atof(VctPoint[n - 3].data());
					m_currentPosition.Y = -atof(VctPoint[n - 2].data());
					vctBasePoint.push_back(m_currentPosition);
					vctBasePoint.push_back(m_currentPosition);
					m_currentPosition.X = atof(VctPoint[n - 1].data());
					m_currentPosition.Y = -atof(VctPoint[n].data());
					vctBasePoint.push_back(m_currentPosition);

				}
				for (int tm = 1; tm <= 50; tm++)
				{
					ClipperLib::DoublePoint FT_Temp;
					float t = tm / 50.0;
					FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 3) + vctBasePoint[1].X * t * pow(1 - t, 2) * 3 + vctBasePoint[2].X * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].X * pow(t, 3);
					FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 3) + vctBasePoint[1].Y * t * pow(1 - t, 2) * 3 + vctBasePoint[2].Y * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].Y * pow(t, 3);
					m_curPath.push_back(CLIPPERPOINT(FT_Temp.X, FT_Temp.Y));
				}
				savePenultPoint(vctBasePoint[2], vctBasePoint[3]);
				m_currentPosition = vctBasePoint.back();
				vctBasePoint.clear();
			}
			else
			{
				ClipperLib::DoublePoint bezierStart = m_currentPosition;
				std::vector<ClipperLib::DoublePoint> vctBasePoint;
				if (m_BezierPoint.first)
				{
					vctBasePoint.push_back(m_currentPosition);
					vctBasePoint.push_back(m_BezierPoint.second);
					m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 3].data());
					m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n - 2].data());
					vctBasePoint.push_back(m_currentPosition);
					m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data());
					m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n].data());
					vctBasePoint.push_back(m_currentPosition);
				}
				else
				{
					vctBasePoint.push_back(m_currentPosition);
					m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 3].data());
					m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n - 2].data());
					vctBasePoint.push_back(m_currentPosition);
					vctBasePoint.push_back(m_currentPosition);
					m_currentPosition.X = bezierStart.X + atof(VctPoint[n - 1].data());
					m_currentPosition.Y = bezierStart.Y - atof(VctPoint[n].data());
					vctBasePoint.push_back(m_currentPosition);

				}
				for (int tm = 1; tm <= 50; tm++)
				{
					ClipperLib::DoublePoint FT_Temp;
					float t = tm / 50.0;
					FT_Temp.X = vctBasePoint[0].X * pow(1 - t, 3) + vctBasePoint[1].X * t * pow(1 - t, 2) * 3 + vctBasePoint[2].X * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].X * pow(t, 3);
					FT_Temp.Y = vctBasePoint[0].Y * pow(1 - t, 3) + vctBasePoint[1].Y * t * pow(1 - t, 2) * 3 + vctBasePoint[2].Y * pow(t, 2) * (1 - t) * 3 + vctBasePoint[3].Y * pow(t, 3);
					m_curPath.push_back(CLIPPERPOINT(FT_Temp.X, FT_Temp.Y));
				}
				savePenultPoint(vctBasePoint[2], vctBasePoint[3]);
				m_currentPosition = vctBasePoint.back();
				vctBasePoint.clear();
			}
		}
	}

	void MultiSVG::pausePathAa(std::string stringPoint)
	{
		savePenultPoint();
	}

	void MultiSVG::savePenultPoint(ClipperLib::DoublePoint penultPoint, ClipperLib::DoublePoint endPoint, bool isBezier)
	{
		m_BezierPoint.first = isBezier;
		m_BezierPoint.second.X = endPoint.X * 2 - penultPoint.X;
		m_BezierPoint.second.Y = endPoint.Y * 2 - penultPoint.Y;
	}

	void MultiSVG::savePenultPoint(bool isBezier)
	{
		m_BezierPoint.first = isBezier;
	}
	
	aabb MultiSVG::getAABB(ClipperLib::Path* path)
	{
		aabb _aabb;
		for (const ClipperLib::IntPoint& p : *path)
		{
			if (_aabb.pMax.X < p.X)
				_aabb.pMax.X = p.X;
			
			if (_aabb.pMmin.X > p.X)
				_aabb.pMmin.X = p.X;

			if (_aabb.pMax.Y < p.Y)
				_aabb.pMax.Y = p.Y;
			
			if (_aabb.pMmin.Y > p.Y)
				_aabb.pMmin.Y = p.Y;
		}
		return _aabb;
	}
}