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

// File   : Style_Model.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S (vadim.sandler@opencascade.com)
//
#include "Style_Model.h"
#include "Style_Salome.h"

#include <QtxResourceMgr.h>

#include <QApplication>
#include <QColor>
#include <QFont>
#include <QPalette>

/*!
  \brief Mix two colors to get color with averaged red, green, blue and alpha-channel values
  \internal
  \param c1 first color
  \param c2 second color
  \return averaged color
*/
static QColor mixColors( const QColor& c1, const QColor& c2 )
{
  return QColor( (c1.red()   + c2.red() )   / 2,
                 (c1.green() + c2.green() ) / 2,
                 (c1.blue()  + c2.blue() )  / 2,
                 (c1.alpha() + c2.alpha() ) / 2 );
}

/*!
  \class Style_Model
  \brief SALOME style model

  Style model class stores properties of the SALOME style, like palette colors,
  widget roundings etc. It allows reading these properties from the resource file
  and write them back to resource file.

  SALOME_Style class provides an access to the global style model (which is applied
  to the application). To get access to the global SALOME style model, use static
  Style_Model::model() function.

  \sa Style_Salome class
*/

/*!
  \brief Application style data
  \internal
*/
Style_Model::AppData* Style_Model::myAppData = 0;

/*!
  \brief Constructor

  Create new SALOME style model with default properties.
*/
Style_Model::Style_Model()
  : myResourceMgr( 0 )
{
  initDefaults(); // init from default values
}

/*!
  \brief Destructor
*/
Style_Model::~Style_Model()
{
}

/*!
  \brief Initialize model from the current application style
  
  This function is useful when it is necessary to set/remove SALOME style
  dynamically. Function fromApplication() saves current application properties
  (style, palette, font) which can be later restored with the restore() function.

  The simplest way it can be done is using static functions of Style_Salome class:
  Style_Salome::apply(), Style_Salome::restore()

  \param reset if \c true model is also initializes preoperties from the application
  \sa restore(), fromResources()
  \sa Style_Salome class
*/
void Style_Model::fromApplication( bool reset )
{
  initDefaults();

  if ( !QApplication::instance() )  // application object is not created yet
    return;

  if ( !myAppData ) // if not yes initialized from the application init myAppData
    myAppData = new AppData;

  // store original application's style, palette, etc
  if ( !Style_Salome::isActive() ) {
    myAppData->myStyle   = QApplication::style();
    myAppData->myPalette = QApplication::palette();
    myAppData->myFont    = QApplication::font();
  }

  // initialize style properties from the application

  if ( !reset ) return;

  // font
  myFont = myAppData->myFont;
  // colors
  for ( int i = (int)QPalette::Active; i <= (int)QPalette::Inactive; i++ ) {
    for ( int j = (int)Style_Model::WindowText; j < (int)Style_Model::NColorRoles; j++ ) {
      myColors[ (QPalette::ColorGroup)i ][ (Style_Model::ColorRole)j ] =
        myAppData->myPalette.color( (QPalette::ColorGroup)i, (QPalette::ColorRole)j );
    }
  }

  QColor dark = myAppData->myPalette.color( QPalette::Dark );
  setColor( BorderTop,       dark.lighter() );
  setColor( BorderBottom,    dark.darker() );
  setColor( TabBorderTop,    dark.lighter().lighter() );
  setColor( TabBorderBottom, dark.darker().darker() );
  setColor( FieldLight,      myAppData->myPalette.color( QPalette::Light ) );
  setColor( FieldDark,       myAppData->myPalette.color( QPalette::Mid ).light( 125 ) );
  setColor( ProgressBar,     myAppData->myPalette.color( QPalette::Highlight ) );
  setColor( Pointer,         myAppData->myPalette.color( QPalette::WindowText ) );
  setColor( Checked,         myAppData->myPalette.color( QPalette::Base ) );
  setColor( GridLine,        myAppData->myPalette.color( QPalette::Mid ) );
  setColor( Header,          myAppData->myPalette.color( QPalette::Button ) );
  setColor( Slider,          myAppData->myPalette.color( QPalette::Button ) );
  setColor( HighlightWidget, myAppData->myPalette.color( QPalette::Button ) );
  setColor( HighlightBorder, myAppData->myPalette.color( QPalette::Button ) );
  setColor( Lines,           myAppData->myPalette.color( QPalette::Mid ) );
}

