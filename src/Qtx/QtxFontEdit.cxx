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

// File:      QtxFontEdit.cxx
// Author:    Sergey TELKOV
//
#include "QtxFontEdit.h"

#include "QtxComboBox.h"

#include <QLayout>
#include <QToolButton>
#include <QFontDialog>
#include <QFontDatabase>
#include <QFontComboBox>

/*!
  \class QtxFontEdit
  \brief The QtxFontEdit class represents a widget for font
  preference items editing.

  The font preference item is represented as the drop-down combo box 
  filled with the list of available fonts. Additional controls for
  modifying font properties ('bold', 'italic', font size, etc) are also
  available for use.

  Initial font value can be set with setCurrentFont() method. Chosen font
  can be retrieved with the currentFont() method.

  Font properties can be set with the setFontSize(), setFontFamily(), 
  setFontScripting() methods and retrieved with fontSize(), fontFamily(), 
  fontScripting() methods.

  Additional widgets for direct modyfing font properties are available
  with use of setFeatures() method.
*/

/*!
  \brief Constructor
  \param feat font widget features (ORed QtxFontEdit::Features flags)
  \param parent parent widget
*/
QtxFontEdit::QtxFontEdit( const int feat, QWidget* parent )
: QFrame( parent ),
  myFeatures( feat ),
  myMode( Native )
{
  initialize();
}

/*!
  \brief Constructor
  \param parent parent widget

  All font widget features are enabled.
*/
QtxFontEdit::QtxFontEdit( QWidget* parent )
: QFrame( parent ),
  myFeatures( All ),
  myMode( Native )
{
  initialize();
}

/*!
  \brief Destructor
*/
QtxFontEdit::~QtxFontEdit()
{
}

/*!
  \brief Get font widget features.
  \return font widget features (ORed QtxFontEdit::Features flags)
  \sa setFeatures()
*/
int QtxFontEdit::features() const
{
  return myFeatures;
}

/*!
  \brief Set font widget features.
  \param f font widget features (ORed QtxFontEdit::Features flags)
  \sa features()
*/
void QtxFontEdit::setFeatures( const int f )
{
  if ( myFeatures == f )
    return;

  myFeatures = f;
  updateState();
}

/*!
  \brief Get currently selected font.
  \return current font
  \sa setCurrentFont()
*/
QFont QtxFontEdit::currentFont() const
{
  QFont fnt( fontFamily(), fontSize() );

  int script = fontScripting();
  fnt.setBold( script & Bold );
  fnt.setItalic( script & Italic );
  fnt.setUnderline( script & Underline );
  fnt.setOverline( script & Shadow ); //addVtkFontPref( tr( "LABELS" ), valLblFontGr, "values_labeling_font" );

  return fnt;
}

/*!
  \brief Set currently selected font.
  \param fnt current font
  \sa currentFont()
*/
void QtxFontEdit::setCurrentFont( const QFont& fnt )
{
  // VSR 25/08/2016: IPAL53224
  // blocking signals of QFontComboBox breaks its internal business logic
  // that prevents correct retrieving of available sizes for the font in case
  // when some non-existent font is replaced by its closest analogue
  //myFamily->blockSignals( true );
  myCustomFams->blockSignals( true );
  mySize->blockSignals( true );
  myB->blockSignals( true );
  myI->blockSignals( true );
  myU->blockSignals( true );
  
  setFontFamily( fnt.family() );
  setFontSize( fnt.pointSize() );
  setFontScripting( ( fnt.bold() ? Bold : 0 ) |
                    ( fnt.italic() ? Italic : 0 ) |
                    ( fnt.underline() ? Underline : 0 ) | 
                    ( fnt.overline() ? Shadow : 0 ) );

  //myFamily->blockSignals( false );
  myCustomFams->blockSignals( false );
  mySize->blockSignals( false );
  myB->blockSignals( false );
  myI->blockSignals( false );
  myU->blockSignals( false );

  emit( changed( currentFont() ) );
}

/*!
  \brief Get selected font family name.
  \return current font family name
  \sa setFontFamily()
*/
QString QtxFontEdit::fontFamily() const
{
  if ( myMode == Native )
    return myFamily->currentFont().family();
  else
    return myCustomFams->currentText();
}

/*!
  \brief Get selected font size.
  \return current font size
  \sa setFontSize()
*/
int QtxFontEdit::fontSize() const
{
  bool ok;
  int pSize = mySize->currentText().toInt( &ok );
  return ok ? pSize : 0;
}

