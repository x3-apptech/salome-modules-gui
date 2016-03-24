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

#include "OCCViewer_CubeAxesDlg.h"

#include "OCCViewer_FontWidget.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewWindow.h"

#include "QtxAction.h"
#include "QtxIntSpinBox.h"

#include <Basics_OCCTVersion.hxx>

#if OCC_VERSION_LARGE > 0x06080000
#include <Graphic3d_GraduatedTrihedron.hxx>
#endif

#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>

/*!
  \class OCCViewer_CubeAxesDlg::AxisWidget
  \brief Axis tab widget of the "Graduated axis" dialog box
  \internal
*/

/*!
  Constructor
*/
OCCViewer_AxisWidget::OCCViewer_AxisWidget (QWidget* theParent)
: ViewerTools_AxisWidgetBase(theParent)
{
}

/*!
  Destructor
*/
OCCViewer_AxisWidget::~OCCViewer_AxisWidget()
{
}

/*!
  Create font widget
*/
ViewerTools_FontWidgetBase* OCCViewer_AxisWidget::createFontWidget( QWidget* theParent )
{
  OCCViewer_FontWidget* aFontWidget = new OCCViewer_FontWidget( theParent );
  aFontWidget->Initialize();
  return aFontWidget;
}

/*!
  Set axis data
*/
void OCCViewer_AxisWidget::SetData( const AxisData& theAxisData )
{
  myNameGrp->setChecked( theAxisData.DrawName );
  myAxisName->setText( theAxisData.Name );
  myNameFont->SetData( theAxisData.NameColor, 0, false, false, false );

  myLabelsGrp->setChecked( theAxisData.DrawValues );
  myLabelNumber->setValue( theAxisData.NbValues );
  myLabelOffset->setValue( theAxisData.Offset );
  myLabelsFont->SetData( theAxisData.Color, 0, false, false, false );

  myTicksGrp->setChecked( theAxisData.DrawTickmarks );
  myTickLength->setValue( theAxisData.TickmarkLength );
}

/*!
  Get axis data
*/
void OCCViewer_AxisWidget::GetData( AxisData& theAxisData )
{
  theAxisData.DrawName = myNameGrp->isChecked();
  theAxisData.Name = myAxisName->text();

  int aNameFamily;
  bool aNameBold, aNameItalic, aNameShadow;
  myNameFont->GetData( theAxisData.NameColor, aNameFamily, aNameBold, aNameItalic, aNameShadow );

  theAxisData.DrawValues = myLabelsGrp->isChecked();
  theAxisData.NbValues = myLabelNumber->value();
  theAxisData.Offset = myLabelOffset->value();

  int aLabelFamily;
  bool aLabelBold, aLabelItalic, aLabelShadow;
  myLabelsFont->GetData( theAxisData.Color, aLabelFamily, aLabelBold, aLabelItalic, aLabelShadow );

  theAxisData.DrawTickmarks = myTicksGrp->isChecked();
  theAxisData.TickmarkLength = myTickLength->value();
}

/*
  Class       : OCCViewer_CubeAxesDlg
  Description : Dialog for specifying cube axes properties
*/

