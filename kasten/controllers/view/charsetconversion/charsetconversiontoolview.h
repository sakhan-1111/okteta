/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    Copyright 2011 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef CHARSETCONVERSIONTOOLVIEW_H
#define CHARSETCONVERSIONTOOLVIEW_H

// lib
#include <kasten/okteta/oktetakastencontrollers_export.h>
// Kasten gui
#include <kasten/abstracttoolview.h>


namespace Kasten
{

class CharsetConversionView;
class CharsetConversionTool;


class OKTETAKASTENCONTROLLERS_EXPORT CharsetConversionToolView : public AbstractToolView
{
  public:
    explicit CharsetConversionToolView( CharsetConversionTool* tool );
    ~CharsetConversionToolView() override;

  public: // AbstractToolView API
    QWidget* widget() const override;
    QString title() const override;
    AbstractTool* tool() const override;

  private:
    CharsetConversionView* mWidget;
};

}

#endif
