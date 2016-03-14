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

// File:      QtxWorkstackAction.h
// Author:    Sergey TELKOV
//
#ifndef QTXWORKSTACKACTION_H
#define QTXWORKSTACKACTION_H

#include "QtxActionSet.h"
#include <QDialog>

class QtxWorkstack;
class QRadioButton;
class QStackedLayout;
class QListWidget;
class QComboBox;
class QLabel;
class QToolButton;

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
  QAction*      getArrangeViewsAction();

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
  QAction*      myArrangeViewsAction;
};

enum QtxSplitDlgMode{ ArrangeViews, CreateSubViews };

class QTX_EXPORT QtxSplitDlg : public QDialog
{
  Q_OBJECT

  enum ViewMode{ XYZ, XY, XZ, YZ };

public:
  QtxSplitDlg( QWidget* = 0, QtxWorkstack* = NULL, QtxSplitDlgMode = ArrangeViews );
  ~QtxSplitDlg();
  int getSplitMode();
  QList<int> getViewsMode();

private :
  QToolButton* createSplitButton( int, int );
  void initialize();
  void valid();

  QStackedLayout* myStackedSplitLayout;
  QRadioButton* myButton2Views;
  QRadioButton* myButton3Views;
  QRadioButton* myButton4Views;
  QPushButton* myButtonApply;
  QPushButton* myButtonPrevious;
  QPushButton* myButtonNext;
  QList<QLabel*> myLabels;
  QList<QComboBox*> myComboBox;
  QListWidget* myViewsList;

  QtxWorkstack* myWorkstack;
  QtxSplitDlgMode myDlgMode;

  int myViewsNB;
  int mySplitMode;
  int myNBSelectedViews;
  bool myIsCloseViews;
  QMap<QComboBox*, ViewMode> myMapComboBoxMode;
  QMap<ViewMode, bool> myMapModeIsBusy;

private slots:

  void onChangeIcons();
  void onSplitChanged(int);
  void onComboBoxChanged(int);
  void onPreviousViews();
  void onNextViews();
  void onSynchronize();
  void onCloseViews();
  void onStackViews();
  void onApply();
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
