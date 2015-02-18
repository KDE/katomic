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


#include <QDebug>
#include <KNS3/KNewStuffButton>
#include <KGlobal>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDir>

#include "levelset.h"
#include "levelsetdelegate.h"

#include "commondefs.h"

ChooseLevelSetDialog::ChooseLevelSetDialog(QWidget* parent)
    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle(i18n("Level Sets"));
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::Apply);
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(mainWidget);
    QPushButton *okButton = m_buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(m_buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &ChooseLevelSetDialog::slotOkClicked);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ChooseLevelSetDialog::slotApplyClicked);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &ChooseLevelSetDialog::reject);

    QWidget* chooseWidget = new QWidget(this);
    m_ui.setupUi(chooseWidget);

    m_ui.m_lwLevelSets->setItemDelegate(new LevelSetDelegate(this));
    m_ui.m_lwLevelSets->setSortingEnabled(true);

    m_ui.m_pbNewStuff->setConfigFile("katomic.knsrc");

    mainLayout->addWidget(chooseWidget);
    mainLayout->addWidget(m_buttonBox);


    resize(550, 350);

    loadData();

    connect(m_ui.m_lwLevelSets, &QListWidget::currentItemChanged, this, &ChooseLevelSetDialog::updateApplyButton);
    connect(m_ui.m_pbNewStuff, &KNS3::Button::dialogFinished, this, &ChooseLevelSetDialog::newStuffDone);
}

void ChooseLevelSetDialog::newStuffDone(const KNS3::Entry::List& entries)
{
    if (!entries.isEmpty())
        loadData();
}

void ChooseLevelSetDialog::loadData()
{
    m_ui.m_lwLevelSets->clear();
    QStringList fileList;
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::DataLocation, "levels", QStandardPaths::LocateDirectory);
    Q_FOREACH (const QString& dir, dirs) {
        const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.dat"));
        Q_FOREACH (const QString& file, fileNames) {
            fileList.append(dir + '/' + file);
        }
    }

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

void ChooseLevelSetDialog::saveSettings()
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

void ChooseLevelSetDialog::slotApplyClicked()
{
    saveSettings();
}

void ChooseLevelSetDialog::slotOkClicked()
{
    saveSettings();
    accept();
}

void ChooseLevelSetDialog::updateApplyButton()
{
    QListWidgetItem* item = m_ui.m_lwLevelSets->currentItem();
    if (item)
    {
        m_buttonBox->button(QDialogButtonBox::Apply)->setEnabled(item->data(KAtomic::LevelSetNameRole).toString() != m_gameCurrentLevelSetName);
    }
}