/*!
  \brief Initialize model from the resources

  This function can be used to retrieve SALOME style properties from the resource file(s).
  Note, that paremeters \a resMgr and \a resSection are stored by the model to be used
  later with save() method.

  \param resMgr resources manager
  \param resSection resources section name; if empty (default), "Theme" section is used instead
  \sa fromApplication(), save(), update()
*/
void Style_Model::fromResources( QtxResourceMgr* resMgr, const QString& resSection )
{
  // init from application
  fromApplication( false );

  myResourceMgr     = resMgr;
  myResourceSection = resSection;

  // init from resource manager
  if ( !resourceMgr() )
    return;

  QString section = resourceSection();

  // colors
  // Button
  readColorValue( Button, "button" );
  // Window text
  readColorValue( WindowText, "window-text" );
  // Light
  readColorValue( Light, "light" );
  // Dark
  readColorValue( Dark, "dark" );
  // Mid
  readColorValue( Mid, "mid" );
  // Text
  readColorValue( Text, "text" );
  // BrightText
  readColorValue( BrightText, "bright-text" );
  // ButtonText
  readColorValue( ButtonText, "button-text" );
  // Base
  readColorValue( Base, "base" );
  // Window
  readColorValue( Window, "window" );
  // AlternateBase
  readColorValue( AlternateBase, "alternate-base" );
  // Midlight
  readColorValue( Midlight, "midlight" );
  // Shadow
  readColorValue( Shadow, "shadow" );
  // Highlight
  readColorValue( Highlight, "highlight" );
  // HighlightedText
  readColorValue( HighlightedText, "highlight-text" );
  // Link
  readColorValue( Link, "link" );
  // LinkVisited
  readColorValue( LinkVisited, "link-visited" );
  // ToolTipBase
  readColorValue( ToolTipBase, "tooltip-base" );
  // ToolTipText
  readColorValue( ToolTipText, "tooltip-text" );
  // BorderTop
  readColorValue( BorderTop, "border-top" );
  // BorderBottom
  readColorValue( BorderBottom, "border-bottom" );
  // TabBorderTop
  readColorValue( TabBorderTop, "tab-border-top" );
  // TabBorderBottom
  readColorValue( TabBorderBottom, "tab-border-bottom" );
  // FieldLight
  readColorValue( FieldLight, "field-light" );
  // FieldDark
  readColorValue( FieldDark, "field-dark" );
  // ProgressBar
  readColorValue( ProgressBar, "progress-bar" );
  // Pointer
  readColorValue( Pointer, "pointer" );
  // Checked
  readColorValue( Checked, "checked" );
  // GridLine
  readColorValue( GridLine, "grid-line" );
  // Header
  readColorValue( Header, "header" );
  // Slider
  readColorValue( Slider, "slider" );
  // HighlightWidget
  readColorValue( HighlightWidget, "highlight-widget" );
  // HighlightBorder
  readColorValue( HighlightBorder, "highlight-border" );
  // Lines
  readColorValue( Lines, "lines" );
  // auto-palette flag (internal)
  if ( resourceMgr()->hasValue( section, "auto-palette" ) ) {
    setAutoPalette( resourceMgr()->booleanValue( section, "auto-palette" ) );
  }
  // lines type
  if ( resourceMgr()->hasValue( section, "lines-type" ) ) {
    int ltype = resourceMgr()->integerValue( section, "lines-type" );
    if ( ltype >= NoLines && ltype <= Inclined )
      setLinesType( (LineType)ltype );
  }
  // lines transparency
  if ( resourceMgr()->hasValue( section, "lines-transparency" ) ) {
    int ltransp = resourceMgr()->integerValue( section, "lines-transparency" );
    if ( ltransp >= 0 && ltransp <= 100 )
      setLinesTransparency( ltransp );
  }
  // application font
  if ( resourceMgr()->hasValue( section, "application-font" ) ) {
    setApplicationFont( resourceMgr()->fontValue( section, "application-font" ) );
  }
  // widgets rounding
  if ( resourceMgr()->hasValue( section, "button-rad" ) ) {
    setWidgetRounding( ButtonRadius, resourceMgr()->doubleValue( section, "button-rad" ) );
  }
  if ( resourceMgr()->hasValue( section, "edit-rad" ) ) {
    setWidgetRounding( EditRadius, resourceMgr()->doubleValue( section, "edit-rad" ) );
  }
  if ( resourceMgr()->hasValue( section, "frame-rad" ) ) {
    setWidgetRounding( FrameRadius, resourceMgr()->doubleValue( section, "frame-rad" ) );
  }
  if ( resourceMgr()->hasValue( section, "slider-rad" ) ) {
    setWidgetRounding( SliderRadius, resourceMgr()->doubleValue( section, "slider-rad" ) );
  }
  // widget effect
  if ( resourceMgr()->hasValue( section, "widget-effect" ) ) {
    int effect = resourceMgr()->integerValue( section, "widget-effect" );
    if ( effect >= NoEffect && effect <= AutoRaiseEffect )
      setWidgetEffect( (WidgetEffect)effect );
  }
  else if ( resourceMgr()->hasValue( section, "is-highlight-widget" ) ||
            resourceMgr()->hasValue( section, "is-raising-widget" ) ) {
    bool highlight = resourceMgr()->booleanValue( section, "is-highlight-widget", false );
    bool autoraise = resourceMgr()->booleanValue( section, "is-highlight-widget", false );
    if ( highlight )
      setWidgetEffect( HighlightEffect );
    else if ( autoraise )
      setWidgetEffect( AutoRaiseEffect );
  }
  if ( resourceMgr()->hasValue( section, "all-antialized" ) ) {
    setAntialiasing( resourceMgr()->booleanValue( section, "all-antialized" ) );
  }
  // handles
  if ( resourceMgr()->hasValue( section, "hor-hadle-delta" ) ) {
    setHandleDelta( Qt::Horizontal, resourceMgr()->integerValue( section, "hor-hadle-delta" ) );
  }
  if ( resourceMgr()->hasValue( section, "ver-hadle-delta" ) ) {
    setHandleDelta( Qt::Vertical, resourceMgr()->integerValue( section, "vsr-hadle-delta" ) );
  }
  if ( resourceMgr()->hasValue( section, "slider-size" ) ) {
    setSliderSize( resourceMgr()->integerValue( section, "slider-size" ) );
  }
  else if ( resourceMgr()->hasValue( section, "slider-increase" ) ) {
    setSliderSize( resourceMgr()->integerValue( section, "slider-increase" ) );
  }
  if ( resourceMgr()->hasValue( section, "split-handle-len" ) ) {
    setSplitHandleLength( resourceMgr()->integerValue( section, "split-handle-len" ) );
  }
}