/*!
  Constructor
*/
OCCViewer_CubeAxesDlg::OCCViewer_CubeAxesDlg(QtxAction* theAction,
                                             OCCViewer_ViewWindow* theParent,
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
OCCViewer_CubeAxesDlg::~OCCViewer_CubeAxesDlg()
{
}

/*!
  Create axis widget
*/
ViewerTools_AxisWidgetBase* OCCViewer_CubeAxesDlg::createAxisWidget( QWidget* theParent )
{
  OCCViewer_AxisWidget* anAxisWidget = new OCCViewer_AxisWidget( theParent );
  anAxisWidget->initialize();
  return anAxisWidget;
}

/*!
  Update dialog fields, connect signals and slots, show dialog
*/
void OCCViewer_CubeAxesDlg::initialize()
{
  ViewerTools_CubeAxesDlgBase::initialize();

  for( int i = 0; i < 3; i++ )
  {
    if( OCCViewer_AxisWidget* anAxisWidget = dynamic_cast<OCCViewer_AxisWidget*>( myAxes[ i ] ) )
    {
      OCCViewer_AxisWidget::AxisData anAxisData;
      switch( i )
      {
        case 0:
          anAxisData.Name = "X";
          anAxisData.NameColor = anAxisData.Color = Qt::red;
          break;
        case 1:
          anAxisData.Name = "Y";
          anAxisData.NameColor = anAxisData.Color = Qt::green;
          break;
        case 2:
          anAxisData.Name = "Z";
          anAxisData.NameColor = anAxisData.Color = Qt::blue;
          break;
      }
      anAxisData.DrawName       = true;
      anAxisData.DrawValues     = true;
      anAxisData.NbValues       = 3;
      anAxisData.Offset         = 2;
      anAxisData.DrawTickmarks  = true;
      anAxisData.TickmarkLength = 5;
      anAxisWidget->SetData( anAxisData );
    }
  }
}

/*!
  Update dialog fields, connect signals and slots, show dialog
*/
void OCCViewer_CubeAxesDlg::Update()
{
  ViewerTools_CubeAxesDlgBase::Update();
}

/*!
  Verify validity of entry data
*/
bool OCCViewer_CubeAxesDlg::onApply()
{
  bool isOk = ViewerTools_CubeAxesDlgBase::onApply();

  try
  {
    QWidget *aCurrWid = this->focusWidget();
    aCurrWid->clearFocus();
    aCurrWid->setFocus();

    if( OCCViewer_ViewPort3d* aViewPort = myMainWindow->getViewPort() )
    {
      Handle(V3d_View) aView = aViewPort->getView();
      if( !aView.IsNull() )
        ApplyData( aView );
    }
  }
  catch(...)
  {
    isOk = false;
  }

  return isOk;
}

/*!
  Get graduated trihedron parameters
*/
void OCCViewer_CubeAxesDlg::GetData( bool& theIsVisible, OCCViewer_AxisWidget::AxisData theAxisData[3] )
{
  theIsVisible = myIsVisible->isChecked();
  for( int i = 0; i < 3; i++ )
    if( OCCViewer_AxisWidget* anAxisWidget = dynamic_cast<OCCViewer_AxisWidget*>( myAxes[ i ] ) )
      anAxisWidget->GetData( theAxisData[i] );
}

/*!
  Set graduated trihedron parameters
*/
void OCCViewer_CubeAxesDlg::SetData( bool theIsVisible, OCCViewer_AxisWidget::AxisData theAxisData[3] )
{
  myIsVisible->setChecked( theIsVisible );
  for( int i = 0; i < 3; i++ )
    if( OCCViewer_AxisWidget* anAxisWidget = dynamic_cast<OCCViewer_AxisWidget*>( myAxes[ i ] ) )
      anAxisWidget->SetData( theAxisData[i] );
}

/*!
  Apply graduated trihedron parameters
*/
void OCCViewer_CubeAxesDlg::ApplyData( const Handle(V3d_View)& theView )
{
  if( theView.IsNull() )
    return;

  if( myIsVisible->isChecked() )
  {
    OCCViewer_AxisWidget::AxisData anAxisData[3];
    for( int i = 0; i < 3; i++ ) {
      if( OCCViewer_AxisWidget* anAxisWidget = dynamic_cast<OCCViewer_AxisWidget*>( myAxes[ i ] ) )
        anAxisWidget->GetData( anAxisData[i] );
    }

    // A gap used offset of axis names' offset
    // (this hard-coded value will be removed when the
    // font support will be introduced in OCC-6.4)
    int aGap = 20;

#if OCC_VERSION_LARGE > 0x06080000
    Graphic3d_GraduatedTrihedron gt;
    // main params
    gt.SetDrawGrid(Standard_True);       // to draw grid
    gt.SetDrawAxes(Standard_True);       // to draw axes
    gt.SetGridColor(Quantity_NOC_WHITE); // grid color
    // axes params
    for ( int i = 0; i < 3; i++ ) {
      Graphic3d_AxisAspect& aspect = gt.ChangeAxisAspect(i);
      aspect.SetName(anAxisData[i].Name.toLatin1().constData());
      aspect.SetDrawName(anAxisData[i].DrawName);
      aspect.SetDrawValues(anAxisData[i].DrawValues);
      aspect.SetDrawTickmarks(anAxisData[i].DrawTickmarks);
      aspect.SetNameColor(Quantity_Color(anAxisData[i].NameColor.redF(),
                                         anAxisData[i].NameColor.greenF(),
                                         anAxisData[i].NameColor.blueF(),
                                         Quantity_TOC_RGB));
      aspect.SetColor(Quantity_Color(anAxisData[i].Color.redF(),
                                     anAxisData[i].Color.greenF(),
                                     anAxisData[i].Color.blueF(),
                                     Quantity_TOC_RGB));
      aspect.SetTickmarksNumber(anAxisData[i].NbValues-1);
      aspect.SetTickmarksLength(anAxisData[i].TickmarkLength);
      aspect.SetValuesOffset(anAxisData[i].Offset);
      aspect.SetNameOffset(anAxisData[i].Offset + aGap); // see above
    }
    // draw trihedron
    theView->GraduatedTrihedronDisplay(gt);
#else
    theView->GraduatedTrihedronDisplay(
      anAxisData[0].Name.toLatin1().constData(),
      anAxisData[1].Name.toLatin1().constData(),
      anAxisData[2].Name.toLatin1().constData(),
      anAxisData[0].DrawName,
      anAxisData[1].DrawName,
      anAxisData[2].DrawName,
      anAxisData[0].DrawValues,
      anAxisData[1].DrawValues,
      anAxisData[2].DrawValues,
      Standard_True, // draw grid
      Standard_False, // draw axes
      anAxisData[0].NbValues - 1,
      anAxisData[1].NbValues - 1,
      anAxisData[2].NbValues - 1,
      anAxisData[0].Offset,
      anAxisData[1].Offset,
      anAxisData[2].Offset,
      anAxisData[0].Offset + aGap, // see above
      anAxisData[1].Offset + aGap, // see above
      anAxisData[2].Offset + aGap, // see above
      anAxisData[0].DrawTickmarks,
      anAxisData[1].DrawTickmarks,
      anAxisData[2].DrawTickmarks,
      anAxisData[0].TickmarkLength,
      anAxisData[1].TickmarkLength,
      anAxisData[2].TickmarkLength,
      Quantity_NOC_WHITE, // grid color
      Quantity_Color( anAxisData[0].NameColor.redF(),
                      anAxisData[0].NameColor.greenF(),
                      anAxisData[0].NameColor.blueF(),
                      Quantity_TOC_RGB ),
      Quantity_Color( anAxisData[1].NameColor.redF(),
                      anAxisData[1].NameColor.greenF(),
                      anAxisData[1].NameColor.blueF(),
                      Quantity_TOC_RGB ),
      Quantity_Color( anAxisData[2].NameColor.redF(),
                      anAxisData[2].NameColor.greenF(),
                      anAxisData[2].NameColor.blueF(),
                      Quantity_TOC_RGB ),
      Quantity_Color( anAxisData[0].Color.redF(),
                      anAxisData[0].Color.greenF(),
                      anAxisData[0].Color.blueF(),
                      Quantity_TOC_RGB ),
      Quantity_Color( anAxisData[1].Color.redF(),
                      anAxisData[1].Color.greenF(),
                      anAxisData[1].Color.blueF(),
                      Quantity_TOC_RGB ),
      Quantity_Color( anAxisData[2].Color.redF(),
                      anAxisData[2].Color.greenF(),
                      anAxisData[2].Color.blueF(),
                      Quantity_TOC_RGB ) );
#endif // OCC_VERSION_LARGE > 0x06080000
  }
  else
    theView->GraduatedTrihedronErase();

  theView->Redraw();
}
