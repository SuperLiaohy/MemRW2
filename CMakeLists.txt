cmake_minimum_required(VERSION 3.28)
project(MemRW)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH "/home/liaohy/User/Env/Qt/SDK/6.9.3/gcc_64")
find_package(Qt6 REQUIRED Quick)
qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(DwarfCpp)
add_executable(MemRW main.cpp)

target_include_directories(MemRW PUBLIC Src/UIComponent Src/Line Src/ExChartView Src/Backend)
qt_add_qml_module(MemRW
        URI MemRWApp
        QML_FILES
        Qml/Main.qml
        Qml/VariWindow.qml
        Qml/LineDialog.qml

        Qml/Module/ExChart.qml
        Qml/Module/ControlBar.qml
        Qml/Module/LineListView.qml
        Qml/Module/StateBar.qml
        Qml/Module/TimeAxisControlBar.qml
        Qml/Module/ValueAxisControlBar.qml
        SOURCES
        Src/Variable/TreeModel.cpp Src/Variable/TreeModel.hpp
        Src/Variable/GetVariTree.cpp Src/Variable/GetVariTree.hpp
        Src/Line/LineModel.cpp Src/Line/LineModel.hpp
        Src/Line/LineGroupModel.cpp Src/Line/LineGroupModel.hpp
        Src/Line/ChartLine.cpp Src/Line/ChartLine.hpp
        Src/UIComponent/CircularReveal.cpp Src/UIComponent/CircularReveal.h Src/UIComponent/stdafx.h
        Src/ExChartView/ExChartView.cpp Src/ExChartView/ExChartView.hpp
        Src/ExChartView/LineAttrModel.cpp Src/ExChartView/LineAttrModel.hpp
        Src/Backend/Backend.cpp Src/Backend/Backend.hpp
        Src/VariMap/VariComponent.cpp Src/VariMap/VariComponent.hpp
        Src/VariMap/DisplayPluginInterface.cpp Src/VariMap/DisplayPluginInterface.hpp
        Src/VariMap/VariMap.cpp Src/VariMap/VariMap.hpp
)

target_link_libraries(MemRW PRIVATE
        Qt6::Quick
        DwarfCpp
)

include(GNUInstallDirs)
install(TARGETS MemRW
        BUNDLE DESTINATION .
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