/*!
  \brief Get selected font scripting.
  \return current font scripting
  \sa setFontScripting()
*/
int QtxFontEdit::fontScripting() const
{
  return ( myB->isChecked() ? Bold : 0 ) |
         ( myI->isChecked() ? Italic : 0 ) |
         ( myU->isChecked() ? Underline : 0 ) |
         ( myS->isChecked() ? Shadow : 0 ) ;
}

/*!
  \brief Set font family name.
  \param fam new font family name
  \sa fontFamily()
*/
void QtxFontEdit::setFontFamily( const QString& fam )
{
  if ( myMode == Native )
  {
    myFamily->setCurrentFont( QFont( fam ) );
    onFontChanged( myFamily->currentFont() );  
  }
  else 
  {
    myCustomFams->setCurrentIndex( myCustomFams->findText( fam ) );
    if ( !myCustomFams->signalsBlocked() )
      emit( changed( currentFont() ) );
  }
}

/*!
  \brief Set font size.
  \param fam new font size
  \sa fontSize()
*/
void QtxFontEdit::setFontSize( const int s )
{
  if ( s <= 0 )
    return;

  int idx = mySize->findText( QString::number( s ) );
  if ( idx != -1 )
    mySize->setCurrentIndex( idx );
  else if ( mySize->isEditable() )
    mySize->setEditText( QString::number( s ) );
}

/*!
  \brief Set font scripting.
  \param fam new font scripting
  \sa fontScripting()
*/
void QtxFontEdit::setFontScripting( const int script )
{
  myB->setChecked( script & Bold );
  myI->setChecked( script & Italic );
  myU->setChecked( script & Underline );
  myS->setChecked( script & Shadow );
}

/*!
  \brief Update widget state
*/
void QtxFontEdit::updateState()
{
  int feat = features();

  myFamily->setVisible( ( feat & Family ) && myMode == Native );
  myCustomFams->setVisible( ( feat & Family ) && myMode == Custom );
  mySize->setVisible( feat & Size );
  myB->setVisible( feat & Bold );
  myI->setVisible( feat & Italic );
  myU->setVisible( feat & Underline );
  myS->setVisible( feat & Shadow );
  myPreview->setVisible( feat & Preview );

  mySize->setEditable( feat & UserSize );
}

/*!
  \brief Called when current font is changed.
  \param f (not used)
*/
void QtxFontEdit::onFontChanged( const QFont& /*f*/ )
{
  bool blocked = mySize->signalsBlocked();
  mySize->blockSignals( true );

  int s = fontSize();
  mySize->clear();

  QList<int> szList = QFontDatabase().pointSizes( fontFamily() );
  QStringList sizes;
  for ( QList<int>::const_iterator it = szList.begin(); it != szList.end(); ++it )
    sizes.append( QString::number( *it ) );
  mySize->addItems( sizes );

  setFontSize( s );

  mySize->blockSignals( blocked );

  if ( !myFamily->signalsBlocked() )
    emit( changed( currentFont() ) );
}

void QtxFontEdit::onPropertyChanged()
{
  emit( changed( currentFont() ) );
}

/*!
  \brief Called when "Preview" button is clicked.
  \param on (not used)
*/
void QtxFontEdit::onPreview( bool /*on*/ )
{
  bool ok;
  QFont fnt = QFontDialog::getFont( &ok, currentFont() );

  if ( ok )
    setCurrentFont( fnt );
}

