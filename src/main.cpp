//#include "qtquickcontrolsapplication.h"
#include <QtQml>
#include <QtQml/QQmlApplicationEngine>
#include <QtGui/QGuiApplication>
#include <QQuickView>
#include <QDebug>
#include <QString>
#include <QApplication>

#include "qlogviewer.h"
#include "inifile.h"

/*
static QObject *lv_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    qDebug() << QObject::tr("provider called");

    QLogViewer *example = new QLogViewer();
    return example;
}
*/
int main(int argc, char *argv[])
{
    int ret = 0;

    int lv_console_level = LOG_LEVEL_NONE;
    int lv_file_level = LOG_LEVEL_NONE;
    int lv_daemon_level = LOG_LEVEL_NONE;

    int br_console_level = LOG_LEVEL_NONE;
    int br_file_level = LOG_LEVEL_NONE;
    int br_daemon_level = LOG_LEVEL_NONE;

    int log_window_capacity = 2000;

    int listen_port_number = DEFAULT_LOGCAST_PORT;

    //qDebug() << QObject::tr("OPENGL version: %1").arg(glGetString(GL_VERSION));

#ifdef _LINUX_
    //TODO: ipc conf and daemon levels
#endif

    TIniFile *ini = new TIniFile();
    assert(ini != NULL);
    if (!ini->OpenIniFile("log_viewer.conf")) {
         qDebug() << QObject::tr("log_viewer.conf was not found, default values would be used.");
    } else {
        //1) Debug levels
        lv_console_level = ini->ReadInt("logging","lv_console_level",LOG_LEVEL_NONE);
        lv_file_level = ini->ReadInt("logging","lv_file_level",LOG_LEVEL_NONE);
        lv_daemon_level = ini->ReadInt("logging","lv_daemon_level",LOG_LEVEL_NONE);
        br_console_level = ini->ReadInt("logging","br_console_level",LOG_LEVEL_NONE);
        br_file_level = ini->ReadInt("logging","br_file_level",LOG_LEVEL_NONE);
        br_daemon_level = ini->ReadInt("logging","br_daemon_level",LOG_LEVEL_NONE);
        //2) Log window capacity
        log_window_capacity = ini->ReadInt("views","log_window_capacity",2000);
        //3) Port listening
        listen_port_number = ini->ReadInt("network","listen_port_number",DEFAULT_LOGCAST_PORT);

        ini->CloseIniFile();
    }
    delete ini;

    //QApplication app(argc, argv);
    //QtQuickControlsApplication app(argc, argv);
    QGuiApplication app(argc, argv);
    //qmlRegisterSingletonType<QLogViewer>("p347.logview", 1, 0, "LVAPI", lv_singletontype_provider);
    QLogViewer* lv = new QLogViewer();

    lv->setConsoleLevel(lv_console_level);
    lv->setFileLevel(lv_file_level,true,NULL);
    lv->broad_client->setConsoleLevel(br_console_level);
    lv->broad_client->setFileLevel(br_file_level,true,NULL);

    lv->logMessage(LOG_LEVEL_FULL,"Starting log_viewer...\n");
#ifdef _LINUX_
    //lv->setDaemonLevel(lv_daemon_level, const char* keyfile_path, int custom_idx);
    //lv->broad_client->setDaemonLevel(br_daemon_level, const char* keyfile_path, int custom_idx);
#endif
    ret = lv->broad_client->setPort(listen_port_number);
    lv->logMessage(LOG_LEVEL_MAIN,"setPort ret %d\n",ret);
    if (ret == 0) {
        lv->Resume();

        //QQmlApplicationEngine engine(QUrl("qrc:/main.qml"));
        QQmlApplicationEngine engine;
        engine.rootContext()->setContextProperty("cppLogView",lv);
        engine.rootContext()->setContextProperty("log_window_capacity",log_window_capacity);
        //engine.load(QUrl("qrc:/main.qml"));
        //engine.load(QUrl("qrc:/test.qml"));
        //engine.load(QUrl("qrc:/test2.qml"));
        engine.load(QUrl("qrc:/test3.qml"));
        //engine.load(QUrl("qrc:/test3_flow.qml"));

        ret = app.exec();

        //if (e_root != NULL)
        //    delete e_root;
    }

    if (lv != NULL)
        delete lv;

    qDebug() << QObject::tr("Application finished with code %1").arg(ret);

    return ret;
}
