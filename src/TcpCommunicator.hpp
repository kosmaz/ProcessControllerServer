#ifndef TCPCOMMUNICATOR_HPP
#define TCPCOMMUNICATOR_HPP


#include <QWebSocketServer>
#include <QNetworkSession>
#include <QList>

class QNetworkConfigurationManager;
class MainController;
class QWebSocket;

class TcpCommunicator : public QWebSocketServer
{
    Q_OBJECT
public:

    enum BodyType {
        EMPTY = 0x00,
        TEXT = 0x01,
        BINARY = 0x02
    };

    enum HttpCommand {
        //Request
        SETID = 0x03,
        VERIFY_SELF = 0x04,

        //Respose
        VERIFY_PASS = 0x05,
        VERIFY_FAIL = 0x06
    };

    class HttpData {
    public:
        HttpData(){}
        quint8 fCommand;
        quint16 fClient_ID;
        quint8 fType;
        QByteArray fBody;
    };

    TcpCommunicator(MainController*);
    ~TcpCommunicator();

signals:
    void dataReceived(TcpCommunicator::HttpData);
    void removeVerifiedClient(int);

private slots:
    void openSession();
    void configUpdated();
    void acceptConnection();
    void sslErrorHandler(QList<QSslError>);
    void writeHttpData(TcpCommunicator::HttpData);
    void networkSessionErrorHandler(QNetworkSession::SessionError);

private:

    QList<QWebSocket*>* fConnected_Sockets;
    QNetworkConfigurationManager* fConfig_Manager;
    QNetworkSession* fNetwork_Session;
};

#endif // TCPCOMMUNICATOR_HPP
