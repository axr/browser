import qbs.base 1.0

Application {
    name: "axrbrowser"

    Depends { name: "cpp" }
    Depends { name: "axrcore" }
    Depends { name: "axrqt" }
    Depends { name: "Qt"; submodules: [ "core", "gui" ] }

    files: [ "*.h", "*.cpp", "*.ui", "*.qrc", "*.rc" ]

    cpp.includePaths: product.buildDirectory
}
