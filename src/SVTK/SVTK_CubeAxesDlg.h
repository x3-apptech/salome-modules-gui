//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SVTK_CubeAxesDlg.h
//  Author : Sergey LITONIN
//  Module : VISU


#ifndef SVTK_CubeAxesDlg_H
#define SVTK_CubeAxesDlg_H

#include "SVTK_DialogBase.h"

#include <qframe.h>

class QWidget;
class QFrame;
class QPushButton;
class QTabWidget;
class QCheckBox;
class QLineEdit;
class QGroupBox;

class QtxAction;
class QtxIntSpinBox;

class vtkAxisActor2D;
class SVTK_CubeAxesActor2D;

class SVTK_FontWidget;
class SVTK_AxisWidget;
class SVTK_MainWindow;

/*!
 * Class       : SVTK_CubeAxesDlg
 * Description : Dialog for specifynig cube axes properties
 */
class SVTK_CubeAxesDlg : public SVTK_DialogBase
{
  Q_OBJECT

public:
                  SVTK_CubeAxesDlg(QtxAction* theAction,
				   SVTK_MainWindow* theParent,
				   const char* theName);
  virtual         ~SVTK_CubeAxesDlg();

  void            Update();

private slots:
  void            onOk();
  bool            onApply();
  void            onClose();

private:
  QWidget*        createButtonFrame( QWidget* );
  QWidget*        createMainFrame  ( QWidget* );
  bool            isValid() const;

private:
  SVTK_MainWindow *myMainWindow;
  SVTK_CubeAxesActor2D* myActor;

  QTabWidget*     myTabWg;
  QCheckBox*      myIsVisible;

  QPushButton*    myOkBtn;
  QPushButton*    myApplyBtn;
  QPushButton*    myCloseBtn;
  SVTK_AxisWidget* myAxes[ 3 ];
};

/*!
 * Class       : SVTK_AxisWidget
 * Description : Tab of dialog
 */
class SVTK_AxisWidget : public QFrame
{
  Q_OBJECT

public:
                  SVTK_AxisWidget( QWidget* );
                  ~SVTK_AxisWidget();

  void            UseName( const bool );
  void            SetName( const QString& );
  void            SetNameFont( const QColor&, const int, const bool, const bool, const bool );
  bool            ReadData( vtkAxisActor2D* );
  bool            Apply( vtkAxisActor2D* );

private slots:
  void            onNameChecked();
  void            onLabelsChecked();
  void            onTicksChecked();

private:
  void            updateControlState();
  void            setEnabled( QGroupBox*, const bool );

private:
  // name
  QGroupBox*      myNameGrp;
  QCheckBox*      myIsNameVisible;
  QLineEdit*      myAxisName;
  SVTK_FontWidget* myNameFont;

  // labels
  QGroupBox*      myLabelsGrp;
  QCheckBox*      myIsLabelsVisible;
  QtxIntSpinBox*  myLabelNumber;
  QtxIntSpinBox*  myLabelOffset;
  SVTK_FontWidget* myLabelsFont;

  // tick marks
  QGroupBox*      myTicksGrp;
  QCheckBox*      myIsTicksVisible;
  QtxIntSpinBox*  myTickLength;

  friend class SVTK_CubeAxesDlg;
};

#endif
