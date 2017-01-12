#include "MainController.hpp"
#include "Database.hpp"
#include <QDataStream>
#include <QSqlQuery>
#include <QSqlError>

Database::Database() :
    QSqlDatabase(addDatabase("QMYSQL"))
{
    setHostName("localhost");
    setPort(3306);
    setDatabaseName("ServerProcessDatabase");
    setUserName("ProcessControl");
    setPassword("4146F8A68D");
    //FIXME: change CLIENT_SSL to a corresponding ca, cert and key file path
    setConnectOptions("MYSQL_OPT_RECONNECT=1");
    if(!open())
    {
        close();
        qFatal("Database::Database -> QSqlDatabase::open -> %s", lastError().text().toStdString().c_str());
    }

#ifdef _DEBUG
    QSqlQuery query;
    if(!query.exec("CREATE TABLE IF NOT EXISTS User("
                   "id BIGINT PRIMARY KEY AUTO_INCREMENT,"
                   "username VARCHAR(20) not null,"
                   "password VARCHAR(45) not null,"
                   "login_count SMALLINT UNSIGNED not null DEFAULT 0,"
                   "last_login DATETIME,"
                   "grant_priv ENUM(\'N\', \'Y\') not null DEFAULT \'N\',"
                   "deploy_priv ENUM(\'N\', \'Y\') not null DEFAULT \'N\',"
                   "takedown_priv ENUM(\'N\', \'Y\') not null DEFAULT \'N\',"
                   "configure_priv ENUM(\'N\', \'Y\') not null DEFAULT \'N\')"))
        qWarning("Database::Databse -> QSqlQuery::exec -> %s", query.lastError().text().toStdString().c_str());
    query.finish();

    if(!query.exec("CREATE UNIQUE INDEX User_Index on User (username, password);"))
        qWarning("Database::Databse -> QSqlQuery::exec -> %s", query.lastError().text().toStdString().c_str());
    query.finish();

    if(!query.exec("INSERT INTO User (username, password, grant_priv, deploy_priv, takedown_priv, configure_priv) "
                   "VALUES(\'administrator\', PASSWORD(\'BF6AD86814\'), \'Y\', \'Y\', \'Y\', \'Y\')"))
        qWarning("Database::Databse -> QSqlQuery::exec -> %s", query.lastError().text().toStdString().c_str());
    query.finish();

    if(!query.exec("CREATE TABLE IF NOT EXISTS ServerProcess("
                   "id BIGINT PRIMARY KEY AUTO_INCREMENT,"
                   "client_name TEXT not null,"
                   "mobile VARCHAR(20) not null,"
                   "email TEXT not null,"
                   "address TEXT not null,"
                   "project_start_date DATETIME not null,"
                   "project_stop_date DATETIME not null,"
                   "server_name VARCHAR(100) not null,"
                   "server_deploy_date DATETIME not null,"
                   "server_takedown_date DATETIME,"
                   "server_state ENUM(\'OFFLINE\', \'ONLINE\') not null DEFAULT \'OFFLINE\',"
                   "server_description TEXT not null,"
                   "server_cost DECIMAL(12, 3) not null,"
                   "deployment_cost DECIMAL(12, 3) not null,"
                   "crash_count SMALLINT UNSIGNED not null DEFAULT 0,"
                   "instance_count TINYINT UNSIGNED not null,"
                   "url VARCHAR(100) not null,"
                   "ip VARCHAR(20) not null DEFAULT \'127.0.0.1\',"
                   "port_start SMALLINT UNSIGNED not null,"
                   "port_stop SMALLINT UNSIGNED not null,"
                   "host_server_name TEXT not null,"
                   "vps_ovz_solution TEXT not null,"
                   "server_load FLOAT(3, 2) not null DEFAULT 0.0,"
                   "directory VARCHAR(255) not null DEFAULT \'/var/servers/\',"
                   "support_ads ENUM(\'N\', \'Y\') not null DEFAULT \'N\',"
                   "other_webapis ENUM(\'N\', \'Y\') not null DEFAULT \'N\',"
                   "webapp ENUM(\'N\', \'Y\') not null DEFAULT \'N\')"))
        qWarning("Database::Databse -> QSqlQuery::exec -> %s", query.lastError().text().toStdString().c_str());
    query.finish();

    if(!query.exec("CREATE UNIQUE INDEX ServerProcess_Index on ServerProcess (server_name, url, port_start, port_stop)"))
        qWarning("Database::Databse -> QSqlQuery::exec -> %s", query.lastError().text().toStdString().c_str());
    query.finish();
#endif
}

