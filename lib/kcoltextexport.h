/***************************************************************************
                          kcoltextexport.h  -  description
                             -------------------
    begin                : Sam Aug 30 2003
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


#ifndef KCOLTEXTEXPORT_H
#define KCOLTEXTEXPORT_H


namespace KHE
{
/**
 * interface for the text export of columns
 * @author Friedrich W. H. Kossebau <Friedrich.W.H@Kossebau.de>
 */
class KColTextExport
{
  public: // API
    virtual void printFirstLine( char **T, int Line ) const = 0;
    virtual void printNextLine( char **T ) const = 0;
    /** tells how much chars per line are needed */
    virtual int charsPerLine() const = 0;
};

}

#endif
