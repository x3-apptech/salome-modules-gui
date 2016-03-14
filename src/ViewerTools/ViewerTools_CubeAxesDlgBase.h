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

#ifndef VIEWERTOOLS_CUBEAXESDLGBASE_H
#define VIEWERTOOLS_CUBEAXESDLGBASE_H

#include "ViewerTools_DialogBase.h"

#include <QFrame>

class QWidget;
class QPushButton;
class QTabWidget;
class QCheckBox;
class QGroupBox;
class QLineEdit;

class QtxAction;
class QtxIntSpinBox;

class ViewerTools_FontWidgetBase;

/*!
 * Class       : ViewerTools_AxisWidgetBase
 * Description : Axis tab widget of the "Graduated axis" dialog box
*/
class VIEWERTOOLS_EXPORT ViewerTools_AxisWidgetBase : public QFrame
{
public:
  ViewerTools_AxisWidgetBase( QWidget* );
  ~ViewerTools_AxisWidgetBase();

  void             UseName( const bool );
  void             SetName( const QString& );
  void             SetNameFont( const QColor&, const int, const bool, const bool, const bool );

public:
  virtual void     initialize();

protected:
  virtual ViewerTools_FontWidgetBase* createFontWidget( QWidget* ) = 0;

protected:
  // name
  QGroupBox*       myNameGrp;
  QLineEdit*       myAxisName;
  ViewerTools_FontWidgetBase* myNameFont;

  // labels
  QGroupBox*       myLabelsGrp;
  QtxIntSpinBox*   myLabelNumber;
  QtxIntSpinBox*   myLabelOffset;
  ViewerTools_FontWidgetBase* myLabelsFont;

  // tick marks
  QGroupBox*       myTicksGrp;
  QtxIntSpinBox*   myTickLength;
};

/*!
 * Class       : ViewerTools_CubeAxesDlgBase
 * Description : Dialog for specifying cube axes properties
 */
class VIEWERTOOLS_EXPORT ViewerTools_CubeAxesDlgBase : public ViewerTools_DialogBase
{
  Q_OBJECT

public:
                  ViewerTools_CubeAxesDlgBase(QtxAction* theAction,
                                              QWidget* theParent,
                                              const char* theName);
  virtual         ~ViewerTools_CubeAxesDlgBase();

  virtual void    initialize();

  virtual void    Update();

protected slots:
  virtual void    onOk();
  virtual bool    onApply();
  virtual void    onClose();

protected:
  virtual QWidget* createButtonFrame( QWidget* );
  virtual QWidget* createMainFrame  ( QWidget* );
  virtual bool    isValid() const;

  virtual ViewerTools_AxisWidgetBase* createAxisWidget( QWidget* ) = 0;

protected:
  QTabWidget*     myTabWg;
  QCheckBox*      myIsVisible;

  QPushButton*    myOkBtn;
  QPushButton*    myApplyBtn;
  QPushButton*    myCloseBtn;
  ViewerTools_AxisWidgetBase* myAxes[ 3 ];
};

#endif
