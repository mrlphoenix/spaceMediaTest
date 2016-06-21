#ifndef TELEDSSHEDULER_H
#define TELEDSSHEDULER_H

#include <QObject>
#include <QTimer>

//this class is wrapper for any timed events in the player
//currently it has only one task - get player, but later we can extend it for any new timed event
class TeleDSSheduler : public QObject
{
    Q_OBJECT
public:
    explicit TeleDSSheduler(QObject *parent = 0);
    ~TeleDSSheduler();

    enum Task{GET_PLAYLIST, ALL};

    void start(Task t);
    void stop(Task t);
    void restart(Task t);

signals:
    void getPlaylist();
    void resourceCounter();
public slots:

private:
    QTimer * getPlaylistTimer;
    QTimer * resourceCounterTimer;
};

#endif // TELEDSSHEDULER_H
