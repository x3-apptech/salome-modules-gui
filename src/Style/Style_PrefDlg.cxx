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

// File   : Style_PrefDlg.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S (vadim.sandler@opencascade.com)
//
#include "Style_PrefDlg.h"
#include "Style_ResourceMgr.h"
#include "Style_Model.h"
#include "Style_Salome.h"

#include <QtxColorButton.h>
#include <QtxDoubleSpinBox.h>
#include <QtxFontEdit.h>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QListWidget>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QVBoxLayout>

static const int MARGIN  = 9;
static const int SPACING = 6;

/*!
  \class Style_PrefDlg::PaletteEditor
  \brief SALOME style palette editor widget
  \internal
*/

/*!
  \brief Constructor
  \param parent parent widget
  \internal
*/
Style_PrefDlg::PaletteEditor::PaletteEditor( QWidget* parent )
  : QFrame( parent ), myCurrentRow( 0 ), myCurrentColumn( -1 )
{
  QGridLayout* l = new QGridLayout( this );
  l->setMargin( MARGIN ); l->setSpacing( SPACING );
  l->addWidget( myQuickButton = new QPushButton( tr( "Quick" ), this ), 0, 0 );
  l->addWidget( myAutoCheck   = new QCheckBox( tr( "Auto" ),    this ), 0, 1 );
  l->addWidget( myContainer   = new QFrame( this ),                     1, 0, 1, 2 );

  l = new QGridLayout( myContainer );
  l->setMargin( 0 ); l->setSpacing( SPACING );

  addColumn( tr( "Base colors" ) );
  for ( int i = (int)Style_Model::WindowText; i < (int)Style_Model::NColorRoles; i++ ) {
    if ( i == Style_Model::NoRole ) continue;
    addItem( i );
  }

  addColumn( tr( "Additional colors" ) );
  for ( int i = (int)Style_Model::BorderTop; i < (int)Style_Model::LastColor; i++ )
    addItem( i );

  connect( myQuickButton, SIGNAL( clicked() ),       this, SLOT( onQuick() ) );
  connect( myAutoCheck,   SIGNAL( toggled( bool ) ), this, SLOT( onAuto()  ) );
}

/*!
  \brief Destructor
  \internal
*/
Style_PrefDlg::PaletteEditor::~PaletteEditor()
{
}

/*!
  \brief Add new color properties column to the widget
  \param title column title
  \sa addItem()
  \internal
*/
void Style_PrefDlg::PaletteEditor::addColumn( const QString& title )
{
  QGridLayout* l = qobject_cast<QGridLayout*>( myContainer->layout() );
  myCurrentRow = 0;
  myCurrentColumn++;

  if ( !title.isEmpty() ) {
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin( 0 ); hbox->setSpacing( SPACING );
    QWidget* ln = line();
    hbox->addWidget( ln );
    hbox->setStretchFactor( ln, 5 );
    QLabel* tlt = new QLabel( title, myContainer );
    tlt->setAlignment( Qt::AlignCenter );
    hbox->addWidget( tlt );
    ln = line();
    hbox->addWidget( ln );
    hbox->setStretchFactor( ln, 5 );
    l->addLayout( hbox, myCurrentRow++, myCurrentColumn*4, 1, 4 );
  }

  l->addWidget( myActiveLab   = new QLabel( tr( "Active" ),   myContainer ), myCurrentRow, myCurrentColumn*4+1 );
  l->addWidget( myInactiveLab = new QLabel( tr( "Inactive" ), myContainer ), myCurrentRow, myCurrentColumn*4+2 );
  l->addWidget( myDisabledLab = new QLabel( tr( "Disabled" ), myContainer ), myCurrentRow, myCurrentColumn*4+3 );
  int w = 0;
  w = qMax( w, myActiveLab->sizeHint().width() );
  w = qMax( w, myInactiveLab->sizeHint().width() );
  w = qMax( w, myDisabledLab->sizeHint().width() );
  myActiveLab->setMinimumWidth( w );
  myInactiveLab->setMinimumWidth( w );
  myDisabledLab->setMinimumWidth( w );
  
  for( int i = 1; i < 4; i++ ) l->setColumnStretch( myCurrentColumn*4+i, 5 );

  myCurrentRow++;
}

/*!
  \brief Add color item to the widget (to the current column)
  \param id color palette identifier (Style_Model::ColorRole)
  \sa addColumn(), items()
  \internal
*/
void Style_PrefDlg::PaletteEditor::addItem( int id )
{
  if ( myButtons.contains( id ) )
    return;

  myButtons[ id ] = Btns();
  myButtons[ id ][ QPalette::Active ]   = new QtxColorButton( myContainer );
  myButtons[ id ][ QPalette::Inactive ] = new QtxColorButton( myContainer );
  myButtons[ id ][ QPalette::Disabled ] = new QtxColorButton( myContainer );
  myButtons[ id ][ QPalette::Active ]->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myButtons[ id ][ QPalette::Inactive ]->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myButtons[ id ][ QPalette::Disabled ]->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

  QGridLayout* l = qobject_cast<QGridLayout*>( myContainer->layout() );
  l->addWidget( new QLabel( idToName( id ), myContainer ), myCurrentRow, myCurrentColumn*4 );
  l->addWidget( myButtons[ id ][ QPalette::Active ],       myCurrentRow, myCurrentColumn*4+1 );
  l->addWidget( myButtons[ id ][ QPalette::Inactive ],     myCurrentRow, myCurrentColumn*4+2 );
  l->addWidget( myButtons[ id ][ QPalette::Disabled ],     myCurrentRow, myCurrentColumn*4+3 );

  connect( myButtons[ id ][ QPalette::Active   ], SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) );
  connect( myButtons[ id ][ QPalette::Inactive ], SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) );
  connect( myButtons[ id ][ QPalette::Disabled ], SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) );

  myCurrentRow++;
}

