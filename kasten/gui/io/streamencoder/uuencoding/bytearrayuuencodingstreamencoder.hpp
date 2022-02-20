/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    SPDX-FileCopyrightText: 2010 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KASTEN_BYTEARRAYUUENCODINGSTREAMENCODER_HPP
#define KASTEN_BYTEARRAYUUENCODINGSTREAMENCODER_HPP

// lib
#include "abstractbytearraystreamencoder.hpp"
// Okteta core
#include <Okteta/OktetaCore>
// Qt
#include <QString>

class KConfigGroup;

namespace Kasten {

class UuencodingStreamEncoderSettings
{
public:
    enum class EncodingType
    {
        Historical = 0,
        Base64 = 1
    };

public:
    UuencodingStreamEncoderSettings();

    bool operator==(const UuencodingStreamEncoderSettings& other) const;

public:
    void loadConfig(const KConfigGroup& configGroup);
    void saveConfig(KConfigGroup& configGroup) const;

public:
    QString fileName;
    EncodingType encodingType = EncodingType::Base64;
};

class ByteArrayUuencodingStreamEncoder : public AbstractByteArrayStreamEncoder
{
    Q_OBJECT

public:
    enum class InputByteIndex
    {
        First = 0,
        Second,
        Third
    };

public:
    ByteArrayUuencodingStreamEncoder();
    ~ByteArrayUuencodingStreamEncoder() override;

public:
    UuencodingStreamEncoderSettings settings() const;
    void setSettings(const UuencodingStreamEncoderSettings& settings);

protected: // AbstractByteArrayStreamEncoder API
    bool encodeDataToStream(QIODevice* device,
                            const ByteArrayView* byteArrayView,
                            const Okteta::AbstractByteArrayModel* byteArrayModel,
                            const Okteta::AddressRange& range) override;

private:
    UuencodingStreamEncoderSettings mSettings;
};

inline UuencodingStreamEncoderSettings ByteArrayUuencodingStreamEncoder::settings() const { return mSettings; }

}

#endif
