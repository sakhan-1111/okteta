/***************************************************************************
                          kbuffercolumn.h  -  description
                             -------------------
    begin                : Mit Mai 14 2003
    copyright            : (C) 2003 by Friedrich W. H. Kossebau
    email                : Friedrich.W.H@Kossebau.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License version 2 as published by the Free Software Foundation.       *
 *                                                                         *
 ***************************************************************************/


#ifndef KBUFFERCOLUMN_H
#define KBUFFERCOLUMN_H

#include "khe.h"
#include "kdatabuffer.h"
#include "kbytecodec.h"
#include "kcolumn.h"
#include "kbufferlayout.h"
#include "ksection.h"

class QPainter;

namespace KHE
{

// class KHexEdit;
class KBufferRanges;

const int NoByteFound = -1;

/** base class of all buffer column displayers
  * holds all information about the vertical layout of a buffer column
  * knows how to paint the data and the editing things (focus, cursor, selection)
  * but does not offer
  *
  *@author Friedrich W. H. Kossebau
  */
class KBufferColumn : public KColumn
{
  public:
    KBufferColumn( KColumnsView/*KHexEdit*/ *HE, KDataBuffer *B, KBufferLayout *L, KBufferRanges *R );
    virtual ~KBufferColumn();


  public: // KColumn-API
    virtual void paintFirstLine( QPainter *P, KPixelX cx, KPixelX cw, int FirstLine );
    virtual void paintNextLine( QPainter *P );

  public:
    void preparePainting( KPixelX cx, KPixelX cw );

  public:
    void paintLine( QPainter *P, int Line );
    void paintPositions( QPainter *P, int Line, KSection Positions );
    void paintCursor( QPainter *P, int Index );
    void paintByte( QPainter *P, int Index );
    /** paints a frame to the char*/
    void paintFrame( QPainter *P, int Index );

  public: // modification access
    /**
      * returns true if there was a change
      */
    bool setCoding( KCoding C );
    /** sets the spacing in the hex column
      * @param ByteSpacingW spacing between the bytes in pixels
      * @param NewNoOfGroupedBytes numbers of grouped bytes, 0 means no grouping
      * @param GroupSpacingW spacing between the groups in pixels
      * returns true if there was a change
      */
    bool setSpacing( KPixelX ByteSpacingW, int NewNoOfGroupedBytes = 0, KPixelX GroupSpacingW = 0 );
    /** sets the spacing between the bytes in the hex column
      * @param ByteSpacingW spacing between the bytes in pixels
      * returns true if there was a change
      */
    bool setByteSpacingWidth( KPixelX ByteSpacingW );
    /** sets the number of grouped bytes in the hex column
      * @param NewNoOfGroupedBytes numbers of grouped bytes, 0 means no grouping
      * returns true if there was a change
      */
    bool setNoOfGroupedBytes( int NewNoOfGroupedBytes );
    /** sets the spacing between the groups of bytes in the hex column
      * @param GroupSpacingW spacing between the groups in pixels
      * returns true if there was a change
      */
    bool setGroupSpacingWidth( KPixelX GroupSpacingW );
    /** sets width of digits and recalculates depend sizes
      * returns true if there was a change
      */
    bool setDigitWidth( KPixelX DW );
    /** sets the spacing in the middle of a binary byte in the hex column
      * @param BinaryGapW spacing in the middle of a binary in pixels
      * returns true if there was a change
      */
    bool setBinaryGapWidth( KPixelX BGW );
    /** sets the metrics of the used font
      * @param NewDigitWidth the new width of a digit
      * @param NewDigitBaseLine the new baseline of the digits
      */
    void setMetrics( KPixelX NewDigitWidth, KPixelY NewDigitBaseLine );
    /** */
    void set( KDataBuffer *B );
    /** creates new buffer for x-values; to be called on any change of NoOfBytesPerLine or metrics */
    void resetXBuffer();

  public: // functional logic
    /** returns byte positions covered by pixels with absolute x-coord x */
    KSection posOfX( KPixelX x, KPixelX w ) const;
    /** returns byte pos at pixel with absolute x-coord x */
    int posOfX( KPixelX x ) const;
    /** returns absolute x-coord of byte at position Pos */
    KPixelX xOfPos( int Pos ) const;
    /** returns right absolute x-coord of byte at position Pos */
    KPixelX rightXOfPos( int Pos ) const;
    /** returns byte pos at pixel with relative x-coord x */
    int posOfRelX( KPixelX x ) const;
    /** returns byte positions covered by pixels with relative x-coord x */
    KSection posOfRelX( KPixelX x, KPixelX w ) const;
    /** returns relative x-coord of byte at position Pos */
    KPixelX relXOfPos( int Pos ) const;
    /** returns right relative x-coord of byte at position Pos */
    KPixelX relRightXOfPos( int Pos ) const;
    /** returns the positions that overlap with the absolute x-coords */
    KSection visiblePositions( KPixelX x, KPixelX w ) const;
    /** returns the */
    KSection wideXPixelsOfPos( KSection Positions ) const;
    /** */
    KSection relWideXPixelsOfPos( KSection Positions ) const;