/*!
  \brief Save SALOME stype properties to the resource file.
  
  If paremeters \a resMgr and \a resSection are not specified, default ones
  (those passed to the fromResources() function) are used instead.

  \param resMgr resources manager
  \param resSection resources section name
  \sa fromResources(), update()
*/
void Style_Model::save( QtxResourceMgr* resMgr, const QString& resSection )
{
  if ( !resMgr )
    resMgr = resourceMgr();
  if ( !resMgr )
    return;

  QString section = resSection.isEmpty() ? resourceSection() : resSection;

  // colors
  // Button
  writeColorValue( Button, "button", resMgr, section );
  // Window text
  writeColorValue( WindowText, "window-text", resMgr, section );
  // Light
  writeColorValue( Light, "light", resMgr, section );
  // Dark
  writeColorValue( Dark, "dark", resMgr, section );
  // Mid
  writeColorValue( Mid, "mid", resMgr, section );
  // Text
  writeColorValue( Text, "text", resMgr, section );
  // BrightText
  writeColorValue( BrightText, "bright-text", resMgr, section );
  // ButtonText
  writeColorValue( ButtonText, "button-text", resMgr, section );
  // Base
  writeColorValue( Base, "base", resMgr, section );
  // Window
  writeColorValue( Window, "window", resMgr, section );
  // AlternateBase
  writeColorValue( AlternateBase, "alternate-base", resMgr, section );
  // Midlight
  writeColorValue( Midlight, "midlight", resMgr, section );
  // Shadow
  writeColorValue( Shadow, "shadow", resMgr, section );
  // Highlight
  writeColorValue( Highlight, "highlight", resMgr, section );
  // HighlightedText
  writeColorValue( HighlightedText, "highlight-text", resMgr, section );
  // Link
  writeColorValue( Link, "link", resMgr, section );
  // LinkVisited
  writeColorValue( LinkVisited, "link-visited", resMgr, section );
  // ToolTipBase
  writeColorValue( ToolTipBase, "tooltip-base", resMgr, section );
  // ToolTipText
  writeColorValue( ToolTipText, "tooltip-text", resMgr, section );
  // BorderTop
  writeColorValue( BorderTop, "border-top", resMgr, section );
  // BorderBottom
  writeColorValue( BorderBottom, "border-bottom", resMgr, section );
  // TabBorderTop
  writeColorValue( TabBorderTop, "tab-border-top", resMgr, section );
  // TabBorderBottom
  writeColorValue( TabBorderBottom, "tab-border-bottom", resMgr, section );
  // FieldLight
  writeColorValue( FieldLight, "field-light", resMgr, section );
  // FieldDark
  writeColorValue( FieldDark, "field-dark", resMgr, section );
  // ProgressBar
  writeColorValue( ProgressBar, "progress-bar", resMgr, section );
  // Pointer
  writeColorValue( Pointer, "pointer", resMgr, section );
  // Checked
  writeColorValue( Checked, "checked", resMgr, section );
  // GridLine
  writeColorValue( GridLine, "grid-line", resMgr, section );
  // Header
  writeColorValue( Header, "header", resMgr, section );
  // Slider
  writeColorValue( Slider, "slider", resMgr, section );
  // HighlightWidget
  writeColorValue( HighlightWidget, "highlight-widget", resMgr, section );
  // HighlightBorder
  writeColorValue( HighlightBorder, "highlight-border", resMgr, section );
  // Lines
  writeColorValue( Lines, "lines", resMgr, section );
  // auto-palette flag (internal)
  resMgr->setValue( section, "auto-palette", isAutoPalette() );

  // lines type
  resMgr->setValue( section, "lines-type", (int)linesType() );
  // lines transparency
  resMgr->setValue( section, "lines-transparency", linesTransparency() );
  // application font
  resMgr->setValue( section, "application-font", applicationFont() );
  // widgets rounding
  resMgr->setValue( section, "button-rad", widgetRounding( ButtonRadius ) );
  resMgr->setValue( section, "edit-rad",   widgetRounding( EditRadius ) );
  resMgr->setValue( section, "frame-rad",  widgetRounding( FrameRadius ) );
  resMgr->setValue( section, "slider-rad", widgetRounding( SliderRadius ) );
  resMgr->setValue( section, "all-antialized", antialiasing() );
  // widget effect
  resMgr->setValue( section, "widget-effect", (int)widgetEffect() );
  // handles
  resMgr->setValue( section, "hor-hadle-delta", handleDelta( Qt::Horizontal ) );
  resMgr->setValue( section, "vsr-hadle-delta", handleDelta( Qt::Vertical ) );
  resMgr->setValue( section, "slider-size", sliderSize() );
  resMgr->setValue( section, "split-handle-len", splitHandleLength() );
}

