import qbs 1.0

Application {
    name: "axrbrowser"

    Depends { name: "cpp" }
    Depends { name: "axrcore" }
    Depends { name: "axrqt" }
    Depends { name: "Qt"; submodules: [ "core", "gui", "widgets" ] }

    cpp.includePaths: [ product.buildDirectory ]

    files: [ "*.h", "*.cpp", "*.ui", "*.qrc", "*.rc" ]
}
