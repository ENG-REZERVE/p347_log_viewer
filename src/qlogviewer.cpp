#include <QDebug>
#include <QString>

#include "qlogviewer.h"

QLogViewer::QLogViewer(QObject *parent) :
    QObject(parent), p347_LogClient()
{
}

int QLogViewer::acceptCommand(const QString &msg) {
    //qDebug() << tr("Called the C++ slot with message: %1").arg(msg);

    emit addLogMessage(QVariant("Local Device"),QVariant("time"), QVariant("prefix1"), QVariant(msg));
    emit addLogMessage(QVariant("device 1"),QVariant("time"), QVariant("prefix2"), QVariant(msg));
    emit addLogMessage(QVariant("device 2"),QVariant("time"), QVariant("prefix3"), QVariant(msg));
    emit addLogMessage(QVariant("Local Device"),QVariant("time"), QVariant("prefix4"), QVariant(msg));
    emit addLogMessage(QVariant("device 1"),QVariant("time"), QVariant("prefix5"), QVariant(msg));
    emit addLogMessage(QVariant("device 2"),QVariant("time"), QVariant("prefix6"), QVariant(msg));
    emit addLogMessage(QVariant("Local Device"),QVariant("time"), QVariant("prefix1"), QVariant(msg));
    emit addLogMessage(QVariant("device 1"),QVariant("time"), QVariant("prefix2"), QVariant(msg));
    emit addLogMessage(QVariant("device 2"),QVariant("time"), QVariant("prefix3"), QVariant(msg));

    return 0;
}

void QLogViewer::callbackNewLocalMessage(std::string msg_time,
                             std::string msg_prefix,
                             std::string msg_message) {
    cs.lock();
    QString lname = "Local Device";
    emit addLogMessage(QVariant(lname), QVariant(QString(msg_time.c_str())),
                                        QVariant(QString(msg_prefix.c_str())),
                                        QVariant(QString(msg_message.c_str())));
    cs.unlock();
}

//TODO: make it look nice
void QLogViewer::callbackNewNetworkMessage(int data_len, void *data) {
    int pos0 = 0;
    int pos1 = 0;
    int pos2 = 0;
    QString input_str = QString::fromLocal8Bit((const char*)data,data_len);
    QString str_devname = "";
    QString str_time = "";
    QString str_prefix = "";
    QString str_message = "";
    QRegExp expression("(:{2,})");//check for delimiters
    //0) Extract name
    pos0 = expression.indexIn(input_str,0);
    if (pos0 < 1) {
        //qDebug() << tr("failed to get devname string");
    } else {
        str_devname = QString::fromLocal8Bit(&((const char*)data)[0],pos0);
        //qDebug() << tr("Devname: %1").arg(str_devname);

        //1) extract time
        pos1 = expression.indexIn(input_str,pos0+3);
        if (pos1 <= (pos0+3)) {
           // qDebug() << tr("failed to get time string");
        } else {
            str_time = QString::fromLocal8Bit(&((const char*)data)[pos0+3],(pos1-pos0-3));
            //qDebug() << tr("Time: %1").arg(str_time);

            //extract prefix
            pos2 = expression.indexIn(input_str,pos1+3);
            if (pos2 <= (pos1+3)) {
                //qDebug() << tr("failed to get prefix string");
            } else {
                str_prefix = QString::fromLocal8Bit(&((const char*)data)[pos1+3],(pos2-pos1-3));
                //qDebug() << tr("Prefix: %1").arg(str_prefix);

                //message is the rest
                str_message = QString::fromLocal8Bit(&((const char*)data)[pos2+3],(data_len-pos2-3));
                //qDebug() << tr("Message: %1").arg(str_message);
            }
        }
    }

    if (str_message.length() > 1) { //suppose valid message
        cs.lock();
        emit addLogMessage(QVariant(str_devname), QVariant(str_time),
                                            QVariant(str_prefix),
                                            QVariant(str_message));
        cs.unlock();
    }
}

/*
void QLogViewer::callbackNewNetworkMessage(std::string device_name,
                               std::string msg_time,
                               std::string msg_prefix,
                               std::string msg_message) {
    cs.lock();
    emit addLogMessage(QVariant(QString(device_name.c_str())), QVariant(QString(msg_time.c_str())),
                       QVariant(QString(msg_prefix.c_str())), QVariant(QString(msg_message.c_str())));
    cs.unlock();
}
*/
