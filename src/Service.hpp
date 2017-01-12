#ifndef SERVICE_HPP
#define SERVICE_HPP

#include "QtSolutions/QtService"

class MainController;

class Service : public QtService<QCoreApplication>
{
public:
    Service(int, char**);
    ~Service();

protected:
    void start();
    void stop();

    /* No implementation for the inherited functions
     * pause() and resume() since our server is not
     * allowed to be suspended thereby rendering any
     * code written for the functions to be completely
     * useless.
     */
    void pause(){}
    void resume(){}

    /* No implementation for the inherited function
     * processCommand(int) since we do not plan to
     * control our server from another process that
     * makes use of the QtServiceController module.
     * This is because our server gets all of its
     * commands from a TCP connection with a client
     */
    void processCommand(int){}

private:
    MainController* fMain_Controller;

};

#endif // SERVICE_HPP
