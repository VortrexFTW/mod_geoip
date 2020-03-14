cmake_minimum_required(VERSION 3.1)

set(MMDB_URL https://github.com/maxmind/libmaxminddb/releases/download/1.3.2/libmaxminddb-1.3.2.tar.gz)
set(MMDB_URL_HASH SHA256=e6f881aa6bd8cfa154a44d965450620df1f714c6dc9dd9971ad98f6e04f6c0f0)

include(ExternalProject)
if(UNIX)
	ExternalProject_Add(
		maxminddb-ext
		URL ${MMDB_URL}
		URL_HASH ${MMDB_URL_HASH}
		CONFIGURE_COMMAND ./configure --prefix=<INSTALL_DIR> --disable-shared --enable-static --with-pic
		BUILD_IN_SOURCE 1
	)

	ExternalProject_Get_Property(maxminddb-ext INSTALL_DIR)
	file(MAKE_DIRECTORY "${INSTALL_DIR}/include/")

	add_library(maxminddb STATIC IMPORTED)
	add_dependencies(maxminddb maxminddb-ext)
	set_target_properties(maxminddb PROPERTIES IMPORTED_LOCATION "${INSTALL_DIR}/lib/libmaxminddb.a")
	set_target_properties(maxminddb PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}/include/")
elseif(MSVC)
	ExternalProject_Add(
		maxminddb-ext
		URL ${MMDB_URL}
		URL_HASH ${MMDB_URL_HASH}
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND ""
	)

	ExternalProject_Get_Property(maxminddb-ext SOURCE_DIR)
	set(MMDB_SRCS
		${SOURCE_DIR}/src/maxminddb.c
		${SOURCE_DIR}/src/data-pool.c
	)
	set_source_files_properties(${MMDB_SRCS} PROPERTIES GENERATED TRUE)

	add_library(maxminddb STATIC ${MMDB_SRCS})
	add_dependencies(maxminddb maxminddb-ext)
	target_include_directories(maxminddb
		PRIVATE ${SOURCE_DIR}/src/
		PUBLIC ${SOURCE_DIR}/include/
		PUBLIC ${SOURCE_DIR}/projects/VS12/
	)

	target_link_libraries(maxminddb ws2_32)
	target_compile_options(maxminddb PRIVATE -D_CRT_SECURE_NO_WARNINGS)
else()
	message(FATAL_ERROR "Unsupported platform.")
endif()