/*!
  \brief Reload SALOME style properties from the resources file(s).
  \sa fromResources(), save()
*/
void Style_Model::update()
{
  fromResources( resourceMgr(), resourceSection() );
}

/*!
  \brief Restore original style, palette and font to the application

  This function should be used in conjunction with fromApplication() method.
  Sets initial style, color palette and font to the application.
  If SALOME style model has not been initialized from the application,
  this function does nothing.

  \sa fromApplication()
*/
void Style_Model::restore()
{
  if ( !QApplication::instance() ) // application object is not created yet
    return;
  if ( !myAppData ) // not initialized from the application yet
    return;

  QApplication::setStyle( myAppData->myStyle );
  QApplication::setPalette( myAppData->myPalette );
  QApplication::setFont( myAppData->myFont );
}

/*!
  \brief Get resource manager used by this SALOME style model.

  \return pointer to the resource manager passed previously to the fromResources() method
  \sa initFromResources(), resourceSection()
*/
QtxResourceMgr* Style_Model::resourceMgr() const
{
  return myResourceMgr;
}

/*!
  \brief Get resources section name

  If section name is empty, default "Theme" is returned

  \return resource section name passed previously to the fromResources() method
  \sa initFromResources(), resourceMgr()
*/
QString Style_Model::resourceSection() const
{
  return !myResourceSection.isEmpty() ? myResourceSection : "Theme";
}

