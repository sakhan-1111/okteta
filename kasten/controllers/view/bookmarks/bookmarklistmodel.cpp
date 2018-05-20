/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009,2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "bookmarklistmodel.h"

// lib
#include "bookmarkstool.h"
// Okteta core
#include <okteta/bookmark.h>
// KF5
#include <KLocalizedString>

namespace Kasten {

BookmarkListModel::BookmarkListModel(BookmarksTool* tool, QObject* parent)
    : QAbstractTableModel(parent)
    , mTool(tool)
{
    mPrintFunction = Okteta::OffsetFormat::printFunction((Okteta::OffsetFormat::Format)tool->offsetCoding());

    connect(mTool, &BookmarksTool::hasBookmarksChanged,
            this, &BookmarkListModel::onHasBookmarksChanged);
    connect(mTool, &BookmarksTool::bookmarksAdded,
            this, QOverload<>::of(&BookmarkListModel::onBookmarksChanged));
    connect(mTool, &BookmarksTool::bookmarksRemoved,
            this, QOverload<>::of(&BookmarkListModel::onBookmarksChanged));
    connect(mTool, &BookmarksTool::bookmarksModified,
            this, QOverload<const QList<int>&>::of(&BookmarkListModel::onBookmarksChanged));
    connect(mTool, &BookmarksTool::offsetCodingChanged,
            this, &BookmarkListModel::onOffsetCodingChanged);
}

BookmarkListModel::~BookmarkListModel() = default;

int BookmarkListModel::rowCount(const QModelIndex& parent) const
{
    return (!parent.isValid()) ? mTool->bookmarksCount() : 0;
}

int BookmarkListModel::columnCount(const QModelIndex& parent) const
{
    return (!parent.isValid()) ? NoOfColumnIds : 0;
}

QVariant BookmarkListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    switch (role)
    {
    case Qt::DisplayRole:
    {
        const int bookmarkIndex = index.row();

        const Okteta::Bookmark& bookmark = mTool->bookmarkAt(bookmarkIndex);

        const int tableColumn = index.column();
        switch (tableColumn)
        {
        case OffsetColumnId:
        {
            mPrintFunction(mCodedOffset, bookmark.offset());

            result = QString::fromLatin1(mCodedOffset);
            break;
        }
        case TitleColumnId:
            result = bookmark.name();
            break;
        default:
            ;
        }
        break;
    }
    case Qt::EditRole:
    {
        const int bookmarkIndex = index.row();

        const int column = index.column();
        if (column == TitleColumnId) {
            const Okteta::Bookmark& bookmark = mTool->bookmarkAt(bookmarkIndex);
            result = bookmark.name();
        }
        break;
    }
    default:
        break;
    }

    return result;
}

Qt::ItemFlags BookmarkListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags result = QAbstractTableModel::flags(index);
    const int column = index.column();
    if (column == TitleColumnId) {
        result |= Qt::ItemIsEditable;
    }

    return result;
}

QVariant BookmarkListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if (role == Qt::DisplayRole) {
        const QString titel =
            section == OffsetColumnId ?  i18nc("@title:column offset of the bookmark",     "Offset") :
            section == TitleColumnId ?   i18nc("@title:column title of the bookmark", "Title") :
            QString();
        result = titel;
    } else {
        result = QAbstractTableModel::headerData(section, orientation, role);
    }

    return result;
}

bool BookmarkListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool result;

    if (role == Qt::EditRole) {
        const int bookmarkIndex = index.row();

        const int column = index.column();
        if (column == TitleColumnId) {
            mTool->setBookmarkName(bookmarkIndex, value.toString());
//             emit dataChanged( index, index );
            result = true;
        } else {
            result = false;
        }
    } else {
        result = QAbstractItemModel::setData(index, value, role);
    }

    return result;
}

const Okteta::Bookmark& BookmarkListModel::bookmark(const QModelIndex& index) const
{
    const int bookmarkIndex = index.row();
    return mTool->bookmarkAt(bookmarkIndex);
}

QModelIndex BookmarkListModel::index(const Okteta::Bookmark& bookmark, int column) const
{
    QModelIndex result;

    const int indexOfBookmark = mTool->indexOf(bookmark);
    if (indexOfBookmark != -1) {
        result = createIndex(indexOfBookmark, column);
    }

    return result;
}

void BookmarkListModel::onHasBookmarksChanged(bool hasBookmarks)
{
    Q_UNUSED(hasBookmarks)

    beginResetModel();
    endResetModel();
}

void BookmarkListModel::onBookmarksChanged()
{
    beginResetModel();
    endResetModel();
}

void BookmarkListModel::onBookmarksChanged(const QList<int>& bookmarkIndizes)
{
    for (int row : bookmarkIndizes) {
        emit dataChanged(index(row, OffsetColumnId), index(row, TitleColumnId));
    }
}

void BookmarkListModel::onOffsetCodingChanged(int offsetCoding)
{
    mPrintFunction = Okteta::OffsetFormat::printFunction((Okteta::OffsetFormat::Format)offsetCoding);
    beginResetModel();
    endResetModel();
}

#if 0
void BookmarkListModel::onRevertedToVersionIndex(int versionIndex)
{
    if (mVersionIndex == versionIndex) {
        return;
    }

    const int oldVersionIndex = mVersionIndex;
    mVersionIndex = versionIndex;

    emit dataChanged(index(versionIndex, CurrentColumnId), index(versionIndex, CurrentColumnId));
    emit dataChanged(index(oldVersionIndex, CurrentColumnId), index(oldVersionIndex, CurrentColumnId));
}

void BookmarkListModel::onHeadVersionDataChanged(const DocumentVersionData& versionData)
{
    Q_UNUSED(versionData)
    const int headVersionIndex = mVersionControl->versionCount() - 1;
    emit dataChanged(index(headVersionIndex, CurrentColumnId), index(headVersionIndex, ChangeDescriptionColumnId));
}
#endif

}
