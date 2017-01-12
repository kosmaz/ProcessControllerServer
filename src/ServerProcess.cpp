#include "ServerProcess.hpp"
#include <QDataStream>
#include <QTimer>
#include <QFile>
#include <QDir>


ServerProcess::ServerProcess(QString name, QString ip, quint16 port, quint16 number, QString working_dir, QObject* controller, Database* database, bool webapp) :
    QProcess(controller),
    fTimer(new QTimer(this)),
    fName(name),
    fWorking_Dir(working_dir),
    fIPAddress(ip),
    fPort(port),
    fInstance_number(number),
    fDatabase(database),
    fRetries(0),
    fProcess_ID(0),
    fCrash_Dir_Created(false),
    fSelf_Terminate(false),
    fWebApp(webapp)
{
    setWorkingDirectory(working_dir);

    fTimer->setInterval(300000 + (number * 1000));    //5 mins + app number interval
    connect(this, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
            &ServerProcess::processFinished);
    connect(this, &ServerProcess::errorOccurred, this, &ServerProcess::processErrorHandler);
    connect(fTimer, &QTimer::timeout, this, &ServerProcess::updateServerLoad);

    if(fWebApp)
        fAccess_Log_File = fWorking_Dir + "logs/access/server" + QString::number(number) + ".log";
    fStatus_Log_File = fWorking_Dir + "logs/status/server" + QString::number(number) + ".log";
    if(fWebApp)
        fCrash_Access_Log_Path = fWorking_Dir + "crash-report/" + QString::number(fInstance_number) + "/access/";
    fCrash_Status_Log_Path = fWorking_Dir + "crash-report/" + QString::number(fInstance_number) + "/status/";
}


ServerProcess::~ServerProcess()
{
    if(state() == QProcess::Running || state() == QProcess::Starting)
        kill();
    if(fTimer->isActive())
        fTimer->stop();
    delete fTimer;
}


void ServerProcess::start(QString user)
{
    if(fTimer->isActive())
        fTimer->stop();
    if(state() == QProcess::NotRunning && state() != QProcess::Starting)
    {
        if(fWebApp)
            QProcess::start("./" + fName + " --http-address " + fIPAddress + " --http-port " + QString::number(fPort) +
                            " --config server-config/server" + QString::number(fInstance_number) +
                            "_config.xml  --accesslog logs/access/server" + QString::number(fInstance_number) + ".log");
        else
            QProcess::start("./" + fName + " --http-address " + fIPAddress + " --http-port " + QString::number(fPort) +
                            " --statuslog logs/status/server" + QString::number(fInstance_number) + ".log");
        qDebug("Process name: %s, Process number: %d ServerProcess::start -> process was started by %s",
               fName.toStdString().c_str(), fInstance_number, user.toStdString().c_str());
        fProcess_ID = processId();
    }
    fTimer->start();
    return;
}


void ServerProcess::stop(QString user)
{
    if(fTimer->isActive())
        fTimer->stop();
    if(state() == QProcess::Running || state() == QProcess::Starting)
    {
        qDebug("Process name: %s, Process number: %d ServerProcess::stop -> process was stopped by %s",
               fName.toStdString().c_str(), fInstance_number, user.toStdString().c_str());
        fSelf_Terminate = true;
        kill();
    }
    return;
}


void ServerProcess::restart(QString user)
{
    stop(user);
    start(user);
    return;
}


float ServerProcess::processCPULoad()
{
    QProcess ps;
    ps.setWorkingDirectory("/bin/");
    QString command_line = "sh -c \"ps -p " + QString::number(processId()) + " -o pcpu | grep -v \'%CPU\'\"";
    ps.start(command_line);
    if(!ps.waitForFinished())
    {
        qWarning("Process name: %s, Process number: %d ServerProcess::processLoad -> QProcess::waitForFinished -> "
                 "Unable to execute ps program in other to retrieve CPU load of the process", fName.toStdString().c_str(),
                 fInstance_number);
        ps.close();
        return -0.0;
    }
    else
    {
        QString result = ps.readAllStandardOutput();
        ps.close();
        return result.toFloat();
    }
}


void ServerProcess::processFinished(int exitcode, QProcess::ExitStatus status)
{
    switch(status)
    {
        case CrashExit:
        {
            if(!fSelf_Terminate)
                handleProcessCrash();
            else
            {
                fSelf_Terminate = false;
                emit processDead();
            }
            break;
        }

        case NormalExit:
        {
            qDebug("Process name: %s, Process number: %d ServerProcess::processFinished -> process exited with exit code %d",
                   fName.toStdString().c_str(), fInstance_number, exitcode);
            emit processDead();
        }
        break;

        default: break;
    }

    return;
}


void ServerProcess::processErrorHandler(QProcess::ProcessError error)
{
    switch(error)
    {
        case FailedToStart:
            emit processDead();
        default:
        {
            if(!fSelf_Terminate)
                qWarning("Process name: %s, Process number: %d ServerProcess::processErrorHandler -> %s",
                         fName.toStdString().c_str(), fInstance_number, errorString().toStdString().c_str());
            break;
        }
    }
    return;
}


void ServerProcess::createCrashDir()
{
    QDir dir(fWorking_Dir);
    if(fWebApp)
        if(!dir.exists(fCrash_Access_Log_Path))
            if(!dir.mkpath(fCrash_Access_Log_Path))
                qWarning("Process name: %s, Process number: %d ServerProcess::createCrashDir -> unable to create directory part %s.",
                     fName.toStdString().c_str(), fInstance_number, fCrash_Access_Log_Path.toStdString().c_str());
    if(!dir.exists(fCrash_Status_Log_Path))
        if(!dir.mkpath(fCrash_Status_Log_Path))
        qWarning("Process name: %s, Process number: %d ServerProcess::createCrashDir -> unable to create directory part %s.",
                  fName.toStdString().c_str(), fInstance_number, fCrash_Status_Log_Path.toStdString().c_str());
    fCrash_Dir_Created = true;
    return;
}


