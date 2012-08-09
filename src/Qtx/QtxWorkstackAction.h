// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:      QtxWorkstackAction.h
// Author:    Sergey TELKOV
//
#ifndef QTXWORKSTACKACTION_H
#define QTXWORKSTACKACTION_H

#include "QtxActionSet.h"

class QtxWorkstack;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxWorkstackAction : public QtxActionSet
{
  Q_OBJECT

public:
  //! Actions (menu items) ID
  enum { SplitVertical   = 0x0001,   //!< "Split window vertically" operation
         SplitHorizontal = 0x0002,   //!< "Split window horizontally" operation
         Windows         = 0x0010,   //!< A list of child windows menu items
         Split           = SplitVertical | SplitHorizontal,
         Standard        = Split | Windows };

  QtxWorkstackAction( QtxWorkstack*, QObject* = 0 );
  virtual ~QtxWorkstackAction();

  QtxWorkstack* workstack() const;

  int           menuActions() const;
  void          setMenuActions( const int );

  QIcon         icon( const int ) const;
  QString       text( const int ) const;
  int           accel( const int ) const;
  QString       statusTip( const int ) const;

  void          setAccel( const int, const int );
  void          setIcon( const int, const QIcon& );
  void          setText( const int, const QString& );
  void          setStatusTip( const int, const QString& );

  void          perform( const int );

private slots:
  void          onAboutToShow();
  void          onTriggered( int );

protected:
  virtual void  addedTo( QWidget* );
  virtual void  removedFrom( QWidget* );

private:
  void          updateContent();
  void          updateWindows();
  void          splitVertical();
  void          splitHorizontal();
  void          activateItem( const int );

private:
  QtxWorkstack* myWorkstack;       //!< parent workstack
  bool          myWindowsFlag;     //!< "show child windows items" flag
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
