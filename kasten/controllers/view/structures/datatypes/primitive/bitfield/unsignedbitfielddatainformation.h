/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2010 Alex Richardson <alex.richardson@gmx.de>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef UNSIGNEDBITFIELDDATAINFORMATION_H_
#define UNSIGNEDBITFIELDDATAINFORMATION_H_

#include "abstractbitfielddatainformation.h"

#include <KLocalizedString>

class UnsignedBitfieldDataInformation : public AbstractBitfieldDataInformation
{
    DATAINFORMATION_CLONE(UnsignedBitfieldDataInformation, AbstractBitfieldDataInformation) {}
public:
    UnsignedBitfieldDataInformation(const QString& name, BitCount32 width, DataInformation* parent = nullptr)
            : AbstractBitfieldDataInformation(name, width, parent) {}
    ~UnsignedBitfieldDataInformation() override {}

    QScriptValue valueAsQScriptValue() const override;
    QWidget* createEditWidget(QWidget* parent) const override;
    QVariant dataFromWidget(const QWidget* w) const override;
    void setWidgetData(QWidget* w) const override;
    Type bitfieldType() const override;
private:
    QString valueStringImpl() const override;
    QString typeNameImpl() const override;
};

inline QString UnsignedBitfieldDataInformation::typeNameImpl() const
{
    return i18ncp("Data type", "unsigned bitfield (%1 bit wide)",
            "unsigned bitfield (%1 bits wide)", width());
}

inline AbstractBitfieldDataInformation::Type UnsignedBitfieldDataInformation::bitfieldType() const
{
    return AbstractBitfieldDataInformation::Unsigned;
}

#endif /* UNSIGNEDBITFIELDDATAINFORMATION_H_ */
