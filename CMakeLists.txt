__cc_find(Tinyxml)
__cc_find(Freetype)
__cc_find(Dxflib)
__cc_find(Librevenge)
__cc_find(Trimesh2)
__cc_find(Clipper)
__cc_find(MMesh)
__cc_find(CMesh)
__cc_find(Rapidjson)

__assert_target(tinyxml)
__assert_target(freetype)
__assert_target(dxflib)
__assert_target(librevenge)
__assert_target(trimesh2)
__assert_target(clipper)
__assert_target(mmesh)
__assert_target(cmesh)
__assert_target(rapidjson)

__files_group(${CMAKE_CURRENT_SOURCE_DIR}/fmesh/ SRCS)

if(NOT RAPIDJSON_INCLUDE_DIRS)
	message(FATAL_ERROR "Please Specified RAPIDJSON_INCLUDE_DIRS")
endif()

if(NOT FREETYPE_INCLUDE_DIRS)
	message(FATAL_ERROR "Please Specified FREETYPE_INCLUDE_DIRS")
endif()



set(LIBS trimesh2 clipper mmesh cmesh freetype dxflib tinyxml librevenge)

if(TARGET  OpenMP::OpenMP_CXX)
	message(STATUS "build fmesh use openmp")
	list(APPEND LIBS OpenMP::OpenMP_CXX)
endif()

set(DEFS FMESH_DLL TIXML_USE_STL)

__add_real_target(fmesh dll SOURCE ${SRCS}
							INC ${INCS} ${RAPIDJSON_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS}
							LIB ${LIBS}
							DEF ${DEFS}
							INC ${CMAKE_CURRENT_SOURCE_DIR}
							INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
							)

if(INSTALL_FMESH)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fmesh DESTINATION include/ FILES_MATCHING PATTERN "*.h")
endif()

if(CC_INSTALL_FMESH)
	__install_directory_specif(${CMAKE_CURRENT_SOURCE_DIR}/ fmesh)
endif()
