disable_rtti()

if(MSVC)
	precompiled_header(pch)
endif()

include(maxminddb.cmake)

file(GLOB_RECURSE SOURCES *.cpp *.h)

new_library_shared(gtac_geoip "${SOURCES}" "")

target_link_libraries(gtac_geoip PRIVATE maxminddb)

math(EXPR _CMAKE_BITS "${CMAKE_SIZEOF_VOID_P} * 8")

set_target_properties(gtac_geoip PROPERTIES FOLDER modules)
