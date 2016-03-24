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

//  SALOME TOOLSGUI : implementation of desktop "Tools" optioins
//  File   : ToolsGUI_CatalogGeneratorDlg.cxx
//  Author : Nicolas REJNERI
//
#include "ToolsGUI_CatalogGeneratorDlg.h"

#include "SUIT_Application.h"
#include "SUIT_MessageBox.h"
//#include "SUIT_Tools.h"
#include "SUIT_Session.h"

#include <stdlib.h>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>
#include <QStringList>
#include <QRegExp>
#include <QIntValidator>
#include <QFile>

#include <OSD_Process.hxx>
#include <OSD_Path.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_CString.hxx>

#include "utilities.h"

#define SPACING_SIZE             6
#define MARGIN_SIZE             11
#define MIN_EDIT_SIZE          250

/*!
  \class ToolsGUI_CatalogGeneratorDlg
  \brief A dialog box which allows converting the IDL files 
         to the XML description.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
ToolsGUI_CatalogGeneratorDlg::ToolsGUI_CatalogGeneratorDlg( QWidget* parent )
: QDialog( parent )
{
  setModal( true );

  setWindowTitle( tr( "TOOLS_CATALOG_GENERATOR" ) );
  setSizeGripEnabled( true );

  QGridLayout* aTopLayout = new QGridLayout(this);
  aTopLayout->setMargin( MARGIN_SIZE );
  aTopLayout->setSpacing( SPACING_SIZE );

  QGroupBox* filesGrp = new QGroupBox( tr( "TOOLS_FILES") , this );
  filesGrp->setObjectName( "filesGrp" );
  QGridLayout* filesGrpLayout = new QGridLayout( filesGrp );
  filesGrpLayout->setAlignment( Qt::AlignTop );
  filesGrpLayout->setSpacing( SPACING_SIZE );
  filesGrpLayout->setMargin( MARGIN_SIZE  );

  myIdlEdit = new QLineEdit( filesGrp );
  myIdlEdit->setObjectName( "myIdlEdit" );
  myIdlEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myIdlEdit->setMinimumSize( MIN_EDIT_SIZE, 0 );
  myXmlEdit = new QLineEdit( filesGrp );
  myXmlEdit->setObjectName( "myXmlEdit" );
  myXmlEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myXmlEdit->setMinimumSize( MIN_EDIT_SIZE, 0 );

  myBrowseIdlBtn = new QPushButton( tr( "TOOLS_BUT_BROWSE" ), filesGrp );
  myBrowseIdlBtn->setObjectName( "myBrowseIdlBtn" );
  myBrowseXmlBtn = new QPushButton( tr( "TOOLS_BUT_BROWSE" ), filesGrp );
  myBrowseXmlBtn->setObjectName( "myBrowseXmlBtn" );
//  QFontMetrics fm(myBrowseIdlBtn->font());
//  myBrowseIdlBtn->setFixedWidth(fm.width(myBrowseIdlBtn->text()) + 10);
//  myBrowseXmlBtn->setFixedWidth(fm.width(myBrowseXmlBtn->text()) + 10);

  filesGrpLayout->addWidget( new QLabel( tr( "TOOLS_IDL_FILE" ), filesGrp ), 0, 0);
  filesGrpLayout->addWidget( myIdlEdit, 0, 1 );
  filesGrpLayout->addWidget( myBrowseIdlBtn, 0, 2 );
  filesGrpLayout->addWidget( new QLabel( tr( "TOOLS_XML_FILE" ), filesGrp ), 1, 0);
  filesGrpLayout->addWidget( myXmlEdit, 1, 1 );
  filesGrpLayout->addWidget( myBrowseXmlBtn, 1, 2 );

  QGroupBox* supplGrp = new QGroupBox(tr( "TOOLS_SUPPLEMENT" )  , this );
  supplGrp->setObjectName( "SupplGrp" );
  QGridLayout* supplGrpLayout = new QGridLayout( supplGrp );
  supplGrpLayout->setAlignment( Qt::AlignTop );
  supplGrpLayout->setSpacing( SPACING_SIZE );
  supplGrpLayout->setMargin( MARGIN_SIZE  );

  QSize myMinimumSize(int(MIN_EDIT_SIZE*0.3), 0);

  myAuthorEdit = new QLineEdit( supplGrp );
  myAuthorEdit->setObjectName( "myAuthorEdit" );
  myAuthorEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myAuthorEdit->setMinimumSize( myMinimumSize );

  OSD_Process aProcess;
  myAuthorEdit->setText(aProcess.UserName().ToCString());

  myVersionEdit = new QLineEdit( supplGrp );
  myVersionEdit->setObjectName( "myVersion" );
  myVersionEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myVersionEdit->setMinimumSize( myMinimumSize );
  QStringList aList = tr( "INF_VERSION" ).split(QRegExp("\\s+"), QString::SkipEmptyParts);
  myVersionEdit->setText(aList.last());

  myPngEdit = new QLineEdit( supplGrp );
  myPngEdit->setObjectName( "myCompIcon" );
  myPngEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myPngEdit->setMinimumSize( MIN_EDIT_SIZE, 0 );

  myBrowsePngBtn = new QPushButton( tr( "TOOLS_BUT_BROWSE" ), supplGrp );
  myBrowsePngBtn->setObjectName( "myBrowsePngBtn" );

  myCompName = new QLineEdit( supplGrp );
  myCompName->setObjectName( "myCompName" );
  myCompName->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myCompName->setMinimumSize( myMinimumSize );

  myCompUserName = new QLineEdit( supplGrp );
  myCompUserName->setObjectName( "myCompUserName" );
  myCompUserName->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myCompUserName->setMinimumSize( (int)(MIN_EDIT_SIZE*0.3), 0 );

  myCompType = new QLineEdit( supplGrp );
  myCompType->setObjectName( "myCompType" );
  myCompType->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myCompType->setMinimumSize( myMinimumSize );
  myCompType->setText("OTHER");

  myCompMultiStd = new QLineEdit( supplGrp );
  myCompMultiStd->setObjectName( "myCompMultiStd" );
  myCompMultiStd->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myCompMultiStd->setMinimumSize( myMinimumSize );
  myCompMultiStd->setText("1");
  QIntValidator *ivalidator = new QIntValidator(myVersionEdit);
  myCompMultiStd->setValidator(ivalidator);
  
  supplGrpLayout->addWidget( new QLabel( tr( "TOOLS_AUTHOR" ), supplGrp ), 0, 0);
  supplGrpLayout->addWidget( myAuthorEdit, 0, 1 );
  supplGrpLayout->addWidget( new QLabel( tr( "TOOLS_COMP_NAME" ), supplGrp ), 0, 2);
  supplGrpLayout->addWidget(myCompName,0,3);
  supplGrpLayout->addWidget( new QLabel( tr( "TOOLS_COMP_USERNAME" ), supplGrp ), 0, 4);
  supplGrpLayout->addWidget(myCompUserName,0,5);
  supplGrpLayout->addWidget( new QLabel( tr( "TOOLS_VERSION" ), supplGrp ), 1, 0);
  supplGrpLayout->addWidget( myVersionEdit, 1, 1);
  supplGrpLayout->addWidget( new QLabel( tr( "TOOLS_COMP_MULTISTD" ), supplGrp ), 1, 2);
  supplGrpLayout->addWidget(myCompMultiStd,1,3);
  supplGrpLayout->addWidget( new QLabel( tr( "TOOLS_COMP_TYPE" ), supplGrp ), 1, 4);
  supplGrpLayout->addWidget(myCompType,1,5);
  supplGrpLayout->addWidget( new QLabel( tr( "TOOLS_PNG_FILE" ), supplGrp ), 2, 0);
  supplGrpLayout->addWidget( myPngEdit, 2,1,1,4 );
  supplGrpLayout->addWidget( myBrowsePngBtn, 2, 5 );

  QHBoxLayout* aBtnLayout = new QHBoxLayout;
  aBtnLayout->setSpacing( SPACING_SIZE );
  aBtnLayout->setMargin( 0 );

  myApplyBtn = new QPushButton( tr( "TOOLS_BUT_APPLY"  ), this );
  myApplyBtn->setObjectName( "myApplyBtn" );
  myApplyBtn->setAutoDefault( true );
  myApplyBtn->setDefault( true );
  myCloseBtn = new QPushButton( tr( "TOOLS_BUT_CLOSE" ), this );
  myCloseBtn->setObjectName( "myCloseBtn" );
  myCloseBtn->setAutoDefault( true );
  
  aBtnLayout->addWidget( myApplyBtn );
  aBtnLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  aBtnLayout->addWidget( myCloseBtn );

  aTopLayout->addWidget( filesGrp, 0, 0 );
  aTopLayout->addWidget( supplGrp, 1, 0 );
  aTopLayout->addLayout( aBtnLayout, 2, 0 ); 

  /* signals and slots connections */
  connect( myApplyBtn,     SIGNAL( clicked() ), this, SLOT( onApply() ) );
  connect( myCloseBtn,     SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( myBrowseIdlBtn, SIGNAL( clicked() ), this, SLOT( onBrowseBtnClicked() ) );
  connect( myBrowseXmlBtn, SIGNAL( clicked() ), this, SLOT( onBrowseBtnClicked() ) );
  connect( myBrowsePngBtn, SIGNAL( clicked() ), this, SLOT( onBrowseBtnClicked() ) );
  connect( myIdlEdit,      SIGNAL( textChanged( const QString& ) ), this, SLOT( updateButtonState() ) );
  connect( myXmlEdit,      SIGNAL( textChanged( const QString& ) ), this, SLOT( updateButtonState() ) );

  updateButtonState();
}

