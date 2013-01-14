import qbs.base 1.0

Project {
    references: [
        "src/core/axrcore.qbs",
        "src/browser/browser.qbs"
    ]

    property int browser_version_major: 0
    property int browser_version_minor: 1
    property int browser_version_patch: 0
    property int browser_version_build: 0
    property string browser_version_string: "0.1"
    property string browser_version_string_real: browser_version_major + "." + browser_version_minor + "." + browser_version_patch + "." + browser_version_build

    Product {
        name: "browser_config_header"
        type: "hpp"
        files: "config.in.h"

        property int browser_version_major: project.browser_version_major
        property int browser_version_minor: project.browser_version_minor
        property int browser_version_patch: project.browser_version_patch
        property int browser_version_build: project.browser_version_build
        property string browser_version_string: project.browser_version_string
        property string browser_version_string_real: project.browser_version_string_real

        Transformer {
            inputs: [ "config.in.h" ]

            Artifact {
                fileName: "config.h"
                fileTags: "hpp"
            }

            prepare: {
                var cmd = new JavaScriptCommand();
                cmd.description = "generating config.h";
                cmd.highlight = "codegen";
                cmd.sourceCode = function() {
                    var file = new TextFile(input.fileName);
                    var content = file.readAll();

                    content = content.replace("@BROWSER_VERSION_MAJOR@", product.browser_version_major);
                    content = content.replace("@BROWSER_VERSION_MINOR@", product.browser_version_minor);
                    content = content.replace("@BROWSER_VERSION_PATCH@", product.browser_version_patch);
                    content = content.replace("@BROWSER_VERSION_BUILD@", product.browser_version_build);
                    content = content.replace("@BROWSER_VERSION_STRING@", product.browser_version_string);

                    file = new TextFile(output.fileName, TextFile.WriteOnly);
                    file.truncate();
                    file.write(content);
                    file.close();
                }

                return cmd;
            }
        }

        ProductModule {
            Depends { name: "cpp" }
            cpp.includePaths: product.buildDirectory
        }
    }
}
