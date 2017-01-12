#include "MainController.hpp"
#include "AppController.hpp"
#include "AppController.hpp"
#include "ServerProcess.hpp"
#include <QDataStream>
#include <QFile>
#include <QDir>

AppController::AppController(MainController* parent, Database* db) :
    QObject(parent),
    fDatabase (db),
    fRunning_Server_File(new QFile("AppController/running-servers.log", this)),
    fServers(new QMultiMap<QString, ServerProcess*>)
{
    QDir dir;
    if(!dir.exists("AppController"))
        if(!dir.mkdir("AppController"))
            qWarning("AppController::Appcontroller -> unable to create directory \'AppController\'");
}


AppController::~AppController()
{
    shutdownAllServers();
    delete fServers;
    delete fRunning_Server_File;
}


void AppController::startupAllServers()
{
    cleanupOldServers();
    Database::SqlData server_list;
    server_list.fCommand = Database::NACK;
    quint8 retries = 0;

    while(server_list.fCommand == Database::NACK && retries < 5)
    {
        server_list = retrieveData(QList<Database::AllTableColumns>({Database::SERVERNAME, Database::IP, Database::PORTSTART,
                                                                     Database::PORTSTOP, Database::DIRECTORY, Database::WEBAPP}));
        ++ retries;
    }

    if(server_list.fCommand == Database::NACK)
        qCritical("AppController::startupAllServers -> unable to retrieve the server list from the database");
    else
    {
        QDataStream in(&server_list.fBody, QIODevice::ReadOnly);
        int result_count = 0;
        in >> result_count;

        for(int i = 0; i < result_count; ++i)
        {
            QString server_name, webapp, directory, ip;
            quint16 port_start, port_stop;
            in >> server_name >> ip >> port_start >> port_stop >> directory >> webapp;
            startServerInstances(server_name, ip, port_start, port_stop, directory, webapp);
        }
    }

    return;
}


void AppController::shutdownAllServers()
{
    for(auto it = fServers->begin(); it != fServers->end(); ++it)
        it.value()->stop("APPCONTROLLER");
    return;
}


void AppController::newDeployedServer(QString server_name)
{
    if(deployServer(server_name))
        emit done("DEPLOY", true);
    else
        emit done("DEPLOY", false);
    return;
}


void AppController::newTakendownServer(QString server_name)
{
    takedownServer(server_name);
    emit done("TAKEDOWN", true);
    return;
}


void AppController::newConfiguredServer(QString server_name)
{
    takedownServer(server_name);
    if(deployServer(server_name))
        emit done("CONFIGURE", true);
    else
        emit done("CONFIGURE", false);
    return;
}


void AppController::cleanupOldServers()
{
    if(!fRunning_Server_File->open(QIODevice::ReadOnly))
        qWarning("AppController::cleanupOldServers -> %s", fRunning_Server_File->errorString().toStdString().c_str());
    QDataStream in(fRunning_Server_File);
    while(!in.atEnd())
    {
        QString kill_process;
        in >> kill_process;
        if(!(kill_process == "0"))
        {
            kill_process.prepend("kill -9 ");
            system(kill_process.toStdString().c_str());
        }
    }
    fRunning_Server_File->close();
    if(!fRunning_Server_File->open(QIODevice::WriteOnly))
            qWarning("AppController::cleanupOldServers -> %s", fRunning_Server_File->errorString().toStdString().c_str());
    fRunning_Server_File->close();
    return;
}


void AppController::addPIDToFile(qint64 process_id)
{
    if(!fRunning_Server_File->open(QIODevice::Append))
        qWarning("AppController::addPIDToFile -> %s", fRunning_Server_File->errorString().toStdString().c_str());
    QDataStream out(fRunning_Server_File);
    out << QString::number(process_id);
    fRunning_Server_File->close();
    return;
}


