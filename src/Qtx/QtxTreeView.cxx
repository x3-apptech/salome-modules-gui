// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File:      QtxTreeView.cxx
// Author:    Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "QtxTreeView.h"

#include <QHeaderView>
#include <QMenu>
#include <QMouseEvent>

/*!
  \class QtxTreeView::Header
  \brief Custom tree view header class.
  \internal
*/

class QtxTreeView::Header : public QHeaderView
{
public:
  Header( const bool, QWidget* = 0 );
  ~Header();

  void     setSortMenuEnabled( const bool );
  bool     sortMenuEnabled() const;

protected:
  void     contextMenuEvent( QContextMenuEvent* );

private:
  bool     myEnableSortMenu;
};

/*!
  \brief Constructor
  \param enableSortMenu show "Sorting" menu if \c true
  \param parent parent widget
  \internal
*/
QtxTreeView::Header::Header( const bool enableSortMenu, QWidget* parent )
: QHeaderView( Qt::Horizontal, parent ),
  myEnableSortMenu( enableSortMenu )
{
}

/*!
  \brief Destructor
  \internal
*/
QtxTreeView::Header::~Header()
{
}

/*
  \brief Enable/disable "Sorting" popup menu command for the header.
  \param enableSortMenu if \c true, enable "Sorting" menu command
  \internal
*/
void QtxTreeView::Header::setSortMenuEnabled( const bool enableSortMenu )
{
  myEnableSortMenu = enableSortMenu;
}

/*
  \brief Check if "Sorting" popup menu command for the header is enabled.
  \return \c true if "Sorting" menu command is enabled
  \internal
*/
bool QtxTreeView::Header::sortMenuEnabled() const
{
  return myEnableSortMenu;
}

/*!
  \brief Customize context menu event.
  \internal

  Shows popup menu with the list of the available columns allowing the user to
  show/hide the specified column.

  \param e context menu event
*/
void QtxTreeView::Header::contextMenuEvent( QContextMenuEvent* e )
{
  QMenu menu;
  QMap<QAction*, int> actionMap;
  for ( int i = 0; i < count(); i++ ) {
    QString  lab         = model()->headerData( i, orientation(), Qt::DisplayRole ).toString();
    QVariant iconData    = model()->headerData( i, orientation(), Qt::DecorationRole );
    QVariant appropriate = model()->headerData( i, orientation(), Qtx::AppropriateRole );
    QIcon icon;
    if ( iconData.isValid() ) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
      if ( iconData.canConvert( QVariant::Icon ) )
        icon = iconData.value<QIcon>();
      else if ( iconData.canConvert( QVariant::Pixmap ) )
        icon = iconData.value<QPixmap>();
#else
      if ( iconData.canConvert( QMetaType::QIcon ) )
        icon = iconData.value<QIcon>();
      else if ( iconData.canConvert( QMetaType::QPixmap ) )
        icon = iconData.value<QPixmap>();
#endif
    }
    if( ( !lab.isEmpty() || !icon.isNull() ) && 
            appropriate.isValid() ? appropriate.toInt()==Qtx::Toggled : true )
        {
      QAction* a = menu.addAction( icon, lab );
      a->setCheckable( true );
      a->setChecked( !isSectionHidden( i ) );
      actionMap.insert( a, i );
    }
  }
  QAction* sortAction = 0;
  if ( count() > 0 && myEnableSortMenu ) {
    menu.addSeparator();
    sortAction = menu.addAction( QtxTreeView::tr( "Enable sorting" ) );
    sortAction->setCheckable( true );
    sortAction->setChecked( isSortIndicatorShown() );
  }
  if ( !menu.isEmpty() ) {
    Qtx::simplifySeparators( &menu );
    QAction* a = menu.exec( e->globalPos() );
    if ( a && actionMap.contains( a ) ) {
      setSectionHidden( actionMap[ a ], !isSectionHidden( actionMap[ a ] ) );
    }
    else if ( a && a == sortAction ) {
      setSortIndicatorShown( a->isChecked() );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
      setClickable( a->isChecked() );
#else
      setSectionsClickable( a->isChecked() );
#endif
      QtxTreeView* view = qobject_cast<QtxTreeView*>( parent() );
      if ( view ) {
        view->emitSortingEnabled( a->isChecked() );
        if ( a->isChecked() ) {
          connect( this, SIGNAL( sectionClicked( int ) ), view, SLOT( onHeaderClicked( int ) ) );
	  setSortIndicator( 0, Qt::AscendingOrder );
          view->sortByColumn( sortIndicatorSection(), sortIndicatorOrder() );
        }
        else {
          disconnect( this, SIGNAL( sectionClicked( int ) ), view, SLOT( onHeaderClicked( int ) ) );
          view->sortByColumn( 0, Qt::AscendingOrder );
        }
      }
    }
  }
  e->accept();
}


