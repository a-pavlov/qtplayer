#include "application.h"
#include "bittorrent/session.h"
#include "bittorrent/torrentinfo.h"

Application::Application(int &argc, char **argv)
    : QCoreApplication (argc, argv) {}

Application::~Application() {
    BitTorrent::Session::freeInstance();
}

int Application::exec() {
    BitTorrent::Session::initInstance();
    //BitTorrent::Session::instance()->addTorrent(":/res/Big_Buck_Bunny_1080p_surround_frostclick.com_frostwire.com.torrent");
    BitTorrent::Session::instance()->addTorrent2(BitTorrent::TorrentInfo::loadFromFile(":/res/ubuntu-18.10-desktop-amd64.iso.torrent"));
    //BitTorrent::Session::instance()->addTorrent(":/res/ubuntu-18.10-desktop-amd64.iso.torrent");
    return QCoreApplication::exec();
}
