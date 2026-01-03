cmake_minimum_required(VERSION 3.16)

project(Dause VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Multimedia MultimediaWidgets Network Widgets)
find_package(qtermwidget6 REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET 
    libavcodec libavformat libavutil libswresample
)

add_executable(${PROJECT_NAME}
    animatedbutton.h
    animatedbutton.cpp
    app.qrc
    dauseterminal.h
    dauseterminal.cpp
    ffmpegaudioanalyzer.h
    ffmpegaudioanalyzer.cpp
    ffmpegsubtitleanalyzer.h
    ffmpegsubtitleanalyzer.cpp
    main.cpp
    mainwindow.h
    mainwindow.ui
    mainwindow.cpp
    marqueelabel.h
    marqueelabel.cpp
    mediawindow.h
    mediawindow.cpp
    roundedimagelabel.h
    roundedimagelabel.cpp
    seekslider.h
    seekslider.cpp
    spectrumwidget.h
    spectrumwidget.cpp
    videolayerwidget.h
    videolayerwidget.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Network
    Qt6::Widgets

    qtermwidget6

    ${FFTW3_LIBRARIES}

    PkgConfig::FFMPEG
    avcodec    # Encoding/Decoding
    avformat   # Muxing/Demuxing
    avutil     # Utility/Math/Logging
    swresample # Audio Conversion/Resampling          [Optional]
    swscale    # Image Scaling/Color Space Conversion [Optional]
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})