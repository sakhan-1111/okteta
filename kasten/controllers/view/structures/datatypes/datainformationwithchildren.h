/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2009, 2010, 2011 Alex Richardson <alex.richardson@gmx.de>
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
#ifndef DATAINFORMATIONWITHCHILDREN_H_
#define DATAINFORMATIONWITHCHILDREN_H_

#include "datainformation.h"

class DataInformationWithChildren: public DataInformation
{
protected:
    QVector<DataInformation*> mChildren;
    explicit DataInformationWithChildren(const DataInformationWithChildren& d);
    virtual int indexOf(const DataInformation* const data) const;
public:
    explicit DataInformationWithChildren(const QString& name, const QVector<DataInformation*>& children
            = QVector<DataInformation*>(), DataInformation* parent = 0);
    virtual ~DataInformationWithChildren();

    virtual QVariant childData(int row, int column, int role) const;
    QVector<DataInformation*> children() const;

    virtual BitCount32 size() const;
    virtual BitCount32 childSize(uint index) const;
    virtual bool setData(const QVariant& value, Okteta::AbstractByteArrayModel* out,
            Okteta::Address address, BitCount64 bitsRemaining, quint8 bitOffset);
    virtual bool setChildData(uint row, const QVariant& value, Okteta::AbstractByteArrayModel* out,
            Okteta::Address address, BitCount64 bitsRemaining, quint8 bitOffset);

    /** this is valid for structs and arrays, union has an own implementation */
    virtual qint64 readData(Okteta::AbstractByteArrayModel *input,
            Okteta::Address address, BitCount64 bitsRemaining, quint8* bitOffset);

    virtual DataInformation* childAt(unsigned int index) const;
    virtual bool hasChildren() const;
    virtual bool canHaveChildren() const;
    virtual unsigned int childCount() const;
    BitCount32 offset(unsigned int index) const;
    virtual QString tooltipString() const;

    virtual QWidget* createEditWidget(QWidget* parent) const;
    /** get the needed data from the widget */
    virtual QVariant dataFromWidget(const QWidget* w) const;
    virtual void setWidgetData(QWidget* w) const;
    virtual void resetValidationState();
    virtual void calculateValidationState();

    /** Takes ownership! */
    void appendChild(DataInformation* child);
    /** Takes ownership of all elements */
    void appendChildren(const QVector<DataInformation*>& newChildren);
    void setChildren(const QVector<DataInformation*>& newChildren);
    void setChildren(QScriptValue newChildren);
    /** the same as setChildren, but without emitting size change signals, since otherwise it would crash */
    void setInitialChildren(const QVector<DataInformation*>& children);

    virtual QScriptValue toScriptValue(QScriptEngine* engine, ScriptHandlerInfo* handlerInfo);
};

inline QVector<DataInformation*> DataInformationWithChildren::children() const
{
    return mChildren;
}
inline bool DataInformationWithChildren::hasChildren() const
{
    return childCount() != 0;
}

inline unsigned int DataInformationWithChildren::childCount() const
{
    return mChildren.size();
}

inline bool DataInformationWithChildren::canHaveChildren() const
{
    return true;
}

#endif /* DATAINFORMATIONWITHCHILDREN_H_ */
