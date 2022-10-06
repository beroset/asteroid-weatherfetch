add_library(asteroid-weatherfetch main.cpp resources.qrc)
add_library(weatherfetch WeatherParser.cpp)
set_target_properties(asteroid-weatherfetch PROPERTIES PREFIX "" SUFFIX "")

target_link_libraries(asteroid-weatherfetch PUBLIC
        Qt5::Qml
	AsteroidApp
        weatherfetch)

target_link_libraries(weatherfetch PUBLIC
        Qt5::Qml
	PkgConfig::GIOMM)

install(TARGETS asteroid-weatherfetch
	DESTINATION ${CMAKE_INSTALL_BINDIR})
