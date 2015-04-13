// Copyright (C) 2015 OPEN CASCADE
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
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QtxFontEdit.h>

/*!
  \class PyEditor_SettingsDlg
  \brief Dialog settings for python editor.
*/

/*!
  \brief Constructor.
  \param theEditor widget that is used to edit and display text
  \param theParent parent widget
*/
PyEditor_SettingsDlg::PyEditor_SettingsDlg( PyEditor_Editor* theEditor, QWidget* theParent ) :
  QDialog( theParent ),
  my_Editor( theEditor )
{
  setWindowTitle( tr("TIT_PY_PREF") );
  QVBoxLayout* aMainLayout = new QVBoxLayout( this );
  
  // . Font settings <start>
  QGroupBox* aFontSetBox = new QGroupBox( tr( "GR_FONT_SET" ) );
  QHBoxLayout* aFontSetLayout = new QHBoxLayout( aFontSetBox );
  w_FontWidget = new QtxFontEdit( QtxFontEdit::Family|QtxFontEdit::Size|QtxFontEdit::UserSize, this );
  aFontSetLayout->addWidget( w_FontWidget );
  // . Font settings <end>
  
  // . Display settings <start>
  QGroupBox* aDisplaySetBox = new QGroupBox( tr( "GR_DISP_SET" ) );
  QVBoxLayout* aDisplaySetLayout = new QVBoxLayout;
  w_HighlightCurrentLine = new QCheckBox( tr( "LBL_CURRLINE_HIGHLIGHT" ) );
  w_TextWrapping = new QCheckBox( tr( "LBL_TEXT_WRAP" ) );
  w_CenterCursorOnScroll = new QCheckBox( tr( "LBL_CURSOR_SCROLL" ) );
  w_LineNumberArea = new QCheckBox( tr( "LBL_LINE_NUMBS_AREA" ) );
  aDisplaySetLayout->addWidget( w_HighlightCurrentLine );
  aDisplaySetLayout->addWidget( w_TextWrapping );
  aDisplaySetLayout->addWidget( w_CenterCursorOnScroll );
  aDisplaySetLayout->addWidget( w_LineNumberArea );
  aDisplaySetLayout->addStretch( 1 );
  aDisplaySetBox->setLayout( aDisplaySetLayout );
  // . Display settings <end>

  QHBoxLayout* aTabVertEdgeLayout = new QHBoxLayout;

  // . Tab settings <start>
  QGroupBox* aTabSetBox = new QGroupBox( tr( "GR_TAB_SET" ) );
  QVBoxLayout* aTabSetLayout = new QVBoxLayout;
  w_TabSpaceVisible = new QCheckBox( tr( "LBL_TAB_SPACES" ) );
  QHBoxLayout* aTabSizeLayout = new QHBoxLayout;
  QLabel* aTabSizeLabel = new QLabel( tr( "LBL_TAB_SIZE" ) );
  w_TabSize = new QSpinBox;
  w_TabSize->setMinimum( 0 );
  w_TabSize->setSingleStep( 1 );
  aTabSizeLayout->addWidget( aTabSizeLabel );
  aTabSizeLayout->addWidget( w_TabSize );
  aTabSizeLayout->addStretch( 1 );
  aTabSetLayout->addWidget( w_TabSpaceVisible );
  aTabSetLayout->addLayout( aTabSizeLayout );
  aTabSetBox->setLayout( aTabSetLayout );
  // . Tab settings <end>

  // . Vertical edge settings <start>
  QGroupBox* aVertEdgeSetBox = new QGroupBox( tr( "GR_VERT_EDGE_SET" ) );
  QVBoxLayout* aVertEdgeLayout = new QVBoxLayout;
  w_VerticalEdge = new QCheckBox( tr( "LBL_VERT_EDGE" ) );
  QHBoxLayout* aNumberColLayout = new QHBoxLayout;
  lbl_NumColumns = new QLabel( tr( "LBL_NUM_COLUMNS" ) );
  w_NumberColumns = new QSpinBox;
  w_NumberColumns->setMinimum( 0 );
  w_NumberColumns->setSingleStep( 1 );
  aNumberColLayout->addWidget( lbl_NumColumns );
  aNumberColLayout->addWidget( w_NumberColumns );
  aNumberColLayout->addStretch( 1 );
  aVertEdgeLayout->addWidget( w_VerticalEdge );
  aVertEdgeLayout->addLayout( aNumberColLayout );
  aVertEdgeSetBox->setLayout( aVertEdgeLayout );
  connect( w_VerticalEdge, SIGNAL( clicked( bool ) ), 
           this, SLOT( onVerticalEdgeChecked( bool ) ) );
  // . Vertical edge settings <end>

  aTabVertEdgeLayout->addWidget( aTabSetBox );
  aTabVertEdgeLayout->addWidget( aVertEdgeSetBox );

  // . "Set as default" check box
  w_DefaultCheck = new QCheckBox( tr( "WDG_SET_AS_DEFAULT_CHECK" ), this );
  
  aMainLayout->addWidget( aFontSetBox );
  aMainLayout->addWidget( aDisplaySetBox );
  aMainLayout->addLayout( aTabVertEdgeLayout );
  aMainLayout->addWidget( w_DefaultCheck );
  aMainLayout->addStretch( 1 );
  
  myOkBtn = new QPushButton( tr( "BUT_OK" ), this );
  myOkBtn->setAutoDefault( TRUE );
  myOkBtn->setDefault( TRUE );
  myCancelBtn = new QPushButton( tr( "BUT_CANCEL" ), this );
  myCancelBtn->setAutoDefault( TRUE );
  myHelpBtn = new QPushButton( tr( "BUT_HELP" ), this );
  myHelpBtn->setAutoDefault( TRUE );
  QHBoxLayout* aButtonLayout = new QHBoxLayout;
  aButtonLayout->addWidget( myOkBtn );
  aButtonLayout->addStretch();
  aButtonLayout->addWidget( myCancelBtn );
  aButtonLayout->addWidget( myHelpBtn );
  aMainLayout->addLayout( aButtonLayout );

  connect( myOkBtn,         SIGNAL( clicked() ), this, SLOT( onOk() ) );
  connect( myCancelBtn,     SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( myHelpBtn,       SIGNAL( clicked() ), this, SLOT( onHelp() ) );
  
  settingsToGui();
}

/*!
  \brief Get "Set settings as default" check box value.
  \return \c true if "Set settings as default" check box is on
*/
bool PyEditor_SettingsDlg::isSetAsDefault()
{
  return w_DefaultCheck->isChecked();
}

/*!
  SLOT: Changes the widget visibility depending on the set theState flag.
  \param theState flag of visibility
 */
void PyEditor_SettingsDlg::onVerticalEdgeChecked( bool theState )
{
  lbl_NumColumns->setEnabled( theState );
  w_NumberColumns->setEnabled( theState );
}

/*!
  \brief Sets settings from preferences dialog.
 */
void PyEditor_SettingsDlg::settingsFromGui()
{
  my_Editor->settings()->p_HighlightCurrentLine = w_HighlightCurrentLine->isChecked();
  my_Editor->settings()->p_TextWrapping = w_TextWrapping->isChecked();
  my_Editor->settings()->p_CenterCursorOnScroll = w_CenterCursorOnScroll->isChecked();
  my_Editor->settings()->p_LineNumberArea = w_LineNumberArea->isChecked();
  my_Editor->settings()->p_TabSpaceVisible = w_TabSpaceVisible->isChecked();
  my_Editor->settings()->p_TabSize = w_TabSize->value();
  my_Editor->settings()->p_VerticalEdge = w_VerticalEdge->isChecked();
  my_Editor->settings()->p_NumberColumns = w_NumberColumns->value();
  my_Editor->settings()->p_Font = w_FontWidget->currentFont();
}

/*!
  \brief Sets settings into preferences dialog.
 */
void PyEditor_SettingsDlg::settingsToGui()
{
  w_HighlightCurrentLine->setChecked( my_Editor->settings()->p_HighlightCurrentLine );
  w_TextWrapping->setChecked( my_Editor->settings()->p_TextWrapping );
  w_CenterCursorOnScroll->setChecked( my_Editor->settings()->p_CenterCursorOnScroll );
  w_LineNumberArea->setChecked( my_Editor->settings()->p_LineNumberArea );
  w_TabSpaceVisible->setChecked( my_Editor->settings()->p_TabSpaceVisible );
  w_TabSize->setValue( my_Editor->settings()->p_TabSize );
  w_VerticalEdge->setChecked( my_Editor->settings()->p_VerticalEdge );
  w_NumberColumns->setValue( my_Editor->settings()->p_NumberColumns );
  w_FontWidget->setCurrentFont( my_Editor->settings()->p_Font );
  w_FontWidget->setSizes();

  onVerticalEdgeChecked( my_Editor->settings()->p_VerticalEdge );
}

/*!
  \brief Sets settings into the setting resources or file
  if the flag is set as default is true.
 */
void PyEditor_SettingsDlg::setSettings()
{
  if ( isSetAsDefault() )
    my_Editor->settings()->writeSettings();

  my_Editor->updateStatement();
}

/*!
  Slot, called when user clicks "OK" button
*/
void PyEditor_SettingsDlg::onOk()
{
  settingsFromGui();
  setSettings();
  accept();
}

/*!
  Slot, called when user clicks "Help" button.
*/
void PyEditor_SettingsDlg::onHelp()
{
  emit onHelpClicked();
}
