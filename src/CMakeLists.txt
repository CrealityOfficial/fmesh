__find_librevenge()
__find_tinyxml()
__find_freetype()
__find_dxflib()

__files_group(${CMAKE_CURRENT_SOURCE_DIR}/fmesh/ SRCS)

set(LIBS trimesh2 clipper mmesh freetype dxflib librevenge tinyxml)
set(DEFS FMESH_DLL TIXML_USE_STL)
__add_real_target(fmesh dll SOURCE ${SRCS} INC ${INCS} LIB ${LIBS} DEF ${DEFS})

set_property(TARGET fmesh PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE)
	__add_openmp_lib(fmesh)
endif()

if(INSTALL_FMESH)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fmesh DESTINATION include/ FILES_MATCHING PATTERN "*.h")
endif()