/*!
  \brief Get palette color value
  \param role color role
  \param cg color group
  \return a color which should be used to draw the corresponding part of the application
  \sa setColor()
*/
QColor Style_Model::color( ColorRole role, QPalette::ColorGroup cg ) const
{
  QColor c = myColors[ cg ][ role ];
  if ( !c.isValid() ) c = myColors[ QPalette::Active ][ role ];
  return c;
}

/*!
  \brief Set palette color value

  If \a inactive and/or \a disabled colors are not specified, they are automatically
  calculated from \a active color.

  \param role color role
  \param active a color to be used with active color group (QPalette::Active)
  \param inactive a color to be used with inactive color group (QPalette::Inactive)
  \param disabled a color to be used with disabled color group (QPalette::Disabled)
  \sa color()
*/
void Style_Model::setColor( Style_Model::ColorRole role, const QColor& active,
                            const QColor& inactive, const QColor& disabled )
{
  QColor ac = active, ic = inactive, dc = disabled;

  if ( !ic.isValid() ) {
    ic = ac;
  }
  if ( !dc.isValid() ) {
    switch ( role ) {
    case WindowText:
    case Text:
    case ButtonText:
      dc = color( Button ).darker();
      break;
    case Base:
      dc = color( Button );
      break;
    case AlternateBase:
      dc = mixColors( color( Base,  QPalette::Inactive ), color( Button, QPalette::Inactive ) );
      break;
    case Midlight:
      dc = mixColors( color( Light, QPalette::Inactive ), color( Button, QPalette::Inactive ) );
      break;
    default:
      dc = ac;
      break;
    }
  }

  setColor( role, QPalette::Active,   ac );
  setColor( role, QPalette::Inactive, ic );
  setColor( role, QPalette::Disabled, dc );
}

/*!
  \brief Set palette color value

  If \a inactive and/or \a disabled colors are not specified, they are automatically
  calculated from \a active color.

  \param role color role
  \param cg color group
  \param c color which should be used to draw the corresponding part of the application
  \sa color()
*/
void Style_Model::setColor( Style_Model::ColorRole role, QPalette::ColorGroup cg, const QColor& c )
{
  myColors[ cg ][ role ] = c;
}

/*!
  \brief Returns 'auto-calculating color values' flag
  \return 'auto-calculating color values' flag
  \internal
  \sa setAutoPalette()
*/
bool Style_Model::isAutoPalette() const
{
  return myAutoPalette;
}

/*!
  \brief Set/clear 'auto-calculating color values' flag
  \param on new value of 'auto-calculating color values' flag
  \internal
  \sa isAutoPalette()
*/
void Style_Model::setAutoPalette( bool on )
{
  myAutoPalette = on;
}

/*!
  \brief Get lines type
  \return current lines type
  \sa setLinesType(), linesTransparency()
*/
Style_Model::LineType Style_Model::linesType() const
{
  return myLinesType;
}

/*!
  \brief Set lines type
  \param lt new lines type
  \sa linesType(), linesTransparency()
*/
void Style_Model::setLinesType( LineType lt )
{
  myLinesType = lt;
}

/*!
  \brief Get lines transparency value
  \return current lines transparency
  \sa setLinesTransparency(), linesType()
*/
int Style_Model::linesTransparency() const
{
  return myLinesTransparency;
}

/*!
  \brief Set lines transparency value
  \param transparency new lines transparency
  \sa linesTransparency(), linesType()
*/
void Style_Model::setLinesTransparency( int transparency )
{
  myLinesTransparency = transparency;
}

/*!
  \brief Get application font
  \return current application font
  \sa setApplicationFont()
*/
QFont Style_Model::applicationFont() const
{
  return myFont;
}

/*!
  \brief Set application font
  \param font new application font
  \sa applicationFont()
*/
void Style_Model::setApplicationFont( const QFont& font )
{
  myFont = font;
}

/*!
  \brief Get widget corners rounding radius value
  \param wr widget type
  \return current widget corners rounding
  \sa setWidgetRounding(), antialiasing()
*/
double Style_Model::widgetRounding( Style_Model::WidgetRounding wr ) const
{
  return myWidgetRounding[ wr ];
}

