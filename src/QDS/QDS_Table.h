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

#ifndef QDS_TABLE_H
#define QDS_TABLE_H

#include "QDS_Datum.h"

#include <QtxTable.h>

#include <QMap>
#include <QVector>

class QDS_EXPORT QDS_Table : public QtxTable
{
  class DeleteFilter;

  Q_OBJECT

  Q_PROPERTY( bool KeepEditors READ isKeepEditors WRITE setKeepEditors )

public:
  QDS_Table( QWidget* = 0, const char* = 0 );
  QDS_Table( int, int, QWidget* = 0, const char* = 0 );
  virtual ~QDS_Table();

  QDS_Datum*       verticalHeaderEditor() const;
  QDS_Datum*       horizontalHeaderEditor() const;
  QDS_Datum*       headerEditor( const Orientation ) const;

  void             setVerticalHeaderEditor( QDS_Datum* );
  void             setHorizontalHeaderEditor( QDS_Datum* );

  void             setHeaderEditor( QDS_Datum* );
  virtual void     setHeaderEditor( const Orientation, QDS_Datum* );

  QDS_Datum*       tableEditor() const;
  virtual void     setTableEditor( QDS_Datum* );

  QDS_Datum*       rowEditor( const int ) const;
  QDS_Datum*       columnEditor( const int ) const;

  virtual void     setRowEditor( const int, QDS_Datum* );
  virtual void     setColumnEditor( const int, QDS_Datum* );

  QDS_Datum*       cellEditor( const int, const int ) const;
  virtual void     setCellEditor( const int, const int, QDS_Datum* );

  QDS_Datum*       actualCellEditor( const int, const int ) const;

  virtual void     setNumRows( int );
  virtual void     setNumCols( int );
  virtual void     clearCellWidget( int, int );

  bool             isKeepEditors() const;
  void             setKeepEditors( const bool );

protected:
  virtual QWidget* createHeaderEditor( QHeader*, const int, const bool = true );
  virtual QWidget* createEditor( int, int, bool ) const;
  virtual void     endEdit( int, int, bool, bool );
  virtual void     setCellContentFromEditor( int, int );
  virtual void     setHeaderContentFromEditor( QHeader*, const int, QWidget* );

private:
  void             initEditor( QDS_Datum* );
  QDS_Datum*       datum( const QWidget* ) const;

private:
  typedef QVector<QDS_Datum*>         DatumVector;
  typedef QMap<int, QDS_Datum*>       DatumMap;
  typedef QMap<int, DatumMap>         CellMap;

private:
  QDS_Datum*       myHorEdit;
  QDS_Datum*       myVerEdit;
  DatumVector      myRowEdit;
  DatumVector      myColEdit;
  CellMap          myCellEdit;
  QDS_Datum*       myTableEdit;
  bool             myKeepEdits;
};

#endif 
