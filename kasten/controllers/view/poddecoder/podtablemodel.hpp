/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    Copyright 2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef PODTABLEMODEL_HPP
#define PODTABLEMODEL_HPP

// Qt
#include <QAbstractTableModel>

namespace Kasten {

class PODDecoderTool;

class PODTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColumnIds
    {
        NameId = 0,
        ValueId = 1,
        // UsedBytes = x,  TODO: add hint how many bytes a datatype uses
        NoOfColumnIds = 2 // TODO: what pattern is usually used to mark number of ids?
    };

public:
    explicit PODTableModel(PODDecoderTool* tool, QObject* parent = nullptr);
    ~PODTableModel() override;

public: // QAbstractItemModel API
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex buddy(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

private Q_SLOTS:
    void onDataChanged();

private:
    PODDecoderTool* mTool;
    const QString mEmptyNote;
};

}

#endif