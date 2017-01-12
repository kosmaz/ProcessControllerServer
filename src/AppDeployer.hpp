#ifndef APPDEPLOYER_HPP
#define APPDEPLOYER_HPP

#include <QObject>
#include "Database.hpp"

class MainController;

class AppDeployer : public QObject
{
    Q_OBJECT
public:
    explicit AppDeployer(MainController*);
    ~AppDeployer();

signals:

private slots:

private:
    MainController* fDirector;
};

#endif // APPDEPLOYER_HPP
