// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include "QDS_Table.h"

class QDS_Table::DeleteFilter : public QObject
{
public:
  DeleteFilter( QObject* = 0 );
  virtual ~DeleteFilter();

  virtual bool eventFilter( QObject*, QEvent* );
};

QDS_Table::DeleteFilter::DeleteFilter( QObject* parent )
: QObject( parent )
{
}

QDS_Table::DeleteFilter::~DeleteFilter()
{
}

bool QDS_Table::DeleteFilter::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::DeferredDelete )
    return true;
  else
    return false;
}

QDS_Table::QDS_Table( QWidget* parent, const char* name )
: QtxTable( parent, name ),
myHorEdit( 0 ),
myVerEdit( 0 ),
myTableEdit( 0 ),
myKeepEdits( false )
{
}

QDS_Table::QDS_Table( int r, int c, QWidget* parent, const char* name )
: QtxTable( r, c, parent, name ),
myHorEdit( 0 ),
myVerEdit( 0 ),
myTableEdit( 0 ),
myKeepEdits( false )
{
  myRowEdit.resize( r );
  myColEdit.resize( c );
}

QDS_Table::~QDS_Table()
{
}

QDS_Datum* QDS_Table::horizontalHeaderEditor() const
{
  return myHorEdit;
}

QDS_Datum* QDS_Table::verticalHeaderEditor() const
{
  return myVerEdit;
}

QDS_Datum* QDS_Table::headerEditor( const Orientation o ) const
{
  return o == Horizontal ? myHorEdit : myVerEdit;
}

void QDS_Table::setVerticalHeaderEditor( QDS_Datum* dat )
{
  setHeaderEditor( Vertical, dat );
}

void QDS_Table::setHorizontalHeaderEditor( QDS_Datum* dat )
{
  setHeaderEditor( Horizontal, dat );
}

void QDS_Table::setHeaderEditor( QDS_Datum* dat )
{
  setHeaderEditor( Vertical, dat );
  setHeaderEditor( Horizontal, dat );
}

void QDS_Table::setHeaderEditor( const Orientation o, QDS_Datum* dat )
{
  if ( headerEditor( o ) == dat )
    return;

  if ( isHeaderEditing() )
    endEditHeader();

  if ( o == Horizontal )
    myHorEdit = dat;
  else
    myVerEdit = dat;

  initEditor( dat );
}

QDS_Datum* QDS_Table::tableEditor() const
{
  return myTableEdit;
}

QDS_Datum* QDS_Table::rowEditor( const int row ) const
{
  if ( row < 0 || row >= (int)myRowEdit.size() )
    return 0;

  return myRowEdit.at( row );
}

QDS_Datum* QDS_Table::columnEditor( const int col ) const
{
  if ( col < 0 || col >= (int)myColEdit.size() )
    return 0;

  return myColEdit.at( col );
}

QDS_Datum* QDS_Table::cellEditor( const int row, const int col ) const
{
  if ( !myCellEdit.contains( row ) )
    return 0;

  const DatumMap& map = myCellEdit[row];
  return map.contains( col ) ? map[col] : 0;
}

void QDS_Table::setTableEditor( QDS_Datum* dat )
{
  if ( tableEditor() == dat )
    return;

  if ( isEditing() && !cellEditor( currEditRow(), currEditCol() ) &&
       !columnEditor( currEditCol() ) && !rowEditor( currEditRow() ) )
    endEdit( currEditRow(), currEditCol(), false, false );

  myTableEdit = dat;
  initEditor( dat );
}

void QDS_Table::setRowEditor( const int row, QDS_Datum* dat )
{
  if ( row < 0 || row >= (int)myRowEdit.size() || rowEditor( row ) == dat )
    return;

  if ( isEditing() && row == currEditRow()&&
       !cellEditor( currEditRow(), currEditCol() ) )
    endEdit( currEditRow(), currEditCol(), false, false );

  myRowEdit.insert( row, dat );
  initEditor( dat );
}

void QDS_Table::setColumnEditor( const int col, QDS_Datum* dat )
{
  if ( col < 0 || col >= (int)myColEdit.size() || columnEditor( col ) == dat )
    return;

  if ( isEditing() && col == currEditCol()&&
       !cellEditor( currEditRow(), currEditCol() ) )
    endEdit( currEditRow(), currEditCol(), false, false );

  myColEdit.insert( col, dat );
  initEditor( dat );
}

