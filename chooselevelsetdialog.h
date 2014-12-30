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

#ifndef CHOOSE_LEVEL_SET_DIALOG_H
#define CHOOSE_LEVEL_SET_DIALOG_H

#include <QDialog>

#include "ui_levelsetwidget.h"
class QDialogButtonBox;
class ChooseLevelSetDialog : public QDialog
{
    Q_OBJECT

public:
    ChooseLevelSetDialog(QWidget* parent=0);

    void setCurrentLevelSet(const QString& levelSetName);
    void loadData();

signals:
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
