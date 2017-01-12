#ifndef APPCONTROLLER_HPP
#define APPCONTROLLER_HPP

#include <QObject>

#include "Database.hpp"

class MainController;
class ServerProcess;
class QFile;

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(MainController*, Database*);
    ~AppController();
    void startupAllServers();
    void shutdownAllServers();

signals:
    void done(QString, bool);

public slots:
    void newDeployedServer(QString);
    void newTakendownServer(QString);
    void newConfiguredServer(QString);

private slots:

private:
    void cleanupOldServers();
    void addPIDToFile(qint64);
    void removePIDFromFile(qint64);
    void updateData(Database::AllTableColumns, QString, QString);
    Database::SqlData retrieveData(QList<Database::AllTableColumns>, QString server_name = "", bool all = true);
    void startServerInstances(QString, QString, quint16, quint16, QString, QString);


    bool deployServer(QString);
    void takedownServer(QString);

    Database* fDatabase;
    QFile* fRunning_Server_File;
    QMultiMap<QString, ServerProcess*>* fServers;

};

#endif // APPCONTROLLER_HPP
