/*
 *   This file is part of the Okteta Kasten module, part of the KDE project.
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

#include "allprimitivetypes.h"
#include <abstractbytearraymodel.h>

//FIXME this code really needs unit tests!
//TODO optimised methods for *bitOffset == 0 && bitCount % 8 == 0

bool AllPrimitiveTypes::writeBits(const quint8 bitCount,
        const AllPrimitiveTypes newValue, Okteta::AbstractByteArrayModel* out,
        const ByteOrder byteOrder, const Okteta::Address address,
        const quint64 bitsRemaining, quint8* const bitOffset)
{
    Q_ASSERT(*bitOffset < 8);
    Q_ASSERT(bitCount <= 64);
    if (bitsRemaining < bitCount)
    {
        ulongValue = 0;
        *bitOffset = 0;
        return false;
    }
    //set if not
    if (ulongValue != newValue.ulongValue)
        ulongValue = newValue.ulongValue;

    if (byteOrder == ByteOrderEnumClass::LittleEndian)
    {
        writeDataLittleEndian(bitCount, newValue, out, address, *bitOffset);
    }
    else if (byteOrder == ByteOrderEnumClass::BigEndian)
    {
        writeDataBigEndian(bitCount, newValue, out, address, *bitOffset);
    }
    else
    {
        kWarning() << "invalid byte order";
        ulongValue = 0;
        return false;
    }
    *bitOffset = (*bitOffset + bitCount) % 8;
    return true;
}

bool AllPrimitiveTypes::readBits(const quint8 bitCount,
        const Okteta::AbstractByteArrayModel* input, const ByteOrder byteOrder,
        const Okteta::Address address, const quint64 bitsRemaining,
        quint8* const bitOffset)
{
    Q_ASSERT(bitCount <= 64);
    Q_ASSERT(*bitOffset < 8);
    if (bitsRemaining < bitCount)
    {
        ulongValue = 0;
        *bitOffset = 0;
        return false;
    }
    //set to zero before reading
    ulongValue = 0;
    if (byteOrder == ByteOrderEnumClass::LittleEndian)
    {
        readDataLittleEndian(bitCount, input, address, *bitOffset);
    }
    else if (byteOrder == ByteOrderEnumClass::BigEndian)
    {
        readDataBigEndian(bitCount, input, address, *bitOffset);
    }
    else
    {
        kWarning() << "invalid byte order specified.";
        ulongValue = 0;
        return false;
    }
    *bitOffset = (*bitOffset + bitCount) % 8;
    return true;
}

void AllPrimitiveTypes::readDataLittleEndian(const quint8 bitCount,
        const Okteta::AbstractByteArrayModel* input, const Okteta::Address address,
        const quint8 bo)
{
    if (bitCount <= (unsigned) (8 - bo))
    {
        //fits completely
        const quint8 lowerMask = 0xff << bo; //all lower bits are 0
        const quint8 higherMask = 0xff >> (8 - (bo + bitCount)); // all higher bits are 0
        const quint8 completeMask = lowerMask & higherMask; //region in the middle
        const quint8 readByte = input->byte(address);
        const quint8 maskedByte = readByte & completeMask;
        ubyteValue = maskedByte >> bo;
    }
    else
    {
        const quint8 firstByteMask = 0xff << bo;
        const quint8 firstByte = input->byte(address);
        const quint8 firstByteMasked = firstByte & firstByteMask;
        ubyteValue = firstByteMasked >> bo;
        //if spans more than this one byte continue
        for (uint i = 8; i < bitCount + bo; i += 8)
        {
            quint8 readVal = input->byte(address + (i / 8));
            if (bitCount + bo < i + 8)
            {
                //this is last byte needed, possibly cut off top values
                const quint8 missingBits = (bitCount + bo) % 8;
                const quint8 mask = (1 << missingBits) - 1;
                readVal &= mask; //mask the top few bits
            }
            //otherwise we need full byte -> nothing to do
            //needs cast since otherwise compiler decides to use 32 bit int and top 32 bits get lost
            const quint64 shiftedVal = (quint64) readVal << i;
            ulongValue |= shiftedVal >> bo; //move to correct byte
        }
    }
}

void AllPrimitiveTypes::readDataBigEndian(const quint8 bitCount,
        const Okteta::AbstractByteArrayModel* input, const Okteta::Address address,
        const quint8 bo)
{
    if (bitCount <= (unsigned) (8 - bo))
    {
        const quint8 lowerMask = 0xff << (8 - (bo + bitCount));
        const quint8 higherMask = 0xff >> bo;
        const quint8 completeMask = lowerMask & higherMask;
        //completeMask maskes the value -> negate it to clear all the bytes
        const quint8 readByte = input->byte(address);
        const quint8 maskedByte = readByte & completeMask;
        ubyteValue = maskedByte >> (8 - (bo + bitCount));
    }
    else
    {
        const quint8 firstByteMask = 0xff >> bo;
        const quint8 firstByte = input->byte(address);
        //needs quint64 since otherwise compiler decides to use 32 bit int when shifting and top 32 bits get lost
        const quint64 firstByteMasked = firstByte & firstByteMask;
        const quint64 firstByteShifted = firstByteMasked << (bo + bitCount - 8);
        ulongValue = firstByteShifted;
        //if spans more than this one byte continue
        for (uint i = 8; i < bitCount + bo; i += 8)
        {
            quint8 readVal = input->byte(address + (i / 8));
            if (bitCount + bo < i + 8)
            {
                //this is last byte needed, possibly cut off lower values
                const quint8 missingBits = (bo + bitCount) % 8;
                const quint8 mask = 0xff << (8 - missingBits);
                const quint8 maskedVal = readVal & mask; //cut off lower bits
                const quint8 shiftedVal = maskedVal >> (8 - missingBits);
                ulongValue |= shiftedVal;
            }
            else
            {
                //otherwise we need full byte -> nothing to do
                //needs cast since otherwise compiler decides to use 32 bit int and top 32 bits get lost
                const quint64 shiftedVal = (quint64) readVal << ((bo + bitCount)
                        - (8 + i)); //move to correct byte
                ulongValue |= shiftedVal;
            }
        }
    }
}

void AllPrimitiveTypes::writeDataLittleEndian(const quint8 bitCount,
        const AllPrimitiveTypes newValue, Okteta::AbstractByteArrayModel *out,
        const Okteta::Address address, const quint8 bo) const
{
    if (bitCount <= (unsigned) (8 - bo))
    {
        //fits completely
        const quint8 lowerMask = (1 << bo) - 1; //all lower bits are 1
        const quint8 higherMask = 0xff << (bo + bitCount); // all higher bits are 1
        const quint8 completeMask = lowerMask | higherMask; //region in the middle is 0
        const quint8 readByte = out->byte(address);
        const quint8 maskedByte = readByte & completeMask;
        const quint8 addedVal = newValue.ubyteValue << bo;
        const quint8 newVal = maskedByte | addedVal;
        out->setByte(address, newVal);
    }
    else
    {
        const quint8 firstByteMask = (1 << bo) - 1;
        const quint8 firstByte = out->byte(address);
        const quint8 firstByteMasked = firstByte & firstByteMask;
        const quint8 firstAddedVal = (newValue.ubyteValue << bo);
        const quint8 firstByteWithValAdded = firstByteMasked | firstAddedVal;
        out->setByte(address, firstByteWithValAdded);
        //if spans more than this one byte continue
        for (uint i = 8; i < bitCount + bo; i += 8)
        {
            const quint8 currentByte = newValue.ulongValue >> (i - bo);
            if (bitCount + bo < i + 8)
            {
                const quint8 readVal = out->byte(address + (i / 8));
                //this is last byte needed, possibly cut off bottom
                const quint8 missingBits = (bitCount + bo) % 8;
                const quint8 mask = 0xff << missingBits;
                const quint8 readValMasked = readVal & mask; //remove the bottom values
                const quint8 resultingVal = readValMasked | currentByte;
                out->setByte(address + (i / 8), resultingVal);
            }
            else
            {
                //otherwise we need full byte -> nothing to do
                out->setByte(address + (i / 8), currentByte);
            }
        }
    }
}

void AllPrimitiveTypes::writeDataBigEndian(const quint8 bitCount,
        const AllPrimitiveTypes newValue, Okteta::AbstractByteArrayModel *out,
        const Okteta::Address address, const quint8 bo) const
{
    if (bitCount <= (unsigned) (8 - bo))
    {
        //fits completely
        const quint8 lowerMask = 0xff >> (bo + bitCount); //all lower bits are 1
        const quint8 higherMask = 0xff << (8 - bo); // all higher bits are 1
        const quint8 completeMask = lowerMask | higherMask; //region in the middle is 0
        const quint8 readByte = out->byte(address);
        const quint8 maskedByte = readByte & completeMask;
        const quint8 addedVal = newValue.ubyteValue << (8 - bo - 1); //move to missing area
        const quint8 maskedByteWithValueAdded = maskedByte | addedVal;
        out->setByte(address, maskedByteWithValueAdded);
    }
    else
    {
        quint8 missingBits = (bitCount + bo) % 8;
        missingBits = (missingBits == 0 ? 8 : missingBits);
        const quint8 lastAddress = address + ((bo + bitCount) / 8) - (missingBits
                > 0 ? 0 : 1);
        const quint8 lastByte = out->byte(lastAddress);
        const quint8 lastByteMask = (1 << missingBits) - 1;
        const quint8 lastByteMasked = lastByte & lastByteMask; //remove the top values
        const quint8 lastByteAddedVal = newValue.ubyteValue << (8 - missingBits);
        const quint8 lastByteWithValAdded = lastByteMasked | lastByteAddedVal;
        out->setByte(lastAddress, lastByteWithValAdded);
        for (int currAddress = lastAddress - 1; currAddress >= address; currAddress--)
        {
            const quint8 currentByte = out->byte(currAddress);
            if (currAddress == address)
            {
                //last byte to read
                const quint8 firstByteMask = 0xff << (8 - bo);
                const quint8 firstByteMasked = currentByte & firstByteMask;
                const quint8 highestByte = newValue.ulongValue >> (bo + bitCount - 8);
                const quint8 firstByteWithValAdded = firstByteMasked | highestByte;
                out->setByte(address, firstByteWithValAdded);
            }
            else
            {
                const int bytesNotToShift = 1 + (lastAddress - address)
                        - (lastAddress - currAddress);
                const quint8 thisByteShifted = newValue.ulongValue >> (bo + bitCount - (8
                        * bytesNotToShift));
                out->setByte(currAddress, thisByteShifted);
            }
        }
    }
}