/*!
  \class QtxTreeView
  \brief Tree view class with possibility to display columns popup menu.

  The QtxTreeView class represents a customized tree view class. In addition to the
  base functionality inherited from the QTreeView class, clicking at the tree view 
  header with the right mouse button displays the popup menu allowing the user
  to show/hide specified columns.

  By default the popup menu contains items corresponding to all the tree view columns.
  In order to disable some columns from being shown in the popup menu one may customize
  the data model (see QAbstractItemModel class). The custom model should implement
  headerData() method and return \c true for the Qtx::AppropriateRole role for
  those columns which should be available in the popup menu and \c false for the columns
  which should not be added to it. 
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxTreeView::QtxTreeView( QWidget* parent )
: QTreeView( parent )
{
  setHeader( new Header( false, this ) );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  header()->setMovable( true );
#else
  header()->setSectionsMovable( true );
#endif
}

/*!
  \brief Constructor.
  \param enableSortMenu show "Sorting" header menu command if \c true
  \param parent parent widget
*/
QtxTreeView::QtxTreeView( const bool enableSortMenu, QWidget* parent )
: QTreeView( parent )
{
  setHeader( new Header( enableSortMenu, this ) );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  header()->setMovable( true );
#else
  header()->setSectionsMovable( true );
#endif
}

/*!
  \brief Destructor.
*/
QtxTreeView::~QtxTreeView()
{
}

/*!
  \brief Expand all branches for specified number of levels.
  
  If \c levels < 0, all branches are expanded (the same results can
  be achieved with expandAll() method).

  \param levels number of levels to be opened
  \sa collapseLevels(), setOpened()
*/
void QtxTreeView::expandLevels( const int levels )
{
  setOpened( rootIndex(), levels+1, true );
}

/*!
  \brief Collapse all branches for specified number of levels.
  
  If \c levels < 0, all branches are collapsed (the same results can
  be achieved with collapseAll() method).

  \param levels number of levels to be collapsed
  \sa expandLevels(), setOpened()
*/
void QtxTreeView::collapseLevels( const int levels )
{
  setOpened( rootIndex(), levels+1, false );
}

/*!
  \brief Expand the branch specifed by the \index and all its
  children recursively.
  \param index model index to be expanded
  \sa collapseAll()
*/
void QtxTreeView::expandAll( const QModelIndex& index )
{
  setOpened( index, -1, true );
}

/*!
  \brief Collapse the branch specifed by the \index and all its
  children recursively.
  \param index model index to be collapsed
  \sa expandAll()
*/
void QtxTreeView::collapseAll( const QModelIndex& index )
{
  setOpened( index, -1, false );
}

/*
  \brief Enable/disable "Sorting" popup menu command for the header.
  \param enableSortMenu if \c true, enable "Sorting" menu command
  \sa sortMenuEnabled()
*/
void QtxTreeView::setSortMenuEnabled( const bool enableSortMenu )
{
  Header* h = dynamic_cast<Header*>( header() );
  if ( h )
    h->setSortMenuEnabled( enableSortMenu );
}

/*
  \brief Check if "Sorting" popup menu command for the header is enabled.
  \return \c true if "Sorting" menu command is enabled
  \sa setSortMenuEnabled()
*/
bool QtxTreeView::sortMenuEnabled() const
{
  Header* h = dynamic_cast<Header*>( header() );
  return h ? h->sortMenuEnabled() : false;
}

