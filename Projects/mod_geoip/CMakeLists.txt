disable_rtti()

include(maxminddb.cmake)

file(GLOB_RECURSE SOURCES *.cpp *.h)

new_library_shared(mod_geoip "${SOURCES}" "")

target_link_libraries(mod_geoip PRIVATE maxminddb)

math(EXPR _CMAKE_BITS "${CMAKE_SIZEOF_VOID_P} * 8")

set_target_properties(mod_geoip PROPERTIES FOLDER modules)
