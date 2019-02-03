/*
    This file is part of the Okteta Core library, made within the KDE community.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef KPIECETABLE_TESTPIECETABLECHANGE_HPP
#define KPIECETABLE_TESTPIECETABLECHANGE_HPP

// lib
#include <piecetable/abstractpiecetablechange.hpp>
#include <piecetable/piece.hpp>
// Qt
#include <QString>

namespace KPieceTable {

class TestPieceTableChange : public AbstractPieceTableChange
{
public:
    TestPieceTableChange(int typeId = -1, const QString& description = QString(),
                         int position = 0, int storagePosition = 0, int storageId = Piece::ChangeStorage,
                         int replacedStoragePosition = 0, int replacedStorageId = Piece::ChangeStorage);

    ~TestPieceTableChange() override;

public: // AbstractPieceTableChange API
    int type() const override;
    QString description() const override;
    bool merge(const AbstractPieceTableChange* other) override;
    AddressRange apply(PieceTable* pieceTable) const override;
    AddressRange revert(PieceTable* pieceTable) const override;
    ArrayChangeMetrics metrics() const override;
    int dataSize() const override;

protected:
    int mTypeId;
    QString mDescription;
    int mPosition;
    int mStoragePosition;
    int mStorageId;

    int mReplacedStoragePosition;
    int mReplacedStorageId;
};

inline TestPieceTableChange::TestPieceTableChange(int typeId, const QString& description,
                                                  int position, int storagePosition, int storageId,
                                                  int replacedStoragePosition, int replacedStorageId)
    : mTypeId(typeId)
    , mDescription(description)
    , mPosition(position)
    , mStoragePosition(storagePosition)
    , mStorageId(storageId)
    , mReplacedStoragePosition(replacedStoragePosition)
    , mReplacedStorageId(replacedStorageId)
{}

}

#endif