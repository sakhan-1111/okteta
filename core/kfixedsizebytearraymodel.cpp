/***************************************************************************
                          kfixedsizebytearraymodel.cpp  -  description
                             -------------------
    begin                : Mit Jun 03 2003
    copyright            : (C) 2003 by Friedrich W. H. Kossebau
    email                : kossebau@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License version 2 as published by the Free Software Foundation.       *
 *                                                                         *
 ***************************************************************************/


#include "kfixedsizebytearraymodel.h"

// C
#include <string.h>


namespace KHECore
{

KFixedSizeByteArrayModel::KFixedSizeByteArrayModel( char *D, unsigned int S, char FUC )
  : Data( D ),
   Size( S ),
   FillUpChar( FUC ),
   ReadOnly( true ),
   Modified( false ),
   AutoDelete( false )
{
}

KFixedSizeByteArrayModel::KFixedSizeByteArrayModel( unsigned int S, char FUC )
  : Data( new char[S] ),
   Size( S ),
   FillUpChar( FUC ),
   ReadOnly( false ),
   Modified( false ),
   AutoDelete( true )
{
  reset( 0, S );
}

KFixedSizeByteArrayModel::~KFixedSizeByteArrayModel()
{
  if( AutoDelete )
    delete [] Data;
}



void KFixedSizeByteArrayModel::setDatum( unsigned int Offset, const char Char )
{
  Data[Offset] = Char;
  Modified = true;

  emit contentsReplaced( Offset, 1, 1 );
  emit contentsChanged( Offset, Offset );
  emit modificationChanged( true );
}



int KFixedSizeByteArrayModel::insert( int Pos, const char* D, int InputLength )
{
  // check all parameters
  if( Pos >= (int)Size || InputLength == 0 )
    return 0;
  if( Pos + InputLength > (int)Size )
    InputLength = Size - Pos;

  unsigned int BehindInsertPos = Pos + InputLength;
  // fmove right data behind the input range
  memmove( &Data[BehindInsertPos], &Data[Pos], Size-BehindInsertPos );
  // insert input
  memcpy( &Data[Pos], D, InputLength );

  Modified = true;

  emit contentsReplaced( Pos, 0, InputLength );
  //emit contentsReplaced( Pos, , 0 ); TODO: how to signal the removed data?
  emit contentsChanged( Pos, Size-1 );
  emit modificationChanged( true );
  return InputLength;
}


int KFixedSizeByteArrayModel::remove( const KSection &R )
{
  KSection Remove( R );
  if( Remove.start() >= (int)Size || Remove.width() == 0 )
    return 0;

  Remove.restrictEndTo( Size-1 );

  int RemoveLength = Remove.width();
  int BehindRemovePos = Remove.end()+1;;
  // fmove right data behind the input range
  memmove( &Data[Remove.start()], &Data[BehindRemovePos], Size-BehindRemovePos );
  // clear freed space
  reset( Size-RemoveLength, RemoveLength );

  Modified = true;

  emit contentsReplaced( Remove.start(), Remove.width(), 0 );
  //emit contentsReplaced( Pos, 0,  ); TODO: how to signal the inserted data?
  emit contentsChanged( Remove.start(), Size-1 );
  emit modificationChanged( true );
  return RemoveLength;
}


unsigned int KFixedSizeByteArrayModel::replace( const KSection &R, const char* D, unsigned int InputLength )
{
  KSection Remove( R );
  // check all parameters
  if( Remove.startsBehind( Size-1 ) || (Remove.width()==0 && InputLength==0) )
    return 0;

  Remove.restrictEndTo( Size-1 );
  if( Remove.start() + InputLength > Size )
    InputLength = Size - Remove.start();

  int SizeDiff = InputLength - Remove.width();

  // is input longer than removed?
  if( SizeDiff > 0 )
  {
    unsigned int BehindInsertPos = Remove.start() + InputLength;
    // fmove right data behind the input range
    memmove( &Data[BehindInsertPos], &Data[Remove.end()+1], Size-BehindInsertPos );
  }
  // is input smaller than removed?
  else if( SizeDiff < 0 )
  {
    unsigned int BehindRemovePos = Remove.end()+1;
    // fmove right data behind the input range
    memmove( &Data[Remove.start()+InputLength], &Data[BehindRemovePos], Size-BehindRemovePos );
   // clear freed space
    reset( Size+SizeDiff, -SizeDiff );
  }
  // insert input
  memcpy( &Data[Remove.start()], D, InputLength );

  Modified = true;

  emit contentsReplaced( Remove.start(), Remove.width(), InputLength );
  //emit contentsReplaced( Pos, 0,  ); TODO: how to signal the changed data at the end?
  emit contentsChanged( Remove.start(), SizeDiff==0?Remove.end():Size-1 );
  emit modificationChanged( true );
  return InputLength;
}


int KFixedSizeByteArrayModel::move( int DestPos, const KSection &S )
{
  KSection SourceSection( S );
  // check all parameters
  if( SourceSection.start() >= (int)Size || SourceSection.width() == 0
      || DestPos > (int)Size || SourceSection.start() == DestPos )
    return SourceSection.start();

  SourceSection.restrictEndTo( Size-1 );
  bool ToRight = DestPos > SourceSection.start();
  int MovedLength = SourceSection.width();
  int DisplacedLength = ToRight ?  DestPos - SourceSection.end()-1 : SourceSection.start() - DestPos;

  // find out section that is smaller
  int SmallPartLength, LargePartLength, SmallPartStart, LargePartStart, SmallPartDest, LargePartDest;
  // moving part is smaller?
  if( MovedLength < DisplacedLength )
  {
    SmallPartStart = SourceSection.start();
    SmallPartLength = MovedLength;
    LargePartLength = DisplacedLength;
    // moving part moves right?
    if( ToRight )
    {
      SmallPartDest = DestPos - MovedLength;
      LargePartStart = SourceSection.end()+1;
      LargePartDest = SourceSection.start();
    }
    else
    {
      SmallPartDest = DestPos;
      LargePartStart = DestPos;
      LargePartDest = DestPos + MovedLength;
    }
  }
  else
  {
    LargePartStart = SourceSection.start();
    LargePartLength = MovedLength;
    SmallPartLength = DisplacedLength;
    // moving part moves right?
    if( ToRight )
    {
      LargePartDest = DestPos - MovedLength;
      SmallPartStart = SourceSection.end()+1;
      SmallPartDest = SourceSection.start();
    }
    else
    {
      LargePartDest = DestPos;
      SmallPartStart = DestPos;
      SmallPartDest = DestPos + MovedLength;
    }
  }

  // copy smaller part to tempbuffer
  char *Temp = new char[SmallPartLength];
  memcpy( Temp, &Data[SmallPartStart], SmallPartLength );

  // move the larger part
  memmove( &Data[LargePartDest], &Data[LargePartStart], LargePartLength );

  // copy smaller part to its new dest
  memcpy( &Data[SmallPartDest], Temp, SmallPartLength );
  delete [] Temp;

  Modified = true;

  emit contentsMoved( DestPos, SourceSection.start(),SourceSection.width()  );
  emit contentsChanged( ToRight?SourceSection.start():DestPos, ToRight?DestPos:SourceSection.end() );
  emit modificationChanged( true );
  return MovedLength < DisplacedLength ? SmallPartDest : LargePartDest;
}


int KFixedSizeByteArrayModel::fill( const char FChar, unsigned int Pos, int FillLength )
{
  // nothing to fill
  if( Pos >= Size )
    return 0;

  unsigned int LengthToEnd = Size - Pos;

  if( FillLength < 0 || FillLength > (int)LengthToEnd )
    FillLength = LengthToEnd;

  memset( &Data[Pos], FChar, FillLength );
  Modified = true;

  emit contentsReplaced( Pos, FillLength, FillLength );
  emit contentsChanged( Pos, Pos+FillLength-1 );
  emit modificationChanged( true );
  return FillLength;
}


int KFixedSizeByteArrayModel::compare( const KAbstractByteArrayModel &Other, const KSection &OR, unsigned int Pos )
{
  KSection OtherRange( OR );
  //kDebug() << QString("Pos: %1, OtherRange: (%3/%4)" ).arg(Pos).arg(OtherRange.start()).arg(OtherRange.end())
  //    << endl;
  // test other values
  if( OtherRange.startsBehind(Other.size()-1) )
    return 1;

  // check own values
  if( Pos >= Size )
    return -1;

  int ValueByLength = 0; // default: equal

  KSection Range = KSection::fromWidth( Pos, OtherRange.width() );
  int Last = Other.size()-1;
  // 
  if( OtherRange.endsBehind(Last) )
  {
    // make shorter
    OtherRange.setEnd( Last );
    if( OtherRange.width() < Range.width() )
      ValueByLength = 1;
  }
  Last = Size-1;
  if( Range.endsBehind(Last) )
  {
    // make shorter
    Range.setEnd( Last );
    if( OtherRange.width() > Range.width() )
      ValueByLength = -1;
  }
  //kDebug()
  //    << QString( "Range: (%1/%2), OtherRange: (%3/%4)" ).arg(Range.start()).arg(Range.end()).arg(OtherRange.start()).arg(OtherRange.end())
  //    << endl;
  int oi = OtherRange.start();
  for( int i=Range.start(); i<=Range.end(); ++i,++oi )
  {
    char OD = Other.datum(oi);
    char D = Data[i];
    //kDebug() << QString("%1==%2").arg((int)D).arg((int)OD) ;
    if( OD == D )
      continue;
    return OD < D ? 1 : -1;
  }

  return ValueByLength;
}


int KFixedSizeByteArrayModel::find(  const char*/*KeyData*/, int /*Length*/, const KSection &/*Section*/  ) const { return 0; }
int KFixedSizeByteArrayModel::rfind( const char*, int /*Length*/, int /*Pos*/ ) const { return 0; }


void KFixedSizeByteArrayModel::reset( unsigned int Pos, unsigned int Length )
{
  memset( &Data[Pos], FillUpChar, Length );
}

}
