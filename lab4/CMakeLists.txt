cmake_minimum_required(VERSION 3.16)

SET(CMAKE_CXX_STANDARD 20)

project(oop_lab4)

if (WIN32)
	add_executable(oop_lab4 WIN32 main.cpp
		Framework.h

		Window.h
		WindowEvent.h
		Window.cpp

		EventSystem.h
		EventSystem.cpp

		HandlePool.h

		Shape.h
		Shape.cpp

		Editor.h
		Editor.cpp

		resource/EditorMainMenu.h
		resource/EditorMainMenu.rc
	)
endif()

target_include_directories(oop_lab4 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(oop_lab4 PRIVATE comctl32)