/*!
  \brief Destructor
*/
ToolsGUI_CatalogGeneratorDlg::~ToolsGUI_CatalogGeneratorDlg()
{  
}

/*!
  \brief Get IDL file name
  \return IDL file name entered by the user
*/
QString ToolsGUI_CatalogGeneratorDlg::getIdlFile()
{
  return myIdlEdit->text().trimmed();
}

/*!
  \brief Get XML file name
  \return XML file name entered by the user
*/
QString ToolsGUI_CatalogGeneratorDlg::getXmlFile()
{
  return myXmlEdit->text().trimmed();
}

/*!
  \brief Get module icon file name
  \return icon file name entered by the user
*/
QString ToolsGUI_CatalogGeneratorDlg::getPngFile()
{
  return myPngEdit->text().trimmed();
}

/*!
  \brief Get author name
  \return author name entered by the user
*/
QString ToolsGUI_CatalogGeneratorDlg::getAuthor()
{
  return myAuthorEdit->text().trimmed();
}

/*!
  \brief Get version number 
  \return version number entered by the user
*/
QString ToolsGUI_CatalogGeneratorDlg::getVersion()
{
  return myVersionEdit->text().trimmed();
}

/*!
  \brief Get component name
  \return name of the component entered by the user
*/
QString ToolsGUI_CatalogGeneratorDlg::getCompName()
{
  return myCompName->text().trimmed();
}

