#
cmake_minimum_required(VERSION 3.11)
project(gcpp
	VERSION 2.0.0
	LANGUAGES CXX
	)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(listVar "")
set(GCPP_PUBLIC_HEADERS "")
foreach(file ${GCPP_SOURCES})
	set(fixed_path "${GCPP_PROJECT_DIR}/src/${file}")
	list(APPEND listVar "${fixed_path}")
	if(${file} MATCHES ".h(pp)?$")
		list(APPEND GCPP_PUBLIC_HEADERS "${fixed_path}")
		message(STATUS " - GCPP_PUBLIC_HEADER[]:${fixed_path}")
	else()
		message(STATUS " - GCPP_SOURCE[]:${fixed_path}")
	endif()
endforeach()

set(GCPP_SOURCES "${listVar}")

add_library(gcpp ${GCPP_SOURCES})

target_include_directories(gcpp PUBLIC
	$<BUILD_INTERFACE:${GCPP_PROJECT_DIR}/src/>
	$<INSTALL_INTERFACE:include/>
	)

target_link_libraries(gcpp PUBLIC ${GCPP_LIBRARIES})

target_compile_options(gcpp PUBLIC
	-Wall -Wpedantic -Wextra
	-Wnon-virtual-dtor
	-Werror
	-Wno-unused-parameter
	-Wno-error=deprecated-declarations
	# ----------------------------------------
	PRIVATE
	-fPIC
	)

include(GNUInstallDirs)

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")

install(TARGETS gcpp
	EXPORT "${TARGETS_EXPORT_NAME}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	# We don't want someone by accident modify his installed files
	PERMISSIONS OWNER_EXECUTE OWNER_READ
	)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${version_config}" COMPATIBILITY SameMajorVersion
)

set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
configure_package_config_file(
	"${GCPP_PROJECT_DIR}/cmake/Config.cmake.in"
	"${project_config}"
	INSTALL_DESTINATION "${config_install_dir}"
)

install(
	FILES "${project_config}" "${version_config}"
	DESTINATION "${config_install_dir}"
	# We don't want someone by accident modify his installed files
	PERMISSIONS OWNER_EXECUTE OWNER_READ
)

install(EXPORT "${TARGETS_EXPORT_NAME}"
	DESTINATION "${config_install_dir}"
	# We don't want someone by accident modify his installed files
	PERMISSIONS OWNER_EXECUTE OWNER_READ
	)

macro(install_public_headers_with_directory HEADER_LIST IGNORE_PREFIX)
	message(STATUS "HEADER_LIST: ${HEADER_LIST}")
	foreach(HEADER ${${HEADER_LIST}})
		get_filename_component(DIR ${HEADER} DIRECTORY)
		string(REPLACE ${IGNORE_PREFIX} "" DIR ${DIR})
		install(FILES ${HEADER} DESTINATION include/${DIR}
			# We don't want someone by accident modify his installed files
			PERMISSIONS OWNER_EXECUTE OWNER_READ
			)
	endforeach(HEADER)

endmacro(install_public_headers_with_directory)

install_public_headers_with_directory(GCPP_PUBLIC_HEADERS "${GCPP_PROJECT_DIR}/src/")
