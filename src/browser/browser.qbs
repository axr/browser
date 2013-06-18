import qbs 1.0

Application {
    name: "axrbrowser"
    targetName: qbs.targetOS.contains("osx") ? "AXR Browser" : name

    Depends { name: "cpp" }
    Depends { name: "axrcore" }
    Depends { name: "axrqt" }
    Depends { name: "Qt"; submodules: [ "core", "gui", "widgets" ] }

    cpp.includePaths: [ product.buildDirectory ]

    files: [ "*.h", "*.cpp", "*.ui", "*.qrc", "*.rc" ]
}
