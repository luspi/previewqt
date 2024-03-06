#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFileInfo>
#include <clocale>

#include <vqc_messagehandler.h>
#include <vqc_scripts.h>
#include <vqc_providerfull.h>

#ifdef VQMGRAPHICSMAGICK
#include <GraphicsMagick/Magick++.h>
#endif

#ifdef VQMIMAGEMAGICK
#include <Magick++.h>
#endif

#ifdef VQMDEVIL
#include <IL/il.h>
#endif

#ifdef VQMLIBVIPS
#include <vips/vips.h>
#endif

#ifdef VQMVIDEOMPV
#include <vqc_mpvobject.h>
#endif

#ifdef VQMFREEIMAGE
#include <FreeImage.h>
#endif

int main(int argc, char *argv[]) {

    // avoids warning for customizing native styles (observed in particular on Windows)
    qputenv("QT_QUICK_CONTROLS_IGNORE_CUSTOMIZATION_WARNINGS", "1");

    // Set app information
    QApplication::setApplicationName("ViewQt");
    QApplication::setOrganizationName("");
    QApplication::setOrganizationDomain("photoqt.org");
    QApplication::setApplicationVersion(VQMVERSION);
    QApplication::setQuitOnLastWindowClosed(true);

    // custom message handler for qDebug/qLog/qInfo/etc.
    qInstallMessageHandler(vqcMessageHandler);

    QApplication app(argc, argv);

#ifdef VQMVIDEOMPV
    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");
#endif

// only one of them will be defined at a time
#if defined(VQMGRAPHICSMAGICK) || defined(VQMIMAGEMAGICK)
    // Initialise Magick as early as possible
    // this needs to happen BEFORE startup check as this might call into Magick
    Magick::InitializeMagick(*argv);
#endif

#ifdef VQMDEVIL
    ilInit();
#endif

#ifdef VQMFREEIMAGE
    FreeImage_Initialise();
#endif

#ifdef VQMLIBVIPS
    VIPS_INIT(argv[0]);
#endif

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/src/qml/VQMainWindow.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    qmlRegisterSingletonInstance("VQCScripts", 1, 0, "VQCScripts", &VQCScripts::get());

    engine.addImageProvider("full", new VQCProviderFull);

    engine.load(url);

    int ret = app.exec();

#ifdef PQMFREEIMAGE
    FreeImage_DeInitialise();
#endif

#ifdef PQMLIBVIPS
    vips_shutdown();
#endif

    return ret;

}
