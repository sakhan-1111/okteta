/*
    This file is part of the Kasten Framework, made within the KDE community.

    SPDX-FileCopyrightText: 2019 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KASTEN_VERSIONCONTROLLERFACTORY_HPP
#define KASTEN_VERSIONCONTROLLERFACTORY_HPP

// lib
#include "kastencontrollers_export.hpp"
// Kasten core
#include <Kasten/AbstractXmlGuiControllerFactory>

namespace Kasten {

class KASTENCONTROLLERS_EXPORT VersionControllerFactory : public AbstractXmlGuiControllerFactory
{
public:
    VersionControllerFactory();
    VersionControllerFactory(const VersionControllerFactory&) = delete;

    ~VersionControllerFactory() override;

    VersionControllerFactory& operator=(const VersionControllerFactory&) = delete;

public: // AbstractXmlGuiControllerFactory API
    AbstractXmlGuiController* create(KXMLGUIClient* guiClient) const override;
};

}

#endif
