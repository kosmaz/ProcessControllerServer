#include "MainController.hpp"


MainController::MainController(QObject *parent) :
    QObject(parent),
    fTcp_Communicator(new TcpCommunicator(this)),
    fDatabase(new Database),
    fVerified_Clients(new QList<int>)
{
    connect(fTcp_Communicator, &TcpCommunicator::dataReceived, this, &MainController::dataReceived);
    connect(fTcp_Communicator, &TcpCommunicator::removeVerifiedClient, this, &MainController::removeVerifiedClient);
}


MainController::~MainController()
{
    delete fTcp_Communicator;
    delete fDatabase;
    delete fVerified_Clients;
}


void MainController::dataReceived(TcpCommunicator::HttpData)
{
    return;
}