/*!
  \brief Get list of currently available color items
  \return list of items identifiers (Style_Model::ColorRole)
  \sa addColumn(), addItem()
  \internal
*/
QList<int> Style_PrefDlg::PaletteEditor::items() const
{
  return myButtons.keys();
}

/*!
  \brief Set color value to the item
  \param id item identifier (Style_Model::ColorRole)
  \param cg color group
  \param c color value
  \sa color()
  \internal
*/
void Style_PrefDlg::PaletteEditor::setColor( int id, QPalette::ColorGroup cg, const QColor& c )
{
  if ( myButtons.contains( id ) ) {
    myButtons[ id ][ cg ]->setColor( c );
  }
}

/*!
  \brief Set color value to the item
  \param id item identifier (Style_Model::ColorRole)
  \param active color to be used with active color group (QPalette::Active)
  \param inactive color to be used with inactive color group (QPalette::Inactive)
  \param disabled color to be used with disabled color group (QPalette::Disabled)
  \sa color()
  \internal
*/
void Style_PrefDlg::PaletteEditor::setColor( int id, const QColor& active,
                                             const QColor& inactive, const QColor& disabled )
{
  setColor( id, QPalette::Active,   active );
  setColor( id, QPalette::Inactive, inactive.isValid() ? inactive : active );
  setColor( id, QPalette::Disabled, disabled.isValid() ? disabled : active );
}

/*!
  \brief Get color value assigned to the item
  \param id item identifier (Style_Model::ColorRole)
  \param cg color group
  \return color value
  \sa setColor()
  \internal
*/
QColor Style_PrefDlg::PaletteEditor::color( int id, QPalette::ColorGroup cg ) const
{
  QColor c;
  if ( myButtons.contains( id ) ) {
    c = myButtons[ id ][ cg ]->color();
  }
  return c;
}

/*!
  \brief Get 'auto-palette' flag value
  \return \c true if inactive/disabled palette colors should be calculated from active one
  \sa setAuto()
  \internal
*/
bool Style_PrefDlg::PaletteEditor::isAuto() const
{
  return myAutoCheck->isChecked();
}

/*!
  \brief Set/clear 'auto-palette' flag value
  \param on if \c true, inactive/disabled palette colors should be calculated from active one
  \sa isAuto()
  \internal
*/
void Style_PrefDlg::PaletteEditor::setAuto( bool on )
{
  myAutoCheck->setChecked( on );
}

