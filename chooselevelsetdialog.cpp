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

#include "levelset.h"
#include "levelsetdelegate.h"

ChooseLevelSetDialog::ChooseLevelSetDialog(QWidget* parent)
    : KDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setCaption(i18n("Choose a level pack"));
    setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel);

    QWidget* chooseWidget = new QWidget(this);
    m_ui.setupUi(chooseWidget);

    m_ui.m_lwLevelSets->setItemDelegate(new LevelSetDelegate(this));

    setMainWidget(chooseWidget);

    loadData();
}

void ChooseLevelSetDialog::loadData()
{
    QStringList fileList = KGlobal::dirs()->findAllResources("appdata", "levels/*.dat", KStandardDirs::NoDuplicates);

    LevelSet ls;
    foreach (const QString& fileName, fileList)
    {
        ls.loadFromFile(fileName);
        QString visibleName = ls.visibleName();
        if (!visibleName.isEmpty())
        {
            QListWidgetItem* item = new QListWidgetItem;
            item->setText(visibleName);
            item->setData(Qt::UserRole, ls.name());
            m_ui.m_lwLevelSets->addItem(item);
        }
    }
}

void ChooseLevelSetDialog::setCurrentLevelSet(const QString& levelSetName)
{
    int count = m_ui.m_lwLevelSets->count();
    for (int i=0;i<count;++i)
    {
        QListWidgetItem* item = m_ui.m_lwLevelSets->item(i);
        if (!item)
            continue;

        if (item->data(Qt::UserRole).toString() == levelSetName)
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
            QString levelSetName = item->data(Qt::UserRole).toString();
            emit levelSetChanged(levelSetName);
        }
    }

    KDialog::slotButtonClicked(but);
}
