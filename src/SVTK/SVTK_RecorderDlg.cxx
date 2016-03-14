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

#include "SVTK_RecorderDlg.h"
#include "SVTK_Recorder.h"

#include <SUIT_FileDlg.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include <QtxDoubleSpinBox.h>
#include <QtxIntSpinBox.h>

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

/*!
 * Constructor
 */
SVTK_RecorderDlg::SVTK_RecorderDlg( QWidget* theParent, SVTK_Recorder* theRecorder ):
  QDialog( theParent ),
  myRecorder( theRecorder )
{
  setWindowTitle( tr( "DLG_RECORDER_TITLE" ) );

  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();

  QVBoxLayout* aTopLayout = new QVBoxLayout( this );
  aTopLayout->setSpacing( 6 );
  aTopLayout->setMargin( 6 );

  // Settings
  QGroupBox* mySettingsBox = new QGroupBox( tr( "SETTINGS" ), this );

  QGridLayout* aSettingsLayout = new QGridLayout( mySettingsBox );
  aSettingsLayout->setSpacing( 6 );
  aSettingsLayout->setMargin( 11 );

  QLabel* aFileNameLabel = new QLabel( tr( "FILE_NAME" ), mySettingsBox );
  myFileNameLineEdit = new QLineEdit( mySettingsBox );
  myFileNameLineEdit->setMinimumWidth( 250 );
  myFileNameLineEdit->setReadOnly( true );

  QLabel* aRecordingModeLabel = new QLabel( tr( "RECORDING_MODE" ), mySettingsBox );
  myRecordingModeComboBox = new QComboBox( mySettingsBox );
  myRecordingModeComboBox->addItem( tr( "SKIPPED_FRAMES" ) );
  myRecordingModeComboBox->addItem( tr( "ALL_DISLPAYED_FRAMES" ) );
  myRecordingModeComboBox->setCurrentIndex( aResourceMgr->integerValue( "VTKViewer", "recorder_mode", 1 ) );

  QLabel* aFPSLabel = new QLabel( tr( "FPS" ), mySettingsBox );
  myFPSSpinBox = new QtxDoubleSpinBox( 0.1, 100.0, 1.0, mySettingsBox );
  myFPSSpinBox->setValue( aResourceMgr->doubleValue( "VTKViewer", "recorder_fps", 10.0 ) );

  QLabel* aQualityLabel = new QLabel( tr( "QUALITY" ), mySettingsBox );
  myQualitySpinBox = new QtxIntSpinBox( 1, 100, 1, mySettingsBox );
  myQualitySpinBox->setValue( aResourceMgr->integerValue( "VTKViewer", "recorder_quality", 80 ) );

  myProgressiveCheckBox = new QCheckBox( tr( "PROGRESSIVE" ), mySettingsBox );
  myProgressiveCheckBox->setChecked( aResourceMgr->booleanValue( "VTKViewer", "recorder_progressive", false ) );

  aSettingsLayout->addWidget( aFileNameLabel, 0, 0 );
  aSettingsLayout->addWidget( myFileNameLineEdit, 1, 0, 1, 2 );
  aSettingsLayout->addWidget( aRecordingModeLabel, 2, 0 );
  aSettingsLayout->addWidget( myRecordingModeComboBox, 2, 1 );
  aSettingsLayout->addWidget( aFPSLabel, 3, 0 );
  aSettingsLayout->addWidget( myFPSSpinBox, 3, 1 );
  aSettingsLayout->addWidget( aQualityLabel, 4, 0 );
  aSettingsLayout->addWidget( myQualitySpinBox, 4, 1 );
  aSettingsLayout->addWidget( myProgressiveCheckBox, 5, 0 );

  aTopLayout->addWidget( mySettingsBox );

  // Start / Close
  QGroupBox* CommonGroup = new QGroupBox( this );
  QGridLayout* CommonGroupLayout = new QGridLayout( CommonGroup );
  CommonGroupLayout->setAlignment( Qt::AlignTop );
  CommonGroupLayout->setSpacing( 6 );
  CommonGroupLayout->setMargin( 11 );

  QPushButton* aStartButton = new QPushButton( tr( "START" ), CommonGroup );
  aStartButton->setAutoDefault( true );
  aStartButton->setDefault( true );
  CommonGroupLayout->addWidget( aStartButton, 0, 0 );
  CommonGroupLayout->addItem( new QSpacerItem( 5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 1 );

  QPushButton* aCloseButton = new QPushButton( tr( "CLOSE" ), CommonGroup );
  aCloseButton->setAutoDefault( true );
  CommonGroupLayout->addWidget( aCloseButton, 0, 2 );

  //QPushButton* aHelpButton = new QPushButton( tr( "HELP" ), CommonGroup );
  //aHelpButton->setAutoDefault( true );
  //CommonGroupLayout->addWidget( aHelpButton, 0, 3 );

  aTopLayout->addWidget( CommonGroup );

  connect( aStartButton, SIGNAL( clicked() ), this, SLOT( onStart() ) );
  connect( aCloseButton, SIGNAL( clicked() ), this, SLOT( onClose() ) );
  //connect( aHelpButton,  SIGNAL( clicked() ), this, SLOT( onHelp() ) );
}

SVTK_RecorderDlg::~SVTK_RecorderDlg()
{
}

void SVTK_RecorderDlg::onStart()
{
  if( myFileName.isNull() )
    return;

  myRecorder->SetName( (const char*)myFileName.toLatin1() );

  myRecorder->SetUseSkippedFrames( myRecordingModeComboBox->currentIndex() == 0 );
  myRecorder->SetNbFPS( myFPSSpinBox->value() );
  myRecorder->SetQuality( myQualitySpinBox->value() );
  myRecorder->SetProgressiveMode( myProgressiveCheckBox->isChecked() );

  accept();

}

void SVTK_RecorderDlg::onClose()
{
  reject();
}

void SVTK_RecorderDlg::onHelp()
{
  /*
  QString aHelpFileName = "animation_in_gauss_viewer.htm";
  LightApp_Application* app = (LightApp_Application*)(SUIT_Session::session()->activeApplication());
  if (app)
    app->onHelpContextModule(app->activeModule() ? app->moduleName(app->activeModule()->moduleName()) : QString(""), aHelpFileName);
  else {
                QString platform;
#ifdef WIN32
                platform = "winapplication";
#else
                platform = "application";
#endif
    SUIT_MessageBox::warning(0, QObject::tr("WRN_WARNING"),
                             QObject::tr("EXTERNAL_BROWSER_CANNOT_SHOW_PAGE").
                             arg(app->resourceMgr()->stringValue("ExternalBrowser", platform)).arg(aHelpFileName) );
  }
  */
}

bool SVTK_RecorderDlg::onBrowseFile()
{
  QStringList aFilter;
  aFilter.append( tr( "FLT_AVI_FILES" ) );
  aFilter.append( tr( "FLT_ALL_FILES" ) );

  QString aFileName = SUIT_FileDlg::getFileName( this, getenv( "HOME" ), aFilter,
                                                 tr( "FILE_NAME" ), false );

  if( aFileName.isNull() )
    return false;

  myFileName = aFileName;
  myFileNameLineEdit->setText( aFileName.section( '/', -1 ) );

  return true;
}

int SVTK_RecorderDlg::exec()
{
  if( !onBrowseFile() )
  {
    reject();
    return 0;
  }
  return QDialog::exec();
}

void SVTK_RecorderDlg::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  if ( e->key() == Qt::Key_F1 )
    {
      e->accept();
      onHelp();
    }
}