  public: // value access
    KPixelX byteWidth()                 const;
    KPixelX digitWidth()                const;
    KPixelX groupSpacingWidth()         const;
    KPixelX byteSpacingWidth()          const;
    KPixelX binaryGapWidth()            const;
    int noOfGroupedBytes()              const;
    KCoding coding()                    const;
    int codingWidth()                   const;
    KByteCodec::coding codingFunction() const;

    int firstPos() const;
    int lastPos()  const;
    KSection visiblePositions() const;

  protected:
    /** */
    void recalcX();
    void drawByte( QPainter *P, const char Byte, const QColor &Color ) const;
//    void drawSelection( QPainter *P, const int Index, int FP, int LP ) const;
//    void drawMarking( QPainter *P, const int Index, int FP, int LP ) const;
//    bool isSelected( int FI, int LI, int *FSI, int *LSI, unsigned int *Flag ) const;
//    bool isMarked( int FI, int LI, int *FSI, int *LSI, unsigned int *Flag ) const;

    void paintGrid( QPainter *P, KSection Range );
    void paintPlain( QPainter *P, KSection Positions, int Index );
    void paintSelection( QPainter *P, KSection Positions, int Index, int Flag );
    void paintMarking( QPainter *P, KSection Positions, int Index, int Flag );
    void paintRange( QPainter *P, const QColor &Color, KSection Positions, int Flag );

    bool isSelected( KSection Range, KSection *Selection, unsigned int *Flag ) const;
    bool isMarked( KSection Range, KSection *Marking, unsigned int *Flag ) const;

    void recalcByteWidth();
    void recalcVerticalGridX();

  protected:
    /** pointer to the buffer */
    KDataBuffer *Buffer;
    /** pointer to the layout */
    KBufferLayout *Layout;
    /** pointer to the ranges */
    KBufferRanges *Ranges;

    /** */
    KPixelX DigitWidth;
    /** */
    KPixelY DigitBaseLine;
    /** */
    KPixelX VerticalGridX;
    /** */
    bool VerticalGrid;

    /** buffer to hold the formatted coding */
    mutable char CodedByte[KByteCodec::MaxCodingWidth+1];

  protected:  // individual data
    /***/
    KCoding Coding;
    /** buffers coding width */
    int CodingWidth;
    KByteCodec::coding CodingFunction;
    /** total width of byte display in pixel */
    KPixelX ByteWidth;
    /** width of inserting cursor in pixel */
    KPixelX CursorWidth;
    /** size of the line margin */
    KPixelX ByteSpacingWidth;
    /** width of spacing in pixel */
    KPixelX GroupSpacingWidth;

    /** */
    KPixelX BinaryGapWidth;
    /** Offset of the */
    KPixelX BinaryHalfOffset;
    /** number of grouped bytes */
    int NoOfGroupedBytes;

    /** pointer to array with buffered positions (relative to column position)
      * a spacing gets assigned to the left byte -> ...c|c|c |c|c...
      */
    KPixelX *PosX;
    KPixelX *PosRightX;
    /** index of right position */
    int LastPos;


  protected: // buffering drawing data
    KSection PaintPositions;
    int PaintLine;
    KPixelX PaintX;
    KPixelX PaintW;
    int SpacingTrigger;
};


inline KPixelX KBufferColumn::byteWidth()         const { return ByteWidth; }
inline KPixelX KBufferColumn::digitWidth()        const { return DigitWidth; }
inline KPixelX KBufferColumn::byteSpacingWidth()  const { return ByteSpacingWidth; }
inline KPixelX KBufferColumn::groupSpacingWidth() const { return GroupSpacingWidth; }
inline KPixelX KBufferColumn::binaryGapWidth()    const { return BinaryGapWidth; }

inline int KBufferColumn::noOfGroupedBytes()      const { return NoOfGroupedBytes; }
inline KCoding KBufferColumn::coding()            const { return Coding; }

inline int KBufferColumn::codingWidth()                      const { return CodingWidth; }
inline KByteCodec::coding KBufferColumn::codingFunction() const { return CodingFunction; }

inline int KBufferColumn::firstPos() const { return PaintPositions.start(); }
inline int KBufferColumn::lastPos()  const { return PaintPositions.end(); }
inline KSection KBufferColumn::visiblePositions() const { return PaintPositions; }

}

#endif
