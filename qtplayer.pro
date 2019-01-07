TEMPLATE = subdirs

unix:!android {
    SUBDIRS = src player console test
    player.depends = src
    console.depends = src
    test.depends = src
}

android {
    SUBDIRS = src player
    player.depends = src
}
