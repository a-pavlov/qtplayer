#include "application.h"
#include "bittorrent/session.h"

Application::Application(int &argc, char **argv)
    : QCoreApplication (argc, argv) {}

Application::~Application() {
    BitTorrent::Session::freeInstance();
}

int Application::exec() {
    BitTorrent::Session::initInstance();
    BitTorrent::Session::instance()->addTorrent(":/res/Big_Buck_Bunny_1080p_surround_frostclick.com_frostwire.com.torrent");
    return QCoreApplication::exec();
}