/*!
  \brief Initialize all palette colors from color \a c
  \param c button color value
  \internal
*/
void Style_PrefDlg::PaletteEditor::fromColor( const QColor& c )
{
  const QPalette pal = QPalette( c );
  for ( int i = 0; i < (int)Style_Model::NColorRoles; i++ ) {
    setColor( i,
              pal.color( QPalette::Active,   (QPalette::ColorRole)i ), 
              pal.color( QPalette::Inactive, (QPalette::ColorRole)i ),
              pal.color( QPalette::Disabled, (QPalette::ColorRole)i ) );
  }

  // Header
  setColor( Style_Model::Header,
            pal.color( QPalette::Active,   QPalette::Button ), 
            pal.color( QPalette::Inactive, QPalette::Button ),
            pal.color( QPalette::Disabled, QPalette::Button ) );
  // Checked
  setColor( Style_Model::Checked,
            pal.color( QPalette::Active,   QPalette::Base ),
            pal.color( QPalette::Inactive, QPalette::Base ),
            pal.color( QPalette::Disabled, QPalette::Base ) );
  // Lines
  setColor( Style_Model::Lines,
            pal.color( QPalette::Active,   QPalette::Mid ),
            pal.color( QPalette::Inactive, QPalette::Mid ),
            pal.color( QPalette::Disabled, QPalette::Mid ) );
  // Slider
  setColor( Style_Model::Slider,
            pal.color( QPalette::Active,   QPalette::Button ),
            pal.color( QPalette::Inactive, QPalette::Button ),
            pal.color( QPalette::Disabled, QPalette::Button ) );
  // ProgressBar
  setColor( Style_Model::ProgressBar,
            pal.color( QPalette::Active,   QPalette::Highlight ),
            pal.color( QPalette::Inactive, QPalette::Highlight ),
            pal.color( QPalette::Disabled, QPalette::Highlight ) );
  // FieldLight
  setColor( Style_Model::FieldLight,
            pal.color( QPalette::Active,   QPalette::Light ),
            pal.color( QPalette::Inactive, QPalette::Light ),
            pal.color( QPalette::Disabled, QPalette::Light ) );
  // FieldDark
  setColor( Style_Model::FieldDark,
            pal.color( QPalette::Active,   QPalette::Mid ).light( 125 ),
            pal.color( QPalette::Inactive, QPalette::Mid ).light( 125 ),
            pal.color( QPalette::Disabled, QPalette::Mid ).light( 125 ) );
  // GridLine
  setColor( Style_Model::GridLine,
            pal.color( QPalette::Active,   QPalette::Mid ),
            pal.color( QPalette::Inactive, QPalette::Mid ),
            pal.color( QPalette::Disabled, QPalette::Mid ) );
  // HighlightWidget
  setColor( Style_Model::HighlightWidget,
            pal.color( QPalette::Active,   QPalette::Button ),
            pal.color( QPalette::Inactive, QPalette::Button ),
            pal.color( QPalette::Disabled, QPalette::Button ) );
  // HighlightWidget
  setColor( Style_Model::HighlightBorder,
            pal.color( QPalette::Active,   QPalette::Button ).dark( 100 ),
            pal.color( QPalette::Inactive, QPalette::Button ).dark( 100 ),
            pal.color( QPalette::Disabled, QPalette::Button ).dark( 100 ) );
  // Pointer
  setColor( Style_Model::Pointer, Qt::black, Qt::black, Qt::black );

  QColor aDarkActive   = pal.color( QPalette::Active,   QPalette::Dark );
  QColor aDarkInactive = pal.color( QPalette::Inactive, QPalette::Dark );
  QColor aDarkDisabled = pal.color( QPalette::Disabled, QPalette::Dark );

  // BorderTop
  setColor( Style_Model::BorderTop,
            aDarkActive.lighter(),
            aDarkInactive.lighter(),
            aDarkDisabled.lighter() );
  // BorderBottom
  setColor( Style_Model::BorderBottom,
            aDarkActive.darker(),
            aDarkInactive.darker(),
            aDarkDisabled.darker() );
  // TabBorderTop
  setColor( Style_Model::TabBorderTop,
            aDarkActive.light().light().light(),
            aDarkInactive.light().light().light(), 
            aDarkDisabled.light().light().light() );
  // TabBorderBottom
  setColor( Style_Model::TabBorderBottom,
            aDarkActive.dark().dark().dark(),
            aDarkInactive.dark().dark().dark(),
            aDarkDisabled.dark().dark().dark() );
}

/*!
  \brief Get title of the item
  \param id item identifier (Style_Model::ColorRole)
  \return item title
  \internal
*/
QString Style_PrefDlg::PaletteEditor::idToName( int id )
{
  QString name;
  switch ( id ) {
  case Style_Model::WindowText:        //  0
    name = tr( "Window text" ); break;
  case Style_Model::Button:            //  1
    name = tr( "Button" ); break;
  case Style_Model::Light:             //  2
    name = tr( "Light" ); break;
  case Style_Model::Midlight:          //  3
    name = tr( "Midlight" ); break;
  case Style_Model::Dark:              //  4
    name = tr( "Dark" ); break;
  case Style_Model::Mid:               //  5
    name = tr( "Mid" );             break;
  case Style_Model::Text:              //  6
    name = tr( "Text" );             break;
  case Style_Model::BrightText:        //  7
    name = tr( "Bright text" );      break;
  case Style_Model::ButtonText:        //  8
    name = tr( "Button text" );      break;
  case Style_Model::Base:              //  9
    name = tr( "Base" );             break;
  case Style_Model::Window:            // 10
    name = tr( "Window" );           break;
  case Style_Model::Shadow:            // 11
    name = tr( "Shadow" );           break;
  case Style_Model::Highlight:         // 12
    name = tr( "Highlight" );        break;
  case Style_Model::HighlightedText:   // 13
    name = tr( "Highlighted text" ); break;
  case Style_Model::Link:              // 14
    name = tr( "Link" );             break;
  case Style_Model::LinkVisited:       // 15
    name = tr( "Visited link" );     break;
  case Style_Model::AlternateBase:     // 16
    name = tr( "Alternate base" );   break;
  case Style_Model::ToolTipBase:       // 18
    name = tr( "Tooltip base" );    break;
  case Style_Model::ToolTipText:       // 19
    name = tr( "Tooltip text" );    break;
  case Style_Model::BorderTop:
    name = tr( "Border top" ); break;
  case Style_Model::BorderBottom:
    name = tr( "Border bottom" ); break;
  case Style_Model::TabBorderTop:
    name = tr( "Tab border top" ); break;
  case Style_Model::TabBorderBottom:
    name = tr( "Tab border bottom" ); break;
  case Style_Model::FieldLight:
    name = tr( "Field light" ); break;
  case Style_Model::FieldDark:
    name = tr( "Field dark" ); break;
  case Style_Model::Slider:
    name = tr( "Slider" ); break;
  case Style_Model::Lines:
    name = tr( "Lines" ); break;
  case Style_Model::HighlightWidget:
    name = tr( "Widget center" ); break;
  case Style_Model::HighlightBorder:
    name = tr( "Widget border" ); break;
  case Style_Model::Header:
    name = tr( "Header" ); break;
  case Style_Model::ProgressBar:
    name = tr( "Progress bar" ); break;
  case Style_Model::Pointer:
    name = tr( "Pointer" ); break;
  case Style_Model::Checked:
    name = tr( "Checked" ); break;
  case Style_Model::GridLine:
    name = tr( "Table grid" ); break;
  default:
    break;
  }
  return name;
}

