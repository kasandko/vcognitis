#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtWebEngine>
#include <QTranslator>

#include "cf.global.h"
#include "cf.initmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

#ifdef Q_OS_LINUX
    QGuiApplication::setWindowIcon(QIcon(":/icons/vcognitis.svg"));
#endif

    QtWebEngine::initialize();

    QQmlApplicationEngine engine;

    QTranslator translator;
    translator.load(":/i18n/vcognitis_ru.qm");
    app.installTranslator(&translator);

    CFInitManager *initManager = new CFInitManager(argc, argv, &app);
    initManager->init(engine);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
