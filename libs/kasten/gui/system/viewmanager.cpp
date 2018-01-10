/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006,2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "viewmanager.h"

// lib
#include "abstractviewfactory.h"
#include "dummyview.h"
// Qt
#include <QListIterator>
#include <QMutableListIterator>

// temporary
#include "modelcodecviewmanager.h"


namespace Kasten
{

ViewManager::ViewManager()
  : mCodecViewManager( new ModelCodecViewManager() )
{
}


void ViewManager::setViewFactory( AbstractViewFactory* factory )
{
    mFactory = factory;
}

QList<AbstractView*> ViewManager::views() const
{
    return mViewList;
}

AbstractView* ViewManager::viewByWidget( QWidget* widget ) const
{
    AbstractView* result = nullptr;

    QListIterator<AbstractView*> it( mViewList );
    while( it.hasNext() )
    {
        AbstractView* view = it.next();
        if( view->widget() == widget)
        {
            result = view;
            break;
        }
    }
    return result;
}

void ViewManager::createCopyOfView( AbstractView* view, Qt::Alignment alignment )
{
    AbstractView* viewCopy = mFactory->createCopyOfView( view, alignment );
    if( ! viewCopy )
    {
        AbstractDocument* documentOfView = view->findBaseModel<AbstractDocument*>();
        viewCopy = new DummyView( documentOfView );
    }

    mViewList.append( viewCopy );

    const QList<Kasten::AbstractView*> views { viewCopy };
    emit opened( views );
}

void ViewManager::createViewsFor( const QList<Kasten::AbstractDocument*>& documents )
{
    QList<Kasten::AbstractView*> openedViews;

    openedViews.reserve(documents.size());
    mViewList.reserve(mViewList.size() + documents.size());
    for( AbstractDocument* document : documents )
    {
        AbstractView* view = mFactory->createViewFor( document );
        if( ! view )
            view = new DummyView( document );

        mViewList.append( view );
        openedViews.append( view );
    }

    if( ! openedViews.isEmpty() )
        emit opened( openedViews );
}


void ViewManager::removeViewsFor( const QList<Kasten::AbstractDocument*>& documents )
{
    QList<Kasten::AbstractView*> closedViews;

    QMutableListIterator<AbstractView*> it( mViewList );
    for( AbstractDocument* document : documents )
    {
        while( it.hasNext() )
        {
            AbstractView* view = it.next();
            AbstractDocument* documentOfView = view->findBaseModel<AbstractDocument*>();
            if( documentOfView == document )
            {
                it.remove();
                closedViews.append( view );
            }
        }
        it.toFront();
    }

    emit closing( closedViews );

    for( AbstractView* view : qAsConst(closedViews) )
    {
//         qCDebug(LOG_KASTEN_GUI) << view->title();
        delete view;
    }
}

void ViewManager::removeViews( const QList<AbstractView*>& views )
{
    for( AbstractView* view : views )
        mViewList.removeOne( view );

    emit closing( views );

    for( AbstractView* view : views )
    {
//         qCDebug(LOG_KASTEN_GUI)<<view->title();
        delete view;
    }
}


ViewManager::~ViewManager()
{
    // TODO: signal closing here, too?
    qDeleteAll( mViewList );

    delete mCodecViewManager;
    delete mFactory;
}

}