/*!
  \brief Create horizontal line widget
  \return new line widget
  \internal
*/
QWidget* Style_PrefDlg::PaletteEditor::line()
{
  QFrame* hline = new QFrame( myContainer );
  hline->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  return hline;
}

/*!
  \brief Called when "Quick" button is clicked
  \internal
*/
void Style_PrefDlg::PaletteEditor::onQuick()
{
  static QColor lastColor = Qt::white;
  QColor c = QColorDialog::getColor( lastColor, this );
  if ( c.isValid() ) {
    fromColor( lastColor = c );
    emit( changed() );
  }
}

/*!
  \brief Called when "Auto" check box is clicked
  \internal
*/
void Style_PrefDlg::PaletteEditor::onAuto()
{
  foreach( Btns btn, myButtons ) {
    btn[ QPalette::Inactive ]->setEnabled( !myAutoCheck->isChecked() );
    btn[ QPalette::Disabled ]->setEnabled( !myAutoCheck->isChecked() );
  }
  emit( changed() );
}

/*!
  \class Style_PrefDlg
  \brief SALOME style prefences dialog box class.

  The dialog box lists all SALOME style themes available via the application and allows
  user to create own schemas.
*/

/*!
  \brief Constructor
  \param parent parent widget
*/
Style_PrefDlg::Style_PrefDlg( QWidget* parent )
  : QtxDialog( parent, true, true, OK | Close | Apply ),
    myResMgr( 0 )
{
  // set title
  setWindowTitle( tr( "SALOME style preferences" ) );

  // create main layout
  QVBoxLayout* main = new QVBoxLayout( mainFrame() );
  main->setMargin( 0 ); main->setSpacing( SPACING );

  // create main widgets
  myStyleCheck = new QCheckBox( tr( "Enable SALOME Style" ), this );
  QFrame* fr = new QFrame( this );
  fr->setFrameStyle( QFrame::Box | QFrame::Sunken );

  main->addWidget( myStyleCheck );
  main->addWidget( fr );

  // create editor widgets
  myStylesList = new QListWidget( fr );
  myStylesTab  = new QTabWidget( fr );

  QHBoxLayout* frLayout = new QHBoxLayout( fr );
  frLayout->setMargin( MARGIN ); frLayout->setSpacing( SPACING );
  frLayout->addWidget( myStylesList );
  frLayout->addWidget( myStylesTab );
  frLayout->setStretchFactor( myStylesList, 1 );
  frLayout->setStretchFactor( myStylesTab, 2 );

  // ...
  QWidget* w1 = new QWidget( myStylesTab );
  QVBoxLayout* vLayout = new QVBoxLayout( w1 );
  vLayout->setMargin( 0 ); vLayout->setSpacing( SPACING );

  myPaletteEditor = new PaletteEditor( w1 );
  vLayout->addWidget( myPaletteEditor );
  vLayout->addStretch();

  // ...
  QWidget* w2 = new QWidget( myStylesTab );
  vLayout = new QVBoxLayout( w2 );
  vLayout->setMargin( MARGIN ); vLayout->setSpacing( SPACING );

  QGroupBox* fontGroup = new QGroupBox( tr( "Font" ), w2 );
  myFontEdit = new QtxFontEdit( fontGroup );

  QHBoxLayout* fontLayout = new QHBoxLayout( fontGroup );
  fontLayout->setMargin( MARGIN ); fontLayout->setSpacing( SPACING );
  fontLayout->addWidget( myFontEdit );

  QGroupBox* linesGroup = new QGroupBox( tr( "Lines" ), w2 );
  QLabel* linesTypeLab = new QLabel( tr( "Type" ), linesGroup );
  myLinesCombo  = new QComboBox( linesGroup );
  myLinesCombo->addItem( tr( "None" ),       Style_Model::NoLines );
  myLinesCombo->addItem( tr( "Horizontal" ), Style_Model::Horizontal );
  myLinesCombo->addItem( tr( "Inclined" ),   Style_Model::Inclined );
  QLabel* linesTranspLab = new QLabel( tr( "Transparency" ), linesGroup );
  myLinesTransparency = new QSlider( Qt::Horizontal, linesGroup );
  myLinesTransparency->setMinimum( 0 );
  myLinesTransparency->setMaximum( 100 );
  myLinesTransparency->setSingleStep( 1 );
  myLinesTransparency->setTracking( false );

  QHBoxLayout* linesLayout = new QHBoxLayout( linesGroup );
  linesLayout->setMargin( MARGIN ); linesLayout->setSpacing( SPACING );
  linesLayout->addWidget( linesTypeLab );
  linesLayout->addWidget( myLinesCombo );
  linesLayout->addWidget( linesTranspLab );
  linesLayout->addWidget( myLinesTransparency );

  QGroupBox* roundGroup = new QGroupBox( tr( "Widgets rounding" ), w2 );
  QLabel* roundButtonLab = new QLabel( tr( "Buttons" ), roundGroup );
  myButtonRound = new QtxDoubleSpinBox( roundGroup );
  QLabel* roundEditLab = new QLabel( tr( "Edit boxes" ), roundGroup );
  myEditRound = new QtxDoubleSpinBox( roundGroup );
  QLabel* roundFrameLab = new QLabel( tr( "Frames" ), roundGroup );
  myFrameRound = new QtxDoubleSpinBox( roundGroup );
  QLabel* roundSliderLab = new QLabel( tr( "Sliders" ), roundGroup );
  mySliderRound = new QtxDoubleSpinBox( roundGroup );
  myAntiAliasing = new QCheckBox( tr( "Anti-aliased borders" ), roundGroup );

  QGridLayout* roundLayout = new QGridLayout( roundGroup );
  roundLayout->setMargin( MARGIN ); roundLayout->setSpacing( SPACING );
  roundLayout->addWidget( roundButtonLab, 0, 0 );
  roundLayout->addWidget( myButtonRound,  0, 1 );
  roundLayout->addWidget( roundEditLab,   0, 2 );
  roundLayout->addWidget( myEditRound,    0, 3 );
  roundLayout->addWidget( roundFrameLab,  1, 0 );
  roundLayout->addWidget( myFrameRound,   1, 1 );
  roundLayout->addWidget( roundSliderLab, 1, 2 );
  roundLayout->addWidget( mySliderRound,  1, 3 );
  roundLayout->addWidget( myAntiAliasing, 2, 0, 1, 4 );
  
  QGroupBox* handleGroup       = new QGroupBox( tr( "Handle" ), w2 );
  QLabel*    horHandleLab      = new QLabel( tr( "Horizontal spacing" ), handleGroup );
  myHorHandleDelta  = new QSpinBox( handleGroup );
  QLabel*    verHandleLab      = new QLabel( tr( "Vertical spacing" ), handleGroup );
  myVerHandleDelta  = new QSpinBox( handleGroup );
  QLabel*    splitterLengthLab = new QLabel( tr( "Splitter handle size" ), handleGroup );
  mySplitterLength  = new QSpinBox( handleGroup );
  QLabel*    sliderLengthLab   = new QLabel( tr( "Slider handle size" ), handleGroup );
  mySliderSize      = new QSpinBox( handleGroup );
  
  QGridLayout* handleLayout = new QGridLayout( handleGroup );
  handleLayout->setMargin( MARGIN ); handleLayout->setSpacing( SPACING );
  handleLayout->addWidget( horHandleLab,      0, 0 );
  handleLayout->addWidget( myHorHandleDelta,  0, 1 );
  handleLayout->addWidget( verHandleLab,      0, 2);
  handleLayout->addWidget( myVerHandleDelta,  0, 3 );
  handleLayout->addWidget( splitterLengthLab, 1, 0 );
  handleLayout->addWidget( mySplitterLength,  1, 1 );
  handleLayout->addWidget( sliderLengthLab,   1, 2 );
  handleLayout->addWidget( mySliderSize,      1, 3 );

  QGroupBox* effectGroup = new QGroupBox( tr( "Widget effect" ), w2 );
  myEffectNone      = new QRadioButton( tr( "None" ),               effectGroup );
  myEffectHighlight = new QRadioButton( tr( "Highlight widgets" ),  effectGroup );
  myEffectAutoRaise = new QRadioButton( tr( "Auto raise widgets" ), effectGroup );
  QButtonGroup* aGroup = new QButtonGroup( w2 );
  aGroup->addButton( myEffectNone );
  aGroup->addButton( myEffectHighlight );
  aGroup->addButton( myEffectAutoRaise );
  myEffectNone->setChecked( true );
  myCurrentEffect = myEffectNone;

  QHBoxLayout* effectLayout = new QHBoxLayout( effectGroup );
  effectLayout->setMargin( MARGIN ); effectLayout->setSpacing( SPACING );
  effectLayout->addWidget( myEffectNone );
  effectLayout->addWidget( myEffectHighlight );
  effectLayout->addWidget( myEffectAutoRaise );

  vLayout->addWidget( fontGroup );
  vLayout->addWidget( linesGroup );
  vLayout->addWidget( roundGroup );
  vLayout->addWidget( handleGroup );
  vLayout->addWidget( effectGroup );
  vLayout->addStretch();

  // ...
  myStylesTab->addTab( w1, tr( "Colors" )  );
  myStylesTab->addTab( w2, tr( "Properties" )  );

  // initialize dialog box
  setFocusProxy( fr );
  setButtonPosition( Right, Close );
  setDialogFlags( AlignOnce );
  myStylesList->setEditTriggers( QAbstractItemView::EditKeyPressed );
  myStylesList->installEventFilter( this );

  QStringList globalStyles = resourceMgr()->styles( Style_ResourceMgr::Global );
  QStringList userStyles   = resourceMgr()->styles( Style_ResourceMgr::User );

  QListWidgetItem* item;

  // current style
  item = new QListWidgetItem( tr( "[ Current ]" ) );
  item->setForeground( QColor( Qt::red ) );
  item->setData( TypeRole, QVariant( Current ) );
  myStylesList->addItem( item );
  // default style
  item = new QListWidgetItem( tr( "[ Default ]" ) );
  item->setForeground( QColor( Qt::green ) );
  item->setData( TypeRole, QVariant( Default ) );
  myStylesList->addItem( item );
  // global styles
  foreach ( QString sname, globalStyles ) {
    item = new QListWidgetItem( sname );
    item->setForeground( QColor( Qt::blue ) );
    item->setData( TypeRole, QVariant( Global ) );
    item->setData( NameRole, QVariant( sname ) );
    myStylesList->addItem( item );
  }
  // user styles
  foreach ( QString sname, userStyles ) {
    item = new QListWidgetItem( sname );
    item->setData( TypeRole, QVariant( User ) );
    item->setData( NameRole, QVariant( sname ) );
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    myStylesList->addItem( item );
  }
  
  // connect widgets
  connect( myStyleCheck,        SIGNAL( toggled( bool ) ),        fr,   SLOT( setEnabled( bool ) ) );
  connect( myStylesList,        SIGNAL( itemSelectionChanged() ), this, SLOT( onStyleChanged() ) );
  connect( myStylesList,        SIGNAL( itemChanged( QListWidgetItem* ) ),       
           this, SLOT( onItemChanged( QListWidgetItem* ) ) );
  connect( myStylesList,        SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), 
           this, SLOT( onApply() ) );
  connect( myLinesCombo,        SIGNAL( activated( int ) ),       this, SLOT( onLinesType() ) );
  connect( myPaletteEditor,     SIGNAL( changed() ),              this, SIGNAL( styleChanged() ) );
  connect( myFontEdit,          SIGNAL( changed( QFont ) ),       this, SIGNAL( styleChanged() ) );
  connect( myLinesTransparency, SIGNAL( valueChanged( int ) ),    this, SIGNAL( styleChanged() ) );
  connect( myButtonRound,       SIGNAL( valueChanged( double ) ), this, SIGNAL( styleChanged() ) );
  connect( myEditRound,         SIGNAL( valueChanged( double ) ), this, SIGNAL( styleChanged() ) );
  connect( myFrameRound,        SIGNAL( valueChanged( double ) ), this, SIGNAL( styleChanged() ) );
  connect( mySliderRound,       SIGNAL( valueChanged( double ) ), this, SIGNAL( styleChanged() ) );
  connect( myAntiAliasing,      SIGNAL( toggled( bool ) ),        this, SIGNAL( styleChanged() ) );
  connect( myHorHandleDelta,    SIGNAL( valueChanged( int ) ),    this, SIGNAL( styleChanged() ) );
  connect( myVerHandleDelta,    SIGNAL( valueChanged( int ) ),    this, SIGNAL( styleChanged() ) );
  connect( mySplitterLength,    SIGNAL( valueChanged( int ) ),    this, SIGNAL( styleChanged() ) );
  connect( mySliderSize,        SIGNAL( valueChanged( int ) ),    this, SIGNAL( styleChanged() ) );
  connect( aGroup,              SIGNAL( buttonClicked( QAbstractButton* ) ),
           this, SLOT( onEffectChanged( QAbstractButton* ) ) );
  connect( this,                SIGNAL( styleChanged() ),         this, SLOT( onChanged() ) );
  
  connect( this, SIGNAL( dlgApply() ), this, SLOT( onApply() ) );
  connect( this, SIGNAL( dlgHelp() ),  this, SLOT( onHelp() ) );

  myStylesList->setCurrentRow( 0 );
  
  myStyleCheck->setChecked( Style_Salome::isActive() );
  fr->setEnabled( Style_Salome::isActive() );
}

