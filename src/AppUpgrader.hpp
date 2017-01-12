#ifndef APPUPGRADER_HPP
#define APPUPGRADER_HPP

#include <QObject>
#include "Database.hpp"

class MainController;

class AppUpgrader : public QObject
{
    Q_OBJECT
public:
    explicit AppUpgrader(MainController*);
    ~AppUpgrader();

signals:

private slots:

private:
    MainController* fDirector;
};

#endif // APPUPGRADER_HPP