/*
  \brief Perform internal intialization.
*/
void QtxFontEdit::initialize()
{
  QHBoxLayout* base = new QHBoxLayout( this );
  base->setMargin( 0 );
  base->setSpacing( 5 );

  base->addWidget( myFamily = new QFontComboBox( this ) );
  base->addWidget( myCustomFams = new QComboBox( this ) );
  base->addWidget( mySize = new QtxComboBox( this ) );
  mySize->setInsertPolicy( QComboBox::NoInsert );
  mySize->setValidator( new QIntValidator( 1, 250, mySize ) );

  base->addWidget( myB = new QToolButton( this ) );
  myB->setText( tr( "B" ) );
  myB->setCheckable( true );

  base->addWidget( myI = new QToolButton( this ) );
  myI->setText( tr( "I" ) );
  myI->setCheckable( true );

  base->addWidget( myU = new QToolButton( this ) );
  myU->setText( tr( "U" ) );
  myU->setCheckable( true );

  base->addWidget( myS = new QToolButton( this ) );
  myS->setText( tr( "S" ) );
  myS->setCheckable( true );

  base->addWidget( myPreview = new QToolButton( this ) );
  myPreview->setText( "..." );

  myFamily->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
  myCustomFams->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );

  connect( myPreview, SIGNAL( clicked( bool ) ),                    this, SLOT( onPreview( bool ) ) );
  connect( myFamily,  SIGNAL( currentFontChanged( const QFont& ) ), this, SLOT( onFontChanged( const QFont& ) ) );
  connect( mySize,    SIGNAL( currentIndexChanged( int ) ),         this, SLOT( onPropertyChanged() ) );
  connect( mySize,    SIGNAL( editTextChanged( QString ) ),         this, SLOT( onPropertyChanged() ) );
  connect( myB,       SIGNAL( toggled( bool ) ),                    this, SLOT( onPropertyChanged() ) );
  connect( myI,       SIGNAL( toggled( bool ) ),                    this, SLOT( onPropertyChanged() ) );
  connect( myU,       SIGNAL( toggled( bool ) ),                    this, SLOT( onPropertyChanged() ) );

  myCustomFams->hide();
  myS->hide();

  updateState();
  onFontChanged( currentFont() );
}

/*!
  \brief Specifies whether widget works in Native or Custom mode. Native mode 
  is intended for working with system fonts. Custom mode is intended for 
  working with manually defined set of fonts. Set of custom fonts can be 
  specified with setCustomFonts() method 
  \param mode mode from QtxFontEdit::Mode enumeration
  \sa mode()
*/
void QtxFontEdit::setMode( const int mode )
{
  if ( myMode == mode )
    return;

  myMode = mode;

  myFamily->setVisible( myMode == Native );
  myCustomFams->setVisible( myMode == Custom );

  updateGeometry();
}

/*!
  \brief Verifies whether widget works in Native or Custom mode
  \return Native or Custom mode
  \sa setMode()
*/
int QtxFontEdit::mode() const
{
  return myMode;
}

/*!
  \brief Sets list of custom fonts. 
  <b>This method is intended for working in Custom mode.</b>
  \param fams list of families
  \sa fonts(), setMode()
*/
void QtxFontEdit::setFonts( const QStringList& fams )
{
  QString currFam = myCustomFams->currentText();
  
  myCustomFams->clear();
  myCustomFams->addItems( fams );

  int ind = myCustomFams->findText( currFam );
  if ( ind != -1 )
    myCustomFams->setCurrentIndex( ind );

  setSizes( QList<int>() );
}

/*!
  \brief Sets list of available font sizes. 
  <b>This method is intended for working in Custom mode.</b> The list of sizes can 
  be empty. In this case system generate listof size automatically from 8 till 72.
  \param sizes list of sizes
  \sa sizes(), setMode()
*/
void QtxFontEdit::setSizes( const QList<int>& sizes )
{
  QString currSize = mySize->currentText();

  mySize->clear();
  if ( !sizes.isEmpty() )
  {
    QStringList szList;
    for ( QList<int>::const_iterator it = sizes.begin(); it != sizes.end(); ++it )
      szList.append( QString::number( *it ) );
    mySize->addItems( szList );
  }
  else
  {
    static QStringList defLst;
    if ( defLst.isEmpty() )
    {
      QString str( "8 9 10 11 12 14 16 18 20 22 24 26 28 36 48 72" );
      defLst = str.split( " " );
    }
    mySize->addItems( defLst );
  }
  
  int ind = mySize->findText( currSize );
  if ( ind != -1 )
    mySize->setCurrentIndex( ind );
}

/*!
  \brief Gets list of custom fonts 
  \return list of families
  \sa setFonts(), setMode()
*/
QStringList QtxFontEdit::fonts() const
{
  QStringList fams;
  for ( int i = 0, n = myCustomFams->count(); i < n; i++ )
    fams.append( myCustomFams->itemText( i ) );
  return fams;
}

/*!
  \brief Gets list of custom fonts 
  \return list of families
  \sa setCustomFonts(), setMode()
*/
QList<int> QtxFontEdit::sizes() const
{
  QList<int> lst;
  for ( int i = 0, n = mySize->count(); i < n; i++ )
    lst.append( mySize->itemText( i ).toInt() );
  return lst;
}




