/*!
  \brief Destructor
*/
Style_PrefDlg::~Style_PrefDlg()
{
}

/*!
  \brief Called when "OK" button is clicked
*/
void Style_PrefDlg::accept()
{
  onApply();
  QtxDialog::accept();
}

/*!
  \brief Process key press event
  \param e key event
*/
bool Style_PrefDlg::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myStylesList && e->type() == QEvent::KeyPress ) {
    QKeyEvent* ke = (QKeyEvent*)e;
    if ( ke->key() == Qt::Key_Delete ) {
      QListWidgetItem* item = myStylesList->currentItem();
      if ( item && item->data( TypeRole ).toInt() == User ) {
	if ( QMessageBox::question( this,
				    tr( "Delete user theme" ),
				    tr( "Remove theme %1?" ).arg( item->text() ),
				    QMessageBox::Yes | QMessageBox::No,
				    QMessageBox::Yes ) == QMessageBox::Yes ) {
	  resourceMgr()->remove( item->data( NameRole ).toString() );
	  resourceMgr()->save();
	  delete item;
	}
      }
    }
    if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return ) {
//       onApply();
//       return true;
    }
  }
  return QtxDialog::eventFilter( o, e );
}

/*!
  \brief Get SALOME themes resource manager
  \return themes resource manager
*/
Style_ResourceMgr* Style_PrefDlg::resourceMgr()
{
  if ( !myResMgr )
    myResMgr = new Style_ResourceMgr();
  return myResMgr;
}

