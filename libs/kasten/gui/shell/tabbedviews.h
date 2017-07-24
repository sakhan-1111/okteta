/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2007,2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef TABBEDVIEWS_H
#define TABBEDVIEWS_H

// lib
#include <kasten/abstractgroupedviews.h>
#include <kasten/toolinlineviewable.h>

class QMimeData;


namespace Kasten
{

class ViewAreaBox;

class TabbedViewsPrivate;


class KASTENGUI_EXPORT TabbedViews : public AbstractGroupedViews,
                                     public If::ToolInlineViewable
{
  Q_OBJECT
  Q_INTERFACES(
    Kasten::If::ToolInlineViewable
  )

  protected:
    explicit TabbedViews( TabbedViewsPrivate* d );

  public:
    TabbedViews();

    ~TabbedViews() override;

  public: // AbstractViewArea API
    void setFocus() override;
    QWidget* widget() const override;
    bool hasFocus() const override;

  public: // AbstractGroupedViews API
    void addViews( const QList<AbstractView*>& views ) override;
    void removeViews( const QList<AbstractView*>& views ) override;
    void setViewFocus( AbstractView* view ) override;

    QList<AbstractView*> viewList() const override;
    int viewCount() const override;
    AbstractView* viewFocus() const override;

  public: // If::ToolInlineViewable API
    void setCurrentToolInlineView( AbstractToolInlineView* view ) override;

  public:
    int indexOf( AbstractView* view ) const;

  Q_SIGNALS:
    void dataOffered( const QMimeData* mimeData, bool& accepted );
    void dataDropped( const QMimeData* mimeData );

  protected:
//     Q_PRIVATE_SLOT( d_func(), void onModifiedChanged( Kasten::AbstractDocument::SyncStates newStates ) )
    Q_PRIVATE_SLOT( d_func(), void onMouseMiddleClick() )
    Q_PRIVATE_SLOT( d_func(), void onDragMoveEvent( const QDragMoveEvent*, bool& ) )
    Q_PRIVATE_SLOT( d_func(), void onDropEvent( QDropEvent* event ) )

  protected:
    Q_DECLARE_PRIVATE( TabbedViews )
};

}
#endif
