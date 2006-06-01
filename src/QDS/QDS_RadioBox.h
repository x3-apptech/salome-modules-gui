// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef QDS_RADIOBOX_H
#define QDS_RADIOBOX_H

#include "QDS_Datum.h"

#include <Qtx.h>

#include <qmap.h>
#include <qpixmap.h>
#include <qstringlist.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

class QButtonGroup;
class QRadioButton;

class QDS_EXPORT QDS_RadioBox : public QDS_Datum
{
  Q_OBJECT

public:
  QDS_RadioBox( const QString&, QWidget* = 0, const int = All, const QString& = QString::null );
  virtual ~QDS_RadioBox();

  int                        count( bool = false ) const;
  void                       values( QValueList<int>&, bool = false ) const;

  int                        columns() const;
  void                       setColumns( const int );

  bool                       state( const int ) const;
  void                       setState( const bool, const int, const bool = true );
  void                       setState( const bool, const QValueList<int>&, const bool = true );
  void                       setValues( const QValueList<int>&, const QStringList& );
  void                       setValues( const QStringList& );

signals:
  void                       activated( int );

protected slots:
  virtual void               onToggled( bool );

protected:
  QButtonGroup*              buttonGroup() const;
  virtual QWidget*           createControl( QWidget* );
  void                       buttons( QPtrList<QRadioButton>& ) const;

  virtual QString            getString() const;
  virtual void               setString( const QString& );

  virtual void               unitSystemChanged( const QString& );

private:
  void                       updateRadioBox();

private:
  typedef QMap<int, QString> IdValueMap;
  typedef QMap<int, bool>    IdStateMap;

private:
  IdValueMap                 myValue;
  IdStateMap                 myState;

  QIntList                   myDataIds;
  QIntList                   myUserIds;
  QStringList                myUserNames;
};

#ifdef WNT
#pragma warning( default:4251 )
#endif

#endif
