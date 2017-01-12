#ifndef SERVERPROCESS_HPP
#define SERVERPROCESS_HPP

#include <QProcess>
#include "Database.hpp"

class QTimer;

class ServerProcess : public QProcess
{
    Q_OBJECT
public:
    ServerProcess(QString name, QString ip, quint16 port, quint16 number, QString working_dir, QObject* controller, Database* database, bool webapp);
    ~ServerProcess();
    void start(QString user = "SELF");
    void stop(QString user = "SELF");
    void restart(QString user = "SELF");
    float processCPULoad();

    QString getName() const{return fName;}
    void setName(const QString& value){fName = value;}

    quint16 getPort() const{return fPort;}
    void setPort(const quint16& value){fPort = value;}

    quint16 getInstanceNumber() const{return fInstance_number;}
    void setInstanceNumber(const quint16& value){fInstance_number = value;}

    qint64 processID() const {return fProcess_ID;}

signals:
    void processDead();

private slots:
    void processFinished(int, QProcess::ExitStatus);
    void processErrorHandler(QProcess::ProcessError);

private:
    void createCrashDir();
    void updateServerLoad();
    float retrieveServerLoad();
    quint16 retrieveCrashCount();
    void handleProcessCrash();
    void updateData(Database::AllTableColumns, QString);
    Database::SqlData retrieveData(Database::AllTableColumns);

    QTimer* fTimer;
    QString fName;
    QString fWorking_Dir;
    QString fIPAddress;
    quint16 fPort;
    quint16 fInstance_number;
    Database* fDatabase;
    quint16 fRetries;
    qint64 fProcess_ID;
    bool fCrash_Dir_Created;
    bool fSelf_Terminate;
    bool fWebApp;
    QString fAccess_Log_File;
    QString fStatus_Log_File;
    QString fCrash_Access_Log_Path;
    QString fCrash_Status_Log_Path;
};

#endif // SERVERPROCESS_HPP
