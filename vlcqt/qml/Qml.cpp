/****************************************************************************
* VLC-Qt - Qt and libvlc connector library
* Copyright (C) 2016 Tadej Novak <tadej@tano.si>
*
* This library is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include <QtQuick/QQuickItem>

#include "core/Enums.h"
#include "core/TrackModel.h"
#include "qml/Qml.h"
#include "qml/QmlPlayer.h"
#include "qml/QmlSource.h"
#include "qml/QmlVideoOutput.h"
#include "qml/QmlVideoPlayer.h"

void VlcQml::registerTypes() {
    qmlRegisterUncreatableType<Vlc>("VLCQt", 1, 1, "Vlc", QStringLiteral("Vlc cannot be instantiated directly"));
    qmlRegisterUncreatableType<VlcQmlSource>("VLCQt", 1, 1, "VlcSource", QStringLiteral("VlcQmlSource cannot be instantiated directly"));
    qmlRegisterUncreatableType<VlcTrackModel>("VLCQt", 1, 1, "VlcTrackModel", QStringLiteral("VlcTrackModel cannot be instantiated directly"));
    qmlRegisterType<VlcQmlPlayer>("VLCQt", 1, 1, "VlcPlayer");
    qmlRegisterType<VlcQmlVideoOutput>("VLCQt", 1, 1, "VlcVideoOutput");
}
