/*
    This file is part of the Okteta Kasten module, part of the KDE project.

    Copyright 2006-2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef INSERTPATTERNCONTROLLER_H
#define INSERTPATTERNCONTROLLER_H

// lib
#include "oktetakastencontroller_export.h"
// Kasten gui
#include <abstractxmlguicontroller.h>

class KXMLGUIClient;
class KAction;


namespace Kasten
{

class InsertPatternTool;
class InsertPatternDialog;


class OKTETAKASTENCONTROLLER_EXPORT InsertPatternController : public AbstractXmlGuiController
{
  Q_OBJECT

  public:
    explicit InsertPatternController( KXMLGUIClient* guiClient );
    virtual ~InsertPatternController();

  public: // AbstractXmlGuiController API
    virtual void setTargetModel( AbstractModel* model );

  protected Q_SLOTS:
    void onActionTriggered();
    void onHasWriteableChanged( bool hasWriteable );

  private Q_SLOTS:
    void onOkClicked();

  protected:
    KAction* mInsertPatternAction;

    InsertPatternTool *mInsertPatternTool;
    InsertPatternDialog *mInsertPatternDialog;
};

}

#endif