/*!
  \brief Initialize dialog box fields from SALOME style model
  \param model style model
*/
void Style_PrefDlg::fromModel( Style_Model* model )
{
  if ( !model ) return;
  
  // colors
  for ( int i = (int)Style_Model::WindowText; i < (int)Style_Model::LastColor; i++ ) {
    if ( i == Style_Model::NoRole ) continue;
    myPaletteEditor->setColor( i, QPalette::Active,   model->color( (Style_Model::ColorRole)i, QPalette::Active ) );
    myPaletteEditor->setColor( i, QPalette::Inactive, model->color( (Style_Model::ColorRole)i, QPalette::Inactive ) );
    myPaletteEditor->setColor( i, QPalette::Disabled, model->color( (Style_Model::ColorRole)i, QPalette::Disabled ) );
  }
  myPaletteEditor->setAuto( model->isAutoPalette() );
  // font
  myFontEdit->setCurrentFont( model->applicationFont() );
  // lines type
  int idx = myLinesCombo->findData( QVariant( model->linesType() ) );
  if ( idx >= 0 ) myLinesCombo->setCurrentIndex( idx );
  // lines transparency
  myLinesTransparency->setValue( model->linesTransparency() );
  // widgets rounding
  myButtonRound->setValue( model->widgetRounding( Style_Model::ButtonRadius ) );
  myEditRound->setValue( model->widgetRounding( Style_Model::EditRadius ) );
  myFrameRound->setValue( model->widgetRounding( Style_Model::FrameRadius ) );
  mySliderRound->setValue( model->widgetRounding( Style_Model::SliderRadius ) );
  // widgets antialiasing
  myAntiAliasing->setChecked( model->antialiasing() );
  // handle delta
  myHorHandleDelta->setValue( model->handleDelta( Qt::Horizontal ) );
  myVerHandleDelta->setValue( model->handleDelta( Qt::Vertical ) );
  // splitter handle lentgh
  mySplitterLength->setValue( model->splitHandleLength() );
  // slider size
  mySliderSize->setValue( model->sliderSize() );
  // widgets effect
  Style_Model::WidgetEffect we = model->widgetEffect();
  if ( we == Style_Model::HighlightEffect ) myEffectHighlight->setChecked( true );
  else if ( we == Style_Model::AutoRaiseEffect ) myEffectAutoRaise->setChecked( true );
  else myEffectNone->setChecked( true );
}
 
