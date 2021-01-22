/*
    This file is part of the KDE project "KAtomic"

    SPDX-FileCopyrightText: 2006-2009 Dmitry Suzdalev <dimsuz@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "levelsetdelegate.h"

#include "commondefs.h"

#include <KLocalizedString>
#include <QFontDatabase>

#include <QPainter>
#include <QApplication>

LevelSetDelegate::LevelSetDelegate(QObject* parent)
    : QStyledItemDelegate(parent), m_lineHeight(-1)
{
}

void LevelSetDelegate::paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& index) const
{
    p->save();
    QStyleOptionViewItem option(opt);
    initStyleOption(&option, index);

    //
    // Draw item with no textwith a decoration and selection (if it's selected)
    //

    // we want to paint text by ourselves, so set it to null, paint selection and then paint text
    QString text = index.data(Qt::DisplayRole).toString();
    option.text = QString();
    option.decorationSize = QSize(48, 48);

    QStyle* style = QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &option, p, nullptr);

    if (option.state & QStyle::State_Selected)
        p->setPen(option.palette.color(QPalette::Normal, QPalette::HighlightedText));
    else
        p->setPen(option.palette.color(QPalette::Normal, QPalette::Text));

    //
    // Draw text
    //
    int marginH = style->pixelMetric( QStyle::PM_FocusFrameHMargin );
    int marginV = style->pixelMetric( QStyle::PM_FocusFrameVMargin );
    int innerSpacing = 9;

    int textStartX = option.decorationSize.width() + innerSpacing;
    QRect r = opt.rect.adjusted(textStartX, marginV*2, 0, 0);

    QFontMetrics fm(opt.font);

    int flags = Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine;
    QFont font = p->font();
    font.setBold(true);
    p->setFont(font);
    p->drawText(r, flags, text);

    //
    // Draw Author name
    //
    QString authorName = index.data(KAtomic::LevelSetAuthorRole).toString();
    if (!authorName.isEmpty())
    {
        if (option.state & QStyle::State_Selected)
            p->setPen(option.palette.color(QPalette::Disabled, QPalette::HighlightedText));
        else
            p->setPen(option.palette.color(QPalette::Disabled, QPalette::Text));

        r = r.adjusted(innerSpacing, fm.lineSpacing(), -marginH*2, 0);
        flags = Qt::AlignLeft | Qt::AlignTop;

        p->setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));

        QString text = i18n("by %1", authorName);
        QString authorEmail = index.data(KAtomic::LevelSetAuthorEmailRole).toString();
        if (!authorEmail.isEmpty())
            text.append(QStringLiteral(" <%1>").arg(authorEmail));

        int numLevels = index.data(KAtomic::LevelSetLevelCountRole).toUInt();
        text.append(i18np(", contains 1 level", ", contains %1 levels", numLevels));

        p->drawText(r, flags, text);
    }

    //
    // Draw description
    //
    QString descr = index.data(KAtomic::LevelSetDescriptionRole).toString();
    if (!descr.isEmpty())
    {
        if (option.state & QStyle::State_Selected)
            p->setPen(option.palette.color(QPalette::Normal, QPalette::HighlightedText));
        else
            p->setPen(option.palette.color(QPalette::Normal, QPalette::Text));

        r = opt.rect.adjusted(textStartX, fm.lineSpacing()*2, -marginH*2, -marginV*2);
        flags = Qt::AlignLeft | Qt::AlignBottom | Qt::TextSingleLine;
        p->setFont(opt.font);
        QString elided = fm.elidedText(descr, Qt::ElideMiddle, r.width(), flags);
        p->drawText(r, flags, descr);
    }

    p->restore();
}

QSize LevelSetDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)

    if (m_lineHeight == -1)
    {
        QFontMetrics fm(option.font);
        m_lineHeight = fm.lineSpacing();
    }

    return QSize(option.rect.width(), qMax(m_lineHeight*3, 64));
}
