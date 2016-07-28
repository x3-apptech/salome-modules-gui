// Copyright (C) 2015-2016  OPEN CASCADE
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
// File   : PyEditor_SettingsDlg.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyEditor_SettingsDlg.h"

#include "PyEditor_Editor.h"
#include "PyEditor_Settings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

/*!
  \class PyEditor_SettingsDlg
  \brief Dialog settings for python editor.
*/

/*!
  \brief Constructor.
  \param theEditor widget that is used to edit and display text
  \param theParent parent widget
*/
PyEditor_SettingsDlg::PyEditor_SettingsDlg( PyEditor_Editor* theEditor,
                                            bool showHelp, QWidget* theParent ) :
  QDialog( theParent ),
  myEditor( theEditor )
{
  setWindowTitle( tr("TIT_PREFERENCES") );
  QVBoxLayout* aMainLayout = new QVBoxLayout( this );
  
  // . Font settings <start>
  QGroupBox* aFontSetBox = new QGroupBox( tr( "GR_FONT_SET" ), this );
  QHBoxLayout* aFontSetLayout = new QHBoxLayout( aFontSetBox );
  myFontFamily = new QFontComboBox( aFontSetBox );
  myFontFamily->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  myFontSize = new QComboBox( aFontSetBox );
  myFontSize->setInsertPolicy( QComboBox::NoInsert );
  myFontSize->setEditable( true );
  myFontSize->setValidator( new QIntValidator( 1, 250, myFontSize ) );
  myFontSize->setSizeAdjustPolicy( QComboBox::AdjustToContents );
  myFontSize->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
  aFontSetLayout->addWidget( myFontFamily );
  aFontSetLayout->addWidget( myFontSize );
  connect( myFontFamily, SIGNAL( currentFontChanged( QFont ) ),
           this, SLOT( onFontChanged() ) );
  aMainLayout->addWidget( aFontSetBox );
  // . Font settings <end>
  
  // . Display settings <start>
  QGroupBox* aDisplaySetBox = new QGroupBox( tr( "GR_DISP_SET" ), this );
  QVBoxLayout* aDisplaySetLayout = new QVBoxLayout( aDisplaySetBox );
  myHighlightCurrentLine = new QCheckBox( tr( "LBL_CURRLINE_HIGHLIGHT" ), aDisplaySetBox );
  myTextWrapping = new QCheckBox( tr( "LBL_TEXT_WRAP" ), aDisplaySetBox );
  myCenterCursorOnScroll = new QCheckBox( tr( "LBL_CURSOR_SCROLL" ), aDisplaySetBox );
  myLineNumberArea = new QCheckBox( tr( "LBL_LINE_NUMBS_AREA" ), aDisplaySetBox );
  aDisplaySetLayout->addWidget( myHighlightCurrentLine );
  aDisplaySetLayout->addWidget( myTextWrapping );
  aDisplaySetLayout->addWidget( myCenterCursorOnScroll );
  aDisplaySetLayout->addWidget( myLineNumberArea );
  aDisplaySetLayout->addStretch( 1 );
  aMainLayout->addWidget( aDisplaySetBox );
  // . Display settings <end>

  // . Tab settings <start>
  QGroupBox* aTabSetBox = new QGroupBox( tr( "GR_TAB_SET" ), this );
  QVBoxLayout* aTabSetLayout = new QVBoxLayout( aTabSetBox );
  myTabSpaceVisible = new QCheckBox( tr( "LBL_TAB_SPACES" ), aTabSetBox );
  QHBoxLayout* aTabSizeLayout = new QHBoxLayout;
  QLabel* aTabSizeLabel = new QLabel( tr( "LBL_TAB_SIZE" ), aTabSetBox );
  myTabSize = new QSpinBox( aTabSetBox );
  myTabSize->setMinimum( 0 );
  myTabSize->setSingleStep( 1 );
  aTabSizeLayout->addWidget( aTabSizeLabel );
  aTabSizeLayout->addWidget( myTabSize );
  aTabSizeLayout->addStretch( 1 );
  aTabSetLayout->addWidget( myTabSpaceVisible );
  aTabSetLayout->addLayout( aTabSizeLayout );
  // . Tab settings <end>

  // . Vertical edge settings <start>
  QGroupBox* aVertEdgeSetBox = new QGroupBox( tr( "GR_VERT_EDGE_SET" ), this );
  QVBoxLayout* aVertEdgeLayout = new QVBoxLayout( aVertEdgeSetBox );
  myVerticalEdge = new QCheckBox( tr( "LBL_VERT_EDGE" ), aVertEdgeSetBox );
  QHBoxLayout* aNumberColLayout = new QHBoxLayout;
  myNumberColumnsLbl = new QLabel( tr( "LBL_NUM_COLUMNS" ), aVertEdgeSetBox );
  myNumberColumns = new QSpinBox( aVertEdgeSetBox );
  myNumberColumns->setMinimum( 0 );
  myNumberColumns->setSingleStep( 1 );
  aNumberColLayout->addWidget( myNumberColumnsLbl );
  aNumberColLayout->addWidget( myNumberColumns );
  aNumberColLayout->addStretch( 1 );
  aVertEdgeLayout->addWidget( myVerticalEdge );
  aVertEdgeLayout->addLayout( aNumberColLayout );
  connect( myVerticalEdge, SIGNAL( clicked( bool ) ), 
           this, SLOT( onVerticalEdgeChecked() ) );
  // . Vertical edge settings <end>

  QHBoxLayout* aTabVertEdgeLayout = new QHBoxLayout;
  aTabVertEdgeLayout->addWidget( aTabSetBox );
  aTabVertEdgeLayout->addWidget( aVertEdgeSetBox );
  aMainLayout->addLayout( aTabVertEdgeLayout );

  // . "Set as default" check box <start>
  if ( PyEditor_Settings::settings() )
  {
    myDefaultCheck = new QCheckBox( tr( "WDG_SET_AS_DEFAULT_CHECK" ), this );
    aMainLayout->addWidget( myDefaultCheck );
  }
  else
  {
    myDefaultCheck = 0;
  }
  // . "Set as default" check box <end>

  // . Control buttons <start>
  QHBoxLayout* aButtonLayout = new QHBoxLayout;

  QPushButton* okBtn = new QPushButton( tr( "BUT_OK" ), this );
  okBtn->setAutoDefault( true );
  okBtn->setDefault( true );
  connect( okBtn, SIGNAL( clicked() ), this, SLOT( onOk() ) );
  aButtonLayout->addWidget( okBtn );

  if ( PyEditor_Settings::settings() )
  {
    QPushButton* defBtn = new QPushButton( tr( "BUT_DEFAULTS" ), this );
    defBtn->setAutoDefault( true );
    connect( defBtn, SIGNAL( clicked() ), this, SLOT( onDefault() ) );
    aButtonLayout->addStretch();
    aButtonLayout->addWidget( defBtn );
  }

  QPushButton* cancelBtn = new QPushButton( tr( "BUT_CANCEL" ), this );
  cancelBtn->setAutoDefault( true );
  connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );
  aButtonLayout->addStretch();
  aButtonLayout->addWidget( cancelBtn );

  if ( showHelp )
  {
    QPushButton* helpBtn = new QPushButton( tr( "BUT_HELP" ), this );
    helpBtn->setAutoDefault( true );
    connect( helpBtn, SIGNAL( clicked() ), this, SLOT( onHelp() ) );
    aButtonLayout->addWidget( helpBtn );
  }
  aMainLayout->addStretch( 1 );
  aMainLayout->addLayout( aButtonLayout );
  // . Control buttons <end>
  
  settingsToGui();
}

