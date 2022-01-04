#include "CDR2externalInterface.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <rapidjson/filereadstream.h>
#include <cstdio>
#include <iostream>

namespace CDRUSERINTERFACE
{
	#define  PI 3.141592 
	#define		SCALE_FACTOR 1000.0
	//#define  ENBALE_JSON_DEBUG
	typedef struct __ARC_PARAM__
	{
            double cx;
            double cy;
            double rx;
            double ry;
            double XbaseAngle;
            double startAngle;
            double deltaAngle;
            double endAngle;
            bool clockwise;
        }ARC_PARAM;
	
	int printUsage()
	{
	  printf("loadCDR filename is not right,ex(*.cdr)\n");
	  return -1;
	}
	double  radian(double ux, double uy, double vx, double vy) {
		double  dot = ux * vx + uy * vy;
		double  mod = std::sqrt((ux * ux + uy * uy) * (vx * vx + vy * vy));
		double  rad = std::acos(dot / mod);
		if (ux * vy - uy * vx < 0.0) {
			rad = -rad;
		}
		return rad;
	}

	// svg : [A | a] (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+
	// x1 y1 rx ry φ fA fS x2 y2
	// sample :  svgArcToCenterParam(200,200,50,50,0,1,1,300,200)
	ARC_PARAM cdrArcToCenterParam(double x1, double y1, double rx, double ry, double phi, bool fA, bool fS, double x2, double y2) {
		double cx, cy, startAngle, deltaAngle, endAngle;
		double PIx2 = PI * 2.0;

		if (rx < 0) {
			rx = -rx;
		}
		if (ry < 0) {
			ry = -ry;
		}
		if (rx == 0.0 || ry == 0.0) { // invalid arguments
			std::cout << "rx and ry can not be 0" << std::endl;
		}

		double s_phi = std::sin(phi);
		double c_phi = std::cos(phi);
		double hd_x = (x1 - x2) / 2.0; // half diff of x
		double hd_y = (y1 - y2) / 2.0; // half diff of y
		double hs_x = (x1 + x2) / 2.0; // half sum of x
		double hs_y = (y1 + y2) / 2.0; // half sum of y

		// F6.5.1
		double x1_ = c_phi * hd_x + s_phi * hd_y;
		double y1_ = c_phi * hd_y - s_phi * hd_x;

		// F.6.6 Correction of out-of-range radii
		//   Step 3: Ensure radii are large enough
		double lambda = (x1_ * x1_) / (rx * rx) + (y1_ * y1_) / (ry * ry);
		if (lambda > 1) {
			rx = rx * std::sqrt(lambda);
			ry = ry * std::sqrt(lambda);
		}

		double rxry = rx * ry;
		double rxy1_ = rx * y1_;
		double ryx1_ = ry * x1_;
		double sum_of_sq = rxy1_ * rxy1_ + ryx1_ * ryx1_; // sum of square
		if (!sum_of_sq) {
			std::cout << "start point can not be same as end point" << std::endl;
		}
		double coe = std::sqrt(std::abs((rxry * rxry - sum_of_sq) / sum_of_sq));
		if (fA == fS) { coe = -coe; }

		// F6.5.2
		double cx_ = coe * rxy1_ / ry;
		double cy_ = -coe * ryx1_ / rx;

		// F6.5.3
		cx = c_phi * cx_ - s_phi * cy_ + hs_x;
		cy = s_phi * cx_ + c_phi * cy_ + hs_y;

		double xcr1 = (x1_ - cx_) / rx;
		double xcr2 = (x1_ + cx_) / rx;
		double ycr1 = (y1_ - cy_) / ry;
		double ycr2 = (y1_ + cy_) / ry;

		// F6.5.5
		startAngle = radian(1.0, 0.0, xcr1, ycr1);

		// F6.5.6
		deltaAngle = radian(xcr1, ycr1, -xcr2, -ycr2);
		while (deltaAngle > PIx2) 
		{ 
			deltaAngle -= PIx2; 
		}
		while (deltaAngle < 0.0) 
		{ 
			deltaAngle += PIx2; 
		}
		if (fS == false || fS == 0) 
		{ 
			deltaAngle -= PIx2; 
		}
		endAngle = startAngle + deltaAngle;
		while (endAngle > PIx2) 
		{ 
			endAngle -= PIx2; 
		}
		while (endAngle < 0.0) 
		{ 
			endAngle += PIx2; 
		}

		ARC_PARAM outputObj;

		outputObj.cx = cx;
		outputObj.cy = cy;
		outputObj.rx = rx;
		outputObj.ry = ry;
		outputObj.XbaseAngle = phi;
		outputObj.startAngle = startAngle;
		outputObj.deltaAngle = deltaAngle;
		outputObj.endAngle = endAngle;
		outputObj.clockwise = fS;

		return outputObj;
	}
	void ArcToPoint(ARC_PARAM arcParam, ClipperLibXYZ::Path *pointPath)
	{

		ClipperLibXYZ::IntPoint pointValue = { 0,0 };
		for (int index = 0; index < 72; index ++ )
		{
		ClipperLibXYZ::DoublePoint pointValueD = { 0,0 };
		double anglevalue = 0.0;
		double deltaAngle = index*(PI / 180) * 5;
		if ((deltaAngle - (PI / 180) * 5)> std::abs(arcParam.deltaAngle))
			return;
		if (arcParam.clockwise == 0)
		{
			anglevalue = arcParam.startAngle - deltaAngle;
		}
		else
			anglevalue = arcParam.startAngle + deltaAngle;
		//if (anglevalue < arcParam.endAngle)
		{
			ClipperLibXYZ::DoublePoint pointValueD_Rotation = { 0,0 };
			double XbaseAngle = 0;
			if(arcParam.clockwise==0)
				 XbaseAngle = -arcParam.XbaseAngle;
			pointValueD.X = arcParam.rx * std::cos(anglevalue);
			pointValueD.Y = arcParam.ry * std::sin(anglevalue);
			pointValueD_Rotation.X = arcParam.cx + pointValueD.X * std::cos(XbaseAngle)+ pointValueD.Y * std::sin(XbaseAngle);
			pointValueD_Rotation.Y = arcParam.cy - pointValueD.X * std::sin(XbaseAngle) + pointValueD.Y * std::cos(XbaseAngle);
			pointValue.X = (int)(pointValueD_Rotation.X*SCALE_FACTOR);
			pointValue.Y = (int)(pointValueD_Rotation.Y*SCALE_FACTOR);
			pointPath->push_back(pointValue);
		}
		}


	}
	int CDR_USER_OBJ::jsonParseBaseCdr(char *Jsonbuff)
	{
		 //char json[] = " { \"hello\" : \"world\", \n \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";

		//const char* file_name = "out_ok.json";
		RAPIDJSON_NAMESPACE::Document dom;
		int retvalue = 0;
		ClipperLibXYZ::Paths* m_Pathstemp = new ClipperLibXYZ::Paths();

		if (Jsonbuff == NULL)
		{
			retvalue = -1;
			goto JSONPARSE_END;
		}
		dom.ParseInsitu(Jsonbuff);
		//dom.Parse(Jsonbuff);
		if (!dom.HasParseError()) 
		{
			int pathTotal = 0;
			if (dom.HasMember("pathTotal") && dom["pathTotal"].IsInt()) 
			{
				pathTotal = dom["pathTotal"].GetInt();
				#ifdef ENBALE_JSON_DEBUG
				std::cout<<"pathTotal==="<<dom["pathTotal"].GetInt() << std::endl;
				#endif
			}
			else
			{
				retvalue = -1;
				goto JSONPARSE_END;
			}
			for (int pathIndex = 0; pathIndex < pathTotal; pathIndex++)
			{
				int LevelTotal=0;
				char pathIndexStr[64];
				memset(pathIndexStr, sizeof(pathIndexStr), 0x00);
				sprintf(pathIndexStr, "pathIndex%d", pathIndex);
				#ifdef ENBALE_JSON_DEBUG
				std::cout <<"********************"<< pathIndexStr << std::endl;
				#endif
				if (dom.HasMember(pathIndexStr) && dom[pathIndexStr].IsObject())
				{

					const rapidjson::Value& objfirst = dom[pathIndexStr];
					ClipperLibXYZ::Path dPath;

					if (objfirst.HasMember("LevelTotal") && objfirst["LevelTotal"].IsInt()) {
						LevelTotal = objfirst["LevelTotal"].GetInt();
						#ifdef ENBALE_JSON_DEBUG
						std::cout <<"******************LevelTotal==="<< objfirst["LevelTotal"].GetInt() << std::endl;
						#endif
					}
					else
						return -1;
					ClipperLibXYZ::DoublePoint movePointValue = { 0,0 };
					for (int levelindex = 0; levelindex < LevelTotal; levelindex++)
					{
						char LevelIndexStr[64];
						bool pointAvalible = false;
						ClipperLibXYZ::IntPoint pointValue = { 0,0 };
						memset(LevelIndexStr, sizeof(LevelIndexStr), 0x00);
						sprintf(LevelIndexStr, "LevelIndex%d", levelindex);
						#ifdef ENBALE_JSON_DEBUG
						std::cout << LevelIndexStr << std::endl;
						#endif
						if (objfirst.HasMember(LevelIndexStr) && objfirst[LevelIndexStr].IsObject())
						{
							const rapidjson::Value& objsecond = objfirst[LevelIndexStr];
#							ifdef ENBALE_JSON_DEBUG
							if (objsecond.HasMember("H") && objsecond["H"].IsDouble()) {
								std::cout <<"no deal type(H)="<< objsecond["H"].GetDouble() << std::endl;
							}
							if (objsecond.HasMember("V") && objsecond["V"].IsDouble()) {
								std::cout << "no deal type(V)=" << objsecond["V"].GetDouble() << std::endl;
							}
							#endif
							if (objsecond.HasMember("M") && objsecond["M"].IsArray()) {
								const rapidjson::Value& arr = objsecond["M"];
								pointAvalible = true;
								#if 1
								movePointValue.X = (arr[0].GetDouble() );
								movePointValue.Y = (arr[1].GetDouble() );
								
								pointValue.X =(int)(movePointValue.X*SCALE_FACTOR);
								pointValue.Y =(int)(movePointValue.Y*SCALE_FACTOR);
								#else
								pointValue.X = (int)(67.5064 * 1000);
								pointValue.Y = (int)(133.5973 * 1000);
								#endif
								#ifdef ENBALE_JSON_DEBUG
								for (int i = 0; i < arr.Size(); ++i) {
									std::cout << "type(M)=" << arr[i].GetDouble() << std::endl;
								}
								#endif
							}
							if (objsecond.HasMember("L") && objsecond["L"].IsArray()) {
								const rapidjson::Value& arr = objsecond["L"];
								pointAvalible = true;
							
								movePointValue.X =(arr[0].GetDouble());
								movePointValue.Y =(arr[1].GetDouble());
								pointValue.X =(int)(movePointValue.X*SCALE_FACTOR);
								pointValue.Y =(int)(movePointValue.Y*SCALE_FACTOR);
								#ifdef ENBALE_JSON_DEBUG
								for (int i = 0; i < arr.Size(); ++i) {
									std::cout << "type(L)=" << arr[i].GetDouble() << std::endl;
								}
								#endif
							}
							if (objsecond.HasMember("T") && objsecond["T"].IsArray()) {
								const rapidjson::Value& arr = objsecond["T"];
								for (int i = 0; i < arr.Size(); ++i) {
									std::cout << "no deal type(T)=" << arr[i].GetDouble() << std::endl;
								}
							}
							if (objsecond.HasMember("Q") && objsecond["Q"].IsArray()) {
								const rapidjson::Value& arr = objsecond["Q"];
								for (int i = 0; i < arr.Size(); ++i) {
									std::cout << "no deal type(Q)=" << arr[i].GetDouble() << std::endl;
								}
								
								ClipperLibXYZ::DoublePoint pointValueTmp[3];
								pointValueTmp[0].X =  movePointValue.X;
								pointValueTmp[0].Y =  movePointValue.Y;
								pointValueTmp[1].X =(arr[0].GetDouble());
								pointValueTmp[1].Y =(arr[1].GetDouble());
								pointValueTmp[2].X = (arr[2].GetDouble());
								pointValueTmp[2].Y = (arr[3].GetDouble() );
								
								DrawBzier(pointValueTmp,3, dPath);
								movePointValue =pointValueTmp[2];
							}
							if (objsecond.HasMember("S") && objsecond["S"].IsArray()) {
								const rapidjson::Value& arr = objsecond["S"];
								for (int i = 0; i < arr.Size(); ++i) {
									std::cout << "no deal type(S)=" << arr[i].GetDouble() << std::endl;
								}
							}
							if (objsecond.HasMember("C") && objsecond["C"].IsArray()) {
								const rapidjson::Value& arr = objsecond["C"];
								#ifdef ENBALE_JSON_DEBUG
								for (int i = 0; i < arr.Size(); ++i) {
									std::cout << "type(C)=" << arr[i].GetDouble() << std::endl;
								}
								#endif
								ClipperLibXYZ::DoublePoint pointValueTmp[4];
								pointValueTmp[0].X =  movePointValue.X;
								pointValueTmp[0].Y =  movePointValue.Y;
								pointValueTmp[1].X =(arr[0].GetDouble());
								pointValueTmp[1].Y =(arr[1].GetDouble());
								pointValueTmp[2].X = (arr[2].GetDouble());
								pointValueTmp[2].Y = (arr[3].GetDouble() );
								pointValueTmp[3].X = (arr[4].GetDouble() );
								pointValueTmp[3].Y = (arr[5].GetDouble() );
								
								
								DrawBzier(pointValueTmp,4, dPath);
								movePointValue =pointValueTmp[3];

							}
							if (objsecond.HasMember("A") && objsecond["A"].IsArray()) {
								const rapidjson::Value& arr = objsecond["A"];
								#ifdef ENBALE_JSON_DEBUG
								for (int i = 0; i < arr.Size(); ++i) {
									std::cout << "type(A)=" << arr[i].GetDouble() << std::endl;
								}
								#endif
								double	x1	= movePointValue.X;
								double	y1	= movePointValue.Y;
								double	rx	= arr[0].GetDouble();
								double	ry	= arr[1].GetDouble(); 
								double	phi	= arr[2].GetDouble()*(PI/180.0); 
								bool	fA	= arr[3].GetInt();
								bool	fS	= arr[4].GetInt();
								double	x2	= arr[5].GetDouble(); 
								double	y2	= arr[6].GetDouble();
								movePointValue.X = x2;
								movePointValue.Y = y2;
								ARC_PARAM  arcParam=cdrArcToCenterParam( x1,  y1,  rx,  ry,  phi,  fA,  fS,  x2,  y2);

								ArcToPoint(arcParam, &dPath);

							}
							if (objsecond.HasMember("Z") && objsecond["Z"].IsInt()) {
								std::cout << "type(Z)=" << objsecond["Z"].GetInt() << std::endl;
								if (dPath.size() > 1)
								{
									ClipperLibXYZ::IntPoint startvalue = dPath.at(0);
									ClipperLibXYZ::IntPoint endvalue = dPath.at(dPath.size()-1);
									if(startvalue.X != endvalue.X|| startvalue.Y != endvalue.Y)
										dPath.push_back(startvalue);
									m_Pathstemp->push_back(dPath);
									dPath.clear();

								}
							}
							if(pointAvalible==true)
								{
								dPath.push_back(pointValue);
								}
						}
						else
						{
							retvalue = -1;
							goto JSONPARSE_END;
						}
						}
					if(dPath.size()>1)
						m_Pathstemp->push_back(dPath);
				}
				else
				{
					retvalue = -1;
					goto JSONPARSE_END;
				}
			}
		}
	JSONPARSE_END:
		if (m_Pathstemp->size() == 0)
			delete  m_Pathstemp;
		else
			m_Paths = m_Pathstemp;
		return retvalue;
	}

