//  VISU VISUGUI : GUI for SMESH component
//
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
//  File   : SVTK_CubeAxesDlg.cxx
//  Author : Sergey LITONIN
//  Module : VISU

#include "SVTK_CubeAxesDlg.h"

#include "SVTK_MainWindow.h"
#include "SVTK_FontWidget.h"

#include "SVTK_CubeAxesActor2D.h"

#include "QtxAction.h"
#include "QtxIntSpinBox.h"

#include <qlayout.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qobjectlist.h>
#include <qvalidator.h>

#include <vtkAxisActor2D.h>
#include <vtkTextProperty.h>

/*!
 * Class       : AxisWg
 * Description : Tab of dialog
 */

/*!
  Constructor
*/
SVTK_AxisWidget::SVTK_AxisWidget (QWidget* theParent)
:  QFrame(theParent)
{
  QValueList< QLabel* > aLabels;

  // "Name" grp

  myNameGrp = new QGroupBox(3, Qt::Vertical, tr("AXIS_NAME"), this);
  myIsNameVisible = new QCheckBox(tr("IS_VISIBLE"), myNameGrp);

  QHBox* aHBox = new QHBox(myNameGrp);
  aHBox->setSpacing(5);
  QLabel* aLabel = new QLabel(tr("NAME"), aHBox);
  myAxisName = new QLineEdit(aHBox);
  aLabels.append(aLabel);

  aHBox = new QHBox(myNameGrp);
  aHBox->setSpacing(5);
  aLabel = new QLabel(tr("FONT"), aHBox);
  myNameFont = new SVTK_FontWidget(aHBox);
  aLabels.append(aLabel);


  // "Labels" grp

  myLabelsGrp = new QGroupBox(4, Qt::Vertical, tr("LABELS"), this);
  myIsLabelsVisible = new QCheckBox(tr("IS_VISIBLE"), myLabelsGrp);

  aHBox = new QHBox(myLabelsGrp);
  aHBox->setSpacing(5);
  aLabel = new QLabel(tr("NUMBER"), aHBox);
  myLabelNumber = new QtxIntSpinBox(0,25,1,aHBox,"SpinBoxLabelNumber");
  aLabels.append(aLabel);

  aHBox = new QHBox(myLabelsGrp);
  aHBox->setSpacing(5);
  aLabel = new QLabel(tr("OFFSET"), aHBox);
  myLabelOffset = new QtxIntSpinBox(0,100,1,aHBox,"SpinBoxLabellOffset");
  aLabels.append(aLabel);

  aHBox = new QHBox(myLabelsGrp);
  aHBox->setSpacing(5);
  aLabel = new QLabel(tr("FONT"), aHBox);
  myLabelsFont = new SVTK_FontWidget(aHBox);
  aLabels.append(aLabel);

  // "Tick marks" grp

  myTicksGrp = new QGroupBox(2, Qt::Vertical, tr("TICK_MARKS"), this);
  myIsTicksVisible = new QCheckBox(tr("IS_VISIBLE"), myTicksGrp);

  aHBox = new QHBox(myTicksGrp);
  aHBox->setSpacing(5);
  aLabel = new QLabel(tr("LENGTH"), aHBox);
  myTickLength = new QtxIntSpinBox(0,100,1,aHBox,"SpinBoxTickLength");
  
  aLabels.append(aLabel);

  // Layout

  QVBoxLayout* aLay = new QVBoxLayout(this, 0, 5);
  aLay->addWidget(myNameGrp);
  aLay->addWidget(myLabelsGrp);
  aLay->addWidget(myTicksGrp);

  // init
  myIsNameVisible->setChecked(true);
  myIsLabelsVisible->setChecked(true);
  myIsTicksVisible->setChecked(true);
  updateControlState();

  // Adjust label widths
  QValueList< QLabel* >::iterator anIter;
  int aMaxWidth = 0;
  for (anIter = aLabels.begin(); anIter != aLabels.end(); anIter++)
    aMaxWidth = QMAX(aMaxWidth, (*anIter)->sizeHint().width());
  for (anIter = aLabels.begin(); anIter != aLabels.end(); anIter++)
    (*anIter)->setFixedWidth(aMaxWidth);

  // connect signals and slots
  connect(myIsNameVisible, SIGNAL(stateChanged(int)), SLOT(onNameChecked()));
  connect(myIsLabelsVisible, SIGNAL(stateChanged(int)), SLOT(onLabelsChecked()));
  connect(myIsTicksVisible, SIGNAL(stateChanged(int)), SLOT(onTicksChecked()));
}

