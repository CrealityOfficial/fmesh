__files_group(${CMAKE_CURRENT_SOURCE_DIR}/fmesh/ SRCS)

__cgal_include_common_packages()
__cgal_include_package(Straight_skeleton_2)
__cgal_include_package(Polygon)

set(LIBS trimesh2 clipper mmesh freetype librevenge)
set(DEFS CGAL_NO_GMP=1 CGAL_NO_MPFR=1)
__add_real_target(fmesh lib SOURCE ${SRCS} INC ${INCS} LIB ${LIBS} DEF ${DEFS})

set_property(TARGET fmesh PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

if(INSTALL_FMESH)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fmesh DESTINATION include/ FILES_MATCHING PATTERN "*.h")
endif()