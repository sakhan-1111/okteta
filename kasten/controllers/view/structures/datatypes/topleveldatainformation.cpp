/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2010, 2011 Alex Richardson <alex.richardson@gmx.de>
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

#include "topleveldatainformation.h"
#include "datainformation.h"
#include "datainformationwithchildren.h"
#include "../script/scripthandler.h"
#include "../script/scriptlogger.h"
#include "../script/scriptengineinitializer.h"
#include "primitivefactory.h"
#include "../allprimitivetypes.h"

#include <abstractbytearraymodel.h>
//QtScript
#include <QtScript/QScriptEngine>

TopLevelDataInformation::TopLevelDataInformation(DataInformation* data, ScriptLogger* logger,
        QScriptEngine* engine, const QFileInfo& structureFile)
        : QObject(0), mData(data), mLogger(logger), mStructureFile(structureFile),
                mIndex(-1), mValid(!data->isDummy()), mChildDataChanged(false)
{
    Q_CHECK_PTR(mData);
    mData->setParent(this);
    setObjectName(mData->name());

    if (!mLogger)
        mLogger.reset(new ScriptLogger());

    if (engine)
        mScriptHandler.reset(new ScriptHandler(engine, this));
}

TopLevelDataInformation::~TopLevelDataInformation()
{
}

void TopLevelDataInformation::validate()
{
    kDebug() << "validation of structure " << mData->name() << "requested";
    if (mScriptHandler)
        mScriptHandler->validateData(mData.data());
    else
        kDebug() << "cannot validate" << mData->name() << "since it has no validators!";
}

QScriptEngine* TopLevelDataInformation::scriptEngine() const
{
    return mScriptHandler->engine();
}

void TopLevelDataInformation::resetValidationState()
{
    mData->resetValidationState();
}

void TopLevelDataInformation::updateElement(DataInformation* elem)
{
    Q_CHECK_PTR(elem);
    if (mScriptHandler)
        mScriptHandler->updateDataInformation(elem);
}

void TopLevelDataInformation::read(Okteta::AbstractByteArrayModel* input,
        Okteta::Address address, const Okteta::ArrayChangeMetricsList& changesList)
{
    mChildDataChanged = false;
    //first of all check if start offset is locked
    if (mLockedPositions.contains(input))
    {
        //use the saved offset
        address = mLockedPositions.value(input);
        //we read from the locked position, so now check whether it is necessary to update
        bool updateNeccessary = isReadingNecessary(changesList, address);

        if (!updateNeccessary)
            return;
    }
    quint64 remainingBits = (input->size() - address) * 8;
    quint8 bitOffset = 0;
    mData->beginRead(); //before reading set wasAbleToRead to false
    mData->resetValidationState(); //reading new data -> validation state is old
    updateElement(mData.data()); //unlikely that this is useful, but maybe someone needs it
    mData->readData(input, address, remainingBits, &bitOffset);

    if (mChildDataChanged)
    {
        emit dataChanged();
        mChildDataChanged = false;
    }
}

bool TopLevelDataInformation::isReadingNecessary(const Okteta::ArrayChangeMetricsList& changesList,
        Okteta::Address address)
{
    if (changesList.isEmpty())
        return true; //default argument
    for (int i = 0; i < changesList.length(); ++i)
    {
        const Okteta::ArrayChangeMetrics& change = changesList.at(i);
        if (change.type() == Okteta::ArrayChangeMetrics::Replacement)
        {
            Okteta::Address structureSizeInBytes = (mData->size() / 8)
                    + (mData->size() % 8 == 0 ? 0 : 1);

            //handle it for replacements
            //TODO insertions and deletions: move structureStart?
            if (change.lengthChange() == 0)
            {
                //only need to check if anything inside structure was changed
                if (change.offset() > address + structureSizeInBytes)
                {
                    //change is after structure, no need to update anything
                    continue;
                }
                //check if change is before structure
                else if (change.offset() + change.insertLength() < address)
                {
                    //only values before start changed -> reading is unnecessary
                    continue;
                }
                else
                {
                    //change affects this structure
                    return true;
                }
            }
            else if (change.type() == Okteta::ArrayChangeMetrics::Swapping)
            {
                //TODO
                return true;
            }
            else
            {
                kFatal() << "logic error, this should never be reached";
            }
        }
    }
    //    kDebug()
    //        << "it was not necessary to update values";
    return true; //just return true for all other cases
    //TODO implement checks for other cases
}

void TopLevelDataInformation::lockPositionToOffset(Okteta::Address offset,
        const Okteta::AbstractByteArrayModel* model)
{
    mLockedPositions.insert(model, quint64(offset));
    kDebug()
    << mData->name() << ": Locking start offset in model "
            << model << "to position " << offset;
    //remove when deleted
    connect(model, SIGNAL(destroyed(QObject*)),
            SLOT(removeByteArrayModelFromList(QObject*)));
}

void TopLevelDataInformation::unlockPosition(
        const Okteta::AbstractByteArrayModel* model)
{
    kDebug()
    << "removing lock at position " << mLockedPositions.value(model) << ", model ="
            << (void*) model;
    //just remove from map
    mLockedPositions.remove(model);
}

void TopLevelDataInformation::removeByteArrayModelFromList(QObject* obj)
{
    const Okteta::AbstractByteArrayModel* model =
            static_cast<Okteta::AbstractByteArrayModel*>(obj);
    mLockedPositions.remove(model);
}

bool TopLevelDataInformation::isLockedFor(const Okteta::AbstractByteArrayModel* model) const
{
    return mLockedPositions.contains(model);
}

quint64 TopLevelDataInformation::lockPositionFor(const Okteta::AbstractByteArrayModel* model) const
{
    Q_ASSERT(mLockedPositions.contains(model));
    return mLockedPositions.value(model);
}

int TopLevelDataInformation::indexOf(const DataInformation* const data) const
{
    Q_ASSERT(data == mData.data());
    return mIndex;
}
