/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    Copyright 2019 Lars Maier <lars.maier@tefax.net>

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

#ifndef KASTEN_CRC64BYTEARRAYCHECKSUMALGORITHM_HPP
#define KASTEN_CRC64BYTEARRAYCHECKSUMALGORITHM_HPP

//
#include "crc64bytearraychecksumparameterset.hpp"
//
#include <abstractbytearraychecksumalgorithm.hpp>

class Crc64ByteArrayChecksumAlgorithm : public AbstractByteArrayChecksumAlgorithm
{
    Q_OBJECT

public:
    Crc64ByteArrayChecksumAlgorithm();
    ~Crc64ByteArrayChecksumAlgorithm() override;

public: // AbstractByteArrayChecksumAlgorithm API
    bool calculateChecksum(QString* result, const Okteta::AbstractByteArrayModel* model, const Okteta::AddressRange& range) const override;
    AbstractByteArrayChecksumParameterSet* parameterSet() override;

private:
    Crc64ByteArrayChecksumParameterSet mParameterSet;
};

#endif