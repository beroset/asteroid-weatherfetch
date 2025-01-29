add_library(asteroid-weatherfetch main.cpp resources.qrc)
set_target_properties(asteroid-weatherfetch PROPERTIES PREFIX "" SUFFIX "")
target_link_libraries(asteroid-weatherfetch PUBLIC
        Qt5::Qml
	AsteroidApp
        weatherfetch)

add_library(weatherfetch WeatherParser.cpp)
set_target_properties(weatherfetch PROPERTIES
    SOVERSION 1
    VERSION 1.0.1
)

target_link_libraries(weatherfetch PUBLIC Qt5::Qml
        PRIVATE Qt5::Core Qt5::Network PkgConfig::GIOMM)

add_executable(weatherfetch_cli weatherfetch_cli.cpp WeatherSettings.cpp UrlFetcher.cpp)
set_target_properties(weatherfetch_cli PROPERTIES CXX_STANDARD 20)
target_link_libraries(weatherfetch_cli PRIVATE weatherfetch)

install(TARGETS asteroid-weatherfetch weatherfetch_cli
	DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS weatherfetch)
