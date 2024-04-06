#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFileInfo>
#include <QMessageBox>
#include <iostream>
#include "scripts.h"

int main(int argc, char *argv[]) {

    // Set app information
    QApplication::setApplicationName("PreviewQtRemote");
    QApplication::setOrganizationName("");
    QApplication::setOrganizationDomain("previewqt.org");
    QApplication::setApplicationVersion("1.0");
    QApplication::setQuitOnLastWindowClosed(true);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/src/PQMainWindow.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    qmlRegisterSingletonInstance("PQCScripts", 1, 0, "PQCScripts", &PQCScripts::get());

    if(argc > 1) {

        for(int i = 1; i < argc; ++i) {

            QString arg = argv[i];

            if(!arg.startsWith("-")) {

                QFileInfo info(arg);

                if(info.exists()) {
                    if(info.isExecutable())
                        PQCScripts::get().setPreviewQtExec(arg);
                    else
                        PQCScripts::get().setPassedOnFilename(arg);
                }

            } else if(arg == "--version" || arg == "-v") {

                std::cout << std::endl
                          << " PreviewQtRemote 1.0"
                          << std::endl << std::endl;

                std::exit(0);
                return 0;

            } else if(arg == "--notext") {

                PQCScripts::get().setShowText(false);

            } else if(arg == "--show-config") {

                std::cout << std::endl
                          << "Current configuration:" << std::endl << std::endl

                          << "Show integrated help message: " << (PQCScripts::get().getShowText() ? "yes" : "no") << std::endl
                          << "        PreviewQt executable: " << PQCScripts::get().getPreviewQtExec().toStdString() << std::endl << std::endl;

                std::exit(0);
                return 0;

            } else if(arg == "-h" || arg == "--help") {

                std::cout << std::endl
                          << "Usage: previewqtremote [options] [configuration]" << std::endl
                          << "Easily interact with PreviewQt." << std::endl << std::endl

                          << "Options:" << std::endl
                          << "     -h, --help   Displays help on commandline options." << std::endl
                          << "  -v, --version   Displays version information." << std::endl
                          << "  --show-config   Display current configuration." << std::endl << std::endl

                          << "Configuration (stored permanently):" << std::endl
                          << "       --notext   Disable integrated help message." << std::endl
                          << "     [filename]   PreviewQt executable." << std::endl << std::endl;

                std::exit(0);
                return 0;

            }
        }

    }

    if(!PQCScripts::get().verifyExecutable()) {
        QMessageBox::warning(0, "Missing PreviewQt executable", "PreviewQt has not (yet) been configured properly. You need to set an executable in the settings before you can use this application.");
        PQCScripts::get().setOpenConfigStart(true);
    }

    PQCScripts::get().storeConfiguration();

    engine.load(url);

    return app.exec();
}
