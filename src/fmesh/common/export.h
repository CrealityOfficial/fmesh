#ifndef FMESH_EXPORT_1603030172076_H
#define FMESH_EXPORT_1603030172076_H
#ifdef QT_CORE_LIB
	#include <QtCore/QObject>
	#ifdef FMESH_DLL
		#define FMESH_API Q_DECL_EXPORT
	#else
		#define FMESH_API Q_DECL_IMPORT
	#endif
#else
	#ifdef FMESH_DLL
		#define FMESH_API __declspec(dllexport)
	#else
		#define FMESH_API __declspec(dllimport)
	#endif			
#endif
#endif // FMESH_EXPORT_1603030172076_H