void QDS_Table::setCellEditor( const int row, const int col, QDS_Datum* dat )
{
  if ( row < 0 || row >= numRows() || col < 0 || col >= numCols() || cellEditor( row, col ) == dat )
    return;

  if ( isEditing() && currEditRow() == row && currEditCol() == col && actualCellEditor( row, col ) != dat )
    endEdit( currEditRow(), currEditCol(), false, false );

  if ( !myCellEdit.contains( row ) )
    myCellEdit.insert( row, DatumMap() );

  myCellEdit[row].insert( col, dat );
  initEditor( dat );
}

QDS_Datum* QDS_Table::actualCellEditor( const int row, const int col ) const
{
  QDS_Datum* dat = cellEditor( row, col );
  if ( !dat )
    dat = columnEditor( col );
  if ( !dat )
    dat = rowEditor( row );
  if ( !dat )
    dat = tableEditor();
  return dat;
}

void QDS_Table::setNumRows( int r )
{
  int old = numRows();
  QtxTable::setNumRows( r );

  if ( isKeepEditors() )
    myRowEdit.resize( QMAX( (int)myRowEdit.size(), r ) );
  else
  {
    myRowEdit.resize( r );
    for ( int i = r + 1; i <= old; i++ )
      myCellEdit.remove( i );
  }
}

void QDS_Table::setNumCols( int c )
{
  int old = numCols();
  QtxTable::setNumCols( c );

  if ( isKeepEditors() )
    myColEdit.resize( QMAX( (int)myColEdit.size(), c ) );
  else
  {
    myColEdit.resize( c );
    for ( CellMap::Iterator it = myCellEdit.begin(); it != myCellEdit.end(); ++it )
    {
      DatumMap& map = it.data();
      for ( int i = c + 1; i <= old; i++ )
        map.remove( i );
    }
  }
}

void QDS_Table::clearCellWidget( int row, int col )
{
  QDS_Datum* dat = actualCellEditor( row, col );
  if ( dat )
    dat->hide();

  QtxTable::clearCellWidget( row, col );
}

bool QDS_Table::isKeepEditors() const
{
  return myKeepEdits;
}

void QDS_Table::setKeepEditors( const bool on )
{
  myKeepEdits = on;
}

QWidget* QDS_Table::createHeaderEditor( QHeader* header, const int sect, const bool init )
{
  if ( !header )
    return 0;

  QDS_Datum* dat = headerEditor( header->orientation() );
  QWidget* wid = dat ? dat->widget( QDS::Control ) : 0;
  if ( wid )
  {
    if ( init )
      dat->setStringValue( header->label( sect ) );
    else
      dat->clear();
    dat->setProperty( "Selection", true );
  }
  else
    wid = QtxTable::createHeaderEditor( header, sect, init );

  return wid;
}

QWidget* QDS_Table::createEditor( int row, int col, bool init ) const
{
  QDS_Datum* dat = actualCellEditor( row, col );
  QWidget* wid = dat ? dat->widget( QDS::Control ) : 0;
  if ( wid )
  {
    if ( init )
      dat->setStringValue( text( row, col ) );
    else
      dat->clear();
    dat->setProperty( "Selection", true );
  }
  else
    wid = QtxTable::createEditor( row, col, init );

  return wid;
}

void QDS_Table::endEdit( int row, int col, bool accept, bool )
{
  QtxTable::endEdit( row, col, accept, true );
}

void QDS_Table::setCellContentFromEditor( int row, int col )
{
  QDS_Datum* dat = actualCellEditor( row, col );
  if ( dat )
    setText( row, col, dat->stringValue() );
  else
    QtxTable::setCellContentFromEditor( row, col );
}

void QDS_Table::setHeaderContentFromEditor( QHeader* header, const int sect, QWidget* editor )
{
  if ( !header )
    return;

  QDS_Datum* dat = headerEditor( header->orientation() );
  if ( dat )
    header->setLabel( sect, dat->stringValue() );
  else
    QtxTable::setHeaderContentFromEditor( header, sect, editor );
}

void QDS_Table::initEditor( QDS_Datum* dat )
{
  if ( !dat )
    return;

  dat->hide();

  static QGuardedPtr<DeleteFilter> _filter = 0;
  if ( !_filter )
    _filter = new DeleteFilter( 0 );

  if ( dat->widget( QDS::Control ) )
    dat->widget( QDS::Control )->installEventFilter( _filter );
}