/*!
  \brief Save values from dialog box fields to SALOME style model
  \param model style model
*/
void Style_PrefDlg::toModel( Style_Model* model ) const
{
  if ( !model ) return;
  
  // colors
  // ... first set Button color 'cause it is used to calculate other ones
  if ( myPaletteEditor->isAuto() )
    model->setColor( Style_Model::Button,
                     myPaletteEditor->color( Style_Model::Button, QPalette::Active ) );
  else 
    model->setColor( Style_Model::Button,
                     myPaletteEditor->color( Style_Model::Button, QPalette::Active ),
                     myPaletteEditor->color( Style_Model::Button, QPalette::Inactive ),
                     myPaletteEditor->color( Style_Model::Button, QPalette::Disabled ));
  // ... then set all other colors
  for ( int i = (int)Style_Model::WindowText; i < (int)Style_Model::LastColor; i++ ) {
    if ( i == Style_Model::NoRole ) continue; // not supported
    if ( i == Style_Model::Button ) continue; // already set
    if ( myPaletteEditor->isAuto() )
      model->setColor( (Style_Model::ColorRole)i,
                       myPaletteEditor->color( i, QPalette::Active ) );
    else 
      model->setColor( (Style_Model::ColorRole)i,
                       myPaletteEditor->color( i, QPalette::Active ),
                       myPaletteEditor->color( i, QPalette::Inactive ),
                       myPaletteEditor->color( i, QPalette::Disabled ));
  }
  model->setAutoPalette( myPaletteEditor->isAuto() ); // internal
  // font
  model->setApplicationFont( myFontEdit->currentFont() );
  // lines type
  model->setLinesType( (Style_Model::LineType)myLinesCombo->itemData( myLinesCombo->currentIndex() ).toInt() );
  // lines transparency
  model->setLinesTransparency( myLinesTransparency->value() );
  // widgets rounding
  model->setWidgetRounding( Style_Model::ButtonRadius, myButtonRound->value() );
  model->setWidgetRounding( Style_Model::EditRadius,   myEditRound->value() );
  model->setWidgetRounding( Style_Model::FrameRadius,  myFrameRound->value() );
  model->setWidgetRounding( Style_Model::SliderRadius, mySliderRound->value() );
  // widgets antialiasing
  model->setAntialiasing( myAntiAliasing->isChecked() );
  // handle delta
  model->setHandleDelta( Qt::Horizontal, myHorHandleDelta->value() );
  model->setHandleDelta( Qt::Vertical,   myVerHandleDelta->value() );
  // splitter handle lentgh
  model->setSplitHandleLength( mySplitterLength->value() );
  // slider size
  model->setSliderSize( mySliderSize->value() );
  // widgets effect
  if      ( myEffectHighlight->isChecked() ) model->setWidgetEffect( Style_Model::HighlightEffect );
  else if ( myEffectAutoRaise->isChecked() ) model->setWidgetEffect( Style_Model::AutoRaiseEffect );
  else                                       model->setWidgetEffect( Style_Model::NoEffect );
}

