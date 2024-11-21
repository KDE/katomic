/*
    SPDX-FileCopyrightText: 1998 Andreas Wüst <AndreasWuest@gmx.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "toplevel.h"

#include "levelset.h"
#include "katomic_version.h"

#include <KLocalizedString>
#include <KAboutData>
#include <KCrash>
#include <KMessageBox>
#include <KDBusService>
#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif
#include <QTimer>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QApplication>

// ##########################
// #	Main                #
// ##########################

int main(int argc, char **argv)
{
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif
    QApplication app(argc, argv);
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("katomic"));

    KAboutData aboutData( QStringLiteral("katomic"), i18n("KAtomic"),
            QStringLiteral(KATOMIC_VERSION_STRING),
            i18n("Atomic Entertainment Game"),
            KAboutLicense::GPL,
            i18n("(c) 1998, Andreas Wuest\n(c) 2007-2009 Dmitry Suzdalev"));
    aboutData.addAuthor(i18n("Andreas Wuest"), i18n("Original author"), QStringLiteral("AndreasWuest@gmx.de"));
    aboutData.addAuthor(i18n("Dmitry Suzdalev"), i18n("Porting to KDE4. Current maintainer"), QStringLiteral("dimsuz@gmail.com"));
    aboutData.addAuthor(i18n("Stephan Kulow"), QString(), QStringLiteral("coolo@kde.org"));
    aboutData.addAuthor(i18n("Cristian Tibirna"), QString(), QStringLiteral("tibirna@kde.org"));
    aboutData.addCredit(i18n("Carsten Pfeiffer"), QString(), QStringLiteral("pfeiffer@kde.org"));
    aboutData.addCredit(i18n("Dave Corrie"), QString(), QStringLiteral("kde@davecorrie.com"));
    aboutData.addCredit(i18n("Kai Jung"), i18n("6 new levels"), QStringLiteral("jung@fh-fresenius.de"));
    aboutData.addCredit(i18n("Danny Allen"), i18n("Game graphics and application icon"), QStringLiteral("danny@dannyallen.co.uk"));
    aboutData.addCredit(i18n("Johann Ollivier Lapeyre"), i18n("New great SVG artwork for KDE4"), QStringLiteral("johann.ollivierlapeyre@gmail.com"));
    aboutData.addCredit(i18n("Brian Croom"), i18n("Port to use KGameRenderer"), QStringLiteral("brian.s.croom@gmail.com"));
    aboutData.setHomepage(QStringLiteral("https://apps.kde.org/katomic"));

    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("katomic")));

    KCrash::initialize();

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption({QStringLiteral("hackmode")}, i18n( "Enable access to all levels" )));
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service;

    if (!LevelSet::isDefaultLevelsAvailable())
    {
        KMessageBox::error(nullptr, i18n("KAtomic failed to find its default level set and will quit. Please check your installation."));
        QTimer::singleShot(0, &app, &QApplication::quit);
    }
    else
    {
        if ( app.isSessionRestored() )
            kRestoreMainWindows<AtomTopLevel>();
        else {
            AtomTopLevel *top = new AtomTopLevel;
            if ( parser.isSet( QStringLiteral("hackmode") ) )
                top->enableHackMode();

            top->show();
        }
    }

    return app.exec();
}