	CDR_USER_OBJ::CDR_USER_OBJ()
	{
		m_initedflg=false;
		m_Paths = NULL;
	}
	CDR_USER_OBJ::~CDR_USER_OBJ()
	{
		m_initedflg=false;
	}

	int CDR_USER_OBJ::loadCDR(char *filename)
	{
	
		//const char* file_name = "out_ok.json";
		const char* file_name = "F:\\WorkFile\\New_FontMesh\\build\\demo\\out.json";
		char buff[2 << 10];//2 KB
		FILE* myFile = fopen(file_name, "r");
		if (myFile)
		{
			rapidjson::FileReadStream inputStream(myFile, buff, sizeof(buff));
			fclose(myFile);
		}
		else
		{
			std::cout << "file" << file_name << "is open filed!" << std::endl;
			return -1;
		}

		jsonParseBaseCdr(buff);
		//return 0;

		return 0;
	}
	int CDR_USER_OBJ::extractCDRData()
	{
		return 0;
	}
	ClipperLibXYZ::Path CDR_USER_OBJ::DrawBzier(ClipperLibXYZ::DoublePoint *pointValue,int pointsize, ClipperLibXYZ::Path & dPath)
	{
		std::cout << "DrawBzier()" << std::endl;
		if (pointsize == 3)
		{
			ClipperLibXYZ::DoublePoint tempvalue = { 0.0,0.0 };
			int MaxValue = 100.0;
			double deltValue = 0.01;
			deltValue = (1.0 / MaxValue);
			for (int index = 0; index < MaxValue; index++)
			{
				double t = (double)index * deltValue;
				tempvalue.X = pow(1.0 - t, 2) * (pointValue + 0)->X + 2 * t * (1.0 - t) * (pointValue + 1)->X + pow(t, 2) * (pointValue + 2)->X;
				tempvalue.Y = pow(1.0 - t, 2) * (pointValue + 0)->Y + 2 * t * (1.0 - t) * (pointValue + 1)->Y + pow(t, 2) * (pointValue + 2)->Y;

				//FT_Temp.x = vctCTemp[0].x * pow(1 - t, 3) + vctCTemp[1].x * t * pow(1 - t, 2) * 3 + vctCTemp[2].x * pow(t, 2)*(1 - t) * 3 + vctCTemp[3].x * pow(t, 3);
				//FT_Temp.y = vctCTemp[0].y * pow(1 - t, 3) + vctCTemp[1].y * t * pow(1 - t, 2) * 3 + vctCTemp[2].y * pow(t, 2)*(1 - t) * 3 + vctCTemp[3].y * pow(t, 3);

				//std::cout<<"DrawBzier(x,y)=="<<"("<<tempvalue.X<<","<<tempvalue.Y<<")"<<std::endl;
				dPath.push_back(ClipperLibXYZ::IntPoint(tempvalue.X * SCALE_FACTOR, tempvalue.Y * SCALE_FACTOR));

			}
		}
		else if(pointsize == 4)
		{
			ClipperLibXYZ::DoublePoint tempvalue = { 0.0,0.0 };
			int MaxValue = 10.0;
			double deltValue = 0.01;
			deltValue = (1.0 / MaxValue);
			for (int index = 0; index < MaxValue; index++)
			{
				double t = (double)index * deltValue;

				tempvalue.X = (pointValue + 0)->X * pow(1 - t, 3) + (pointValue + 1)->X * t * pow(1 - t, 2) * 3 + (pointValue + 2)->X * pow(t, 2)*(1 - t) * 3 + (pointValue + 3)->X * pow(t, 3);
				//tempvalue.Y = vctCTemp[0].y * pow(1 - t, 3) + vctCTemp[1].y * t * pow(1 - t, 2) * 3 + vctCTemp[2].y * pow(t, 2)*(1 - t) * 3 + vctCTemp[3].y * pow(t, 3);
				tempvalue.Y = (pointValue + 0)->Y * pow(1 - t, 3) + (pointValue + 1)->Y * t * pow(1 - t, 2) * 3 + (pointValue + 2)->Y * pow(t, 2)*(1 - t) * 3 + (pointValue + 3)->Y * pow(t, 3);

				//std::cout<<"DrawBzier(x,y)=="<<"("<<tempvalue.X<<","<<tempvalue.Y<<")"<<std::endl;
				dPath.push_back(ClipperLibXYZ::IntPoint(tempvalue.X * SCALE_FACTOR, tempvalue.Y * SCALE_FACTOR));

			}
		}
		return dPath;
	}



}
 