/*!
  \brief Set widget corners rounding radius value
  \param wr widget type
  \param value new widget corners rounding
  \sa widgetRounding(), antialiasing()
*/
void Style_Model::setWidgetRounding( WidgetRounding wr, double value )
{
  myWidgetRounding[ wr ] = value;
}

/*!
  \brief Get anti-aliasing flag value
  \return \c true if widgets borders should be antialiased
  \sa setAntialiasing(), widgetRounding()
*/
bool Style_Model::antialiasing() const
{
  return myAntiAliasing;
}

/*!
  \brief Set anti-aliasing flag value
  \param value if \c true, widgets borders should be antialiased
  \sa antialiasing(), widgetRounding()
*/
void Style_Model::setAntialiasing( bool value )
{
  myAntiAliasing = value;
}

/*!
  \brief Get widget effect
  \return current widget effect
  \sa setWidgetEffect()
*/
Style_Model::WidgetEffect Style_Model::widgetEffect() const
{
  return myWidgetEffect;
}

/*!
  \brief Set widget effect
  \param we new widget effect
  \sa widgetEffect()
*/
void Style_Model::setWidgetEffect( WidgetEffect we )
{
  myWidgetEffect = we;
}

/*!
  \brief Get handle spacing value
  \param o handle spacing direction
  \return current handle spacing value
  \sa setHandleDelta()
*/
int Style_Model::handleDelta( Qt::Orientation o ) const
{
  return myHandleDelta[ o ];
}

/*!
  \brief Set handle spacing value
  \param o handle spacing direction
  \param value new handle spacing value
  \sa handleDelta()
*/
void Style_Model::setHandleDelta( Qt::Orientation o, int value )
{
  myHandleDelta[ o ] = value;
}

/*!
  \brief Get splitter handle length
  \return current splitter handle length
  \sa setSplitHandleLength()
*/
int Style_Model::splitHandleLength() const
{
  return mySplitHandleLength;
}

/*!
  \brief Set splitted handle length
  \param value new splitter handle length
  \sa splitHandleLength()
*/
void Style_Model::setSplitHandleLength( int value )
{
  mySplitHandleLength = value; 
}

/*!
  \brief Get slider handle extra size
  \return current slider handle extra size
  \sa setSliderSize()
*/
int Style_Model::sliderSize() const
{
  return mySliderSize;
}

/*!
  \brief Set slider handle extra size
  \param value new slider handle extra size
  \sa sliderSize()
*/
void Style_Model::setSliderSize( int value )
{
  mySliderSize = value;
}

