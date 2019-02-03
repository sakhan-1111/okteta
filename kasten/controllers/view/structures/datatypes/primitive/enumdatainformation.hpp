/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2009, 2010, 2011, 2012 Alex Richardson <alex.richardson@gmx.de>
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

#ifndef ENUMDATAINFORMATION_H_
#define ENUMDATAINFORMATION_H_

#include "primitivedatainformation.hpp"
#include "enumdefinition.hpp"

class EnumDataInformation : public PrimitiveDataInformationWrapper
{
    DATAINFORMATION_CLONE(EnumDataInformation, PrimitiveDataInformationWrapper), mEnum(d.mEnum) {}

public:
    /** this object takes ownership of @p type */
    EnumDataInformation(const QString& name, PrimitiveDataInformation* type,
                        const EnumDefinition::Ptr& enumDef, DataInformation* parent = nullptr);
    ~EnumDataInformation() override;

    bool isEnum() const override;

    EnumDefinition::Ptr enumValues() const;
    void setEnumValues(const QMap<AllPrimitiveTypes, QString>& newValues);

private:
    QScriptClass* scriptClass(ScriptHandlerInfo* handlerInfo) const override;
    QString valueStringImpl() const override;
    QString typeNameImpl() const override;

protected:
    EnumDefinition::Ptr mEnum;
};

inline EnumDefinition::Ptr EnumDataInformation::enumValues() const
{
    return mEnum;
}

inline void EnumDataInformation::setEnumValues(const QMap<AllPrimitiveTypes, QString>& newValues)
{
    mEnum->setValues(newValues);
}
#endif /* ENUMDATAINFORMATION_H_ */