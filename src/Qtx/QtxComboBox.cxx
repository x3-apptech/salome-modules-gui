// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
// File:      QtxComboBox.cxx
// Author:    Sergey TELKOV

#include "QtxComboBox.h"

#include <qpixmap.h>
#include <qlineedit.h>
#include <qvaluelist.h>

QtxComboBox::QtxComboBox( QWidget* parent, const char* name )
: QComboBox( parent, name ),
myCleared( false )
{
    connect( this, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
    connect( this, SIGNAL( activated( const QString& ) ), this, SLOT( onActivated( const QString& ) ) );
}

QtxComboBox::QtxComboBox( bool rw, QWidget* parent, const char* name )
: QComboBox( rw, parent, name ),
myCleared( false )
{
    connect( this, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
    connect( this, SIGNAL( activated( const QString& ) ), this, SLOT( onActivated( const QString& ) ) );
}

QtxComboBox::~QtxComboBox()
{
}

bool QtxComboBox::isCleared() const
{
    return myCleared;
}

void QtxComboBox::setCleared( const bool isClear )
{
    if ( myCleared == isClear )
        return;
    
    myCleared = isClear;
    
    if ( editable() )
    {
        if ( myCleared )
            lineEdit()->setText( "" );
        else
            lineEdit()->setText( text( currentItem() ) );
    }
    
    update();
}

void QtxComboBox::setCurrentItem( int idx )
{
    if ( idx < 0 || idx >= count() )
        return;
    
    myCleared = false;
    QComboBox::setCurrentItem( idx );
}

void QtxComboBox::setCurrentText( const QString& txt )
{
    myCleared = false;
#if QT_VER < 3
    int i = -1;
    for ( int j = 0; j < count() && i == -1; j++ )
        if ( text( j ) == txt )
            i = j;
    if ( i >= 0 && i < count() )
        setCurrentItem( i );
    else if ( editable() )
        lineEdit()->setText( txt );
    else
        changeItem( txt, currentItem() );
#else
    QComboBox::setCurrentText( txt );
#endif
}

int QtxComboBox::currentId() const
{
    return id( currentItem() );
}

void QtxComboBox::setCurrentId( int num )
{
    setCurrentItem( index( num ) );
}

void QtxComboBox::paintEvent( QPaintEvent* e )
{
    if ( !count() || !myCleared || editable() )
        QComboBox::paintEvent( e );
    else
        paintClear( e );
}

void QtxComboBox::onActivated( int idx )
{
    resetClear();
    
    if ( myIndexId.contains( idx ) )
        emit activatedId( myIndexId[idx] );
}

void QtxComboBox::onActivated( const QString& )
{
    resetClear();
}

void QtxComboBox::resetClear()
{
    if ( !myCleared )
        return;
    
    myCleared = false;
    update();
}

void QtxComboBox::paintClear( QPaintEvent* e )
{
    int curIndex = currentItem();
    QString curText = text( curIndex );
    
    QPixmap curPix;
    if ( pixmap( curIndex ) )
        curPix = *pixmap( curIndex );
    
    bool upd = isUpdatesEnabled();
    setUpdatesEnabled( false );
    
    changeItem( "", curIndex );
    QComboBox::paintEvent( e );
    
    if ( curPix.isNull() )
        changeItem( curText, curIndex );
    else
        changeItem( curPix, curText, curIndex );
    
    setUpdatesEnabled( upd );
}

int QtxComboBox::id( const int idx ) const
{
    int id = -1;
    if ( myIndexId.contains( idx ) )
        id = myIndexId[idx];
    return id;
}

int QtxComboBox::index( const int id ) const
{
    int idx = -1;
    for ( IndexIdMap::ConstIterator it = myIndexId.begin();
    it != myIndexId.end() && idx == -1; ++it )
        if ( it.data() == id )
            idx = it.key();
        return idx;
}
