/*
    This file is part of the Okteta Core library, made within the KDE community.

    Copyright 2003,2007 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef OKTETA_FILEBYTEARRAYMODEL_H
#define OKTETA_FILEBYTEARRAYMODEL_H

// lib
#include <okteta/abstractbytearraymodel.h>


namespace Okteta
{

class FileByteArrayModelPrivate;

/**
  *@author Friedrich W. H. Kossebau
  */

class OKTETACORE_EXPORT FileByteArrayModel : public AbstractByteArrayModel
{
    friend class FileByteArrayModelPrivate;

  Q_OBJECT

  public:
    /** default is only 50*4k = 200k memory image */
    explicit FileByteArrayModel( int pageNumber = 50, int pageSize = 4096, QObject* parent = nullptr );
    ~FileByteArrayModel() override;

  public: // AbstractByteArrayModel API
    Byte byte( Address offset ) const override;
    Size size() const override;
    bool isReadOnly() const override;
    bool isModified() const override;

    Size insert( Address offset, const Byte* insertData, int insertLength ) override;
    Size remove( const AddressRange& removeRange ) override;
    Size replace( const AddressRange& removeRange, const Byte* insertData, int insertLength ) override;
    bool swap( Address firstStart, const AddressRange& secondRange ) override;
    Size fill( Byte fillByte, Address offset = 0, Size fillLength = -1 ) override;
    void setByte( Address offset, Byte byte ) override;

    void setModified( bool modified = true ) override;

  public:
    void setReadOnly( bool readOnly = true ) override;
    bool isOpen() const;
    bool open( const QString &filename );
    bool close();

  protected:
    FileByteArrayModelPrivate* const d;
};

}

#endif
