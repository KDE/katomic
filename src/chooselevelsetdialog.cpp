/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: 2006-2009 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "chooselevelsetdialog.h"


#include <KNS3/Button>
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

    m_ui.m_pbNewStuff->setConfigFile(QStringLiteral("katomic.knsrc"));

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
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("levels"), QStandardPaths::LocateDirectory);
    for (const QString& dir : dirs) {
        const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.dat"));
        for (const QString& file : fileNames) {
            fileList.append(dir + QLatin1Char('/') + file);
        }
    }

    LevelSet ls;
    for (const QString& fileName : fileList)
    {
        ls.loadFromFile(fileName);
        QString visibleName = ls.visibleName();
        if (!visibleName.isEmpty())
        {
            QListWidgetItem* item = new QListWidgetItem;
            item->setIcon(QIcon::fromTheme( QStringLiteral( "katomic" )));
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
        Q_EMIT levelSetChanged(levelSetName);

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
