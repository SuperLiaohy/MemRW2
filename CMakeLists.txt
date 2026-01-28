cmake_minimum_required(VERSION 3.28)
project(MemRW2)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED Quick)
qt_standard_project_setup(REQUIRES 6.8)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED IMPORTED_TARGET libusb-1.0)

add_subdirectory(DwarfCpp)

if(WIN32)
        add_executable(MemRW2  WIN32 main.cpp)
else()
        add_executable(MemRW2 main.cpp)
endif()

target_include_directories(MemRW2 PUBLIC Src/UIComponent Src/USBInterface Src/ExChartView Src/Backend Src/Variable Src/ExTable)
qt_add_qml_module(MemRW2
        URI MemRWApp
        QML_FILES
        Qml/Main.qml
        Qml/VariWindow.qml
        Qml/ToolBar.qml
        Qml/DockSystem.qml

        Qml/ChartPlugin/LineDialog.qml
        Qml/ChartPlugin/ExChart.qml
        Qml/ChartPlugin/LineListView.qml
        Qml/ChartPlugin/ChartBar.qml
        Qml/ChartPlugin/ChartPlugin.qml
        Qml/TablePlugin/ExTableView.qml

        SOURCES
        Src/Variable/TreeModel.cpp Src/Variable/TreeModel.hpp
        Src/Variable/GetVariTree.cpp Src/Variable/GetVariTree.hpp
        Src/UIComponent/CircularReveal.cpp Src/UIComponent/CircularReveal.h Src/UIComponent/stdafx.h
        Src/ExChartView/ExChartView.cpp Src/ExChartView/ExChartView.hpp
        Src/ExChartView/LineAttrModel.cpp Src/ExChartView/LineAttrModel.hpp
        Src/ExTable/ExTableModel.cpp Src/ExTable/ExTableModel.h
        Src/Backend/Backend.cpp Src/Backend/Backend.hpp
        Src/Backend/VariComponent.cpp Src/Backend/VariComponent.hpp
        Src/Backend/DisplayPluginInterface.cpp Src/Backend/DisplayPluginInterface.hpp
        Src/USBInterface/DAPReader.cpp Src/USBInterface/DAPReader.h
        Src/USBInterface/USBBulk.cpp Src/USBInterface/USBBulk.h
        Src/USBInterface/USBDevDesc.cpp Src/USBInterface/USBDevDesc.h Src/USBInterface/SWReg.h
        RESOURCES
                app-icon.jpg
)

target_link_libraries(MemRW2 PRIVATE
        Qt6::Quick
        DwarfCpp
)

target_link_libraries(MemRW2 PRIVATE
        PkgConfig::LIBUSB
)

include(GNUInstallDirs)
install(TARGETS MemRW2
        BUNDLE DESTINATION .
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

