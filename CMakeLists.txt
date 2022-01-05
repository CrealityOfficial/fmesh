__assert_target(tinyxml)
__assert_target(freetype)
__assert_target(dxflib)
__assert_target(librevenge)

__files_group(${CMAKE_CURRENT_SOURCE_DIR}/fmesh/ SRCS)

set(LIBS trimesh2 clipper mmesh cmesh freetype dxflib tinyxml librevenge)
set(DEFS FMESH_DLL TIXML_USE_STL)

__add_real_target(fmesh dll SOURCE ${SRCS}
							INC ${INCS} ${RAPIDJSON_INCLUDE_DIRS}
							LIB ${LIBS}
							DEF ${DEFS}
							INC ${CMAKE_CURRENT_SOURCE_DIR}
							INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
							)

if(APPLE)
	__add_openmp_lib(fmesh)
endif()

if(INSTALL_FMESH)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fmesh DESTINATION include/ FILES_MATCHING PATTERN "*.h")
endif()