/*!
  Destructor
*/
SVTK_AxisWidget::~SVTK_AxisWidget()
{
}

void SVTK_AxisWidget::updateControlState()
{
  onNameChecked();
  onLabelsChecked();
  onTicksChecked();
}

void SVTK_AxisWidget::setEnabled(QGroupBox* theGrp, const bool theState)
{
  QObjectList aChildren(*theGrp->children());
  QObject* anObj;
  for(anObj = aChildren.first(); anObj !=0; anObj = aChildren.next())
    if (anObj !=0 && anObj->inherits("QHBox"))
      ((QHBox*)anObj)->setEnabled(theState);
}

void SVTK_AxisWidget::onLabelsChecked()
{
  setEnabled(myLabelsGrp, myIsLabelsVisible->isChecked());
}

void SVTK_AxisWidget::onTicksChecked()
{
  setEnabled(myTicksGrp, myIsTicksVisible->isChecked());
}

void SVTK_AxisWidget::onNameChecked()
{
  setEnabled(myNameGrp, myIsNameVisible->isChecked());
}

void SVTK_AxisWidget::UseName(const bool toUse)
{
  myIsNameVisible->setChecked(toUse);
}

void SVTK_AxisWidget::SetName(const QString& theName)
{
  myAxisName->setText(theName);
}

void SVTK_AxisWidget::SetNameFont(const QColor& theColor,
                                  const int theFont,
                                  const bool theIsBold,
                                  const bool theIsItalic,
                                  const bool theIsShadow)
{
  myNameFont->SetData(theColor, theFont, theIsBold, theIsItalic, theIsShadow);
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
    vtkFloatingPointType c[ 3 ];
    aTitleProp->GetColor(c);
    aTitleColor.setRgb((int)(c[ 0 ] * 255), (int)(c[ 1 ] * 255), (int)(c[ 2 ] * 255));
    aTitleFontFamily = aTitleProp->GetFontFamily();
    isTitleBold = aTitleProp->GetBold() ? true : false;
    isTitleItalic = aTitleProp->GetItalic() ? true : false;
    isTitleShadow = aTitleProp->GetShadow() ? true : false;
  }

  myIsNameVisible->setChecked(useName);
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
    vtkFloatingPointType c[ 3 ];
    aLabelsProp->GetColor(c);
    aLabelsColor.setRgb((int)(c[ 0 ] * 255), (int)(c[ 1 ] * 255), (int)(c[ 2 ] * 255));
    aLabelsFontFamily = aLabelsProp->GetFontFamily();
    isLabelsBold = aLabelsProp->GetBold() ? true : false;
    isLabelsItalic = aLabelsProp->GetItalic() ? true : false;
    isLabelsShadow = aLabelsProp->GetShadow() ? true : false;
  }

  myIsLabelsVisible->setChecked(useLabels);
  myLabelNumber->setValue(nbLabels);
  myLabelOffset->setValue(anOffset);
  myLabelsFont->SetData(aLabelsColor, aLabelsFontFamily, isLabelsBold, isLabelsItalic, isLabelsShadow);

  // Tick marks
  bool useTickMarks = theActor->GetTickVisibility();
  int aTickLength = theActor->GetTickLength();

  myIsTicksVisible->setChecked(useTickMarks);
  myTickLength->setValue(aTickLength);

  return true;
}

bool SVTK_AxisWidget::Apply(vtkAxisActor2D* theActor)
{
   if (theActor == 0)
    return false;

  // Name

  theActor->SetTitleVisibility(myIsNameVisible->isChecked() ? 1 : 0);
  theActor->SetTitle(myAxisName->text().latin1());

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

  theActor->SetLabelVisibility(myIsLabelsVisible->isChecked() ? 1 : 0);

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
  theActor->SetTickVisibility(myIsTicksVisible->isChecked());
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
				   SVTK_MainWindow* theParent,
				   const char* theName):
  SVTK_DialogBase(theAction,
		  theParent, 
		  theName),
  myMainWindow(theParent)
{
  setCaption(tr("CAPTION"));

  QVBoxLayout* aLay = new QVBoxLayout(this, 5, 5);
  aLay->addWidget(createMainFrame(this));
  aLay->addWidget(createButtonFrame(this));

  connect(theParent, SIGNAL(Show( QShowEvent * )), this, SLOT(onParentShow()));
  connect(theParent, SIGNAL(Hide( QHideEvent * )), this, SLOT(onParentHide()));
}

