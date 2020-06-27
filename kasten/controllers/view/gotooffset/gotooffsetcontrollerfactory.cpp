/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    SPDX-FileCopyrightText: 2019 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "gotooffsetcontrollerfactory.hpp"

// lib
#include "gotooffsetcontroller.hpp"

namespace Kasten {

GotoOffsetControllerFactory::GotoOffsetControllerFactory(If::ToolInlineViewable* toolInlineViewable)
    : m_toolInlineViewable(toolInlineViewable)
{
}

GotoOffsetControllerFactory::~GotoOffsetControllerFactory() = default;

AbstractXmlGuiController* GotoOffsetControllerFactory::create(KXMLGUIClient* guiClient) const
{
    return new GotoOffsetController(m_toolInlineViewable, guiClient);
}

}
