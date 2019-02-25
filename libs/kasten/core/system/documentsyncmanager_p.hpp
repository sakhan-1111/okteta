/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007,2009,2011,2019 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef KASTEN_DOCUMENTSYNCMANAGER_P_HPP
#define KASTEN_DOCUMENTSYNCMANAGER_P_HPP

// lib
#include "documentsyncmanager.hpp"

namespace Kasten {

class DocumentSyncManagerPrivate
{
public:
    explicit DocumentSyncManagerPrivate(DocumentSyncManager* q, DocumentManager* manager);

    ~DocumentSyncManagerPrivate();

public:
    void load(const QUrl& url);
    bool setSynchronizer(AbstractDocument* document);
    bool canClose(AbstractDocument* document);
    void reload(AbstractDocument* document);
    void save(AbstractDocument* document);

public:
    QStringList supportedRemoteTypes() const;
    bool hasSynchronizerForLocal(const QString& mimeType) const;
    QUrl urlOf(AbstractDocument* document) const;

public:
    void setDocumentSynchronizerFactory(AbstractModelSynchronizerFactory* synchronizerFactory);
    void setSaveDiscardDialog(AbstractSaveDiscardDialog* saveDiscardDialog);
    void setOverwriteDialog(AbstractOverwriteDialog* overwriteDialog);

private: // slots
    void onDocumentLoaded(Kasten::AbstractDocument* document);

private:
    DocumentSyncManager* const q_ptr;

    // unless there is a singleton
    DocumentManager* mManager;

    // temporary hack: hard coded factories for byte arrays
    AbstractModelSynchronizerFactory* mSynchronizerFactory = nullptr;

    AbstractSaveDiscardDialog* mSaveDiscardDialog = nullptr;
    AbstractOverwriteDialog* mOverwriteDialog = nullptr;

    Q_DECLARE_PUBLIC(DocumentSyncManager)
};

}

#endif
