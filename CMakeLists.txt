cmake_minimum_required(VERSION 3.28)
project(MemRW)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH "/home/liaohy/User/Env/Qt/SDK/6.10.1/gcc_64")
find_package(Qt6 REQUIRED Quick)
qt_standard_project_setup(REQUIRES 6.8)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED IMPORTED_TARGET libusb-1.0)

add_subdirectory(DwarfCpp)
add_executable(MemRW main.cpp)

target_include_directories(MemRW PUBLIC Src/UIComponent Src/USBInterface Src/ExChartView Src/Backend)
qt_add_qml_module(MemRW
        URI MemRWApp
        QML_FILES
        Qml/Main.qml
        Qml/VariWindow.qml
        Qml/ToolBar.qml

        Qml/ChartPlugin/LineDialog.qml
        Qml/ChartPlugin/ExChart.qml
        Qml/ChartPlugin/LineListView.qml
        Qml/ChartPlugin/ChartBar.qml
        Qml/ChartPlugin/ChartPlugin.qml

        SOURCES
        Src/Variable/TreeModel.cpp Src/Variable/TreeModel.hpp
        Src/Variable/GetVariTree.cpp Src/Variable/GetVariTree.hpp
        Src/UIComponent/CircularReveal.cpp Src/UIComponent/CircularReveal.h Src/UIComponent/stdafx.h
        Src/ExChartView/ExChartView.cpp Src/ExChartView/ExChartView.hpp
        Src/ExChartView/LineAttrModel.cpp Src/ExChartView/LineAttrModel.hpp
        Src/Backend/Backend.cpp Src/Backend/Backend.hpp
        Src/Backend/VariComponent.cpp Src/Backend/VariComponent.hpp
        Src/Backend/DisplayPluginInterface.cpp Src/Backend/DisplayPluginInterface.hpp
        Src/USBInterface/DAPReader.cpp Src/USBInterface/DAPReader.h
        Src/USBInterface/USBBulk.cpp Src/USBInterface/USBBulk.h
        Src/USBInterface/USBDevDesc.cpp Src/USBInterface/USBDevDesc.h Src/USBInterface/SWReg.h
#        Src/Backend/DisplayPlugin.cpp Src/Backend/DisplayPluginInterface.hpp
)

target_link_libraries(MemRW PRIVATE
        Qt6::Quick
        DwarfCpp
)

target_link_libraries(MemRW PRIVATE
        PkgConfig::LIBUSB
)


message(STATUS libusb-- "${LIBUSB_INCLUDE_DIRS} ${LIBUSB_LIBRARY_DIRS} ${LIBUSB_LIBRARIES} ${LIBUSB_CFLAGS_OTHER}")


include(GNUInstallDirs)
install(TARGETS MemRW
        BUNDLE DESTINATION .
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

