#include "TcpCommunicator.hpp"
#include "MainController.hpp"
#include <QNetworkConfigurationManager>
#include <QWebSocketCorsAuthenticator>
#include <QNetworkConfiguration>
#include <QSslConfiguration>
#include <QNetworkProxy>
#include <QDataStream>
#include <QWebSocket>
#include <QByteArray>
#include <QSslKey>
#include <QFile>


TcpCommunicator::TcpCommunicator(MainController* parent) :
    QWebSocketServer("webappcontrollerserver", SecureMode, parent),
    fConnected_Sockets(new QList<QWebSocket*>),
    fConfig_Manager(new QNetworkConfigurationManager(this)),
    fNetwork_Session(nullptr)
{
    connect(parent, &MainController::sendData, this, &TcpCommunicator::writeHttpData);
    QSslConfiguration ssl_config;
    ssl_config.setProtocol(QSsl::SecureProtocols);
    ssl_config.setSslOption(QSsl::SslOptionDisableCompression, true);
    ssl_config.setSslOption(QSsl::SslOptionDisableEmptyFragments, false);
    ssl_config.setSslOption(QSsl::SslOptionDisableLegacyRenegotiation, true);
    ssl_config.setSslOption(QSsl::SslOptionDisableServerNameIndication, false);
    ssl_config.setSslOption(QSsl::SslOptionDisableSessionPersistence, true);
    ssl_config.setSslOption(QSsl::SslOptionDisableSessionSharing, false);
    ssl_config.setSslOption(QSsl::SslOptionDisableSessionTickets, false);
    ssl_config.setPeerVerifyDepth(0);
    ssl_config.setPeerVerifyMode(QSslSocket::QueryPeer);

    QFile ssl_certificate_file("SSL/WebAppController.crt");
    ssl_certificate_file.open(QIODevice::ReadOnly);
    ssl_config.setLocalCertificate(QSslCertificate(&ssl_certificate_file));
    ssl_config.setCaCertificates(QList<QSslCertificate>({QSslCertificate(&ssl_certificate_file)}));
    QFile ssl_privatekey("SSL/WebAppController.key");
    ssl_privatekey.open(QIODevice::ReadOnly);
    ssl_config.setPrivateKey(QSslKey(&ssl_privatekey,QSsl::Rsa));

    setSslConfiguration(ssl_config);

    if(fConfig_Manager->capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        connect(fConfig_Manager, SIGNAL(updateCompleted()), this, SLOT(configUpdated()));
        fConfig_Manager->updateConfigurations();
    }
    else
        openSession();
}


TcpCommunicator::~TcpCommunicator()
{
    if(fNetwork_Session)
    {
        fNetwork_Session->close();
        delete fNetwork_Session;
    }
    close();
    delete fConfig_Manager;
    foreach(QWebSocket* socket, *fConnected_Sockets)
        if(socket)
        {
            socket->close();
            delete socket;
        }
    delete fConnected_Sockets;
}


void TcpCommunicator::writeHttpData(TcpCommunicator::HttpData data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << data.fCommand << data.fClient_ID << data.fType << data.fBody;
    block = qCompress(block);

    /* ping the client socket first and wait for a corresponding
     * pong from the client before sending the payload to the
     * client. This is to avoid sending data to an already disconnected
     * client.
     */
    connect(fConnected_Sockets->at(data.fClient_ID), &QWebSocket::pong, fConnected_Sockets->at(data.fClient_ID),
            [=](const quint64&, const QByteArray&)
    {
        qint64 sent= fConnected_Sockets->at(data.fClient_ID)->sendBinaryMessage(block);
        if((qint64)block.size() != sent)
            qWarning("TcpCommunicator::writeHttpData -> block size: %d _ Number of bytes actually sent: %lld", block.size(), sent);
    });
    fConnected_Sockets->at(data.fClient_ID)->ping();
    return;
}


void TcpCommunicator::openSession()
{
    connect(this, SIGNAL(newConnection()), SLOT(acceptConnection()));
    connect(this, &TcpCommunicator::sslErrors, this, &TcpCommunicator::sslErrorHandler);
    setProxy(QNetworkProxy::NoProxy);
    if(!listen(QHostAddress::Any, 48004))
    {
        qFatal("TcpCommunicator::openSession > Cannot start the WebSocket server: %s", errorString().toStdString().c_str());
        close();
        return;
    }

    connect(this, &TcpCommunicator::acceptError, this, [=](const QAbstractSocket::SocketError&)
    {
        qWarning("QAbstractSocket::SocketError: %s", errorString().toStdString().c_str());
    });

    connect(this, &TcpCommunicator::serverError, this, [=](const QWebSocketProtocol::CloseCode& closecode)
    {
        qWarning("QWebSocketServer::serverError -> CloseCode: %d", closecode);
    });

    connect(this, &TcpCommunicator::originAuthenticationRequired, this, [=](QWebSocketCorsAuthenticator* origin)
    {
        origin->setAllowed(true);
    });
    return;
}


