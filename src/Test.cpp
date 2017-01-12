#ifdef TEST

#include "MainController.hpp"
#include "ServerProcess.hpp"
#include "AppConfigurer.hpp"
#include "AppController.hpp"
#include "AppDeployer.hpp"
#include "AppTakeDown.hpp"
#include "AppUpgrader.hpp"
#include "Database.hpp"
#include "TcpCommunicator.hpp"


#include <QCoreApplication>
#include <QDataStream>
#include <QTimer>

int main(int argc, char** argv)
{

#define NAME(a) database.columnNameInString(a)
#define FIELD( b) Database::b


//#define DATABASE_TEST
//#define APPCONTROLLER_TEST
//#define SERVERPROCESS_TEST

    QCoreApplication a(argc, argv);


#ifdef DATABASE_TEST

    /**************************BEGIN DATABASE OBJECT TEST****************************/
    Database database;
    Database::SqlData data;

    int result_count = 0;
    int request_count = 0;
    QString field;
    QString value;
    QString where;
    QString table("User");

    QDataStream out(&data.fBody, QIODevice::WriteOnly);
    QDataStream in(&data.fBody, QIODevice::ReadOnly);

    //BEGIN INSERT QUERY
    request_count = 4;
    out << request_count << table;
    field = NAME(FIELD(USERNAME));
    out << field;
    field = NAME(FIELD(PASSWORD));
    out << field;
    field = NAME(FIELD(GRANTPRIV));
    out << field;
    field = NAME(FIELD(TAKEDOWNPRIV));
    out << field;

    value = "\'test\'";
    out << value;
    value = "PASSWORD(\'test\')";
    out << value;
    value = "\'Y\'";
    for(int i = 0; i < 2; ++ i)
        out << value;

    data.fObject_ID = Database::MAINCONTROLLER;
    data.fCommand = Database::CREATE;
    data = database.queryDB(data);
    //END OF INSERT QUERY


    //BEGIN SELECT QUERY
    out.device()->seek(0);
    data.fBody.clear();
    request_count = 4;
    where = "WHERE username = \'test\'";
    out << request_count << table << where;
    field = NAME(FIELD(USERNAME));
    out << field;
    field = NAME(FIELD(PASSWORD));
    out << field;
    field = NAME(FIELD(DEPLOYPRIV));
    out << field;
    field = NAME(FIELD(CONFIGUREPRIV));
    out << field;

    data.fObject_ID = Database::MAINCONTROLLER;
    data.fCommand = Database::READ;
    data = database.queryDB(data);

    qDebug("SELECT QUERY RESULT\n");
    in >> result_count;
    for(int i = 0; i < result_count; ++i)
    {
        QString str;
        in >> str;
        qDebug("%s\n", str.toStdString().c_str());
        in >> str;
        qDebug("%s\n", str.toStdString().c_str());
        in >> str;
        qDebug("%s\n", str.toStdString().c_str());
        in >> str;
        qDebug("%s\n\n", str.toStdString().c_str());
    }
    //END OF SELECT QUERY


    //BEGIN UPDATE QUERY
    out.device()->seek(0);
    data.fBody.clear();
    request_count = 2;
    where = "WHERE username = \'test\'";
    out << request_count << table << where;
    field = NAME(FIELD(DEPLOYPRIV)) + " = \'Y\'";
    out << field;
    field = NAME(FIELD(CONFIGUREPRIV)) + " = \'Y\'";
    out << field;

    data.fObject_ID = Database::MAINCONTROLLER;
    data.fCommand = Database::UPDATE;
    data = database.queryDB(data);
    //END OF UPDATE QUERY


    //BEGIN DELETE QUERY
    out.device()->seek(0);
    data.fBody.clear();
    where = "WHERE username = \'test\'";
    out << table << where;

    data.fObject_ID = Database::MAINCONTROLLER;
    data.fCommand = Database::DELETE;
    data = database.queryDB(data);
    //END OF DELETE QUERY


    /**************************END OF DATABASE OBJECT TEST***************************/
#endif



#ifdef SERVERPROCESS_TEST

    /**************************BEGIN SERVERPROCESS OBJECT TEST****************************/

    Database database;
    Database::SqlData data;

    int request_count = 0;
    QString field;
    QString value;
    QString table("ServerProcess");

    QDataStream out(&data.fBody, QIODevice::WriteOnly);

    //BEGIN INSERT QUERY
    request_count = 20;
    out << request_count << table;
    field = NAME(FIELD(CLIENTNAME)), out << field;
    field = NAME(FIELD(MOBILE)), out << field;
    field = NAME(FIELD(EMAIL)), out << field;
    field = NAME(FIELD(ADDRESS)), out << field;
    field = NAME(FIELD(PROJECTSTARTDATE)), out << field;
    field = NAME(FIELD(PROJECTSTOPDATE)), out << field;
    field = NAME(FIELD(SERVERNAME)), out << field;
    field = NAME(FIELD(SERVERDEPLOYDATE)), out << field;
    field = NAME(FIELD(SERVERTAKEDOWNDATE)), out << field;
    field = NAME(FIELD(SERVERSTATE)), out << field;
    field = NAME(FIELD(SERVERDESCRIPTION)), out << field;
    field = NAME(FIELD(SERVERCOST)), out << field;
    field = NAME(FIELD(DEPLOYMENTCOST)), out << field;
    field = NAME(FIELD(INSTANCECOUNT)), out << field;
    field = NAME(FIELD(URL)), out << field;
    field = NAME(FIELD(PORTSTART)), out << field;
    field = NAME(FIELD(PORTSTOP)), out << field;
    field = NAME(FIELD(HOSTSERVERNAME)), out << field;
    field = NAME(FIELD(VPSOVZSOLUTION)), out << field;
    field = NAME(FIELD(WEBAPP)), out << field;

    value = "\'kosmaz\'", out << value;
    value = "\'+2348139278033\'", out << value;
    value = "\'kosmaz2009@yahoo.com\'", out << value;
    value = "\'Holy Family Youth village, Amansea\'", out << value;
    value = "\'" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "\'", out << value;
    value = "\'" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "\'", out << value;
    value = "\'Nestor_s-Challenge.wt\'", out << value;
    value = "\'" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "\'", out << value;
    value = "\'" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "\'", out << value;
    value = "1", out << value;
    value = "\'Unit testing of ServerProcess class'", out << value;
    value = "0.0", out << value;
    value = "0.0", out << value;
    value = "1", out << value;
    value = "\'localhost'", out << value;
    value = "8081", out << value;
    value = "8081", out << value;
    value = "\'kosmaz-linux-box\'", out << value;
    value = "\'local hosting\'", out << value;
    value = "\'Y\'", out << value;

    data.fObject_ID = Database::MAINCONTROLLER;
    data.fCommand = Database::CREATE;
    data = database.queryDB(data);
    //END OF INSERT QUERY

    ServerProcess app("Nestor_s-Challenge.wt", "127.0.0.1", 8081, 0, "/home/kosmaz/Software-Development/Wt_Projects/projects/Nestor_s-Challenge/",
                      0, &database, true);
    app.start();

    QTimer load_check;
    load_check.setInterval(10000);
    QObject::connect(&load_check, &QTimer::timeout, [&]()
    {
        qDebug("CPU Load: %f", app.processCPULoad());
    });
    load_check.start();

    QTimer app_timeout;
    app_timeout.setInterval(60000);
    QObject::connect(&app_timeout, &QTimer::timeout, [&]()
    {
        app.stop();
        load_check.stop();
        app_timeout.stop();

        //BEGIN DELETE QUERY
        out.device()->seek(0);
        data.fBody.clear();
        QString where = "WHERE server_name = \'Nestor_s-Challenge.wt\'";
        out << table << where;

        data.fObject_ID = Database::MAINCONTROLLER;
        data.fCommand = Database::DELETE;
        data = database.queryDB(data);
        //END OF DELETE QUERY
    });
    app_timeout.start();


    /**************************END OF SERVERPROCESS OBJECT TEST***************************/

#endif




#ifdef APPCONTROLLER_TEST
    /**************************BEGIN APPCONTROLLER OBJECT TEST****************************/

    Database database;
    Database::SqlData data;

    int request_count = 0;
    QString field;
    QString value;
    QString table("ServerProcess");

    QDataStream out(&data.fBody, QIODevice::WriteOnly);

    //BEGIN INSERT QUERY
    request_count = 20;
    out << request_count << table;
    field = NAME(FIELD(CLIENTNAME)), out << field;
    field = NAME(FIELD(MOBILE)), out << field;
    field = NAME(FIELD(EMAIL)), out << field;
    field = NAME(FIELD(ADDRESS)), out << field;
    field = NAME(FIELD(PROJECTSTARTDATE)), out << field;
    field = NAME(FIELD(PROJECTSTOPDATE)), out << field;
    field = NAME(FIELD(SERVERNAME)), out << field;
    field = NAME(FIELD(SERVERDEPLOYDATE)), out << field;
    field = NAME(FIELD(SERVERSTATE)), out << field;
    field = NAME(FIELD(SERVERDESCRIPTION)), out << field;
    field = NAME(FIELD(SERVERCOST)), out << field;
    field = NAME(FIELD(DEPLOYMENTCOST)), out << field;
    field = NAME(FIELD(INSTANCECOUNT)), out << field;
    field = NAME(FIELD(URL)), out << field;
    field = NAME(FIELD(PORTSTART)), out << field;
    field = NAME(FIELD(PORTSTOP)), out << field;
    field = NAME(FIELD(HOSTSERVERNAME)), out << field;
    field = NAME(FIELD(VPSOVZSOLUTION)), out << field;
    field = NAME(FIELD(DIRECTORY)), out << field;
    field = NAME(FIELD(WEBAPP)), out << field;

    value = "\'kosmaz\'", out << value;
    value = "\'+2348139278033\'", out << value;
    value = "\'kosmaz2009@yahoo.com\'", out << value;
    value = "\'Holy Family Youth village, Amansea\'", out << value;
    value = "\'" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "\'", out << value;
    value = "\'" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "\'", out << value;
    value = "\'Nestor_s-Challenge.wt\'", out << value;
    value = "\'" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "\'", out << value;
    value = "1", out << value;
    value = "\'Unit testing of ServerProcess class'", out << value;
    value = "0.0", out << value;
    value = "0.0", out << value;
    value = "6", out << value;
    value = "\'localhost'", out << value;
    value = "8081", out << value;
    value = "8086", out << value;
    value = "\'kosmaz-linux-box\'", out << value;
    value = "\'local hosting'", out << value;
    value = "\'/home/kosmaz/Software-Development/Wt_Projects/projects/Nestor_s-Challenge/\'", out << value;
    value = "\'Y\'", out << value;

    data.fObject_ID = Database::MAINCONTROLLER;
    data.fCommand = Database::CREATE;
    data = database.queryDB(data);
    //END OF INSERT QUERY

    AppController backbone(0, &database);
    backbone.startupAllServers();

    QTimer app_timeout;
    app_timeout.setInterval(120000);
    QObject::connect(&app_timeout, &QTimer::timeout, [&]()
    {
        backbone.shutdownAllServers();
        app_timeout.stop();

        //BEGIN DELETE QUERY
        out.device()->seek(0);
        data.fBody.clear();
        QString where = "WHERE server_name = \'Nestor_s-Challenge.wt\'";
        out << table << where;

        data.fObject_ID = Database::MAINCONTROLLER;
        data.fCommand = Database::DELETE;
        data = database.queryDB(data);
        //END OF DELETE QUERY
    });
    app_timeout.start();


    /**************************END OF APPCONTROLLER OBJECT TEST***************************/

#endif








    return a.exec();
}

#endif
