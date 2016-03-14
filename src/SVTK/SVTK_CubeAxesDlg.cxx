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

//  VISU VISUGUI : GUI for SMESH component
//  File   : SVTK_CubeAxesDlg.cxx
//  Author : Sergey LITONIN
//  Module : VISU
//
#include "SVTK_CubeAxesDlg.h"

#include "SVTK_ViewWindow.h"
#include "SVTK_FontWidget.h"

#include "SVTK_CubeAxesActor2D.h"

#include "QtxAction.h"
#include "QtxIntSpinBox.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>

#include <vtkAxisActor2D.h>
#include <vtkTextProperty.h>

/*!
  \class SVTK_CubeAxesDlg::AxisWidget
  \brief Axis tab widget of the "Graduated axis" dialog box
  \internal
*/

/*!
  Constructor
*/
SVTK_AxisWidget::SVTK_AxisWidget (QWidget* theParent)
: ViewerTools_AxisWidgetBase(theParent)
{
}

/*!
  Destructor
*/
SVTK_AxisWidget::~SVTK_AxisWidget()
{
}

ViewerTools_FontWidgetBase* SVTK_AxisWidget::createFontWidget( QWidget* theParent )
{
  SVTK_FontWidget* aFontWidget = new SVTK_FontWidget( theParent );
  aFontWidget->Initialize();
  return aFontWidget;
}

bool SVTK_AxisWidget::ReadData(vtkAxisActor2D* theActor)
{
  if (theActor == 0)
    return false;

  // Name

  bool useName = theActor->GetTitleVisibility();
  QString aTitle(theActor->GetTitle());

  QColor aTitleColor(255, 255, 255);
  int aTitleFontFamily = VTK_ARIAL;
  bool isTitleBold = false;
  bool isTitleItalic = false;
  bool isTitleShadow = false;

  vtkTextProperty* aTitleProp = theActor->GetTitleTextProperty();
  if (aTitleProp !=0)
  {
    double c[ 3 ];
    aTitleProp->GetColor(c);
    aTitleColor.setRgb((int)(c[ 0 ] * 255), (int)(c[ 1 ] * 255), (int)(c[ 2 ] * 255));
    aTitleFontFamily = aTitleProp->GetFontFamily();
    isTitleBold = aTitleProp->GetBold() ? true : false;
    isTitleItalic = aTitleProp->GetItalic() ? true : false;
    isTitleShadow = aTitleProp->GetShadow() ? true : false;
  }

  myNameGrp->setChecked(useName);
  myAxisName->setText(aTitle);
  myNameFont->SetData(aTitleColor, aTitleFontFamily, isTitleBold, isTitleItalic, isTitleShadow);

  // Labels

  bool useLabels = theActor->GetLabelVisibility();
  int nbLabels = theActor->GetNumberOfLabels();
  int anOffset = theActor->GetTickOffset();

  QColor aLabelsColor(255, 255, 255);
  int aLabelsFontFamily = VTK_ARIAL;
  bool isLabelsBold = false;
  bool isLabelsItalic = false;
  bool isLabelsShadow = false;

  vtkTextProperty* aLabelsProp = theActor->GetLabelTextProperty();
  if (aLabelsProp !=0)
  {
    double c[ 3 ];
    aLabelsProp->GetColor(c);
    aLabelsColor.setRgb((int)(c[ 0 ] * 255), (int)(c[ 1 ] * 255), (int)(c[ 2 ] * 255));
    aLabelsFontFamily = aLabelsProp->GetFontFamily();
    isLabelsBold = aLabelsProp->GetBold() ? true : false;
    isLabelsItalic = aLabelsProp->GetItalic() ? true : false;
    isLabelsShadow = aLabelsProp->GetShadow() ? true : false;
  }

  myLabelsGrp->setChecked(useLabels);
  myLabelNumber->setValue(nbLabels);
  myLabelOffset->setValue(anOffset);
  myLabelsFont->SetData(aLabelsColor, aLabelsFontFamily, isLabelsBold, isLabelsItalic, isLabelsShadow);

  // Tick marks
  bool useTickMarks = theActor->GetTickVisibility();
  int aTickLength = theActor->GetTickLength();

  myTicksGrp->setChecked(useTickMarks);
  myTickLength->setValue(aTickLength);

  return true;
}

