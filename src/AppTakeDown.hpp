#ifndef APPTAKEDOWN_HPP
#define APPTAKEDOWN_HPP

#include <QObject>
#include "Database.hpp"

class MainController;

class AppTakeDown : public QObject
{
    Q_OBJECT
public:
    explicit AppTakeDown(MainController*);
    ~AppTakeDown();

signals:

private slots:

private:
    MainController* fDirector;
};

#endif // APPTAKEDOWN_HPP
