#ifndef APPCONFIGURER_HPP
#define APPCONFIGURER_HPP

#include <QObject>
#include "Database.hpp"

class MainController;

class AppConfigurer : public QObject
{
    Q_OBJECT
public:
    explicit AppConfigurer(MainController*);
    ~AppConfigurer();

signals:

private slots:

private:
    MainController* fDirector;
};

#endif // APPCONFIGURER_HPP