void AppController::removePIDFromFile(qint64 process_id)
{
    if(!fRunning_Server_File->open(QIODevice::ReadOnly))
        qWarning("AppController::removePIDFromFile -> %s", fRunning_Server_File->errorString().toStdString().c_str());
    QDataStream in(fRunning_Server_File);
    QStringList server_ids;
    while(!in.atEnd())
    {
        QString id;
        in >> id;
        server_ids.append(id);
    }
    for(QString it : server_ids)
        if(it == QString::number(process_id))
        {
            server_ids.removeOne(it);
            break;
        }
    fRunning_Server_File->close();
    if(!fRunning_Server_File->open(QIODevice::WriteOnly))
        qWarning("AppController::removePIDFromFile -> %s", fRunning_Server_File->errorString().toStdString().c_str());
    QDataStream out(fRunning_Server_File);
    for(QString it : server_ids)
        out << it;
    fRunning_Server_File->close();
    return;
}


void AppController::updateData(Database::AllTableColumns column, QString value, QString server_name)
{
    int request_count = 1;
    QString field = fDatabase->columnNameInString(column) + " = " + value;
    QString table = "ServerProcess";
    QString where = "WHERE server_name = \'" + server_name + "\'";
    QByteArray body;
    QDataStream out(&body, QIODevice::WriteOnly);
    out << request_count << table << where << field;

    Database::SqlData data;
    data.fCommand = Database::UPDATE;
    data.fObject_ID = Database::APPCONTROLLER;
    data.fBody = body;
    fDatabase->queryDB(data);
    return;
}


Database::SqlData AppController::retrieveData(QList<Database::AllTableColumns> columns, QString server_name, bool all)
{
    int request_count = columns.size();
    QString field, where = "";
    QString table = "ServerProcess";
    if(!all)
        where = "WHERE server_name = \'" + server_name + "\'";
    QByteArray body;
    QDataStream out(&body, QIODevice::WriteOnly);
    out << request_count << table << where;
    for(Database::AllTableColumns column : columns)
    {
        field = fDatabase->columnNameInString(column);
        out << field;
    }

    Database::SqlData data;
    data.fCommand = Database::READ;
    data.fObject_ID = Database::APPCONTROLLER;
    data.fBody = body;
    Database::SqlData result= fDatabase->queryDB(data);

    int retries = 0;
    while(result.fCommand == Database::NACK && retries <= 5)
        result = fDatabase->queryDB(data), ++retries;

    return result;
}


void AppController::startServerInstances(QString server_name, QString ip, quint16 port_start, quint16 port_end, QString directory, QString webapp)
{
    quint8 instance_count = 0;
    for(quint16 it = port_start; it <= port_end; ++ it)
    {
        ServerProcess* server = new ServerProcess(server_name, ip, it, instance_count, directory, this, fDatabase, (webapp == "Y"));
        connect(server, &ServerProcess::processDead, this, [=]()
        {
            removePIDFromFile(server->processID());
            fServers->remove(server->getName(), server);
            if(!fServers->contains(server->getName()))
                updateData(Database::SERVERSTATE, "\'OFFLINE\'", server->getName());
            server->deleteLater();
        });
        fServers->insert(server_name, server);
        updateData(Database::SERVERSTATE, "\'ONLINE\'", server_name);
        server->start("APPCONTROLLER");
        addPIDToFile(server->processID());
        ++ instance_count;
    }
    return;
}


bool AppController::deployServer(QString server_name)
{
    bool successful = false;
    Database::SqlData server_info;
    server_info.fCommand = Database::NACK;
    quint8 retries = 0;

    while(server_info.fCommand == Database::NACK && retries < 5)
    {
        server_info = retrieveData(QList<Database::AllTableColumns>({Database::IP, Database::PORTSTART, Database::PORTSTOP, Database::DIRECTORY,
                                                                     Database::WEBAPP}), server_name, false);
        ++ retries;
    }

    if(server_info.fCommand == Database::NACK)
        qCritical("AppController::newDeployedServer -> unable to retrieve the server %s's information from the database", server_name.toStdString().c_str());
    else
    {
        QDataStream in(&server_info.fBody, QIODevice::ReadOnly);
        int result_count = 0;
        in >> result_count;

        QString webapp, directory, ip;
        quint16 port_start, port_stop;
        in >> ip >> port_start >> port_stop >> directory >> webapp;
        startServerInstances(server_name, ip, port_start, port_stop, directory, webapp);
        successful = true;
    }
    return successful;
}


void AppController::takedownServer(QString server_name)
{
    for(auto it = fServers->begin(); it != fServers->end(); ++ it)
        if(it.key() == server_name)
            it.value()->stop("APPCONTROLLER");
    return;
}
