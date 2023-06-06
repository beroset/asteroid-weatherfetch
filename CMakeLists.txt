cmake_minimum_required(VERSION 3.20)

project(asteroid-weatherfetch
	VERSION 1.0.1
	DESCRIPTION "Fetch weather data from the internet for AsteroidOS")

find_package(ECM REQUIRED NO_MODULE)
find_package(AsteroidApp REQUIRED)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ASTEROID_MODULE_PATH})

include(FeatureSummary)
include(GNUInstallDirs)
include(ECMFindQmlModule)
include(AsteroidCMakeSettings)
include(AsteroidTranslations)

find_package(Qt5 COMPONENTS Core Qml Network REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GIOMM giomm-2.4 REQUIRED IMPORTED_TARGET)
set_property(GLOBAL APPEND PROPERTY _CMAKE_giomm-2.4_TYPE REQUIRED)

if(GIOMM_FOUND)
	set_property(GLOBAL APPEND PROPERTY PACKAGES_FOUND giomm-2.4)
else()
	set_property(GLOBAL APPEND PROPERTY PACKAGES_NOT_FOUND giomm-2.4)
endif()

add_subdirectory(src)

build_translations(i18n)
generate_desktop(${CMAKE_SOURCE_DIR} asteroid-weatherfetch)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
