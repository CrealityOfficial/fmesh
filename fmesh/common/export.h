#ifndef FMESH_EXPORT_1603030172076_H
#define FMESH_EXPORT_1603030172076_H

#if WIN32
	#ifdef FMESH_DLL
		#define FMESH_API __declspec(dllexport)
	#else
		#define FMESH_API __declspec(dllimport)
	#endif	
#else
	#ifdef FMESH_DLL
		#define FMESH_API __attribute__((visibility("default")))
	#else
		#define FMESH_API
	#endif
#endif

#endif // FMESH_EXPORT_1603030172076_H