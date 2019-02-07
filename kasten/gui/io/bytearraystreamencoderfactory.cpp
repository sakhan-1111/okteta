/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    Copyright 2009-2010 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "bytearraystreamencoderfactory.hpp"

// lib
#include "streamencoder/chars/bytearraycharsstreamencoder.hpp"
#include "streamencoder/values/bytearrayvaluesstreamencoder.hpp"
#include "streamencoder/sourcecode/bytearraysourcecodestreamencoder.hpp"
#include "streamencoder/viewtext/bytearrayviewtextstreamencoder.hpp"
#include "streamencoder/base64/bytearraybase64streamencoder.hpp"
#include "streamencoder/base32/bytearraybase32streamencoder.hpp"
#include "streamencoder/base85/bytearraybase85streamencoder.hpp"
#include "streamencoder/uuencoding/bytearrayuuencodingstreamencoder.hpp"
#include "streamencoder/xxencoding/bytearrayxxencodingstreamencoder.hpp"
#include "streamencoder/srec/bytearraysrecstreamencoder.hpp"
#include "streamencoder/ihex/bytearrayihexstreamencoder.hpp"
//// NEWBYTEARRAYSTREAMENCODER(start)
//// Here add the name of your header file of your streamencoder,
//// e.g.
//// #include "my_bytearraystreamencoder.hpp"
//// NEWBYTEARRAYSTREAMENCODER(end)
// Qt
#include <QVector>

namespace Kasten {

QVector<AbstractModelStreamEncoder*> ByteArrayStreamEncoderFactory::createStreamEncoders()
{
    const QVector<AbstractModelStreamEncoder*> result {
        new ByteArrayValuesStreamEncoder(),
        new ByteArrayCharsStreamEncoder(),
        new ByteArrayBase64StreamEncoder(),
        new ByteArrayBase32StreamEncoder(),
        new ByteArrayBase85StreamEncoder(),
        new ByteArrayUuencodingStreamEncoder(),
        new ByteArrayXxencodingStreamEncoder(),
        new ByteArrayIHexStreamEncoder(),
        new ByteArraySRecStreamEncoder(),
        new ByteArraySourceCodeStreamEncoder(),
        new ByteArrayViewTextStreamEncoder(),
//// NEWBYTEARRAYSTREAMENCODER(start)
//// Here add the creation of an object of your streamencoder class and add it to the list,
//// e.g.
////         new My_ByteArrayStreamEncoder(),
//// NEWBYTEARRAYSTREAMENCODER(end)
    };
    return result;
}

}