/*!
  Create frame containing dialog's input fields
*/
QWidget* SVTK_CubeAxesDlg::createMainFrame(QWidget* theParent)
{
  QFrame* aFrame = new QFrame(theParent);

  myTabWg = new QTabWidget(aFrame);

  myAxes[ 0 ] = new SVTK_AxisWidget(myTabWg);
  myAxes[ 1 ] = new SVTK_AxisWidget(myTabWg);
  myAxes[ 2 ] = new SVTK_AxisWidget(myTabWg);

  myTabWg->addTab(myAxes[ 0 ], tr("X_AXIS"));
  myTabWg->addTab(myAxes[ 1 ], tr("Y_AXIS"));
  myTabWg->addTab(myAxes[ 2 ], tr("Z_AXIS"));

  myTabWg->setMargin(5);

  myIsVisible = new QCheckBox(tr("IS_VISIBLE"), aFrame);

  QVBoxLayout* aLay = new QVBoxLayout(aFrame, 0, 5);
  aLay->addWidget(myTabWg);
  aLay->addWidget(myIsVisible);

  return aFrame;
}

/*!
  Create frame containing buttons
*/
QWidget* SVTK_CubeAxesDlg::createButtonFrame(QWidget* theParent)
{
  QFrame* aFrame = new QFrame(theParent);
  aFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);

  myOkBtn    = new QPushButton(tr("BUT_OK"), aFrame);
  myApplyBtn = new QPushButton(tr("BUT_APPLY"), aFrame);
  myCloseBtn = new QPushButton(tr("BUT_CLOSE"), aFrame);

  QSpacerItem* aSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

  QHBoxLayout* aLay = new QHBoxLayout(aFrame, 5, 5);

  aLay->addWidget(myOkBtn);
  aLay->addWidget(myApplyBtn);
  aLay->addItem(aSpacer);
  aLay->addWidget(myCloseBtn);

  connect(myOkBtn,    SIGNAL(clicked()), SLOT(onOk()));
  connect(myApplyBtn, SIGNAL(clicked()), SLOT(onApply()));
  connect(myCloseBtn, SIGNAL(clicked()), SLOT(onClose()));

  return aFrame;
}

/*!
  Destructor
*/
SVTK_CubeAxesDlg::~SVTK_CubeAxesDlg()
{
}

/*!
  Update dialog fields, connect signals and slots, show dialog
*/
void SVTK_CubeAxesDlg::Update()
{
  myActor = myMainWindow->GetCubeAxes();

  myAxes[ 0 ]->ReadData(myActor->GetXAxisActor2D());
  myAxes[ 1 ]->ReadData(myActor->GetYAxisActor2D());
  myAxes[ 2 ]->ReadData(myActor->GetZAxisActor2D());

  myIsVisible->setChecked(myActor->GetVisibility() ? true : false);
}

/*!
  Verify validity of entry data
*/
bool SVTK_CubeAxesDlg::isValid() const
{
  return true;
}

/*!
  Verify validity of entry data
*/
bool SVTK_CubeAxesDlg::onApply()
{
  bool isOk = true;

  try
  {
    QWidget *aCurrWid = this->focusWidget();
    aCurrWid->clearFocus();
    aCurrWid->setFocus();

    isOk = isOk && myAxes[ 0 ]->Apply(myActor->GetXAxisActor2D());
    isOk = isOk && myAxes[ 1 ]->Apply(myActor->GetYAxisActor2D());
    isOk = isOk && myAxes[ 2 ]->Apply(myActor->GetZAxisActor2D());


    //myActor->SetXLabel(myAxes[ 0 ]->myAxisName->text());
    //myActor->SetYLabel(myAxes[ 1 ]->myAxisName->text());
    //myActor->SetZLabel(myAxes[ 2 ]->myAxisName->text());

    //myActor->SetNumberOfLabels(myActor->GetXAxisActor2D()->GetNumberOfLabels());
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

/*!
  SLOT called when "Ok" button pressed.
*/
void SVTK_CubeAxesDlg::onOk()
{
  if (onApply())
    onClose();
}

/*!
  SLOT: called when "Close" button pressed. Close dialog
*/
void SVTK_CubeAxesDlg::onClose()
{
  reject();
}
