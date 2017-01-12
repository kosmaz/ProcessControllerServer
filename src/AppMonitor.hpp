#ifndef APPMONITOR_HPP
#define APPMONITOR_HPP

#include <QObject>

class AppMonitor : public QObject
{
    Q_OBJECT
public:
    explicit AppMonitor(QObject *parent = 0);

signals:

public slots:
};

#endif // APPMONITOR_HPP