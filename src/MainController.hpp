#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP

#include "TcpCommunicator.hpp"
#include "Database.hpp"
#include <QList>


class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);
    ~MainController();
    inline Database* database();

signals:
    void sendData(TcpCommunicator::HttpData);

private slots:
    void dataReceived(TcpCommunicator::HttpData);
    inline void removeVerifiedClient(int);

private:
    TcpCommunicator* fTcp_Communicator;
    Database* fDatabase;


    /* To be updated with a clients ID number after
     * verifying the client's username and password
     * from the server's database.
     * This implies that for a client's command to
     * be accepted and processed by the server it
     * will have to identify itself to be fully
     * authorized to do so by sending its username
     * and password to the server for verification.
     * fVerified_Clients is used to store the ID
     * of every verified client that is connected to
     * the server
     */
    QList<int>* fVerified_Clients;
};


Database* MainController::database()
{
    return fDatabase;
}


inline void MainController::removeVerifiedClient(int id)
{
    fVerified_Clients->removeOne(id);
    return;
}

#endif // MAINCONTROLLER_HPP
