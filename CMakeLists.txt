cmake_minimum_required(VERSION 3.15)
project(FileRecovery VERSION 1.0.0 LANGUAGES CXX)

# C++17 requis
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# FetchContent for external dependencies
include(FetchContent)

# Options de compilation
if(MSVC)
    add_compile_options(/W4 /WX- /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Includes
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Sources
set(SOURCES
    src/main.cpp
    src/core/RecoveryEngine.cpp
    src/core/DiskScanner.cpp
    src/filesystem/NTFSParser.cpp
    src/filesystem/FAT32Parser.cpp
    src/filesystem/FilesystemDetector.cpp
    src/scanner/DeepScanner.cpp
    src/scanner/QuickScanner.cpp
    src/scanner/SignatureScanner.cpp
    src/recovery/FileReconstructor.cpp
    src/recovery/RecoveryManager.cpp
    src/signatures/FileSignatures.cpp
    src/signatures/SignatureDatabase.cpp
    src/signatures/SignatureLoader.cpp
    src/utils/DiskIO.cpp
    src/utils/Logger.cpp
    src/utils/ProgressBar.cpp
    src/utils/FileQuality.cpp
)

# Headers
set(HEADERS
    include/RecoveryEngine.h
    include/DiskScanner.h
    include/FileSignatures.h
    include/RecoveryTypes.h
    src/filesystem/NTFSParser.h
    src/filesystem/FAT32Parser.h
    src/filesystem/FilesystemDetector.h
    src/scanner/DeepScanner.h
    src/scanner/QuickScanner.h
    src/scanner/SignatureScanner.h
    src/recovery/FileReconstructor.h
    src/recovery/RecoveryManager.h
    src/signatures/SignatureDatabase.h
    src/signatures/SignatureLoader.h
    src/utils/DiskIO.h
    src/utils/Logger.h
    src/utils/ProgressBar.h
    include/FileQuality.h
)

# Executable principal
add_executable(file_recovery ${SOURCES} ${HEADERS})

# Threading support
find_package(Threads REQUIRED)
target_link_libraries(file_recovery PRIVATE Threads::Threads)

# Windows specific
if(WIN32)
    target_link_libraries(file_recovery PRIVATE setupapi)
endif()

# Batch Quality Analyzer tool
add_executable(batch_analyzer 
    batch_quality_analyzer.cpp
    src/utils/FileQuality.cpp
    src/utils/Logger.cpp
)
target_link_libraries(batch_analyzer PRIVATE Threads::Threads)

# GUI Application
option(BUILD_GUI "Build GUI application" ON)
if(BUILD_GUI)
    # Include ImGui configuration
    include(${CMAKE_SOURCE_DIR}/cmake/ImGuiConfig.cmake)
    
    # GUI sources
    set(GUI_SOURCES
        gui_main.cpp
        src/gui/RecoveryGUI.cpp
        src/gui/ResultsPagination.cpp
        src/gui/BatchOperations.cpp
        src/scanner/DeepScanner.cpp
        src/scanner/QuickScanner.cpp
        src/scanner/SignatureScanner.cpp
        src/filesystem/NTFSParser.cpp
        src/filesystem/FAT32Parser.cpp
        src/recovery/RecoveryManager.cpp
        src/signatures/FileSignatures.cpp
        src/signatures/SignatureDatabase.cpp
        src/signatures/SignatureLoader.cpp
        src/utils/DiskIO.cpp
        src/utils/FileQuality.cpp
        src/utils/Logger.cpp
    )
    
    # GUI executable
    add_executable(file_recovery_gui ${GUI_SOURCES})
    
    target_include_directories(file_recovery_gui PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
    )
    
    target_link_libraries(file_recovery_gui PRIVATE 
        imgui
        Threads::Threads
    )
    
    if(WIN32)
        target_link_libraries(file_recovery_gui PRIVATE setupapi)
    endif()
    
    # Install GUI
    install(TARGETS file_recovery_gui DESTINATION bin)
endif()

# Installation
install(TARGETS file_recovery batch_analyzer DESTINATION bin)

# Tests (optionnel)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