void ServerProcess::updateServerLoad()
{
    float past_record = retrieveServerLoad();
    if(past_record >= 0.0)
    {
        float present_record = processCPULoad();
        present_record += past_record;
        present_record /= 2;
        updateData(Database::SERVERLOAD, QString::number(present_record));
    }
    else
        qWarning("Process name: %s, Process number: %d ServerProcess::updateServerLoad -> unable to retrieve server_load record from"
                 "database therefore server_load computation for this interval will be skipped.", fName.toStdString().c_str(),
                 fInstance_number);
    return;
}


float ServerProcess::retrieveServerLoad()
{
    Database::SqlData result = retrieveData(Database::SERVERLOAD);

    float server_load = 0.0;
    if(result.fCommand == Database::ACK)
    {
        int result_count = 0;
        QString server_load_str;
        QDataStream in(&result.fBody, QIODevice::ReadOnly);
        in >> result_count >> server_load_str;
        server_load = server_load_str.toFloat();
    }
    else
        server_load = -1.0;

    return server_load;
}


quint16 ServerProcess::retrieveCrashCount()
{
    Database::SqlData result = retrieveData(Database::CRASHCOUNT);

    quint16 crash_count = 0;
    if(result.fCommand == Database::ACK)
    {
        int result_count = 0;
        QDataStream in(&result.fBody, QIODevice::ReadOnly);
        in >> result_count >> crash_count;
    }
    else
        crash_count = -1;

    return crash_count;
}


void ServerProcess::handleProcessCrash()
{
    quint16 crash_count = retrieveCrashCount();
    if(crash_count != (quint16)-1)
    {
        updateData(Database::CRASHCOUNT, QString::number(crash_count + 1));
        if(!fCrash_Dir_Created)
            createCrashDir();

        QDir dir(fWorking_Dir);
        QDir dir2(fWorking_Dir);

        if(fWebApp)
            if(!dir.cd(fCrash_Access_Log_Path))
                qWarning("Process name: %s, Process number: %d ServerProcess::handleProcessCrash -> unable to cd into %s.",
                     fName.toStdString().c_str(), fInstance_number, fCrash_Access_Log_Path.toStdString().c_str());
        if(!dir2.cd(fCrash_Status_Log_Path))
            qWarning("Process name: %s, Process number: %d ServerProcess::handleProcessCrash -> unable to cd into %s.",
                     fName.toStdString().c_str(), fInstance_number, fCrash_Status_Log_Path.toStdString().c_str());

        /* Subtract 2 from the directory count returned by the Linux OS. This is to account for the . and the ..
         * directories that are contained by every directory in linux platform.
         */
        if(fWebApp)
            if(!QFile::copy(fAccess_Log_File, fCrash_Access_Log_Path + "crash_report" + QString::number(dir.count() - 2) + ".log"))
                qWarning("Process name: %s, Process number: %d ServerProcess::handleProcessCrash -> unable to copy file %s to %s.",
                     fName.toStdString().c_str(), fInstance_number, fAccess_Log_File.toStdString().c_str(),
                     fCrash_Access_Log_Path.toStdString().c_str());
        if(!QFile::copy(fStatus_Log_File, fCrash_Status_Log_Path + "crash_report" + QString::number(dir2.count() - 2) + ".log"))
            qWarning("Process name: %s, Process number: %d ServerProcess::handleProcessCrash -> unable to copy file %s to %s.",
                     fName.toStdString().c_str(), fInstance_number, fStatus_Log_File.toStdString().c_str(),
                     fCrash_Status_Log_Path.toStdString().c_str());
    }
    else
        qCritical("Process name: %s, Process number: %d. ServerProcess::handleProcessCrash -> unable to retrieve the crash count from "
                  "the database therefore the record will be -1 incorrect but the webapp will be restarted",
                  fName.toStdString().c_str(), fInstance_number);
    if(fRetries == 9)
        emit processDead();
    else
    {
        restart();
        ++ fRetries;
    }
    return;
}


void ServerProcess::updateData(Database::AllTableColumns column, QString value)
{
    int request_count = 1;
    QString field = fDatabase->columnNameInString(column) + " = " + value;
    QString table = "ServerProcess";
    QString where = "WHERE server_name = \'" + fName + "\'";
    QByteArray body;
    QDataStream out(&body, QIODevice::WriteOnly);
    out << request_count << table << where << field;

    Database::SqlData data;
    data.fCommand = Database::UPDATE;
    data.fObject_ID = Database::SERVERPROCESS;
    data.fBody = body;
    fDatabase->queryDB(data);
    return;
}


Database::SqlData ServerProcess::retrieveData(Database::AllTableColumns column)
{
    int request_count = 1;
    QString field = fDatabase->columnNameInString(column);
    QString table = "ServerProcess";
    QString where = "WHERE server_name = \'" + fName + "\'";
    QByteArray body;
    QDataStream out(&body, QIODevice::WriteOnly);
    out << request_count << table << where << field;

    Database::SqlData data;
    data.fCommand = Database::READ;
    data.fObject_ID = Database::SERVERPROCESS;
    data.fBody = body;
    Database::SqlData result= fDatabase->queryDB(data);

    int retries = 0;
    while(result.fCommand == Database::NACK && retries <= 5)
        result = fDatabase->queryDB(data), ++retries;

    return result;
}
