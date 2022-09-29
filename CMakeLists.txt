cmake_minimum_required(VERSION 3.20)

project(asteroid-weatherfetch
	VERSION 0.0.2
	DESCRIPTION "Fetch weather data from the internet for AsteroidOS")

find_package(ECM REQUIRED NO_MODULE)
find_package(AsteroidApp REQUIRED)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ASTEROID_MODULE_PATH})

include(FeatureSummary)
include(GNUInstallDirs)
include(ECMFindQmlModule)
include(AsteroidCMakeSettings)
include(AsteroidTranslations)

find_package(Qt5 COMPONENTS Core Qml REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GIOMM giomm-2.4 REQUIRED IMPORTED_TARGET)
set_property(GLOBAL APPEND PROPERTY _CMAKE_giomm-2.4_TYPE REQUIRED)

if(GIOMM_FOUND)
	set_property(GLOBAL APPEND PROPERTY PACKAGES_FOUND giomm-2.4)
else()
	set_property(GLOBAL APPEND PROPERTY PACKAGES_NOT_FOUND giomm-2.4)
endif()

add_subdirectory(src)

generate_desktop(${CMAKE_SOURCE_DIR} asteroid-weatherfetch)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CPACK_GENERATOR "DEB")
string(TOLOWER "${CMAKE_PROJECT_NAME}" lcproject_name)
set(CPACK_DEBIAN_FILE_NAME "${lcproject_name}-${CMAKE_PROJECT_VERSION}.ipk")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armv7vehf-neon)
if (NOT CPACK_PACKAGE_CONTACT) 
    set(CPACK_PACKAGE_CONTACT bogus@example.org)
    message(WARNING "No package contact specified: using ${CPACK_PACKAGE_CONTACT}")
endif()
include(CPack)
