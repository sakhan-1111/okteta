/*
    This file is part of the Okteta Gui library, made within the KDE community.

    Copyright 2004,2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef OKTETA_VALUEEDITOR_HPP
#define OKTETA_VALUEEDITOR_HPP

// lib
#include "abstracteditor.hpp"
// Okteta core
#include <Okteta/Byte>
// Qt
#include <QString>

namespace Okteta {
class AbstractByteArrayView;

class ValueEditor : public AbstractEditor
{
private:
    enum ValueEditAction
    {
        EnterValue,
        IncValue,
        DecValue,
        ValueAppend,
        ValueEdit,
        LeaveValue,
        ValueBackspace
    };

public:
    ValueEditor(ByteArrayTableCursor* cursor, AbstractByteArrayView* view, AbstractController* parent);
    ~ValueEditor() override;

public: // AbstractController API
    bool handleKeyPress(QKeyEvent* keyEvent) override;

public:
    void reset();

    void adaptToValueCodecChange();
    void finishEdit();
    void cancelEdit(bool undoChanges = true);

public:
    bool isInEditMode() const;
    Byte value() const;
    QString valueAsString() const;

private:
    void startEdit(const QString& description);
    /**
     * executes keyboard Action \a Action. This is normally called by a key event handler.
     * @param action action to be done
     * @param input data to be used for the action
     */
    void doValueEditAction(ValueEditAction action, int input = -1);

private:
    /** flag whether we are in editing mode */
    bool mInEditMode : 1;
    /** flag whether byte edit mode was reached by inserting */
    bool mEditModeByInsert : 1;
    /** */
    Byte mEditValue;
    /** stores the old byte value */
    Byte mOldValue; // TODO: this or rely on undo?
    /** */
    unsigned int mInsertedDigitsCount;
    /** buffer with the  */
    QString mValueString;
};

inline bool ValueEditor::isInEditMode() const { return mInEditMode; }
inline void ValueEditor::reset() { mInEditMode = false; }
inline Byte ValueEditor::value() const { return mEditValue; }
inline QString ValueEditor::valueAsString() const { return mValueString; }

}

#endif
