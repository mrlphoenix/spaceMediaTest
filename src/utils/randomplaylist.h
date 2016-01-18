#ifndef RANDOMPLAYLIST_H
#define RANDOMPLAYLIST_H

#include <QObject>
#include <QWidget>

class RandomPlaylist : public QObject
{
    Q_OBJECT
public:
    explicit RandomPlaylist(QObject *parent = 0);

signals:

public slots:
};

#endif // RANDOMPLAYLIST_H
