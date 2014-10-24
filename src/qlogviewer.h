#ifndef QLOGVIEWER_H
#define QLOGVIEWER_H

#include <QObject>
#include <QVariant>
#include <QMutex>

#include "log_client.hpp"

class QLogViewer : public QObject, public p347_LogClient
{
    Q_OBJECT
public:
    explicit QLogViewer(QObject *parent = 0);
    virtual void callbackNewLocalMessage(std::string msg_time,
                                         std::string msg_prefix,
                                         std::string msg_message);
    virtual void callbackNewNetworkMessage(int data_len, void *data);
    /*
    virtual void callbackNewNetworkMessage(std::string device_name,
                                           std::string msg_time,
                                           std::string msg_prefix,
                                           std::string msg_message);
    */
signals:
    //void addLogMessage(QVariant message);
    void addLogMessage(QVariant d_name, QVariant m_time, QVariant m_prefix, QVariant m_message);
public slots:
    //void cppSlot(const QString &msg);
    int acceptCommand(const QString &msg);
private:
    QMutex cs;
};

#endif // QLOGVIEWER_H
