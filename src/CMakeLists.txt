add_library(asteroid-weatherfetch main.cpp resources.qrc)
set_target_properties(asteroid-weatherfetch PROPERTIES PREFIX "" SUFFIX "")

target_link_libraries(asteroid-weatherfetch PUBLIC
	AsteroidApp)

install(TARGETS asteroid-weatherfetch
	DESTINATION ${CMAKE_INSTALL_BINDIR})
