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

// File   : Style_PrefDlg.h
// Author : Vadim SANDLER, Open CASCADE S.A.S (vadim.sandler@opencascade.com)
//
#ifndef STYLE_PREFDLG_H
#define STYLE_PREFDLG_H

#include "Style.h"

#include <QtxDialog.h>
#include <QFrame>
#include <QMap>

class QCheckBox;
class QComboBox;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QRadioButton;
class QSlider;
class QSpinBox;
class QTabWidget;
class QtxColorButton;
class QtxDoubleSpinBox;
class QtxFontEdit;
class Style_ResourceMgr;
class Style_Model;

class STYLE_SALOME_EXPORT Style_PrefDlg : public QtxDialog
{
  Q_OBJECT

  class PaletteEditor;
  enum { Current, Default, Global, User };
  enum { TypeRole = Qt::UserRole + 123, NameRole  };

public:
  Style_PrefDlg( QWidget* = 0 );
  ~Style_PrefDlg();

  void               accept();
  bool               eventFilter( QObject*, QEvent* );

private:
  Style_ResourceMgr* resourceMgr();
  void               fromModel( Style_Model* );
  void               toModel( Style_Model* ) const;
  
  QString            findUniqueName( const QString&, QListWidgetItem* = 0, bool = false );

signals:
  void               styleChanged();

private slots:
  void               onApply();
  void               onHelp();

  void               onStyleChanged();
  void               onLinesType();
  void               onChanged();
  void               onItemChanged( QListWidgetItem* );
  void               onEffectChanged( QAbstractButton* );

private:
  Style_ResourceMgr* myResMgr;
  QCheckBox*         myStyleCheck;
  QListWidget*       myStylesList;
  QTabWidget*        myStylesTab;

  PaletteEditor*     myPaletteEditor;
  QtxFontEdit*       myFontEdit;
  QComboBox*         myLinesCombo;
  QSlider*           myLinesTransparency;
  QtxDoubleSpinBox*  myEditRound;
  QtxDoubleSpinBox*  myButtonRound;
  QtxDoubleSpinBox*  myFrameRound;
  QtxDoubleSpinBox*  mySliderRound;
  QCheckBox*         myAntiAliasing;
  QSpinBox*          myHorHandleDelta;
  QSpinBox*          myVerHandleDelta;
  QSpinBox*          mySplitterLength;
  QSpinBox*          mySliderSize;
  QRadioButton*      myEffectNone;
  QRadioButton*      myEffectHighlight;
  QRadioButton*      myEffectAutoRaise;

  QRadioButton*      myCurrentEffect;
};

class Style_PrefDlg::PaletteEditor : public QFrame
{
  Q_OBJECT

public:
  PaletteEditor( QWidget* = 0 );
  ~PaletteEditor();
 
  void               addColumn( const QString& );
  void               addItem( int );
  QList<int>         items() const;

  void               setColor( int, QPalette::ColorGroup, const QColor& );
  void               setColor( int, const QColor&, const QColor& = QColor(), const QColor& = QColor() );
  QColor             color( int, QPalette::ColorGroup ) const;

  bool               isAuto() const;
  void               setAuto( bool );

  void               fromColor( const QColor& );

private:
  static QString     idToName( int );
  QWidget*           line();

private slots:
  void               onQuick();
  void               onAuto();

signals:
  void               changed();

private:
  typedef QMap<QPalette::ColorGroup, QtxColorButton*> Btns;

  QFrame*            myContainer;
  QPushButton*       myQuickButton;
  QCheckBox*         myAutoCheck;
  QLabel*            myActiveLab;
  QLabel*            myInactiveLab;
  QLabel*            myDisabledLab;

  QMap<int, Btns>    myButtons;
  int                myCurrentRow;
  int                myCurrentColumn;

  friend class Style_PrefDlg;
};

#endif // STYLE_PREFDLG_H