bool SVTK_AxisWidget::Apply(vtkAxisActor2D* theActor)
{
   if (theActor == 0)
    return false;

  // Name

  theActor->SetTitleVisibility(myNameGrp->isChecked() ? 1 : 0);
  theActor->SetTitle(myAxisName->text().toLatin1());

  QColor aTitleColor(255, 255, 255);
  int aTitleFontFamily = VTK_ARIAL;
  bool isTitleBold = false;
  bool isTitleItalic = false;
  bool isTitleShadow = false;

  myNameFont->GetData(aTitleColor, aTitleFontFamily, isTitleBold, isTitleItalic, isTitleShadow);
  vtkTextProperty* aTitleProp = theActor->GetTitleTextProperty();
  if (aTitleProp)
  {
    aTitleProp->SetColor(aTitleColor.red() / 255.,
                          aTitleColor.green() / 255.,
                          aTitleColor.blue() / 255.);
    aTitleProp->SetFontFamily(aTitleFontFamily);

    aTitleProp->SetBold(isTitleBold ? 1 : 0);
    aTitleProp->SetItalic(isTitleItalic ? 1 : 0);
    aTitleProp->SetShadow(isTitleShadow ? 1 : 0);

    theActor->SetTitleTextProperty(aTitleProp);
  }

  // Labels

  theActor->SetLabelVisibility(myLabelsGrp->isChecked() ? 1 : 0);

  int nbLabels = myLabelNumber->value();
  theActor->SetNumberOfLabels(nbLabels);

  int anOffset = myLabelOffset->value();
  theActor->SetTickOffset(anOffset);

  QColor aLabelsColor(255, 255, 255);
  int aLabelsFontFamily = VTK_ARIAL;
  bool isLabelsBold = false;
  bool isLabelsItalic = false;
  bool isLabelsShadow = false;

  myLabelsFont->GetData(aLabelsColor, aLabelsFontFamily, isLabelsBold, isLabelsItalic, isLabelsShadow);
  vtkTextProperty* aLabelsProp = theActor->GetLabelTextProperty();
  if (aLabelsProp)
  {
    aLabelsProp->SetColor(aLabelsColor.red() / 255.,
                          aLabelsColor.green() / 255.,
                          aLabelsColor.blue() / 255.);
    aLabelsProp->SetFontFamily(aLabelsFontFamily);

    aLabelsProp->SetBold(isLabelsBold ? 1 : 0);
    aLabelsProp->SetItalic(isLabelsItalic ? 1 : 0);
    aLabelsProp->SetShadow(isLabelsShadow ? 1 : 0);

    aLabelsProp->Modified();
    theActor->SetLabelTextProperty(aLabelsProp);
  }


  // Tick marks
  theActor->SetTickVisibility(myTicksGrp->isChecked());
  int aTickLength = myTickLength->value();
  theActor->SetTickLength(aTickLength);

  return true;
}

/*
  Class       : SVTK_CubeAxesDlg
  Description : Dialog for specifynig cube axes properties
*/

/*!
  Constructor
*/
SVTK_CubeAxesDlg::SVTK_CubeAxesDlg(QtxAction* theAction,
                                   SVTK_ViewWindow* theParent,
                                   const char* theName):
  ViewerTools_CubeAxesDlgBase(theAction,
                              theParent, 
                              theName),
  myMainWindow(theParent)
{
}

/*!
  Destructor
*/
SVTK_CubeAxesDlg::~SVTK_CubeAxesDlg()
{
}

/*!
  Create axis widget
*/
ViewerTools_AxisWidgetBase* SVTK_CubeAxesDlg::createAxisWidget( QWidget* theParent )
{
  SVTK_AxisWidget* anAxisWidget = new SVTK_AxisWidget( theParent );
  anAxisWidget->initialize();
  return anAxisWidget;
}

/*!
  Update dialog fields, connect signals and slots, show dialog
*/
void SVTK_CubeAxesDlg::Update()
{
  ViewerTools_CubeAxesDlgBase::Update();

  myActor = myMainWindow->GetCubeAxes();

  ((SVTK_AxisWidget*)myAxes[ 0 ])->ReadData(myActor->GetXAxisActor2D());
  ((SVTK_AxisWidget*)myAxes[ 1 ])->ReadData(myActor->GetYAxisActor2D());
  ((SVTK_AxisWidget*)myAxes[ 2 ])->ReadData(myActor->GetZAxisActor2D());

  myIsVisible->setChecked(myActor->GetVisibility() ? true : false);
}

/*!
  Verify validity of entry data
*/
bool SVTK_CubeAxesDlg::onApply()
{
  bool isOk = ViewerTools_CubeAxesDlgBase::onApply();

  try
  {
    QWidget *aCurrWid = this->focusWidget();
    aCurrWid->clearFocus();
    aCurrWid->setFocus();

    isOk = isOk && ((SVTK_AxisWidget*)myAxes[ 0 ])->Apply(myActor->GetXAxisActor2D());
    isOk = isOk && ((SVTK_AxisWidget*)myAxes[ 1 ])->Apply(myActor->GetYAxisActor2D());
    isOk = isOk && ((SVTK_AxisWidget*)myAxes[ 2 ])->Apply(myActor->GetZAxisActor2D());

    //myActor->SetXLabel(myAxes[ 0 ]->myAxisName->text()); // to remove?
    //myActor->SetYLabel(myAxes[ 1 ]->myAxisName->text()); // to remove?
    //myActor->SetZLabel(myAxes[ 2 ]->myAxisName->text()); // to remove?

    //myActor->SetNumberOfLabels(myActor->GetXAxisActor2D()->GetNumberOfLabels()); // to remove?
    if (myIsVisible->isChecked())
      myActor->VisibilityOn();
    else
      myActor->VisibilityOff();

    if (isOk)
      myMainWindow->Repaint();
  }
  catch(...)
  {
    isOk = false;
  }

  return isOk;
}