void TcpCommunicator::configUpdated()
{
    QNetworkConfiguration config = fConfig_Manager->defaultConfiguration();
    if(config.isValid())
    {
        fNetwork_Session = new QNetworkSession(config, this);
        connect(fNetwork_Session, SIGNAL(opened()), this, SLOT(openSession()));
        connect(fNetwork_Session, SIGNAL(error(QNetworkSession::SessionError)), this,
                SLOT(networkSessionErrorHandler(QNetworkSession::SessionError)));
        fNetwork_Session->open();
        connect(fNetwork_Session, &QNetworkSession::preferredConfigurationChanged, fNetwork_Session,
                [=](const QNetworkConfiguration&, const bool&){fNetwork_Session->ignore();});
    }
    else
        fConfig_Manager->updateConfigurations();
    return;
}


void TcpCommunicator::acceptConnection()
{
    if(hasPendingConnections())
    {
        QWebSocket* socket = nextPendingConnection();
        connect(socket, &QWebSocket::binaryMessageReceived, socket, [=](const QByteArray& block)
        {
            QByteArray temp_block(qUncompress(block));
            QDataStream in(&temp_block, QIODevice::ReadOnly);
            TcpCommunicator::HttpData data;
            in >> data.fCommand >> data.fClient_ID >> data.fType >> data.fBody;
            emit dataReceived(data);

        });
        connect(socket, &QWebSocket::disconnected, socket, [=]()
        {
            //BUG: can lead to removal of the wrong client ID from the list of verified clients in the MainController
            emit removeVerifiedClient(fConnected_Sockets->indexOf(socket));
            fConnected_Sockets->removeOne(socket);
            socket->deleteLater();
        });
        fConnected_Sockets->push_back(socket);

        /* Allow the server to give each client their respective
         * socket Id's on the server to facilitate multiple client
         * single server relation. This way multiple clients can
         * remain connected to the server without the server terminating
         * their respective connections. This way their will be
         * increase in performance on both ends since the same process
         * of ssl handshake, verification and encryption need be
         * done only once during the first attempt by the client to
         * connect to the server.
         */
        TcpCommunicator::HttpData data;
        data.fCommand = HttpCommand::SETID;
        data.fClient_ID = fConnected_Sockets->size() - 1;   //BUG: can lead to conflicting client IDs. Correct
        data.fType = BodyType::EMPTY;
        writeHttpData(data);

        /* call acceptConnection() recursively incase if their
         * is a new connection that led to the emission of the
         * newConnection signal while this function was busy
         * with the present new connection
         */
        acceptConnection();
    }
    return;
}


void TcpCommunicator::sslErrorHandler(QList<QSslError> error_list)
{
    if(error_list.size())
        foreach(QSslError error, error_list)
            qWarning("TcpCommunicator::sslErrorHandler > %s",error.errorString().toStdString().c_str());
    return;
}


void TcpCommunicator::networkSessionErrorHandler(QNetworkSession::SessionError error)
{
    switch(error)
    {
        case QNetworkSession::RoamingError: {
            qWarning("TcpCommunicator::networkSessionErrorHandler > %s.", fNetwork_Session->errorString().toStdString().c_str());
            break;
        }

        case QNetworkSession::SessionAbortedError: {
            qCritical("TcpCommunicator::networkSessionErrorHandler > %s.", fNetwork_Session->errorString().toStdString().c_str());
            if(isListening())
                close();
            if(fConfig_Manager->capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
            {
                fNetwork_Session->close();
                delete fNetwork_Session;
                fConfig_Manager->updateConfigurations();
            }
            else
                openSession();
            break;
        }


        case QNetworkSession::UnknownSessionError:
        case QNetworkSession::InvalidConfigurationError:
        case QNetworkSession::OperationNotSupportedError: {
            qWarning("TcpCommunicator::networkSessionErrorHandler > %s.", fNetwork_Session->errorString().toStdString().c_str());
            if(isListening())
                close();
            if(fConfig_Manager->capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
            {
                fNetwork_Session->close();
                delete fNetwork_Session;
                fConfig_Manager->updateConfigurations();
            }
            else
                openSession();
            break;
        }
    }

    return;
}