/*!
  \brief Resizes the given column in order to enclose its contents.
  The size will be changed only if it is smaller than the size of
  contents.
  \param column number of column
*/
void QtxTreeView::resizeColumnToEncloseContents( int column )
{
  if (column < 0 || column >= header()->count())
    return;

  int contentsSizeHint = sizeHintForColumn(column);
  int headerSizeHint = header()->isHidden() ? 0 : header()->sectionSizeHint(column);
  int sizeHint = qMax(contentsSizeHint, headerSizeHint);

  int currentSize = columnWidth( column );
  if (currentSize < sizeHint)
    setColumnWidth( column, sizeHint );
}

/*
  \brief Called when the header section is clicked.
  \param column header column index
*/
void QtxTreeView::onHeaderClicked( int column )
{
  sortByColumn( column, header()->sortIndicatorOrder() );
}

/*!
  \brief Called when the selection is changed.
  
  Emits selectionChanged() signal.
  
  \param selected new selection
  \param deselected previous selection
*/
void QtxTreeView::selectionChanged( const QItemSelection& selected, 
                                    const QItemSelection& deselected )
{
  QTreeView::selectionChanged( selected, deselected );
  emit( selectionChanged() );
}

/*!
  \brief Called when rows are about to be removed.
  \param parent model index
  \param start first row to remove
  \param end last row to remove
*/
void QtxTreeView::rowsAboutToBeRemoved( const QModelIndex& parent, int start, int end )
{
  QModelIndex curIndex = currentIndex();
  while ( curIndex.isValid() && curIndex.parent() != parent )
    curIndex = curIndex.parent();
  if ( curIndex.isValid() && curIndex.row() >= start && curIndex.row() <= end )
    setCurrentIndex( QModelIndex() );
  QTreeView::rowsAboutToBeRemoved( parent, start, end );
}

/*!
  \brief Expand/collapse the specified item (recursively).
  \param index model index
  \param levels number of levels to be expanded/collapsed
  \param open if \c true, item is expanded, otherwise it is collapsed
  \sa expandLevels(), collapseLevels()
*/
void QtxTreeView::setOpened( const QModelIndex& index, const int levels, bool open )
{
  if ( !levels )
    return;

  if ( !index.isValid() && index != rootIndex() )
    return;

  setExpanded( index, open );

  for ( int i = 0; i < model()->rowCount( index ); i++ ) {
    QModelIndex child = model()->index( i, 0, index );
    setOpened( child, levels-1, open );
  }
}

/*!
  \fn QtxTreeView::sortingEnabled( bool on );
  \brief Emitted when "Sorting" commans is enabled/disabled from the popup menu.
  \param on \c true if sorting is enabled and \c false otherwise
*/

/*!
  \fn QtxTreeView::selectionChanged();
  \brief Emitted when selection is changed in the tree view.
*/

/*!
  \brief Emit sortingEnabled(bool) signal.
  \param enabled "enable sorting" flag state
*/
void QtxTreeView::emitSortingEnabled( bool enabled )
{
  emit( sortingEnabled( enabled ) );
}

void QtxTreeView::setModel( QAbstractItemModel* m )
{
  if( model() )
    disconnect( model(), SIGNAL( headerDataChanged( Qt::Orientation, int, int ) ),
                this, SLOT( onAppropriate( Qt::Orientation, int, int ) ) );
  QTreeView::setModel( m );
  if ( model() )
    connect( model(), SIGNAL( headerDataChanged( Qt::Orientation, int, int ) ),
             this, SLOT( onAppropriate( Qt::Orientation, int, int ) ) );
}

void QtxTreeView::onAppropriate( Qt::Orientation orient, int first, int last )
{
  if( orient==Qt::Horizontal )
    for( int i=first; i<=last; i++ )
        {
          int appr = model()->headerData( i, orient, Qtx::AppropriateRole ).toInt();
          header()->setSectionHidden( i, appr==Qtx::Hidden );
        }
}