/*!
  Destructor.
*/
PyEditor_SettingsDlg::~PyEditor_SettingsDlg()
{
}

/*!
  SLOT: Changes the widget visibility depending on the set theState flag.
  \param theState flag of visibility
 */
void PyEditor_SettingsDlg::onVerticalEdgeChecked()
{
  myNumberColumnsLbl->setEnabled( myVerticalEdge->isChecked() );
  myNumberColumns->setEnabled( myVerticalEdge->isChecked() );
}

/*!
  SLOT: Fills font sizesc combo-box with available values depending on the
  chosen font family.
 */
void PyEditor_SettingsDlg::onFontChanged()
{
  bool blocked = myFontSize->blockSignals( true );

  QString oldSize = myFontSize->currentText();

  QList<int> szList = QFontDatabase().pointSizes( myFontFamily->currentFont().family() );
  QStringList sizes;
  foreach ( int size, szList )
    sizes.append( QString::number( size ) );

  myFontSize->clear();
  myFontSize->addItems( sizes );
  setFontSize( oldSize );

  myFontSize->blockSignals( blocked );
}

/*!
  \brief Sets settings from preferences dialog.
 */
void PyEditor_SettingsDlg::settingsFromGui()
{
  const PyEditor_Settings& oldSettings = myEditor->settings();
  PyEditor_Settings settings;
  
  QFont font = oldSettings.font();
  font.setFamily( myFontFamily->currentFont().family() );
  bool ok;
  int size = myFontSize->currentText().toInt( &ok );
  if ( ok )
    font.setPointSize( size );

  settings.setHighlightCurrentLine( myHighlightCurrentLine->isChecked() );
  settings.setTextWrapping( myTextWrapping->isChecked() );
  settings.setCenterCursorOnScroll( myCenterCursorOnScroll->isChecked() );
  settings.setLineNumberArea( myLineNumberArea->isChecked() );
  settings.setTabSpaceVisible( myTabSpaceVisible->isChecked() );
  settings.setTabSize( myTabSize->value() );
  settings.setVerticalEdge( myVerticalEdge->isChecked() );
  settings.setNumberColumns( myNumberColumns->value() );
  settings.setFont( font );
  myEditor->setSettings(settings); // updateContent()

  PyEditor_Settings* globals = PyEditor_Settings::settings();
  if ( globals && myDefaultCheck && myDefaultCheck->isChecked() )
    globals->copyFrom( settings );
}

