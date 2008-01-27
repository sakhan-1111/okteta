/***************************************************************************
                          kbytearrayrawfilesynchronizer.h  -  description
                             -------------------
    begin                : Mon Nov 12 2007
    copyright            : 2007 by Friedrich W. H. Kossebau
    email                : kossebau@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License           *
 *   version 2 as published by the Free Software Foundation.               *
 *                                                                         *
 ***************************************************************************/


#include "kbytearrayrawfilesynchronizer.h"

// lib
#include "kbytearraydocument.h"
// Okteta core
#include <kbytearraymodel.h>
// KDE
#include <KUrl>
// Qt
#include <QtCore/QDataStream>
#include <QtCore/QFile>



KByteArrayRawFileSynchronizer::KByteArrayRawFileSynchronizer( const KUrl &url )
{
    KAbstractDocument *document = loadFromUrl( url );
    mDocument = document ? qobject_cast<KByteArrayDocument*>( document ) : 0;
    if( mDocument )
        onUrlChange( url );
    connect( this, SIGNAL(urlChanged(const KUrl&)), SLOT(onUrlChange( const KUrl & )) );
}

KByteArrayRawFileSynchronizer::KByteArrayRawFileSynchronizer( KAbstractDocument *document, const KUrl &url,
                                                              KAbstractDocumentSynchronizer::ConnectOption option )
{
    // TODO: is synchronizer->document() really a good signal for success? see also above
    mDocument = document ? qobject_cast<KByteArrayDocument*>( document ) : 0;
    if( mDocument )
    {
        if( !syncWithUrl(url,option) )
            mDocument = 0;
    }
    if( mDocument )
        onUrlChange( url );
    connect( this, SIGNAL(urlChanged(const KUrl&)), SLOT(onUrlChange( const KUrl & )) );
}

KAbstractDocument *KByteArrayRawFileSynchronizer::document() const { return mDocument; }

KAbstractDocument *KByteArrayRawFileSynchronizer::loadFromFile( const QString &workFilePath/*, int *success*/ )
{
    KByteArrayDocument *document = 0;

    QFile file( workFilePath );
    file.open( QIODevice::ReadOnly );
    QDataStream inStream( &file );
    int fileSize = file.size();

    // TODO: should the decoder know this?
    // it is in the api now (constructor)
    KHECore::KByteArrayModel *byteArray = new KHECore::KByteArrayModel( fileSize );

    inStream.readRawData( byteArray->data(), fileSize );

    byteArray->setModified( false );

    //registerDiskModifyTime( file ); TODO move into synchronizer

    const bool streamIsOk = ( inStream.status() == QDataStream::Ok );
//     if( success )
//         *success = streamIsOk ? 0 : 1;
    if( streamIsOk )
        document = new KByteArrayDocument( byteArray );
    else
        delete byteArray;

    return document;
}

bool KByteArrayRawFileSynchronizer::reloadFromFile( const QString &workFilePath )
{
    QFile file( workFilePath );
    file.open( QIODevice::ReadOnly );
    QDataStream inStream( &file );
    int fileSize = file.size();

    // TODO: should the decoder know this?
    char *newData = new char[fileSize];
    inStream.readRawData( newData, fileSize );

    //registerDiskModifyTime( file ); TODO move into synchronizer

    const bool streamIsOk = ( inStream.status() == QDataStream::Ok );
//     if( success )
//         *success = streamIsOk ? 0 : 1;
    if( streamIsOk )
    {
        KHECore::KByteArrayModel *byteArray = qobject_cast<KHECore::KByteArrayModel*>( mDocument->content() );
        byteArray->setData( newData, fileSize, fileSize, false );
    }
    else
        delete [] newData;

    return streamIsOk;
}

bool KByteArrayRawFileSynchronizer::writeToFile( const QString &workFilePath )
{
    KHECore::KByteArrayModel *byteArray = qobject_cast<KHECore::KByteArrayModel*>( mDocument->content() );

    QFile file( workFilePath );
    file.open( QIODevice::WriteOnly );

    QDataStream outStream( &file );
    outStream.writeRawData( byteArray->data(), byteArray->size() );

    byteArray->setModified( false );

    //registerDiskModifyTime( file );TODO move into synchronizer

    return outStream.status() == QDataStream::Ok;
}

bool KByteArrayRawFileSynchronizer::syncWithFile( const QString &workFilePath,
                                                  KAbstractDocumentSynchronizer::ConnectOption option )
{
Q_UNUSED( option );
    return writeToFile( workFilePath );
}

void KByteArrayRawFileSynchronizer::onUrlChange( const KUrl &url )
{
    mDocument->setTitle( url.fileName() );
}