Database::~Database()
{
    commit();
    close();
}


Database::SqlData Database::queryDB(Database::SqlData data)
{
    SqlData response;
    switch(data.fCommand)
    {
        case CREATE:
            response = insertQuery(data.fObject_ID, data.fBody);
        break;

        case UPDATE:
            response = updateQuery(data.fObject_ID, data.fBody);
        break;

        case READ:
            response = selectQuery(data.fObject_ID, data.fBody);
        break;

        case DELETE:
            response = deleteQuery(data.fObject_ID, data.fBody);
        break;

        default: break;
    }
    return response;
}


QString Database::columnNameInString(AllTableColumns col)
{
    QString name;
    switch(col)
    {
        case USERNAME:
            name = "username";
        break;

        case PASSWORD:
            name = "password";
        break;

        case LOGINCOUNT:
            name = "login_count";
        break;

        case LASTLOGIN:
            name = "last_login";
        break;

        case GRANTPRIV:
            name = "grant_priv";
        break;

        case DEPLOYPRIV:
            name = "deploy_priv";
        break;

        case TAKEDOWNPRIV:
            name = "takedown_priv";
        break;

        case CONFIGUREPRIV:
            name = "configure_priv";
        break;

        case CLIENTNAME:
            name = "client_name";
        break;

        case MOBILE:
            name = "mobile";
        break;

        case EMAIL:
            name = "email";
         break;

        case ADDRESS:
            name = "address";
        break;

        case PROJECTSTARTDATE:
            name = "project_start_date";
        break;

        case PROJECTSTOPDATE:
            name = "project_stop_date";
        break;

        case SERVERNAME:
            name = "server_name";
        break;

        case SERVERDEPLOYDATE:
            name = "server_deploy_date";
        break;

        case SERVERTAKEDOWNDATE:
            name = "server_takedown_date";
        break;

        case SERVERSTATE:
            name = "server_state";
        break;

        case SERVERDESCRIPTION:
            name = "server_description";
        break;

        case SERVERCOST:
            name = "server_cost";
        break;

        case DEPLOYMENTCOST:
            name = "deployment_cost";
        break;

        case CRASHCOUNT:
            name = "crash_count";
        break;

        case INSTANCECOUNT:
            name = "instance_count";
        break;

        case URL:
            name = "url";
        break;

        case IP:
            name = "ip";
        break;

        case PORTSTART:
            name = "port_start";
        break;

        case PORTSTOP:
            name = "port_stop";
        break;

        case HOSTSERVERNAME:
            name = "host_server_name";
        break;

        case VPSOVZSOLUTION:
            name = "vps_ovz_solution";
        break;

        case SERVERLOAD:
            name = "server_load";
        break;

        case DIRECTORY:
            name = "directory";
        break;

        case SUPPORTADS:
            name = "support_ads";
        break;

        case OTHERWEBAPIS:
            name = "other_webapis";
        break;

        case WEBAPP:
            name = "webapp";
        break;

        default: break;
    }
    return name;
}


Database::SqlData Database::insertQuery(ObjectID& object_id, QByteArray& body)
{
    int request_count = 0;
    QString table, fields, values;

    QDataStream in(&body, QIODevice::ReadOnly);
    in >> request_count >> table;

    for(int i = 0; i < request_count; ++i)
    {
        QString str;
        in >> str;
        fields.append(", " + str);
    }

    for(int i = 0; i < request_count; ++i)
    {
        QString str;
        in >> str;
        values.append(", " + str);
    }

    QString sqlstatement = QString("INSERT INTO %1 (%2) VALUES(%3)").arg(table, fields.remove(0, 1), values.remove(0, 1));
    return nonSelectDBQuery(object_id, sqlstatement);

}