/*!
  \brief Find unique name for the theme name
  \param name theme name template
  \param item if not 0, used to be ignored when browsing through items list
  \param addSuffix if \c true, the integrer suffix is always added to the theme name (otherwise
  suffix is added only if item name is not unique)
  \return new unique theme name
*/
QString Style_PrefDlg::findUniqueName( const QString& name, QListWidgetItem* item, bool addSuffix )
{
  bool found = false;
  int idx = 0;
  for( int i = 2; i < myStylesList->count(); i++ ) {
    if ( item == myStylesList->item( i ) ) continue;
    QString iname = myStylesList->item( i )->text();
    if ( iname == name ) {
      found = true;
    }
    else {
      iname = iname.mid( name.length() ).trimmed();
      bool ok = false;
      int nx = iname.toInt( &ok );
      if ( ok ) idx = qMax( idx, nx );
    }
  }
  return found || addSuffix ? QString( "%1 %2" ).arg( name ).arg( idx+1 ) : name;
}

/*!
  \brief Called when "Apply" button is pressed
*/
void Style_PrefDlg::onApply()
{
  // save user schemas
  resourceMgr()->save();
  // set selected schema as current
  if ( myStylesList->currentRow() >= 0 ) {
    Style_Model* model = Style_Salome::model();
    toModel( model );
    model->save();
    Style_Salome::update();
    if ( myStyleCheck->isChecked() )
      Style_Salome::apply();
    else
      Style_Salome::restore();
  }
}

/*!
  \brief Called when "Help" button is pressed
*/
void Style_PrefDlg::onHelp()
{
}

/*!
  \brief Called when user selects any theme item in the themes list
*/
void Style_PrefDlg::onStyleChanged()
{
  blockSignals( true );

  QListWidgetItem* item = myStylesList->currentItem();
  int type = item->data( TypeRole ).toInt();

  Style_Model* model = 0;

  switch ( type ) {
  case Current:
    // current style
    model = Style_Salome::model();
    break;
  case Default:
    // default style
    model = new Style_Model();
    break;
  case Global:
  case User:
    // global style, user style
    model = new Style_Model();
    model->fromResources( resourceMgr(), item->data( NameRole ).toString() );
    break;
  default:
    break;
  }

  fromModel( model );
  if ( type != Current )
    delete model;

  onLinesType();

  blockSignals( false );
}

/*!
  \brief Called when lines type is changed
*/
void Style_PrefDlg::onLinesType()
{
  myLinesTransparency->setEnabled( myLinesCombo->itemData( myLinesCombo->currentIndex() ) != Style_Model::NoLines );
  emit( styleChanged() );
}

/*!
  \brief Called when any style parameter is changed by the user
*/
void Style_PrefDlg::onChanged()
{
  QListWidgetItem* item = myStylesList->currentItem();
  int type = item->data( TypeRole ).toInt();

  // for the current and user schemas do not perform any actions
  if ( type == Current ) {
    Style_Model model = *( Style_Salome::model() );
    toModel( &model );
    model.save();
    blockSignals( true );
    fromModel( &model );
    blockSignals( false );
  }
  else if ( type == User ) {
    Style_Model model;
    toModel( &model );
    QString oldName = item->data( NameRole ).toString(), newName = item->text();
    if ( oldName == newName ) {
      model.save( resourceMgr(), oldName );
    }
    else {
      resourceMgr()->remove( oldName );
      model.save( resourceMgr(), newName );
      item->setData( NameRole, newName );
    }
    blockSignals( true );
    fromModel( &model );
    blockSignals( false );
  }
  else {
    // if user tries to change global (or default) schema, we create new user schema basing on selected one
    QString newName = findUniqueName( tr( "Custom schema" ), 0, true );
    item = new QListWidgetItem( newName );
    item->setData( TypeRole, QVariant( User ) );
    item->setData( NameRole, QVariant( newName ) );
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    myStylesList->addItem( item );
    
    Style_Model model;
    toModel( &model );
    model.save( resourceMgr(), newName );
    
    myStylesList->setCurrentItem( item );
  }
}

/*!
  \brief Called when user theme is renamed by the user
*/
void Style_PrefDlg::onItemChanged( QListWidgetItem* item )
{
  QString newName = item->text();
  QString uniqueName = findUniqueName( newName, item );
  if ( uniqueName != newName ) {
    myStylesList->blockSignals( true );
    item->setText( uniqueName );
    myStylesList->blockSignals( false );
  }
  onChanged();
}                               

/*!
  \brief Called when widget effect is changed
*/
void Style_PrefDlg::onEffectChanged( QAbstractButton* rb )
{
  if ( rb != myCurrentEffect )
    emit( styleChanged() );
  myCurrentEffect = qobject_cast<QRadioButton*>( rb );
}
