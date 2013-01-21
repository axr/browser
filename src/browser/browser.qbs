import qbs.base 1.0

Application {
    name: "axrbrowser"

    Depends { name: "cpp" }
    Depends { name: "axrcore" }
    Depends { name: "axrqt" }
    Depends { name: "Qt"; submodules: [ "core", "gui", "widgets" ] }

    files: [ "*.h", "*.cpp", "*.ui", "*.qrc", "*.rc", "../3rdparty/qtmacextras/src/*.h", "../3rdparty/qtmacextras/src/*.mm" ]

    cpp.includePaths: [ "../3rdparty/qtmacextras/src", product.buildDirectory ]

    Properties {
        condition: qbs.targetOS === "mac"
        cpp.frameworks: "Cocoa"
    }
}