Database::SqlData Database::selectQuery(ObjectID& object_id, QByteArray& body)
{
    int request_count = 0;
    QString table, where, fields;
    QVector<ColumnDataType> columns;

    QDataStream in(&body, QIODevice::ReadOnly);
    in >> request_count >> table >> where;

    for(int i = 0; i < request_count; ++i)
    {
        QString str;
        in >> str;
        fields.append(", " + str);
        columns.push_back(columnDataType(str));
    }

    QString sqlstatement = QString("SELECT %1 FROM %2 " + where).arg(fields.remove(0, 1), table);
    return selectDBQuery(object_id, sqlstatement, columns);
}


Database::SqlData Database::updateQuery(ObjectID& object_id, QByteArray& body)
{
    int request_count = 0;
    QString table, where, fields;

    QDataStream in(&body, QIODevice::ReadOnly);
    in >> request_count >> table >> where;

    for(int i = 0; i < request_count; ++i)
    {
        QString str;
        in >> str;
        fields.append(", " + str);
    }

    QString sqlstatement = QString("UPDATE %1 SET %2 " + where).arg(table, fields.remove(0, 1));
    return nonSelectDBQuery(object_id, sqlstatement);
}


Database::SqlData Database::deleteQuery(ObjectID& object_id, QByteArray& body)
{
    QString table, where;
    QDataStream in(&body, QIODevice::ReadOnly);
    in >> table >> where;
    QString sqlstatment = "DELETE FROM " + table + " " + where;
    return nonSelectDBQuery(object_id, sqlstatment);
}


Database::SqlData Database::nonSelectDBQuery(ObjectID& object_id, QString& sqlstatement)
{
    SqlData response;
    QSqlQuery query;
    if(query.exec(sqlstatement))
    {
        response.fObject_ID = object_id;
        response.fCommand = ACK;
        response.fResult = EMPTY;
    }
    else
    {
        qWarning("DataBase::nonSelectDBQuery -> QSqlQuery::exec -> %s", query.lastError().text().toStdString().c_str());
        response.fObject_ID = object_id;
        response.fCommand = NACK;
        response.fResult = EMPTY;
    }
    query.finish();
    return response;
}


Database::SqlData Database::selectDBQuery(ObjectID& object_id, QString& sqlstatement, QVector<ColumnDataType>& columns)
{
    SqlData response;
    QSqlQuery query;
    if(query.exec(sqlstatement))
    {
        QByteArray body;
        QDataStream out(&body, QIODevice::WriteOnly);

        int result_count = 0;
        out << result_count;
        while(query.next())
        {
            for(int i = 0; i < columns.size(); ++i)
                switch(columns.at(i))
                {
                    case TINYINTUNSIGNED:
                        out << (quint8)query.value(i).toBool();
                    break;

                    case SMALLINTUNSIGNED:
                        out << (quint16)query.value(i).toUInt();
                    break;

                    case STRING:
                        out << query.value(i).toString();
                    break;

                    case DATETIME:
                        out << query.value(i).toDateTime();
                    break;

                    default: break;
                }
            ++result_count;
        }
        out.device()->seek(0);
        out << result_count;

        response.fObject_ID = object_id;
        response.fCommand = ACK;
        response.fResult = SQLRECORD;
        response.fBody = body;
    }
    else
    {
        qWarning("DataBase::SelectDBQuery -> QSqlQuery::exec -> %s", query.lastError().text().toStdString().c_str());
        response.fObject_ID = object_id;
        response.fCommand = NACK;
        response.fResult = EMPTY;
    }
    query.finish();
    return response;
}


Database::ColumnDataType Database::columnDataType(QString name)
{
    if(name == "instance_count")
        return TINYINTUNSIGNED;
    else if(name == "login_count" || name == "crash_count" || name == "port_start" || name == "port_stop")
        return SMALLINTUNSIGNED;
    else if(name == "username" || name == "password" || name == "client_name" || name == "mobile" || name == "email"
            || name == "address" || name == "server_name" || name == "server_description" || name == "url" || name == "ip"
            || name == "host_server_name" || name == "vps_ovz_solution" || name == "grant_priv" || name == "deploy_priv"
            || name == "takedown_priv" || name == "configure_priv" || name == "support_ads" || name == "other_webapis"
            || name == "server_load" || name == "server_cost" || name == "deployment_cost" || name == "server_state"
            || name == "directory" || name == "webapp")
        return STRING;
    else if(name == "last_login" || name == "project_start_date" || name == "project_stop_date" || name == "server_deploy_date"
            || name == "server_takedown_date")
        return DATETIME;
    return NULLVALUE;
}
