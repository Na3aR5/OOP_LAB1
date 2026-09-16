cmake_minimum_required(VERSION 3.16)

SET(CMAKE_CXX_STANDARD 20)

project(oop_lab1)

if (WIN32)
	add_executable(oop_lab1 WIN32 main.cpp
		module1/Module1.h
		module1/Module1.cpp
		module1/ModuleResource1.h
		module1/ModuleResource1.rc

		module2/Module2.h
		module2/Module2.cpp
		module2/ModuleResource2.h
		module2/ModuleResource2.rc
		
		mainres.h
		mainres.rc
	)
endif()

target_include_directories(oop_lab1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})