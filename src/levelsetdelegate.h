/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: 2006-2009 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef LEVEL_SET_DELEGATE_H
#define LEVEL_SET_DELEGATE_H

#include <QStyledItemDelegate>

class LevelSetDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit LevelSetDelegate(QObject* parent = nullptr);

    void paint(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    mutable int m_lineHeight;
};

#endif