/*!
  \brief Sets settings into preferences dialog.
 */
void PyEditor_SettingsDlg::settingsToGui()
{
  const PyEditor_Settings& settings = myEditor->settings();

  myHighlightCurrentLine->setChecked( settings.highlightCurrentLine() );
  myTextWrapping->setChecked( settings.textWrapping() );
  myCenterCursorOnScroll->setChecked( settings.centerCursorOnScroll() );
  myLineNumberArea->setChecked( settings.lineNumberArea() );
  myTabSpaceVisible->setChecked( settings.tabSpaceVisible() );
  myTabSize->setValue( settings.tabSize() );
  myVerticalEdge->setChecked( settings.verticalEdge() );
  myNumberColumns->setValue( settings.numberColumns() );
  myFontFamily->setCurrentFont( settings.font() );
  setFontSize( QString::number( settings.font().pointSize() ) );

  onVerticalEdgeChecked();
  onFontChanged();
}

/*!
  \brief Set font size value to the combo box.
  \param size new size value
*/
void PyEditor_SettingsDlg::setFontSize( const QString& size )
{
  int idx = myFontSize->findText( size );
  if ( idx != -1 )
    myFontSize->setCurrentIndex( idx );
  else
    myFontSize->setEditText( size );
}

/*!
  Slot, called when user clicks "OK" button
*/
void PyEditor_SettingsDlg::onOk()
{
  settingsFromGui();
  accept();
}

/*!
  Slot, called when user clicks "Defaults" button
*/
void PyEditor_SettingsDlg::onDefault()
{
  PyEditor_Settings* settings = PyEditor_Settings::settings();
  if ( settings )
  {
    settings->load(); // to reload from the resources
    
    QFont font = settings->font();

    myHighlightCurrentLine->setChecked( settings->highlightCurrentLine() );
    myTextWrapping->setChecked( settings->textWrapping() );
    myCenterCursorOnScroll->setChecked( settings->centerCursorOnScroll() );
    myLineNumberArea->setChecked( settings->lineNumberArea() );
    myTabSpaceVisible->setChecked( settings->tabSpaceVisible() );
    myTabSize->setValue( settings->tabSize() );
    myVerticalEdge->setChecked( settings->verticalEdge() );
    myNumberColumns->setValue( settings->numberColumns() );
    myFontFamily->setCurrentFont( font );
    setFontSize( QString::number( font.pointSize() ) );
    
    onVerticalEdgeChecked();
    onFontChanged();
  }
}

/*!
  Slot, called when user clicks "Help" button.
  Emits help() signal.
*/
void PyEditor_SettingsDlg::onHelp()
{
  emit help();
}
