/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: 2006-2009 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CHOOSE_LEVEL_SET_DIALOG_H
#define CHOOSE_LEVEL_SET_DIALOG_H

#include "ui_levelsetwidget.h"

#include <QDialog>

class QDialogButtonBox;

class ChooseLevelSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseLevelSetDialog(QWidget* parent=nullptr);

    void setCurrentLevelSet(const QString& levelSetName);
    void loadData();

Q_SIGNALS:
    void levelSetChanged(QString);

protected Q_SLOTS:
    void newStuffDone(const KNS3::Entry::List& entries);

private Q_SLOTS:
    void updateApplyButton();
    void slotApplyClicked();
    void slotOkClicked();

private:
    void saveSettings();
    Ui::LevelSetWidget m_ui;
    QString m_gameCurrentLevelSetName;
    QDialogButtonBox *m_buttonBox;
};

#endif
