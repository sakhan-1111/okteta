/*
    This file is part of the Kasten Framework, made within the KDE community.

    SPDX-FileCopyrightText: 2008-2009, 2019 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "modelcodecviewmanager_p.hpp"

// lib
#include <filesystem/modelencoderfilesystemexporterconfigeditorfactory.hpp>
#include <abstractmodelstreamencoderconfigeditorfactory.hpp>
#include <abstractmodelstreamencoderconfigeditor.hpp>
#include <abstractmodelexporterconfigeditorfactory.hpp>
#include <abstractmodelexporterconfigeditor.hpp>
#include <abstractmodeldatageneratorconfigeditorfactory.hpp>
#include <abstractmodeldatageneratorconfigeditor.hpp>
// Kasten core
#include <Kasten/AbstractModelExporter>
#include <Kasten/AbstractModelDataGenerator>
#include <Kasten/AbstractModelStreamEncoder>
// Std
#include <utility>

namespace Kasten {

ModelCodecViewManagerPrivate::ModelCodecViewManagerPrivate() = default;

ModelCodecViewManagerPrivate::~ModelCodecViewManagerPrivate()
{
    qDeleteAll(mExporterFactoryList);
}

void ModelCodecViewManagerPrivate::setStreamEncoderConfigEditorFactories(std::vector<std::unique_ptr<AbstractModelStreamEncoderConfigEditorFactory>>&& factoryList)
{
    mEncoderFactoryList = std::move(factoryList);

    qDeleteAll(mExporterFactoryList);
    mExporterFactoryList.clear();

    mExporterFactoryList.reserve(mEncoderFactoryList.size());
    for (const auto& factory : mEncoderFactoryList) {
        mExporterFactoryList << new ModelEncoderFileSystemExporterConfigEditorFactory(factory.get());
    }
}

void ModelCodecViewManagerPrivate::setDataGeneratorConfigEditorFactories(std::vector<std::unique_ptr<AbstractModelDataGeneratorConfigEditorFactory>>&& factoryList)
{
    mGeneratorFactoryList = std::move(factoryList);
}

AbstractModelStreamEncoderConfigEditor* ModelCodecViewManagerPrivate::createConfigEditor(AbstractModelStreamEncoder* encoder) const
{
    AbstractModelStreamEncoderConfigEditor* result = nullptr;

    for (const auto& factory : mEncoderFactoryList) {
        result = factory->tryCreateConfigEditor(encoder);
        if (result) {
            break;
        }
    }

    return result;
}

AbstractModelExporterConfigEditor* ModelCodecViewManagerPrivate::createConfigEditor(AbstractModelExporter* exporter) const
{
    AbstractModelExporterConfigEditor* result = nullptr;

    for (const AbstractModelExporterConfigEditorFactory* factory : mExporterFactoryList) {
        result = factory->tryCreateConfigEditor(exporter);
        if (result) {
            break;
        }
    }

    return result;
}

AbstractModelDataGeneratorConfigEditor* ModelCodecViewManagerPrivate::createConfigEditor(AbstractModelDataGenerator* generator) const
{
    AbstractModelDataGeneratorConfigEditor* result = nullptr;

    for (const auto& factory : mGeneratorFactoryList) {
        result = factory->tryCreateConfigEditor(generator);
        if (result) {
            break;
        }
    }

    return result;
}

}
