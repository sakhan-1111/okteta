/*
    This file is part of the Okteta Kakao module, part of the KDE project.

    Copyright 2007-2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "stringsextracttool.h"

// lib
#include "extractstringsjob.h"
#include <kbytearraydocument.h>
// Kakao gui
#include <kabstractview.h>
// Okteta gui
#include <kbytearrayview.h>
// Okteta core
#include <kcharcodec.h>
#include <kbytearraymodel.h>
// KDE
#include <KLocale>
// Qt
#include <QtGui/QApplication>


static const int DefaultMinLength = 3;

StringsExtractTool::StringsExtractTool()
 : mExtractedStringsUptodate( false ), mSourceByteArrayModelUptodate( false ),
   mCharCodec( KHECore::KCharCodec::createCodec(KHECore::LocalEncoding) ), mMinLength( DefaultMinLength ),
   mByteArrayView( 0 ), mByteArrayModel( 0 ), mSourceByteArrayModel( 0 ), mSourceMinLength( 0 )
{
    setObjectName( "Strings" );
}

bool StringsExtractTool::isApplyable() const
{
    return ( mByteArrayModel != 0 && mByteArrayView->hasSelectedData() && mMinLength > 0 );
}

bool StringsExtractTool::canHighlightString() const
{
    return ( mSourceByteArrayModel == mByteArrayModel
             && mByteArrayView && mSourceByteArrayModelUptodate );
}

QString StringsExtractTool::title() const { return i18nc("@title:window of the tool to extract strings", "Strings"); }

// TODO: add model with offset and string
// doubleclick moves cursor to offset
// filter für Suche, inkl. Regulärausdrücke
// groß/kleinschreibung
// voll strings, auch mit Leerzeichen
void StringsExtractTool::setTargetModel( AbstractModel* model )
{
    if( mByteArrayView ) mByteArrayView->disconnect( this );

    KAbstractView* view = model ? qobject_cast<KAbstractView*>( model ) : 0;
    mByteArrayView = view ? qobject_cast<KHEUI::KByteArrayView *>( view->widget() ) : 0;

    KByteArrayDocument* document = view ? qobject_cast<KByteArrayDocument*>( view->baseModel() ) : 0;
    mByteArrayModel = document ? document->content() : 0;

    if( mByteArrayView )
    {
        connect( mByteArrayView,  SIGNAL(charCodecChanged( const QString & )),
                 SLOT(setCharCodec( const QString &)) );
        connect( mByteArrayView,  SIGNAL(selectionChanged( bool )),
                 SLOT(onSelectionChanged( bool )) );

        setCharCodec( mByteArrayView->encodingName() );
    }

    // TODO: if there is no view, there is nothing to extract.
    // or this could be the view where we got the strings from and it did not change
    checkUptoDate();
    emit uptodateChanged( mExtractedStringsUptodate );
    emit isApplyableChanged( isApplyable() );
    emit canHighlightStringChanged( canHighlightString() );
}

void StringsExtractTool::setMinLength( int minLength )
{
    mMinLength = minLength;
    checkUptoDate();
    emit uptodateChanged( mExtractedStringsUptodate );
}

void StringsExtractTool::setCharCodec( const QString &codecName )
{
    if( codecName == mCharCodec->name() )
        return;

    delete mCharCodec;
    mCharCodec = KHECore::KCharCodec::createCodec( codecName );
}

void StringsExtractTool::checkUptoDate()
{
    mExtractedStringsUptodate =
        ( mSourceByteArrayModel == mByteArrayModel
          && mByteArrayView && mSourceSelection == mByteArrayView->selection()
          && mSourceMinLength == mMinLength
          && mSourceByteArrayModelUptodate );
}

void StringsExtractTool::selectString( int stringId )
{
    const ContainedString &containedString = mContainedStringList.at(stringId);
    const int offset = containedString.offset();
    const int length = containedString.string().length();
    mByteArrayView->setSelection( offset, offset+length-1 );
    mByteArrayView->setFocus();
}

void StringsExtractTool::onSelectionChanged( bool hasSelection )
{
// TODO: could be quicker
Q_UNUSED( hasSelection )
    checkUptoDate();
    emit uptodateChanged( mExtractedStringsUptodate );
    emit isApplyableChanged( isApplyable() );
}

void StringsExtractTool::onSourceChanged()
{
    mExtractedStringsUptodate = false;
    mSourceByteArrayModelUptodate = false;
    emit uptodateChanged( false );
    emit canHighlightStringChanged( false );
}


// TODO: use KWordBufferService
void StringsExtractTool::extractStrings()
{
    // forget old string source
    if( mSourceByteArrayModel ) mSourceByteArrayModel->disconnect( this );

    QApplication::setOverrideCursor( Qt::WaitCursor );

    ExtractStringsJob *extractStringsJob =
        new ExtractStringsJob( mByteArrayModel, mByteArrayView->selection(), mCharCodec, mMinLength,
                               &mContainedStringList );
    extractStringsJob->exec();

    QApplication::restoreOverrideCursor();

    // remember new string source
    mSourceByteArrayModel = mByteArrayModel;
    mSourceSelection = mByteArrayView->selection();
    mSourceMinLength = mMinLength;
    connect( mSourceByteArrayModel,  SIGNAL(contentsChanged( const KHE::ArrayChangeMetricsList & )),
             SLOT(onSourceChanged()) );

    mExtractedStringsUptodate = true;
    mSourceByteArrayModelUptodate = true;
    emit uptodateChanged( true );
    emit canHighlightStringChanged( true );
}


StringsExtractTool::~StringsExtractTool()
{
    delete mCharCodec;
}

#include "stringsextracttool.moc"
