/*******************************************************************
 *
 * Copyright 2006-2009 Dmitry Suzdalev <dimsuz@gmail.com>
 *
 * This file is part of the KDE project "KAtomic"
 *
 * KAtomic is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KAtomic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KAtomic; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 ********************************************************************/
#include "chooselevelsetdialog.h"

#include <KStandardDirs>
#include <KDebug>
#include <KNS3/KNewStuffButton>
#include <KGlobal>

#include "levelset.h"
#include "levelsetdelegate.h"

#include "commondefs.h"

ChooseLevelSetDialog::ChooseLevelSetDialog(QWidget* parent)
    : KDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setCaption(i18n("Level Sets"));
    setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);

    QWidget* chooseWidget = new QWidget(this);
    m_ui.setupUi(chooseWidget);

    m_ui.m_lwLevelSets->setItemDelegate(new LevelSetDelegate(this));
    m_ui.m_lwLevelSets->setSortingEnabled(true);

    m_ui.m_pbNewStuff->setConfigFile("katomic.knsrc");

    setMainWidget(chooseWidget);

    resize(550, 350);

    loadData();

    connect(m_ui.m_lwLevelSets, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            SLOT(updateApplyButton()));
    connect(m_ui.m_pbNewStuff, SIGNAL(dialogFinished(KNS3::Entry::List)), SLOT(newStuffDone(KNS3::Entry::List)));
}

void ChooseLevelSetDialog::newStuffDone(const KNS3::Entry::List& entries)
{
    if (!entries.isEmpty())
        loadData();
}

void ChooseLevelSetDialog::loadData()
{
    m_ui.m_lwLevelSets->clear();

    QStringList fileList = KGlobal::dirs()->findAllResources("appdata", "levels/*.dat", KStandardDirs::NoDuplicates);

    LevelSet ls;
    foreach (const QString& fileName, fileList)
    {
        ls.loadFromFile(fileName);
        QString visibleName = ls.visibleName();
        if (!visibleName.isEmpty())
        {
            QListWidgetItem* item = new QListWidgetItem;
            item->setIcon(QIcon::fromTheme( QLatin1String( "katomic" )));
            item->setText(visibleName);
            item->setData(KAtomic::LevelSetNameRole, ls.name());
            item->setData(KAtomic::LevelSetDescriptionRole, ls.description());
            item->setData(KAtomic::LevelSetAuthorRole, ls.author());
            item->setData(KAtomic::LevelSetAuthorEmailRole, ls.authorEmail());
            item->setData(KAtomic::LevelSetLevelCountRole, ls.levelCount());
            m_ui.m_lwLevelSets->addItem(item);
        }
    }

    // reselect current levelset if it is set
    if(!m_gameCurrentLevelSetName.isEmpty())
        setCurrentLevelSet(m_gameCurrentLevelSetName);
}

void ChooseLevelSetDialog::setCurrentLevelSet(const QString& levelSetName)
{
    m_gameCurrentLevelSetName = levelSetName;

    int count = m_ui.m_lwLevelSets->count();
    for (int i=0;i<count;++i)
    {
        QListWidgetItem* item = m_ui.m_lwLevelSets->item(i);
        if (!item)
            continue;

        if (item->data(KAtomic::LevelSetNameRole).toString() == levelSetName)
        {
            m_ui.m_lwLevelSets->setCurrentItem(item);
            break;
        }
    }
}

void ChooseLevelSetDialog::slotButtonClicked(int but)
{
    if (but == KDialog::Ok || but == KDialog::Apply)
    {
        QListWidgetItem* item = m_ui.m_lwLevelSets->currentItem();
        if (item)
        {
            QString levelSetName = item->data(KAtomic::LevelSetNameRole).toString();
            emit levelSetChanged(levelSetName);

            m_gameCurrentLevelSetName = levelSetName;
            updateApplyButton();
        }
    }

    KDialog::slotButtonClicked(but);
}

void ChooseLevelSetDialog::updateApplyButton()
{
    QListWidgetItem* item = m_ui.m_lwLevelSets->currentItem();
    if (item)
    {
        enableButtonApply(item->data(KAtomic::LevelSetNameRole).toString() != m_gameCurrentLevelSetName);
    }
}
