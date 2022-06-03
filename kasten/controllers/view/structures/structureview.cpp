/*
    This file is part of the Okteta Kasten Framework, made within the KDE community.

    SPDX-FileCopyrightText: 2009, 2010, 2012 Alex Richardson <alex.richardson@gmx.de>
    SPDX-FileCopyrightText: 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "structureview.hpp"

// controller
#include "structuretreemodel.hpp"
#include "structurestool.hpp"
#include "structuresmanager.hpp"
#include "structureviewitemdelegate.hpp"
#include "structlogging.hpp"
// settings
#include "structureviewpreferences.h"
#include "settings/structureviewsettingswidget.hpp"
#include "settings/structuresmanagerview.hpp"
#include "settings/structureaddremovewidget.hpp"

#include "datatypes/datainformation.hpp"
#include "script/scriptutils.hpp"
#include "script/scriptloggerview.hpp"

// #include "modeltest.hpp"

// KF
#include <KComboBox>
#include <KStandardAction>
#include <KLocalizedString>
#include <KConfigDialog>
// Qt
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLayout>
#include <QTreeView>
#include <QPushButton>
#include <QHeaderView>
#include <QFocusEvent>
#include <QMenu>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>

namespace Kasten {

StructureView::StructureView(StructuresTool* tool, QWidget* parent)
    : QWidget(parent)
    , mTool(tool)
    , mDelegate(new StructureViewItemDelegate(this))
    , mStructTreeViewFocusChild(nullptr)
{
    QBoxLayout* baseLayout = new QVBoxLayout(this);
    setLayout(baseLayout);
    baseLayout->setContentsMargins(0, 0, 0, 0);
    // table
    mStructureTreeModel = new StructureTreeModel(mTool, this);
    //  mModeltest = new ModelTest(mStructureTreeModel, this);
    mStructTreeView = new QTreeView(this);
    mStructTreeView->setObjectName(QStringLiteral("StructTree"));
    mStructTreeView->setRootIsDecorated(true);
    mStructTreeView->setAlternatingRowColors(true);
    mStructTreeView->setItemsExpandable(true);
    mStructTreeView->setUniformRowHeights(true);
    mStructTreeView->setAllColumnsShowFocus(true);
    mStructTreeView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    mStructTreeView->setItemDelegate(mDelegate);
    mStructTreeView->setDragEnabled(false);
    mStructTreeView->setSortingEnabled(false);
    mStructTreeView->setModel(mStructureTreeModel);
    mStructTreeView->setHeaderHidden(false);
    mStructTreeView->setSortingEnabled(false);
    mStructTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    mStructTreeView->installEventFilter(this);
    QHeaderView* header = mStructTreeView->header();
    header->setSectionResizeMode(QHeaderView::Interactive);
    connect(mStructTreeView, &QWidget::customContextMenuRequested,
            this, &StructureView::onCustomContextMenuRequested);

    baseLayout->addWidget(mStructTreeView, 10);

    // settings
    QBoxLayout* settingsLayout = new QHBoxLayout();
    settingsLayout->setContentsMargins(0, 0, 0, 0);

    baseLayout->addLayout(settingsLayout);

    QIcon validateIcon = QIcon::fromTheme(QStringLiteral("document-sign"));
    mValidateButton = new QPushButton(validateIcon, i18nc("@action:button", "Validate"), this);
    const QString validationToolTip = i18nc("@info:tooltip", "Validate all structures.");
    mValidateButton->setToolTip(validationToolTip);
    mValidateButton->setEnabled(false); // no point validating without file open
    connect(mValidateButton, &QPushButton::clicked, mTool, &StructuresTool::validateAllStructures);
    connect(mTool, &StructuresTool::byteArrayModelChanged,
            this, &StructureView::onByteArrayModelChanged);
    // TODO also disable the button if the structure has no validatable members
    settingsLayout->addWidget(mValidateButton);

    mLockStructureButton = new QPushButton(this);
    mLockStructureButton->setCheckable(true);
    mLockStructureButton->setChecked(false);
    mLockStructureButton->setEnabled(false); // won't work at beginning
    onLockButtonToggled(false);
    connect(mLockStructureButton, &QPushButton::clicked, this, &StructureView::onLockButtonClicked);
    connect(mLockStructureButton, &QPushButton::toggled, this, &StructureView::onLockButtonToggled);

    settingsLayout->addWidget(mLockStructureButton);

    settingsLayout->addStretch(); // stretch before the settings button

    QIcon console = QIcon::fromTheme(QStringLiteral("utilities-terminal"));
    mScriptConsoleButton = new QPushButton(console, i18nc("@action:button", "Script console"), this);
    mScriptConsoleButton->setToolTip(i18nc("@info:tooltip", "Open script console."));
    connect(mScriptConsoleButton, &QPushButton::pressed, this, &StructureView::openScriptConsole);
    settingsLayout->addWidget(mScriptConsoleButton);

    QIcon settings = QIcon::fromTheme(QStringLiteral("configure"));
    mSettingsButton = new QPushButton(settings, i18nc("@action:button", "Settings"), this);
    const QString settingsTooltip = i18nc("@info:tooltip", "Open settings.");
    mSettingsButton->setToolTip(settingsTooltip);
    connect(mSettingsButton, &QPushButton::pressed, this, &StructureView::openSettingsDlg);
    settingsLayout->addWidget(mSettingsButton);

    connect(mStructTreeView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, &StructureView::onCurrentRowChanged);
}

StructureView::~StructureView() = default;

StructuresTool* StructureView::tool() const
{
    return mTool;
}

void StructureView::openSettingsDlg()
{
    // An instance of your dialog could be already created and could be cached,
    // in which case you want to display the cached dialog instead of creating
    // another one
    if (KConfigDialog::showDialog(QStringLiteral("Structures Tool Settings"))) {
        return;
    }

    // KConfigDialog didn't find an instance of this dialog, so lets create it :
    KConfigDialog* dialog = new KConfigDialog(this, QStringLiteral("Structures Tool Settings"),
                                              StructureViewPreferences::self());

    auto* displaySettings = new StructureViewSettingsWidget();
    KPageWidgetItem* displ = dialog->addPage(displaySettings, i18n("Value Display"),
                                             QStringLiteral("configure"));

    // cannot use StructuresManagerView directly as page even if the only element
    // because KConfigDialogManager only scans the children of the page for kcfg_ elements
    QWidget* structSelectionPage = new QWidget();
    auto* hbox = new QHBoxLayout();
    hbox->setContentsMargins(0, 0, 0, 0);
    structSelectionPage->setLayout(hbox);
    auto* structureSettings = new StructuresManagerView(mTool, this);
    structureSettings->setObjectName(QStringLiteral("kcfg_LoadedStructures"));
    hbox->addWidget(structureSettings);
    dialog->addPage(structSelectionPage, i18n("Structures management"),
                    QStringLiteral("preferences-plugin"));

    // User edited the configuration - update your local copies of the configuration data
    connect(dialog, &KConfigDialog::settingsChanged, mTool, &StructuresTool::setSelectedStructuresInView);

    // TODO: kconfig_compiler signals work now, use those signals and not the generic KConfigDialog::settingsChanged
    dialog->setCurrentPage(displ);
    dialog->show();
}

bool StructureView::eventFilter(QObject* object, QEvent* event)
{
    if (object == mStructTreeView) {
        if (event->type() == QEvent::FocusIn) {
            const QModelIndex current = mStructTreeView->selectionModel()->currentIndex();

            if (current.isValid()) {
                mTool->mark(current);
            } else {
                mTool->unmark();
            }
        } else if (event->type() == QEvent::FocusOut) {
            QWidget* treeViewFocusWidget = mStructTreeView->focusWidget();
            const bool subChildHasFocus = (treeViewFocusWidget != mStructTreeView);
            if (subChildHasFocus) {
                mStructTreeViewFocusChild = treeViewFocusWidget;
                mStructTreeViewFocusChild->installEventFilter(this);
            } else {
                mTool->unmark();
            }
        }
    } else if (object == mStructTreeViewFocusChild) {
        // TODO: it is only assumed the edit widget will be removed if it loses the focus
        if (event->type() == QEvent::FocusOut) {
            if (!mStructTreeView->hasFocus()) {
                mTool->unmark();
            }
            mStructTreeViewFocusChild->removeEventFilter(this);
            mStructTreeViewFocusChild = nullptr;
        }
    }

    return QWidget::eventFilter(object, event);
}

void StructureView::setLockButtonState(const QModelIndex& current)
{
    // qCDebug(LOG_KASTEN_OKTETA_CONTROLLERS_STRUCTURES) << "setLockButtonState() for" << current;

    mLockStructureButton->setEnabled(mTool->canStructureBeLocked(current));
    mLockStructureButton->setChecked(mTool->isStructureLocked(current));
}

void StructureView::onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous)
    if (current.isValid() && mTool->byteArrayModel()) {
        mTool->mark(current);
    } else {
        mTool->unmark();
    }
    setLockButtonState(current);
}

void StructureView::onLockButtonClicked(bool checked)
{
    // qCDebug(LOG_KASTEN_OKTETA_CONTROLLERS_STRUCTURES) << "Lock button toggled";

    const QModelIndex current = mStructTreeView->selectionModel()->currentIndex();
    if (!current.isValid()) {
        qCWarning(LOG_KASTEN_OKTETA_CONTROLLERS_STRUCTURES) << "it should not be possible to toggle this button when current index is invalid!";
        return;
    }

    if (checked) {
        mTool->lockStructure(current);
    } else {
        mTool->unlockStructure(current);
    }
}

void StructureView::onLockButtonToggled(bool structureLocked)
{
    if (structureLocked) {
        mLockStructureButton->setIcon(QIcon::fromTheme(QStringLiteral("object-locked")));
        mLockStructureButton->setText(i18nc("@action:button"
                                            " unlock the starting offset of the current structure", "Unlock"));
        mLockStructureButton->setToolTip(i18nc("@info:tooltip",
                                               "Unlock selected structure, i.e. the starting offset is"
                                               " always set to the current cursor position."));
    } else {
        mLockStructureButton->setIcon(QIcon::fromTheme(QStringLiteral("object-unlocked")));
        mLockStructureButton->setText(i18nc("@action:button"
                                            " unlock the starting offset of the current structure", "Lock"));
        mLockStructureButton->setToolTip(i18nc("@info:tooltip",
                                               "Lock selected structure to current offset."));
    }
}

void StructureView::selectBytesInView()
{
    auto* action = static_cast<QAction*>(sender());
    const QModelIndex index = action->data().value<QModelIndex>();

    mTool->selectBytesInView(index);
}

void StructureView::copyToClipboard()
{
    auto* action = static_cast<QAction*>(sender());
    const QModelIndex index = action->data().value<QModelIndex>();
    auto* item = static_cast<DataInformation*> (index.internalPointer());

    auto* mimeData = new QMimeData;

    mimeData->setText(item->valueString());
    mimeData->setData(QStringLiteral("application/octet-stream"), mTool->bytes(item));

    QApplication::clipboard()->setMimeData(mimeData);
}

void StructureView::openScriptConsole()
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(i18nc("@title:window", "Structures Script Console"));
    auto* layout = new QVBoxLayout;
    auto* dialogButtonBox = new QDialogButtonBox;
    QPushButton* closeButton = dialogButtonBox->addButton(QDialogButtonBox::Close);
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);
    layout->addWidget(new ScriptLoggerView(mTool->allData()));
    layout->addWidget(dialogButtonBox);
    dialog->setLayout(layout);
    dialog->show();
}

void StructureView::onByteArrayModelChanged(Okteta::AbstractByteArrayModel* model)
{
    const bool validModel = (model != nullptr);
    const QModelIndex current = mStructTreeView->currentIndex();
    setLockButtonState(current);
    mValidateButton->setEnabled(validModel);
}

void StructureView::onCustomContextMenuRequested(QPoint pos)
{
    const QModelIndex index = mStructTreeView->indexAt(pos);
    if (!index.isValid()) {
        return;
    }
    const auto* data = static_cast<const DataInformation*>(index.internalPointer());
    if (!data) {
        return;
    }
    if (!data->wasAbleToRead()) {
        return;
    }

    auto* menu = new QMenu(this);

    // TODO: split into explicit "Copy As Data" and "Copy As Text"
    auto* copyAction =  KStandardAction::copy(this, &StructureView::copyToClipboard,  this);
    copyAction->setShortcut(QKeySequence());
    copyAction->setData(index);
    menu->addAction(copyAction);

    // TODO: reusing string due to string freeze
    auto* selectAction = new QAction(i18nc("@action:button", "&Select"), this);
    connect(selectAction, &QAction::triggered,
            this, &StructureView::selectBytesInView);
    selectAction->setData(index);
    menu->addAction(selectAction);

    menu->popup(mStructTreeView->viewport()->mapToGlobal(pos));
}

}
