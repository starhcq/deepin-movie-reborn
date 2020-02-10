#include "presenter.h"

Presenter::Presenter(MainWindow *mw, QObject *parent)
    : QObject(parent), _mw(mw)
{
    MprisPlayer *mprisPlayer =  new MprisPlayer();
    mprisPlayer->setServiceName("Deepinmovie");

    //mprisPlayer->setSupportedMimeTypes();
    mprisPlayer->setSupportedUriSchemes(QStringList() << "file");
    mprisPlayer->setCanQuit(true);
    mprisPlayer->setCanRaise(true);
    mprisPlayer->setCanSetFullscreen(false);
    mprisPlayer->setHasTrackList(false);
    // setDesktopEntry: see https://specifications.freedesktop.org/mpris-spec/latest/Media_Player.html#Property:DesktopEntry for more
    mprisPlayer->setDesktopEntry("deepin-movie");
    mprisPlayer->setIdentity("Deepin Movie Player");

    mprisPlayer->setCanControl(true);
    mprisPlayer->setCanPlay(true);
    mprisPlayer->setCanGoNext(true);
    mprisPlayer->setCanGoPrevious(true);
    mprisPlayer->setCanPause(true);
    initMpris(mprisPlayer);
}

void Presenter::initMpris(MprisPlayer *mprisPlayer)
{
    if (!mprisPlayer) {
        return ;
    }
    m_mprisplayer = mprisPlayer;
    connect(_mw->engine(), &PlayerEngine::stateChanged, this, &Presenter::slotstateChanged);
    connect(mprisPlayer, &MprisPlayer::playRequested, this, &Presenter::slotpause);
    connect(mprisPlayer, &MprisPlayer::pauseRequested, this, &Presenter::slotpause);
    connect(mprisPlayer, &MprisPlayer::nextRequested, this, &Presenter::slotplaynext);
    connect(mprisPlayer, &MprisPlayer::previousRequested, this, &Presenter::slotplayprev);
    connect(mprisPlayer, &MprisPlayer::volumeRequested, this, &Presenter::slotvolumeRequested);
    connect(mprisPlayer, &MprisPlayer::openUriRequested, this, &Presenter::slotopenUriRequested);
    connect(mprisPlayer, &MprisPlayer::openUriRequested, this, [ = ] {_mw->requestAction(ActionFactory::Exit);});

//    connect(_mw->toolbox()->get_progBar(), &Presenter::progrossChanged,
//    this, [ = ](qint64 pos, qint64) {
//        mprisPlayer->setPosition(pos);
//    });

}

void Presenter::slotpause()
{
    _mw->requestAction(ActionFactory::TogglePause);
}

void Presenter::slotplaynext()
{
    _mw->requestAction(ActionFactory::GotoPlaylistNext);
}

void Presenter::slotplayprev()
{
    _mw->requestAction(ActionFactory::GotoPlaylistPrev);
}

void Presenter::slotvolumeRequested(double volume)
{
    if (_mw->engine()->muted()) {
        _mw->engine()->toggleMute();
    }

    _mw->engine()->changeVolume(volume);
    Settings::get().setInternalOption("global_volume", qMin(_mw->engine()->volume(), 140));
    double pert = _mw->engine()->volume();
    if (pert > VOLUME_OFFSET) {
        pert -= VOLUME_OFFSET;
    }
    _mw->get_nwComm()->updateWithMessage(tr("Volume: %1%").arg(pert));
}

void Presenter::slotopenUriRequested(const QUrl url)
{
    _mw->play(url);
}

void Presenter::slotstateChanged()
{
    switch (_mw->engine()->state()) {
    case PlayerEngine::CoreState::Idle:
        m_mprisplayer->setPlaybackStatus(Mpris::Stopped);
        break;
    case PlayerEngine::CoreState::Playing:
        m_mprisplayer->setPlaybackStatus(Mpris::Playing);
        break;
    case PlayerEngine::CoreState::Paused:
        m_mprisplayer->setPlaybackStatus(Mpris::Paused);
        break;
    }
}