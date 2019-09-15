#
cmake_minimum_required(VERSION 3.11)
project(CommonCpp
	VERSION 1.0.0
	LANGUAGES CXX
	)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(CommonCpp ${COMMON_CPP_SRC})
add_library(Dexode::CommonCpp ALIAS CommonCpp)

target_include_directories(CommonCpp PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common-cpp/src/>
	$<INSTALL_INTERFACE:include/>
	)

target_link_libraries(CommonCpp PUBLIC ${CommonCpp_LIBRARIES})

target_compile_options(CommonCpp PUBLIC
	-Wall -Wpedantic -Wextra
	-Wnon-virtual-dtor
	-Werror
	-Wno-unused-parameter
	-Wno-error=deprecated-declarations
	# ----------------------------------------
	PRIVATE
	-fPIC
	)

#add_library(SecretInput
#	common-cpp/src/acme/input/SecretCin.cpp common-cpp/src/acme/input/SecretCin.h
#	)

include(GNUInstallDirs)

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "Dexode::")

install(TARGETS CommonCpp
	EXPORT "${TARGETS_EXPORT_NAME}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${version_config}" COMPATIBILITY SameMajorVersion
)

set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
configure_package_config_file(
	"cmake/Config.cmake.in"
	"${project_config}"
	INSTALL_DESTINATION "${config_install_dir}"
)

install(
	FILES "${project_config}" "${version_config}"
	DESTINATION "${config_install_dir}"
)

install(EXPORT "${TARGETS_EXPORT_NAME}"
	DESTINATION "${config_install_dir}"
	NAMESPACE "${namespace}"
	)

macro(install_public_headers_with_directory HEADER_LIST IGNORE_PREFIX)
	message(STATUS "HEADER_LIST: ${HEADER_LIST}")
	foreach(HEADER ${${HEADER_LIST}})
		get_filename_component(DIR ${HEADER} DIRECTORY)
		string(REPLACE ${IGNORE_PREFIX} "" DIR ${DIR})
		install(FILES ${HEADER} DESTINATION include/${DIR})
	endforeach(HEADER)

endmacro(install_public_headers_with_directory)

install_public_headers_with_directory(CommonCpp_PUBLIC_HEADERS "src/")
