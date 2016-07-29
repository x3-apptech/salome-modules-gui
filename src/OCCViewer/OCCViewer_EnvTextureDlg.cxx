// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "OCCViewer_EnvTextureDlg.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewFrame.h"

// GUI includes
#include <SUIT_Session.h>

// OCCT includes
#include <Graphic3d_TextureEnv.hxx>

// QT Includes
#include <QGroupBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>
#include <QLineEdit>

/*!
  \class OCCViewer_EnvTextureDlg
  \brief Dialog allowing to assign parameters of environment texture
*/

/*!
  \brief Constructor
  \param view - parent widget
*/
OCCViewer_EnvTextureDlg::OCCViewer_EnvTextureDlg( OCCViewer_ViewWindow* view )
  :QDialog( view )
{

  // get current view frame (OCCViewer_ViewWindow->QFrame->OCCViewer_ViewFrame)
  myViewFrame = dynamic_cast<OCCViewer_ViewFrame*>( view->parent()->parent() );
  myView3d = view->getViewPort()->getView();

  setObjectName( "OCCViewer_EnvTextureDlg" );
  setWindowTitle( tr( "ENV_TEXTURE" ) );
  setModal( false );

  setAttribute( Qt::WA_DeleteOnClose, true );

  // Create layout for this dialog
  QVBoxLayout* dlglayout = new QVBoxLayout( this );
  dlglayout->setSpacing( 6 );
  dlglayout->setMargin( 11 );

  // Create "Environment texture" group

  myEnvTextureGroup = new QGroupBox( tr( "ENV_TEXTURE" ) );
  myEnvTextureGroup->setCheckable( true );

  QGridLayout* envTextureLayout = new QGridLayout( myEnvTextureGroup );
  envTextureLayout->setSpacing( 6 );
  envTextureLayout->setMargin( 11 );

  myEnvTextureId = new QComboBox();
  myEnvTextureId->addItem( tr( "ENV_CLOUDS" ) );
  myEnvTextureId->addItem( tr( "ENV_CV" ) );
  myEnvTextureId->addItem( tr( "ENV_MEDIT" ) );
  myEnvTextureId->addItem( tr( "ENV_PEARL" ) );
  myEnvTextureId->addItem( tr( "ENV_SKY1" ) );
  myEnvTextureId->addItem( tr( "ENV_SKY2" ) );
  myEnvTextureId->addItem( tr( "ENV_LINES" ) );
  myEnvTextureId->addItem( tr( "ENV_ROAD" ) );
  myEnvTextureId->addItem( tr( "ENV_CUSTOM" ) );
  myEnvTextureId->setMinimumWidth( 300 );

  myEnvTextureName = new QLineEdit();
  myEnvTextureName->setVisible( false );
  myFileButton = new QPushButton();
  myFileButton->setText("...");
  myFileButton->setVisible( false );

  envTextureLayout->addWidget( myEnvTextureId,   0, 0, 1, 2 );
  envTextureLayout->addWidget( myEnvTextureName, 1, 0 );
  envTextureLayout->addWidget( myFileButton,     1, 1 );

  // Create "Buttons" group

  QGroupBox* groupButtons = new QGroupBox( this );
  QHBoxLayout* groupButtonsLayout = new QHBoxLayout( groupButtons );
  groupButtonsLayout->setSpacing( 6 );
  groupButtonsLayout->setMargin( 11 );

  QPushButton* buttonClose = new QPushButton( tr( "BUT_CLOSE" ) );
  buttonClose->setDefault( true );

  QPushButton* buttonHelp = new QPushButton( tr( "GEOM_BUT_HELP" ) );

  groupButtonsLayout->addStretch();
  groupButtonsLayout->addWidget( buttonClose );
  groupButtonsLayout->addWidget( buttonHelp );

  dlglayout->addWidget( myEnvTextureGroup );
  dlglayout->addWidget( groupButtons );

  // Initializations
  initParam();

  // Signals and slots connections
  connect( myEnvTextureGroup,    SIGNAL( toggled(bool) ),            this, SLOT( onEnvTexture(bool) ) );
  connect( myEnvTextureId,       SIGNAL( currentIndexChanged(int) ), this, SLOT( onTextureChanged() ) );
  connect( myFileButton,         SIGNAL( clicked() ),                this, SLOT( onFileSelectionClicked() ) );
  connect( buttonClose,          SIGNAL( clicked() ),                this, SLOT( close() ) );
  connect( buttonHelp,           SIGNAL( clicked() ),                this, SLOT( ClickOnHelp() ) );
}

/*!
  \brief Destructor
*/
OCCViewer_EnvTextureDlg::~OCCViewer_EnvTextureDlg()
{
}

