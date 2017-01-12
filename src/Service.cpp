#include "Service.hpp"
#include "MainController.hpp"

Service::Service(int argc, char ** argv) :
    QtService<QCoreApplication>(argc, argv, "WebappController daemon"),
    fMain_Controller(nullptr)
{
    setServiceDescription("An Autonomous controller for managing all deployed and running webapps developed using Wt API");
    setStartupType(QtServiceController::AutoStartup);
    setServiceFlags(QtServiceBase::Default);
}


Service::~Service(){}


void Service::start()
{
    if(fMain_Controller == nullptr)
        fMain_Controller = new MainController;
    qDebug("Server started");
    return;
}


void Service::stop()
{
    if(fMain_Controller != nullptr)
        delete fMain_Controller;
    qDebug("Server stopped");
    return;
}
