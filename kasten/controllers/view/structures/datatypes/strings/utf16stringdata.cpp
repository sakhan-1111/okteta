/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2011 Alex Richardson <alex.richardson@gmx.de>
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



#include "utf16stringdata.h"

#include <QVarLengthArray>
#include <abstractbytearraymodel.h>

#include "../topleveldatainformation.h"
#include "../dummydatainformation.h"
#include "stringdatainformation.h"

Utf16StringData::Utf16StringData(StringDataInformation* parent)
    : StringData(parent), mNonBMPCount(0), mLittleEndian(true)
{
}

Utf16StringData::~Utf16StringData()
{
}

QString Utf16StringData::charType() const
{
    return mLittleEndian ? i18n("UTF16-LE char") : i18n("UTF16-BE char");
}

QString Utf16StringData::typeName() const
{
    return mLittleEndian ? i18n("UTF16-LE string") : i18n("UTF16-BE string");
}

int Utf16StringData::count() const
{
    if (mEofReached)
        return mCodePoints.size() + 1;
    return mCodePoints.size();
}

QString Utf16StringData::stringValue(int row) const
{
    //TODO details 
    Q_ASSERT(row >= 0 && row < count());
    if (mEofReached && row == mCodePoints.size())
        return i18n("End of file reached prematurely");
    uint val = mCodePoints.at(row);
    if (val > UNICODE_MAX)
        return QString(QChar::ReplacementCharacter);
    else if (val > BMP_MAX) {
        QString ret(2, Qt::Uninitialized);
        ret[0] = QChar::lowSurrogate(val);
        ret[1] = QChar::highSurrogate(val);
        return ret;
    }
    else
        return QString(QChar(mCodePoints.at(row)));
}

QString Utf16StringData::completeString(bool skipInvalid) const
{
    QVarLengthArray<QChar> data(mCodePoints.size() + mNonBMPCount);
    int codePointCount = mCodePoints.size();
    int i = 0;
    for (int idx = 0; idx < codePointCount; ++idx) {
        uint val = mCodePoints.at(idx);
        if (val > UNICODE_MAX)
        {
            if (skipInvalid)
                continue;
            else
                data[i] = QChar::ReplacementCharacter;
        }
        else if (val > BMP_MAX) {
            data[i] = QChar::lowSurrogate(val);
            i++;
            data[i] = QChar::highSurrogate(val);
        }
        else
        {
            data[i] = QChar((ushort)val);
        }
        i++;
    }
    return QString(data.constData(), i);
}

qint64 Utf16StringData::read(Okteta::AbstractByteArrayModel* input, Okteta::Address address,
            quint64 bitsRemaining)
{
    const int oldSize = count();
    bool haveToEmit = true;
    if (mMode == CharCount)
    {
        mCodePoints.reserve(mLength.maxChars);
    }
    else if (mMode == ByteCount)
    {
        mCodePoints.reserve(mLength.maxBytes / 2);
    }

    if (oldSize != 0)
    {
        emit mParent->topLevelDataInformation()->
            _childrenAboutToBeRemoved(mParent, 0, oldSize);
        emit mParent->topLevelDataInformation()->
            _childrenRemoved(mParent, 0, oldSize);
    }

    const int oldMax = mCodePoints.size();
    quint64 remaining = bitsRemaining;
    Okteta::Address addr = address;
    int count = 0;
    mEofReached = false;
    if (((mMode & CharCount) && mLength.maxChars == 0)
            || ((mMode & ByteCount) && mLength.maxBytes < 2))
        return 0;

    bool eofAtStart = false;
    if (bitsRemaining < 16)
        eofAtStart = true;

    while (true)
    {
        if (remaining < 16)
        {
            mEofReached = true;
            break;
        }
        uint codePoint;
        ushort val;
        bool terminate = false;

        if (mLittleEndian)
            val = input->byte(addr) | (input->byte(addr + 1) << 8);
        else
            val = (input->byte(addr) << 8) | input->byte(addr + 1);
        //high surrogate -> if is followed by low surrogate we have a 4 bit char
        if (QChar::isHighSurrogate(val))
        {
            if (remaining < 32 || ((mMode & ByteCount) && (addr + 2 - address) / 2 >= mLength.maxBytes / 2))
            {
                codePoint = val;
                mEofReached = true;
                terminate = true;
            }
            else
            {
                ushort val2;
                if (mLittleEndian)
                    val2 = input->byte(addr + 2) | (input->byte(addr + 3) << 8);
                else
                    val2 = (input->byte(addr + 2) << 8) | input->byte(addr + 3);

                if (QChar::isLowSurrogate(val2))
                {
                    codePoint = QChar::surrogateToUcs4(val, val2);
                    remaining -= 16;
                    addr += 2;
                    mNonBMPCount++; // codepoint > 0xffff -> non BMP
                }
                else
                    codePoint = val;
            }
        }
        else
        {
            codePoint = val;
        }

        if (count < oldMax)
            mCodePoints[count] = val;
        else
            mCodePoints.append(val);

        remaining -= 16;
        addr += 2;
        count++;

        //now check if we have to terminate
        if (mMode & Sequence)
        {
            if (val == mTerminationCodePoint)
                terminate = true;
        }
        if (mMode & ByteCount)
        {
            // divide by two in case someone set length to an odd number of bytes
            if ((addr - address) / 2 >= mLength.maxBytes / 2)
                terminate = true;
        }
        if (mMode & CharCount)
        {
            if (count >= mLength.maxChars)
                terminate = true;
        }
        if (mMode == None) {
            kDebug() << "no termination mode set!!";
            Q_ASSERT(false);
        }
        if (terminate)
            break;
    }
    mCodePoints.resize(count);
    emit mParent->topLevelDataInformation()->
        _childrenAboutToBeInserted(mParent, 0, qMax(mEofReached ? count : count - 1, 0));
    emit mParent->topLevelDataInformation()->
        _childrenInserted(mParent, 0, qMax(mEofReached ? count : count - 1, 0));

    if (eofAtStart)
        return -1;
    return (addr - address) * 8;
}

unsigned int Utf16StringData::size() const
{
    //add 16 for every non BMP char, since they use 32 bits
    return (mCodePoints.size() + mNonBMPCount) * 16;
}

quint64 Utf16StringData::sizeAt(int i) const
{
    Q_ASSERT(i >= 0 && i <= count());
    if (mEofReached && i == mCodePoints.size() )
        return 0;
    uint val = mCodePoints.at(i);
    return val > 0xffff ? 32 : 16;
}