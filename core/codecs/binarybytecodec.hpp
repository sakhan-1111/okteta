/*
    This file is part of the Okteta Core library, made within the KDE community.

    Copyright 2004,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef OKTETA_BINARYBYTECODEC_HPP
#define OKTETA_BINARYBYTECODEC_HPP

// lib
#include <valuecodec.hpp>

namespace Okteta {

class BinaryByteCodec : public ValueCodec
{
public: // ValueCodec API
    unsigned int encodingWidth() const override;
    Byte digitsFilledLimit() const override;

    void encode(QString* digits, unsigned int pos, Byte byte) const override;
    void encodeShort(QString* digits, unsigned int pos, Byte byte) const override;
    bool appendDigit(Byte* byte, unsigned char digit) const override;
    void removeLastDigit(Byte* byte) const override;
    bool isValidDigit(unsigned char digit) const override;
    bool turnToValue(unsigned char* digit) const override;
};

}

#endif
