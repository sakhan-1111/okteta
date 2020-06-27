/*
    This file is part of the Okteta Kasten Framework, made within the KDE community.

    SPDX-FileCopyrightText: 2010 Alex Richardson <alex.richardson@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KASTEN_SIGNEDBITFIELDDATAINFORMATION_HPP
#define KASTEN_SIGNEDBITFIELDDATAINFORMATION_HPP

#include "abstractbitfielddatainformation.hpp"

class SignedBitfieldDataInformation : public AbstractBitfieldDataInformation
{
    DATAINFORMATION_CLONE(SignedBitfieldDataInformation, AbstractBitfieldDataInformation) {}

public:
    SignedBitfieldDataInformation(const QString& name, BitCount32 width, DataInformation* parent = nullptr)
        : AbstractBitfieldDataInformation(name, width, parent)
    {}
    ~SignedBitfieldDataInformation() override = default;
    AllPrimitiveTypes fromVariant(const QVariant& variant, bool* ok) const override;

    QScriptValue valueAsQScriptValue() const override;
    QWidget* createEditWidget(QWidget* parent) const override;
    QVariant dataFromWidget(const QWidget* w) const override;
    void setWidgetData(QWidget* w) const override;
    /** sign extend the value if it is negative, so it can always be treated as a plain qint64 */
    void setValue(AllPrimitiveTypes newVal) override;
    Type bitfieldType() const override;

private:
    QString valueStringImpl() const override;
    QString typeNameImpl() const override;
};

#endif /* KASTEN_SIGNEDBITFIELDDATAINFORMATION_HPP */
