cmake_minimum_required(VERSION 3.20)

project(
    asteroid-weatherfetch
    VERSION 3.0.0
    DESCRIPTION "Fetch weather data from the internet for AsteroidOS"
)

find_package(ECM REQUIRED NO_MODULE)
find_package(AsteroidApp6 REQUIRED)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ASTEROID_MODULE_PATH})

include(FeatureSummary)
include(GNUInstallDirs)
include(ECMFindQmlModule)
include(Asteroid6CMakeSettings)
include(Asteroid6Translations)

find_package(Qt6 COMPONENTS Core Qml Network REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MLITE mlite6 REQUIRED IMPORTED_TARGET)

add_subdirectory(src)

build_translations(i18n)
generate_desktop(${CMAKE_SOURCE_DIR} asteroid-weatherfetch)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