/*!
  \brief Get component title (user name)
  \return title of the component entered by the user
*/
QString ToolsGUI_CatalogGeneratorDlg::getCompUserName()
{
  return myCompUserName->text().trimmed();
}

/*!
  \brief Get multistudy flag
  \return multistudy flag for the component entered by the user
*/
QString ToolsGUI_CatalogGeneratorDlg::getCompMultiStd()
{
  return myCompMultiStd->text().trimmed();
}

/*!
  \brief Get component type
  \return type of the component entered by the user
*/
QString ToolsGUI_CatalogGeneratorDlg::getCompType()
{
  return myCompType->text().trimmed();
}

/*!
  \brief Get IDL path of the modules
  \return IDL path of modules
*/
QString ToolsGUI_CatalogGeneratorDlg::getIdlPath()
{
  cout << "QAD_Desktop::getCatalogue() is not implemented!!";
  if ( true )
    return QString( "" );
/*
  SALOME_ModuleCatalog::ModuleCatalog_var aCatalog = 
    SALOME_ModuleCatalog::ModuleCatalog::_narrow( QAD_Application::getDesktop()->getCatalogue() );

  SALOME_ModuleCatalog::ListOfIAPP_Affich_var list_composants =
    aCatalog->GetComponentIconeList();

  QString IDLpath = "";

  for (unsigned int ind = 0; ind < list_composants->length();ind++) {
    QString modulename = CORBA::string_dup(list_composants[ind].modulename) ;
    
    QCString dir;
    if (dir = getenv( modulename + "_ROOT_DIR")) {
      IDLpath = IDLpath + "-I" + SUIT_Tools::addSlash( SUIT_Tools::addSlash(dir) + 
                                                      SUIT_Tools::addSlash("idl") + 
                                                      SUIT_Tools::addSlash("salome")) + " ";
    }
  }

  //  MESSAGE ( " IDLpath = " << IDLpath);

  return IDLpath;
*/
}