/*!
  \brief Initialize model with the default values
*/
void Style_Model::initDefaults()
{
  // default application font
  myFont.fromString( "Sans Serif,9,-1,5,50,0,0,0,0,0" );

  // default palette colors
  myAutoPalette = false;
  QColor btn = QColor( "#e6e7e6" );
  QColor fg  = QColor( "#000000" );
  QColor bg  = QColor( "#ffffff" );
  setColor( Button,          btn );                  // = (230, 231, 230)
  setColor( WindowText,      fg );                   // = (  0,   0,   0)
  setColor( Light,           bg );                   // = (255, 255, 255)
  setColor( Dark,            btn.darker() );         // = (115, 115, 115) // btn.darker( 130 ) = (177, 178, 177)
  setColor( Mid,             btn.darker( 150 ) );    // = (153, 154, 153)
  setColor( Text,            fg );                   // = (  0,   0,   0)
  setColor( BrightText,      bg );                   // = (255, 255, 255) // fg = (  0,  0,  0)
  setColor( ButtonText,      fg );                   // = (  0,   0,   0)
  setColor( Base,            bg );                   // = (255, 255, 255)
  setColor( Window,          btn );                  // = (230, 231, 230)
  setColor( AlternateBase,   mixColors( bg, btn ) ); // = (242, 243, 242)
  setColor( Midlight,        mixColors( bg, btn ) ); // = (242, 243, 242)
  setColor( Shadow,          fg );                   // = (  0,   0,   0)
  setColor( Highlight,       "#000080" );            // = (  0,   0, 128) // (  33,  68, 156 )
  setColor( HighlightedText, bg );                   // = (255, 255, 255)
  setColor( Link,            "#0000ff" );            // = (  0,   0, 255)
  setColor( LinkVisited,     "#ff00ff" );            // = (255,   0, 255)
  setColor( ToolTipBase,     "#ffffdc" );            // = (255, 255, 220) // ( 230, 231, 230 )
  setColor( ToolTipText,     fg );                   // = (  0,   0,   0)
  setColor( BorderTop,       "#adadad" );            // = (173, 173, 173) // ( 255, 255, 255 )
  setColor( BorderBottom,    "#393939" );            // = ( 57,  57,  57) // (  88,  89,  88 )
  setColor( TabBorderTop,    "#ffffff" );            // = (255, 255, 255)
  setColor( TabBorderBottom, "#0e0e0e" );            // = ( 14,  14,  14) // (  44,  44,  44 )
  setColor( FieldLight,      "#ffffff" );            // = (255, 255, 255)
  setColor( FieldDark,       "#c0c1c0" );            // = (192, 193, 192) // ( 240, 241, 240 )
  setColor( ProgressBar,     "#000080" );            // = (  0,   0, 128) // (  33,  68, 156 )
  setColor( Pointer,         "#000000" );            // = (  0,   0,   0)
  setColor( Checked,         "#ffffff" );            // = (255, 255, 255)
  setColor( GridLine,        "#999a99" );            // = (153, 154, 153) // ( 192, 192, 192 )
  setColor( Header,          "#e6e7e6" );            // = (230, 231, 230)
  setColor( Slider,          "#e6e7e6" );            // = (230, 231, 230)
  setColor( HighlightWidget, "#e6e7e6" );            // = (230, 231, 230)
  setColor( HighlightBorder, "#e6e7e6" );            // = (230, 231, 230)
  setColor( Lines,           "#999a99" );            // = (153, 154, 153) // ( 192, 193, 192 )

  // default values
  myLinesType          = NoLines;
  myWidgetEffect       = NoEffect;
  myAntiAliasing       = false;
  myLinesTransparency  = 0;
  myWidgetRounding[ EditRadius   ] = 0.0;
  myWidgetRounding[ ButtonRadius ] = 0.0;
  myWidgetRounding[ FrameRadius  ] = 0.0;
  myWidgetRounding[ SliderRadius ] = 0.0;
  myHandleDelta[ Qt::Horizontal ]  = 3;
  myHandleDelta[ Qt::Vertical ]    = 3;
  mySplitHandleLength              = 20;
  mySliderSize                     = 2;
}

/*!
  \brief Read palette color values from resources manager
  \param role color role
  \param prefix palette color value resource name prefix
  \sa writeColorValue()
*/
void Style_Model::readColorValue( ColorRole role, const QString& prefix )
{
  if ( !resourceMgr() ) return;

  QString section = resourceSection();
  QString active   = QString( "%1-color" ).arg( prefix );
  QString inactive = QString( "%1-color-inactive" ).arg( prefix );
  QString disabled = QString( "%1-color-disabled" ).arg( prefix );

  if ( resourceMgr()->hasValue( section, active ) )
    setColor( role, resourceMgr()->colorValue( section, active ) );
  if ( resourceMgr()->hasValue( section, inactive ) )
    setColor( role, QPalette::Inactive, resourceMgr()->colorValue( section, inactive ) );
  if ( resourceMgr()->hasValue( section, disabled ) )
    setColor( role, QPalette::Disabled, resourceMgr()->colorValue( section, disabled ) );
}

/*!
  \brief Write palette color values to resources manager
  \param role color role
  \param prefix palette color value resource name prefix
  \param resMgr resource manager
  \param resSection resource section name
  \sa readColorValue()
*/
void Style_Model::writeColorValue( ColorRole role, const QString& prefix,
                                   QtxResourceMgr* resMgr, const QString& resSection ) const
{
  QString active   = QString( "%1-color" ).arg( prefix );
  QString inactive = QString( "%1-color-inactive" ).arg( prefix );
  QString disabled = QString( "%1-color-disabled" ).arg( prefix );

  resMgr->setValue( resSection, active,   color( role, QPalette::Active ) );
  resMgr->setValue( resSection, inactive, color( role, QPalette::Inactive ) );
  resMgr->setValue( resSection, disabled, color( role, QPalette::Disabled ) );
}
