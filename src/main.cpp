#include "Service.hpp"
#include <QLoggingCategory>
#include <QDateTime>
#include <cstdio>


#ifndef _DEBUG
FILE* file = fopen("server-status.log", "a");
void myMessageOutput(QtMsgType type, const QMessageLogContext&, const QString& msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
            fprintf(file, "[%s] %lld - [Debug] %s\n",
                    QDateTime::currentDateTimeUtc().toString("yyyy-MMM-dd HH:mm:ss.zzz").toStdString().c_str(),
                    QCoreApplication::instance()->applicationPid(), localMsg.constData());
            break;
        case QtInfoMsg:
            fprintf(file, "[%s] %lld - [Info] %s\n",
                    QDateTime::currentDateTimeUtc().toString("yyyy-MMM-dd HH:mm:ss.zzz").toStdString().c_str(),
                    QCoreApplication::instance()->applicationPid(), localMsg.constData());
        case QtWarningMsg:
            fprintf(file, "[%s] %lld - [Warning] %s\n",
                    QDateTime::currentDateTimeUtc().toString("yyyy-MMM-dd HH:mm:ss.zzz").toStdString().c_str(),
                    QCoreApplication::instance()->applicationPid(), localMsg.constData());
            break;
        case QtCriticalMsg:
            fprintf(file, "[%s] %lld - [Critical] %s\n",
                    QDateTime::currentDateTimeUtc().toString("yyyy-MMM-dd HH:mm:ss.zzz").toStdString().c_str(),
                    QCoreApplication::instance()->applicationPid(), localMsg.constData());
            break;
        case QtFatalMsg:
        {
            fprintf(file, "[%s] %lld - [Fatal] %s\n",
                    QDateTime::currentDateTimeUtc().toString("yyyy-MMM-dd HH:mm:ss.zzz").toStdString().c_str(),
                    QCoreApplication::instance()->applicationPid(), localMsg.constData());
            abort();
        }
    }
}
#endif


#ifndef TEST
int main(int argc, char** argv)
{
#ifndef _DEBUG
    qInstallMessageHandler(myMessageOutput);
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
#endif
    Service service(argc, argv);
    return service.exec();
}
#endif
