/***************************************************************************
                          kbytearraydocumenttest.cpp  -  description
                            -------------------
    begin                : Wed Mai 23 2007
    copyright            : 2007 by Friedrich W. H. Kossebau
    email                : kossebau@kde.org
***************************************************************************/
/*
    Part creating temporary data file is
    Copyright 2005,2006 David Jarvie <software@astrojar.org.uk>
*/
/***************************************************************************
*                                                                         *
*   This library is free software; you can redistribute it and/or         *
*   modify it under the terms of the GNU Library General Public           *
*   License version 2 as published by the Free Software Foundation.       *
*                                                                         *
***************************************************************************/


#include "kbytearraydocumenttest.h"

// test object
#include <kbytearraydocument.h>
// Okteta core
#include <kbytearraymodel.h>
// Qt
#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QByteArray>


static const char Title[] = "title";


void KByteArrayDocumentTest::testCreateNew()
{
    KByteArrayDocument *document = new KByteArrayDocument();

    QVERIFY( document != 0 );
    QVERIFY( !document->hasLocalChanges() );

    KHECore::KByteArrayModel *byteArray = qobject_cast<KHECore::KByteArrayModel*>( document->content() );
    QVERIFY( byteArray != 0 );
    QCOMPARE( byteArray->size(), 0 );
    QVERIFY( !byteArray->isModified() );

    delete document;
}

void KByteArrayDocumentTest::testSetTitle()
{
    KByteArrayDocument *document = new KByteArrayDocument();
    QSignalSpy *titleChangeSpy =  new QSignalSpy( document, SIGNAL(titleChanged( const QString& )) );

    const QLatin1String title( Title );
    document->setTitle( title );
    QCOMPARE( document->title(), title );
    QVERIFY( titleChangeSpy->isValid() );
    QCOMPARE( titleChangeSpy->count(), 1 );
    const QList<QVariant> arguments = titleChangeSpy->takeFirst();
    QCOMPARE( arguments.at(0).toString(), title );

    delete document;
    delete titleChangeSpy;
}


QTEST_MAIN( KByteArrayDocumentTest )