QString OCCViewer_EnvTextureDlg::getName()
{
  // return the name of object
  return QString( "OCCViewer_EnvTextureDlg" );
}

/*!
  Initialization of initial values of widgets
*/
void OCCViewer_EnvTextureDlg::initParam()
{
  Handle(Graphic3d_TextureEnv) aTexture = myView3d->TextureEnv();
  bool anIsTexture = !aTexture.IsNull();
  myEnvTextureGroup->setChecked( anIsTexture );
  if ( anIsTexture ) {
    int aTextureId = myView3d->TextureEnv()->Name();
    myEnvTextureId->setCurrentIndex( aTextureId );
    if ( aTextureId == myEnvTextureId->count() - 1 ) {
      TCollection_AsciiString aFileName;
      aTexture->Path().SystemName( aFileName );
      myEnvTextureName->setText( QString( aFileName.ToCString() ) );
      myFileButton->setVisible( true );
      myEnvTextureName->setVisible( true );
    }
  }
}

/*!
  SLOT on "Environment texture" group click
*/
void OCCViewer_EnvTextureDlg::onEnvTexture( bool theIsChecked )
{
  if ( theIsChecked )
    onTextureChanged();
  else {
    Handle(Graphic3d_TextureEnv) aTexture;
#if OCC_VERSION_LARGE > 0x07000000
    setEnvTexture( aTexture );
#else
    setEnvTexture( aTexture, V3d_TEX_ALL );
#endif
  }
}

/*!
  SLOT on when user changed the texture
*/
void OCCViewer_EnvTextureDlg::onTextureChanged()
{
  Handle(Graphic3d_TextureEnv) aTexture;
  bool isCustom = myEnvTextureId->currentIndex() == myEnvTextureId->count()-1;
  myEnvTextureName->setVisible( isCustom );
  myFileButton->setVisible( isCustom );
  if( isCustom ) {
    if( myEnvTextureName->text().isEmpty() )
      onFileSelectionClicked();
    aTexture = new Graphic3d_TextureEnv( TCollection_AsciiString( myEnvTextureName->text().toStdString().c_str() ) );
  }
  else {
    myEnvTextureName->setText("");
    Graphic3d_NameOfTextureEnv aStandardTexture;
    QList<Graphic3d_NameOfTextureEnv> aTextures;
    aTextures << Graphic3d_NOT_ENV_CLOUDS << Graphic3d_NOT_ENV_CV   << Graphic3d_NOT_ENV_MEDIT
    		  << Graphic3d_NOT_ENV_PEARL  << Graphic3d_NOT_ENV_SKY1 << Graphic3d_NOT_ENV_SKY2
    		  << Graphic3d_NOT_ENV_LINES  << Graphic3d_NOT_ENV_ROAD;
    aTexture = new Graphic3d_TextureEnv( aTextures.at( myEnvTextureId->currentIndex() ) );
  }
#if OCC_VERSION_LARGE > 0x07000000
  setEnvTexture( aTexture );
#else
  setEnvTexture( aTexture, V3d_TEX_ENVIRONMENT );
#endif
}

/*!
  SLOT on file selection button click
*/
void OCCViewer_EnvTextureDlg::onFileSelectionClicked()
{
  QString selFile = QFileDialog::getOpenFileName( this,tr( "GEOM_SELECT_IMAGE" ),QString(), tr( "OCC_TEXTURE_FILES" ) );
  if ( !selFile.isEmpty() ) {
    myEnvTextureName->setText( selFile );
    onTextureChanged();
  }
}

/*!
  SLOT on help button click: opens a help page
*/
void OCCViewer_EnvTextureDlg::ClickOnHelp()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( app )
    app->onHelpContextModule( "GUI", "occ_3d_viewer_page.html", "env_texture" );
}

/*!
  Sets current texture environment for all view in the viewer
*/
#if OCC_VERSION_LARGE > 0x07000000
void OCCViewer_EnvTextureDlg::setEnvTexture( Handle(Graphic3d_TextureEnv) theTexture)
#else
void OCCViewer_EnvTextureDlg::setEnvTexture( Handle(Graphic3d_TextureEnv) theTexture, V3d_TypeOfSurfaceDetail theMode )
#endif
{
  for ( int i = OCCViewer_ViewFrame::BOTTOM_RIGHT; i <= OCCViewer_ViewFrame::TOP_RIGHT; i++ ) {
    if ( OCCViewer_ViewWindow* aViewWindow = myViewFrame->getView(i) ) {
      Handle(V3d_View) aView = aViewWindow->getViewPort()->getView();
      aView->SetTextureEnv( theTexture );
#if OCC_VERSION_LARGE <= 0x07000000
      aView->SetSurfaceDetail( theMode );
#endif
      aView->Redraw();
    }
  }
}
