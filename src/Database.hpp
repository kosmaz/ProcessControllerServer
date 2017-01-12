#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <QSqlDatabase>


class Database : public QSqlDatabase
{
public:

   enum SqlCommand {
        //Request
        CREATE = 0x00,
        READ = 0x01,
        UPDATE = 0x02,
        DELETE = 0x03,

        //Response
        ACK = 0x04,
        NACK = 0x05
    };

    enum SqlResult {
        EMPTY = 0x06,
        SQLRECORD = 0x07
    };

    enum ObjectID {
        APPCONFIGURER = 0x08,
        APPCONTROLLER = 0x09,
        APPDEPLOYER = 0x0A,
        APPMONITOR = 0x0B,
        APPTAKEDOWN = 0x0C,
        APPUPGRADER = 0x0D,
        MAINCONTROLLER = 0x0E,
        SERVERPROCESS = 0x0F
    };

    enum AllTableColumns {
        //User table
        USERNAME = 0x10,
        PASSWORD = 0x11,
        LOGINCOUNT = 0x12,
        LASTLOGIN = 0x13,
        GRANTPRIV = 0x14,
        DEPLOYPRIV = 0x15,
        TAKEDOWNPRIV = 0x16,
        CONFIGUREPRIV = 0x17,

        //ServerProcess table
        CLIENTNAME = 0x18,
        MOBILE = 0x19,
        EMAIL = 0x1A,
        ADDRESS = 0x1B,
        PROJECTSTARTDATE = 0x1C,
        PROJECTSTOPDATE = 0x1D,
        SERVERNAME = 0x1E,
        SERVERDEPLOYDATE = 0x1F,
        SERVERTAKEDOWNDATE = 0x20,
        SERVERSTATE = 0x21,
        SERVERDESCRIPTION = 0x22,
        SERVERCOST = 0x23,
        DEPLOYMENTCOST = 0x24,
        CRASHCOUNT = 0x25,
        INSTANCECOUNT = 0x26,
        URL = 0x27,
        IP = 0x28,
        PORTSTART = 0x29,
        PORTSTOP = 0x2A,
        HOSTSERVERNAME = 0x2B,
        VPSOVZSOLUTION = 0x2C,
        SERVERLOAD = 0x2D,
        DIRECTORY = 0x2E,

        //Specifically for web servers
        SUPPORTADS = 0x2F,
        OTHERWEBAPIS = 0x30,
        WEBAPP = 0x31,

        //Dummy field
        DUMMY = 0x32
    };

    enum ColumnDataType {
        TINYINTUNSIGNED = 0x33,
        SMALLINTUNSIGNED = 0x34,
        STRING = 0x35,
        DATETIME = 0x36,
        NULLVALUE = 0x37
    };

    class SqlData {
    public:
        SqlData(){}
        SqlCommand fCommand;
        SqlResult fResult;
        ObjectID fObject_ID;
        QByteArray fBody;
    };

    Database();
    ~Database();
    Database::SqlData queryDB(Database::SqlData);
    QString columnNameInString(AllTableColumns);

private:
    SqlData insertQuery(ObjectID&, QByteArray&);    //C
    SqlData selectQuery(ObjectID&, QByteArray&);    //R
    SqlData updateQuery(ObjectID&, QByteArray&);    //U
    SqlData deleteQuery(ObjectID&, QByteArray&);    //D

    SqlData nonSelectDBQuery(ObjectID&, QString&);
    SqlData selectDBQuery(ObjectID&, QString&, QVector<ColumnDataType>&);

    ColumnDataType columnDataType(QString);
};

#endif // DATABASE_HPP