/*!
  \brief Called when user presses "Browse" button
*/
void ToolsGUI_CatalogGeneratorDlg::onBrowseBtnClicked()
{
  QPushButton* send = (QPushButton*)sender();
  
  SUIT_Application* app = SUIT_Session::session()->activeApplication();

  if ( send == myBrowseIdlBtn ) {
    QString file = app->getFileName( true, myIdlEdit->text().trimmed(), tr("TOOLS_MEN_IMPORT_IDL"), tr("TOOLS_MEN_IMPORT"), 0 );
    if ( !file.isEmpty() ) {
      myIdlEdit->setText(file);
    }
  } 
  else if ( send == myBrowseXmlBtn ) {
    QString file = app->getFileName( false, myXmlEdit->text().trimmed(), tr("TOOLS_MEN_EXPORT_XML"), tr("TOOLS_MEN_EXPORT"), 0 );
    if ( !file.isEmpty() ) {
      myXmlEdit->setText(file);
    }
  } else if ( send == myBrowsePngBtn ) {
    QString file = app->getFileName( true, myPngEdit->text().trimmed(), tr("TOOLS_MEN_IMPORT_PNG"), tr("TOOLS_MEN_IMPORT"), 0 );
    if ( !file.isEmpty() ) {
      myPngEdit->setText(file);
    
    }
  }
  updateButtonState();
}

/*!
  \brief Update <OK> button's state.
*/
void ToolsGUI_CatalogGeneratorDlg::updateButtonState()
{
  myApplyBtn->setEnabled( !myIdlEdit->text().trimmed().isEmpty() && 
                          !myXmlEdit->text().trimmed().isEmpty() );
}

/*!
  \brief Called when user presses <Apply> button

  Performs IDL to XML file conversion using \c runIDLparser SALOME utility.
*/
void ToolsGUI_CatalogGeneratorDlg::onApply()
{
  QString IDLpath = getIdlPath();
  QString XmlFile = getXmlFile();
  QString IdlFile = getIdlFile();
  QString Author  = getAuthor();
  QString Version = getVersion();
  QString PngFile = getPngFile();
  QString CompName = getCompName(); //gets component name 
  QString CompUserName = getCompUserName(); //gets component username 
  QString CompType = getCompType(); //gets component type
  QString CompMultiStd = getCompMultiStd();

  if ( !XmlFile.isEmpty() && !IdlFile.isEmpty() ) {
    if ( !QFile::exists( IdlFile ) ) {
      SUIT_MessageBox::critical( this, 
                                 tr("TOOLS_ERR_ERROR"), 
                                 tr("TOOLS_ERR_FILE_NOT_EXIST").arg(IdlFile) );
    }
    else {
      QString command = "";
      if ( getenv("KERNEL_ROOT_DIR")  )
        command = QString( getenv( "KERNEL_ROOT_DIR" ) ) + "/bin/salome/runIDLparser -K " + IDLpath + " -Wbcatalog=" + XmlFile;
      else {
        SUIT_MessageBox::critical( this, 
                                   tr("TOOLS_ERR_ERROR"), 
                                   tr("KERNEL_ROOT_DIR variable is not defined") );
      }

      if (!Author.isEmpty()) command += ",author=" + Author; 
      if (!Version.isEmpty()) command += ",version=" + Version;
      if (!PngFile.isEmpty()) {
        OSD_Path aPath((Standard_CString)PngFile.toLatin1().constData()); 
        TCollection_AsciiString aFile = aPath.Name() + aPath.Extension();
        command += QString(",icon=") + QString(aFile.ToCString());
      }
      if (!CompName.isEmpty()) command += ",name=" + CompName;
      if (!CompUserName.isEmpty()) command += ",username=" + CompUserName;
      if (!CompType.isEmpty()) command += ",type=" + CompType;
      if (!CompMultiStd.isEmpty()) command += ",multistudy=" + CompMultiStd;
      command += " " + IdlFile;
      MESSAGE( "shell command is : " << command.toLatin1().constData() );
      int res;
      res = system( ( char* )( command.toLatin1().constData() ) );
      if ( res == -1 ) {
        MESSAGE( "work failed (system command result = " << res );
      } else if (res == 217) {
        MESSAGE( "shell exec failed (system command result = " << res );
      }
    }
  }
}
