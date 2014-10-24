QT += qml quick
TARGET = log_viewer

win32:INCLUDEPATH += C:/Qt/boost_1_55_0

!android: !ios: !blackberry: qtHaveModule(widgets): QT += widgets

win32:LIBS += -L"$(QTDIR)/../../Tools/mingw48_32/i686-w64-mingw32/lib" -lws2_32

CONFIG += console

include(src/src.pri)
#include(../shared/shared.pri)

#QMAKE_CXXFLAGS += -Wunused-local-typedefs -Wcpp -Wtype-limits -Wwrite-strings -Wunused-parameter
#QMAKE_CFLAGS += -Wunused-local-typedefs -Wcpp -Wtype-limits -Wwrite-strings -Wunused-parameter

OTHER_FILES += \
    main.qml \
    test2.qml \
    test.qml \
    View_item2.qml \
    MainTab.qml \
    EmptyTab.qml \
    test3.qml \
    FlowTab.qml \
    test3_flow.qml \
    PrefixList.qml \
    PrefixElement.qml \
    SimpleColorPicker.qml \
    View_item.qml

RESOURCES += \
    resources.qrc

HEADERS +=

