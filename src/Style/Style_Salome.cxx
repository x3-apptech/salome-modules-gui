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

// File   : Style_Salome.cxx
// Author : Natalia Ermolaeva, Open CASCADE S.A.S.
//
#include "Style_Salome.h"
#include "Style_Tools.h"
#include "Style_Model.h"

#include <QApplication>
#include <QPainter>
#include <QMap>
#include <QPainterPath>
#include <QWidget>
#include <QComboBox>
#include <QStyleOptionFrame>
#include <QStyleOptionComplex>
#include <QStyleOptionSpinBox>
#include <QStyleOptionButton>
#include <QStyleOptionTab>
#include <QStyleOptionToolButton>
#include <QStyleOptionFocusRect>
#include <QStyleOption>
#include <QSize>
#include <QToolBar>
#include <QMenuBar>
#include <QToolButton>
#include <QTabWidget>
#include <QTabBar>
#include <QToolTip>
#include <QDockWidget>
#include <QTextEdit>
#include <QTreeView>
#include <QListView>
#include <QHeaderView>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QAbstractSpinBox>
#include <QLineEdit>
#include <QScrollBar>
#include <QSlider>
#include <QMainWindow>
#include <QPixmapCache>
#include <QTileRules>

#define SHADOW          1
#define LINE_GR_MARGIN  10
#define HIGH_WDG_MARGIN 10

#define BUT_PERCENT_COL 125
#define BUT_PERCENT_ON  115

#define DELTA_H_TAB     1
#define DELTA_SLIDER    3

static const char* const hole_xpm[] = {
"3 3 3 1",
". c None",
"a c #999999",
"b c #FFFFFF",
"aa.",
"aab",
".bb"
};

static const char* const cross_xpm[] = {
"12 12 4 1",
". c None",
"a c #000000",
"b c #FFFFFF",
"c c #666666",
"............",
"............",
"............",
".aaaa..aaaa.",
"..abbaabba..",
"...abbbba...",
"....abba....",
"...abbbba...",
"..abbaabba..",
".aaaa..aaaa.",
"............",
"............"
};

static const char* const maximize_xpm[] = {
"12 12 4 1",
". c None",
"a c #000000",
"b c #FFFFFF",
"c c #666666",
"............",
".aaaaaaaaaa.",
".acccccccca.",
".acccccccca.",
".abbbbbbbba.",
".abbbbbbbba.",
".abbbbbbbba.",
".abbbbbbbba.",
".abbbbbbbba.",
".abbbbbbbba.",
".aaaaaaaaaa.",
"............"
};

static const char* const normal_xpm[] = {
"12 12 4 1",
". c None",
"a c #000000",
"b c #FFFFFF",
"c c #666666",
"............",
"...aaaaaaaa.",
"...acccccca.",
"...abbbbbba.",
"...aaaaaaba.",
".aaaaaaaaba.",
".accccccaba.",
".abbbbbbaaa.",
".abbbbbba...",
".abbbbbba...",
".aaaaaaaa...",
"............"
};

static const char* const minimize_xpm[] = {
"12 12 4 1",
". c None",
"a c #000000",
"b c #FFFFFF",
"c c #666666",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"aaaaaaaaaaaa",
"abbbbbbbbbba",
"aaaaaaaaaaaa",
"............"
};

///////////////////////////////////////////////////////////
// FOR debug purposes only!!!
//
int DEBUG_LEVEL = 0;

static bool checkDebugLevel( int level )
{
  return DEBUG_LEVEL == level || DEBUG_LEVEL == -1;
}
//
///////////////////////////////////////////////////////////

Style_Model* Style_Salome::myModel = 0;

/*!
  \class Style_Salome
  \brief SALOME style class.

  To set SALOME style to the application, use static function Style_Salome::apply().
  Static function restore() can be used then to restore initial application style.

  The usual way to use SALOME style is as following:
  \code
  Style_Salome::initialize( resourceMgr() );     // initialize SALOME style from resources
  if ( needSalomeStyle ) Style_Salome::apply();  // set SALOME style to the application
  \endcode

  Style_PrefDlg class can be then used to change style properties, select one of the
  predefined style themes or even create user themes:
  \code
  Style_PrefDlg dlg( desktop() );
  dlg.exec();
  \endcode
*/

/*!
  \brief Constructor

  Use Style_Salome::apply() static function to set SALOME style to the application.
*/
Style_Salome::Style_Salome() : BaseStyle()
{
  // initialize SALOME style resources
  Q_INIT_RESOURCE( Style );
}

/*!
  \brief Destructor
*/
Style_Salome::~Style_Salome()
{
}


/*!
  \brief Initialize SALOME style.

  This method should be called at the very beginning of the application
  which uses SALOME style.
  The method initializes style properties from the current settings.

  If the parameter \a resMgr is not 0, the style is initialized from the
  resources manager.

  The parameter \a section allows customizing of the resources file section
  used to store style's properties. By default "Theme" section name is used.

  \param resMgr resources manager
  \param section resources file section name
  \sa apply(), restore()
*/
void Style_Salome::initialize( QtxResourceMgr* resMgr, const QString& section )
{
  model()->fromResources( resMgr, section );
}

/*!
  \brief Set SALOME style to the application
  \sa initialize(), restore()
*/
void Style_Salome::apply()
{
  // application object is not created yet
  if ( !QApplication::instance() )
    return;

  // update model (from the resources manager if needed)
  model()->update();

  // check if SALOME style is already set to the application
  if ( !isActive() ) {
    // set SALOME style
    QApplication::style()->setParent( 0 );           // avoid deleting original application style
    QApplication::setStyle( new Style_Salome() );    // set style
    QApplication::setDesktopSettingsAware(false);    // prevent the style changing from another tools
  }
  update();                                          // update style
}

/*!
  \brief Restore original style to the application
  \sa initialize(), apply()
*/
void Style_Salome::restore()
{
  // application object is not created yet
  if ( !QApplication::instance() )
    return;
  // check if SALOME style is set to the application
  if ( !isActive() )
    return;

  QApplication::setDesktopSettingsAware(true);

  // restore previous style
  model()->restore();
}

/*!
  \brief Check if the SALOME style is currently set to application.
  \return \c true if SALOME style is set to the applcation or \c false otherwise
*/
bool Style_Salome::isActive()
{
  return QApplication::instance() != 0 && qobject_cast<Style_Salome*>( QApplication::style() ) != 0;
}

/*!
  \brief Update style from resources.
*/
void Style_Salome::update()
{
  // application object is not created yet
  if ( !QApplication::instance() )
    return;
  // check if SALOME style is set to the application
  if ( Style_Salome* style = qobject_cast<Style_Salome*>( QApplication::style() ) )
    style->polish( qobject_cast<QApplication*>( QApplication::instance() ) );
}

/*!
  \brief Get style model
  \return style model
*/
Style_Model* Style_Salome::model()
{
  if ( !myModel )
    myModel = new Style_Model();
  return myModel;
}

/*!
  \brief Initialize the appearance of the application.
  
  Performs late initialization of the given application object.

  \param app application object
*/
void Style_Salome::polish ( QApplication* app )
{
  BaseStyle::polish( app );
  
  if ( checkDebugLevel(1) ) {
    return;
  }

  updatePaletteColors();
  app->setFont( model()->applicationFont() );
  updateAllWidgets( app );
}

/*!
  \brief Initializes the appearance of the given widget.

  This function is called for every widget at some point after
  it has been fully created but just before it is shown for the very first time.

  \param w widget object
*/
void Style_Salome::polish ( QWidget* w )
{
  if ( checkDebugLevel(2) ) {
    BaseStyle::polish( w );
    return;
  }

  if ( w && hasHover() ) {
    if ( qobject_cast<QPushButton*>(w) || qobject_cast<QToolButton*>(w)      ||
         qobject_cast<QCheckBox*>(w)   || qobject_cast<QRadioButton*>(w)     ||
         qobject_cast<QComboBox*>(w)   || qobject_cast<QAbstractSpinBox*>(w) ||
         qobject_cast<QLineEdit*>(w)   || qobject_cast<QScrollBar*>(w)       ||
         qobject_cast<QTabBar*>(w)     || qobject_cast<QSlider*>(w)          ||
         qobject_cast<QMenuBar*>(w)    || qobject_cast<QDockWidget*>(w) )
      w->setAttribute( Qt::WA_Hover );
  }
    BaseStyle::polish( w );
}

/*!
  \brief Uninitialize the given widget's appearance.

  This function is the counterpart to polish().
  It is called for every polished widget whenever the style
  is dynamically changed; the former style has to unpolish
  its settings before the new style can polish them again.

  \param w widget object
*/
void Style_Salome::unpolish( QWidget* w )
{
  if ( checkDebugLevel(3) ) {
    BaseStyle::unpolish( w );
    return;
  }

  if ( w && hasHover() ) {
    if ( qobject_cast<QPushButton*>(w) || qobject_cast<QToolButton*>(w)||
         qobject_cast<QCheckBox*>(w) || qobject_cast<QRadioButton*>(w) ||
         qobject_cast<QComboBox*>(w) || qobject_cast<QAbstractSpinBox*>(w) ||
         qobject_cast<QLineEdit*>(w) || qobject_cast<QScrollBar*>(w) ||
         qobject_cast<QTabBar*>(w) || qobject_cast<QSlider*>(w) ||
         qobject_cast<QMenuBar*>(w) || qobject_cast<QDockWidget*>(w) )
      w->setAttribute( Qt::WA_Hover, false );
  }
  BaseStyle::unpolish( w );
}

/*!
  \brief Draws the given control using the provided painter \p with the style options specified by \a opt.
  \param cc complex control type
  \param opt style option
  \param p painter
  \param w widget
*/
void Style_Salome::drawComplexControl( ComplexControl cc, const QStyleOptionComplex* opt,
                                       QPainter* p, const QWidget* w ) const
{
  if ( checkDebugLevel(4) ) {
    BaseStyle::drawComplexControl( cc, opt, p, w );
    return;
  }

  const QPalette& pal = w->palette();
  switch( cc ) {
    case CC_SpinBox:
      if (const QStyleOptionSpinBox *spin = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
        bool hover = hasHover() && (opt->state & State_Enabled) && (opt->state & State_MouseOver);
        QRect optr = opt->rect, arUp =   subControlRect( cc, spin, SC_SpinBoxUp, w );
        if (spin->buttonSymbols != QAbstractSpinBox::NoButtons)
          optr.setWidth( arUp.x()-optr.x()+1 );
        double aRad = model()->widgetRounding( Style_Model::EditRadius );
        bool antialized = model()->antialiasing();
        QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
        QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
        if ( hover )
          drawHoverRect(p, optr, opt->palette.color( QPalette::Window ), aRad, Style_Tools::Left, true);
        else
          Style_Tools::shadowRect( p, optr, aRad, LINE_GR_MARGIN, SHADOW, Style_Tools::Left,
                                   model()->color( Style_Model::FieldLight ), // Style_Model::fld_light_clr,
                                   model()->color( Style_Model::FieldDark ),  // Style_Model::fld_dark_clr,
                                   aBrdTopCol, aBrdBotCol, antialized, false );
        QRect aBtnRect = QRect( QPoint( arUp.x(), optr.y() ), QPoint( arUp.right(), optr.bottom() ) );
        QColor aBtnCol = opt->palette.color( QPalette::Button );
        bool aStateOn = opt->state & ( State_Sunken | State_On );
        if ( hover )
          drawHoverRect(p, aBtnRect, opt->palette.color( QPalette::Window ), aRad, Style_Tools::Right, true);
        else
          Style_Tools::shadowRect( p, aBtnRect, aRad, 0.0, SHADOW, Style_Tools::Right,
                                  aBtnCol.light( BUT_PERCENT_COL ), aBtnCol.dark( BUT_PERCENT_COL ),
                                  aBrdTopCol, aBrdBotCol, antialized, true, aStateOn );
        QStyleOptionSpinBox copy;
        PrimitiveElement pe;
        if (spin->subControls & SC_SpinBoxUp) {
          copy.subControls = SC_SpinBoxUp;
          QPalette pal2 = spin->palette;
          if (!(spin->stepEnabled & QAbstractSpinBox::StepUpEnabled) ||
              !(spin->state & State_Enabled)) {
            pal2.setCurrentColorGroup(QPalette::Disabled);
            copy.state &= ~State_Enabled;
          }
          else
            copy.state |= State_Enabled;
          copy.palette = pal2;
          if ( spin->activeSubControls == SC_SpinBoxUp && ( spin->state & State_Sunken ) ) {
            copy.state |= State_On;
            copy.state |= State_Sunken;
          } else {
            copy.state |= State_Raised;
            copy.state &= ~State_Sunken;
          }
          pe = (spin->buttonSymbols == QAbstractSpinBox::PlusMinus ? PE_IndicatorSpinPlus
               : PE_IndicatorSpinUp);
          copy.rect = aBtnRect;
          drawPrimitive(pe, &copy, p, w);
        }
        if (spin->subControls & SC_SpinBoxDown) {
          copy.subControls = SC_SpinBoxDown;
          copy.state = spin->state;
          QPalette pal2 = spin->palette;
          if (!(spin->stepEnabled & QAbstractSpinBox::StepDownEnabled) ||
               !(spin->state & State_Enabled)) {
            pal2.setCurrentColorGroup(QPalette::Disabled);
            copy.state &= ~State_Enabled;
          }
          else
            copy.state |= State_Enabled;
          copy.palette = pal2;
          if (spin->activeSubControls == SC_SpinBoxDown && (spin->state & State_Sunken)) {
             copy.state |= State_On;
             copy.state |= State_Sunken;
          } else {
            copy.state |= State_Raised;
            copy.state &= ~State_Sunken;
          }
          pe = (spin->buttonSymbols == QAbstractSpinBox::PlusMinus ? PE_IndicatorSpinMinus
               : PE_IndicatorSpinDown);
          copy.rect = aBtnRect;
          drawPrimitive(pe, &copy, p, w);
        }
        break;
     }
     case CC_ComboBox: {
      if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
        bool hover = hasHover() && (opt->state & State_Enabled) && (opt->state & State_MouseOver);
        QRect optr = opt->rect,
              ar = subControlRect( cc, cmb, SC_ComboBoxArrow, w );
        optr.setY( ar.y() );
        optr.setHeight( ar.height() );
        optr.setWidth( ar.x()-optr.x()+1 );
        bool antialized = model()->antialiasing();
        double aRad = model()->widgetRounding( Style_Model::EditRadius );
        QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
        QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
        if ( hover )
          drawHoverRect(p, optr, opt->palette.color( QPalette::Window ), aRad, Style_Tools::Left, true);
        else
          Style_Tools::shadowRect( p, optr, aRad, LINE_GR_MARGIN, SHADOW, Style_Tools::Left,
                                   model()->color( Style_Model::FieldLight ), // Style_Model::fld_light_clr,
                                   model()->color( Style_Model::FieldDark ),  // Style_Model::fld_dark_clr,
                                   aBrdTopCol, aBrdBotCol, antialized, false );
        if (cmb->subControls & SC_ComboBoxArrow) {
          State flags = State_None;
          QColor aBtnCol = opt->palette.color( QPalette::Button );
          bool aStateOn = opt->state & ( State_Sunken | State_On );
          if ( hover )
            drawHoverRect(p, ar, opt->palette.color( QPalette::Window ), aRad, Style_Tools::Right, true);
          else
            Style_Tools::shadowRect( p, ar, aRad, 0.0, SHADOW, Style_Tools::Right,
                                     aBtnCol.light( BUT_PERCENT_COL ), aBtnCol.dark( BUT_PERCENT_COL ),
                                     aBrdTopCol, aBrdBotCol, antialized, true, aStateOn );
          if (opt->state & State_Enabled)
            flags |= State_Enabled;
          if (cmb->activeSubControls == SC_ComboBoxArrow)
            flags |= State_Sunken;
          QStyleOption arrowOpt(0);
          arrowOpt.rect = ar;
          arrowOpt.palette = cmb->palette;
          arrowOpt.state = flags;
          drawPrimitive(PE_IndicatorArrowDown, &arrowOpt, p, w);
        }
        if (cmb->subControls & SC_ComboBoxEditField) {
          bool hi = cmb->state & State_HasFocus && !cmb->editable;
          if( hi ) {
            Style_Tools::shadowRect( p, optr, aRad, 0.0, SHADOW, Style_Tools::Left,
                                     pal.color( QPalette::Highlight ),
                                     pal.color( QPalette::Highlight ), aBrdTopCol, aBrdBotCol,
                                     antialized, false );
            p->setPen(cmb->palette.highlightedText().color());
          }
          else
            p->setPen(cmb->palette.text().color());
        }
        break;
      }
    }
    case CC_Slider: {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            QRect groove = subControlRect(CC_Slider, slider, SC_SliderGroove, w);
            QRect handle = subControlRect(CC_Slider, slider, SC_SliderHandle, w);

            if( hasHover() && (slider->state & State_Enabled) &&
                         (slider->state & State_MouseOver) ) {
              QRect moderated = w->rect();
                drawHoverRect(p, moderated, opt->palette.color( QPalette::Window ),
                              model()->widgetRounding( Style_Model::ButtonRadius ), Style_Tools::All, false);
            }

            if ((slider->subControls & SC_SliderGroove) && groove.isValid()) {
              QStyleOptionSlider tmpSlider = *slider;
              tmpSlider.subControls = SC_SliderGroove;
              BaseStyle::drawComplexControl(cc, &tmpSlider, p, w);
            }
            if (slider->subControls & SC_SliderTickmarks) {
              QStyleOptionSlider tmpSlider = *slider;
              tmpSlider.subControls = SC_SliderTickmarks;
              QCommonStyle::drawComplexControl(cc, &tmpSlider, p, w);
            }
            if (slider->subControls & SC_SliderHandle) {
              if (slider->state & State_HasFocus) {
                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*slider);
                fropt.rect = subElementRect(SE_SliderFocusRect, slider, w);
                drawPrimitive(PE_FrameFocusRect, &fropt, p, w);
              }
              int x = handle.x(), y = handle.y(), wi = handle.width(), he = handle.height();
              bool horiz = slider->orientation == Qt::Horizontal;
              bool tickAbove = slider->tickPosition == QSlider::TicksAbove;
              bool tickBelow = slider->tickPosition == QSlider::TicksBelow;
              QColor aBtnCol = model()->color( Style_Model::Slider );          // Style_Model::slider_clr
              QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
              QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
              // rect was changed on +/-DELTA_SLIDER value for correct painting Antialised border of slider
              int aDelta = DELTA_SLIDER-1;
              QRect slRect = QRect(x+aDelta, y+aDelta, wi-aDelta, he-aDelta);
              int aXAdd = (int)(slRect.width()/6), aYAdd = (int)(slRect.height()/6);
              Style_Tools::SliderType type = Style_Tools::SlNone;
              if ((tickAbove && tickBelow) || (!tickAbove && !tickBelow))
                type = Style_Tools::SlNone;
              else {
                if (horiz)
                  if (tickAbove) {
                    type = Style_Tools::SlUp;
                    slRect.setTop( slRect.top()-aYAdd );
                  }
                  else {
                    type = Style_Tools::SlDown;
                    slRect.setBottom( slRect.bottom()+aYAdd );
                  }
                else
                  if (tickAbove) {
                    type = Style_Tools::SlLeft;
                    slRect.setLeft( slRect.left()-aXAdd );
                  }
                  else {
                    type = Style_Tools::SlRight;
                    slRect.setRight( slRect.right()+aXAdd );
                 }
              }
              Style_Tools::drawSlider( p, slRect, model()->widgetRounding( Style_Model::SliderRadius ), type,
                                       aBtnCol.light( BUT_PERCENT_COL ),
                                       aBtnCol.dark( BUT_PERCENT_COL ), aBrdTopCol, aBrdBotCol );
              QRect aHRect = handle;
              int aXRect = (int)(aHRect.width()/5),
                  aYRect = (int)(aHRect.height()/5);
              aHRect = QRect( aHRect.x()+aXRect, aHRect.y()+aYRect,
                              aHRect.width()-2*aXRect, aHRect.height()-2*aYRect );
              drawHandle( p, aHRect, horiz );
            }
            break;
        }
    }
    case CC_ToolButton: {
      if (const QStyleOptionToolButton *toolbutton
           = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
        QRect button, menuArea;
        button = subControlRect(cc, toolbutton, SC_ToolButton, w);
        menuArea = subControlRect(cc, toolbutton, SC_ToolButtonMenu, w);

        if (w && ( qobject_cast<QToolBar *>(w->parentWidget() ) || 
                   ( toolbutton->state & State_AutoRaise && !( toolbutton->state & State_MouseOver ) ) )
            ) {
          BaseStyle::drawComplexControl( cc, opt, p, w );
          return;
        }
        int aMinDelta = (int)model()->widgetRounding( Style_Model::ButtonRadius );
        if ( !toolbutton->icon.isNull() )
          aMinDelta = Style_Tools::getMinDelta( toolbutton->rect, toolbutton->iconSize, aMinDelta );
        bool aStateOn = opt->state & ( State_Sunken | State_On );
        QColor aBtnCol = opt->palette.color( QPalette::Button ),
               top    = aBtnCol.light( BUT_PERCENT_COL ),
               bottom = aBtnCol.dark( BUT_PERCENT_COL );
        bool isMenuBtn = toolbutton->features == QStyleOptionToolButton::Menu;
        bool antialized = model()->antialiasing();
        bool isAutoRaising = model()->widgetEffect() == Style_Model::AutoRaiseEffect;
        bool isHighWdg     = model()->widgetEffect() == Style_Model::HighlightEffect;
        bool hover = hasHover() && (opt->state & State_Enabled) && ( opt->state & State_MouseOver );
        QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
        QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
        QRect aRect = button;
        int aType = isMenuBtn ? Style_Tools::Left : Style_Tools::All;
        if ( toolbutton->subControls & SC_ToolButton ) {
          if ( isAutoRaising && hover && !aStateOn )
            Style_Tools::shadowRect( p, aRect, aMinDelta, -1, SHADOW, aType, bottom, top, aBrdTopCol,
                                     aBrdBotCol, antialized, true, aStateOn );
          else if ( isHighWdg && hover && !aStateOn )
            drawHoverRect( p, aRect, opt->palette.color( QPalette::Window ), aMinDelta, aType, true );
          else
            Style_Tools::shadowRect( p, aRect, aMinDelta, -1, SHADOW, aType, top, bottom,
                                     aBrdTopCol, aBrdBotCol, antialized, true, aStateOn );
        }
        if (toolbutton->subControls & SC_ToolButtonMenu) {
          p->save();
          if ( aMinDelta > menuArea.height()/2 )
            aMinDelta = menuArea.height()/2;
          if ( aMinDelta > menuArea.width()/2 )
            aMinDelta = menuArea.width()/2;
          aRect = menuArea;
          aType = isMenuBtn ? Style_Tools::Right : Style_Tools::All;
          if ( isAutoRaising && hover && !aStateOn )
            Style_Tools::shadowRect( p, aRect, aMinDelta, -1, SHADOW, aType, bottom, top,
                                     aBrdTopCol, aBrdBotCol, antialized, true, aStateOn );
          else if ( isHighWdg && hover && !aStateOn )
            drawHoverRect( p, aRect, opt->palette.color( QPalette::Window ), aMinDelta, aType, true );
          else
            Style_Tools::shadowRect( p, aRect, aMinDelta, -1, SHADOW, aType, top, bottom,
                                     aBrdTopCol, aBrdBotCol, antialized, true, aStateOn );
          QStyleOption tool(0);
          tool.palette = toolbutton->palette;
          tool.rect = menuArea;
          State bflags = toolbutton->state;
          if (bflags & State_AutoRaise) {
              if (!(bflags & State_MouseOver)) {
                  bflags &= ~State_Raised;
              }
          }
          State mflags = bflags;
            if (toolbutton->activeSubControls & SC_ToolButton)
                bflags |= State_Sunken;
            if (toolbutton->activeSubControls & SC_ToolButtonMenu)
                mflags |= State_Sunken;
          tool.state = mflags;
          drawPrimitive(PE_IndicatorArrowDown, &tool, p, w);
          p->restore();
        }

        if (toolbutton->state & State_HasFocus) {
          QStyleOptionFocusRect fr;
          fr.QStyleOption::operator=(*toolbutton);
          fr.rect.adjust(3, 3, -3, -3);
          if (toolbutton->features & QStyleOptionToolButton::Menu)
            fr.rect.adjust(0, 0, -pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, w), 0);
          Style_Tools::drawFocus( p, fr.rect, aMinDelta-1,  isMenuBtn ? Style_Tools::Left :
                                  Style_Tools::All, aBrdBotCol );
        }
        QStyleOptionToolButton label = *toolbutton;
        int fw = pixelMetric( PM_DefaultFrameWidth, opt, w );
        label.rect = button.adjusted( fw, fw, -fw, -fw );
        drawControl( CE_ToolButtonLabel, &label, p, w );

        break;
      }
    }
    case CC_TitleBar: {
      BaseStyle::drawComplexControl( cc, opt, p, w );
      break;
    }
    case CC_GroupBox:
      if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const 
                                                 QStyleOptionGroupBox *>(opt)) {
        // Draw frame
        QRect textRect = subControlRect( cc, opt, SC_GroupBoxLabel, w );
        QRect checkBoxRect = subControlRect( cc, opt, SC_GroupBoxCheckBox, w);
        if (groupBox->subControls & QStyle::SC_GroupBoxFrame) {
          QRect aRect = subControlRect( cc, opt, SC_GroupBoxFrame, w);
          QColor aBrdTopCol = model()->color( Style_Model::TabBorderTop );    // Style_Model::border_tab_top_clr
          QColor aBrdBotCol = model()->color( Style_Model::TabBorderBottom ); // Style_Model::border_tab_bot_clr
          Style_Tools::shadowRect( p, aRect, model()->widgetRounding( Style_Model::FrameRadius ), 0.,
                                   SHADOW, Style_Tools::All,
                                   model()->color( Style_Model::FieldLight ), // Style_Model::fld_light_clr
                                   model()->color( Style_Model::FieldDark ),  // Style_Model::fld_dark_clr
                                   aBrdTopCol, aBrdBotCol, false, false, false, false);
        }
        // Draw title
        if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
          QColor textColor = groupBox->textColor;
          if (textColor.isValid())
            p->setPen(textColor);
          int alignment = int(groupBox->textAlignment);
          if (!styleHint(QStyle::SH_UnderlineShortcut, opt, w))
            alignment |= Qt::TextHideMnemonic;
          QColor aColor = opt->palette.color( QPalette::Window );
          Style_Tools::arrowRect( p, textRect, opt->palette.color( QPalette::Dark ),
                                 aColor.dark(BUT_PERCENT_COL), aColor );
          drawItemText(p, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
                       pal, groupBox->state & State_Enabled, groupBox->text,
                       textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);
          if (groupBox->state & State_HasFocus) {
            QStyleOptionFocusRect fropt;
            fropt.QStyleOption::operator=(*groupBox);
            fropt.rect = textRect;
            drawPrimitive(PE_FrameFocusRect, &fropt, p, w);
          }
        }
        // Draw checkbox
        if (groupBox->subControls & SC_GroupBoxCheckBox) {
            QStyleOptionButton box;
          box.QStyleOption::operator=(*groupBox);
          box.rect = checkBoxRect;
          drawPrimitive(PE_IndicatorCheckBox, &box, p, w);
        }
        break;
      }
    case CC_Dial: {
      BaseStyle::drawComplexControl( cc, opt, p, w );
      break;
    }
    default:
      BaseStyle::drawComplexControl( cc, opt, p, w );
  }
}

/*!
  \brief Draws the given element with the provided painter \p with the style options specified by \a opt.
  \param ce control element type
  \param opt style option
  \param p painter
  \param w widget
*/
void Style_Salome::drawControl( ControlElement ce, const QStyleOption* opt,
                               QPainter* p, const QWidget* w ) const
{
  if ( checkDebugLevel(5) ) {    
    BaseStyle::drawControl( ce, opt, p, w );
    return;
  }

  switch ( ce ) {
    case CE_PushButton:
      if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
        drawControl(CE_PushButtonBevel, btn, p, w);
        QStyleOptionButton subopt = *btn;
        subopt.rect = subElementRect(SE_PushButtonContents, btn, w);
        drawControl(CE_PushButtonLabel, &subopt, p, w);
        if (btn->state & State_HasFocus) {
          QRect aRect = subElementRect( SE_PushButtonFocusRect, btn, w );
          Style_Tools::drawFocus( p, aRect, model()->widgetRounding( Style_Model::ButtonRadius ),
                                  Style_Tools::All, model()->color( Style_Model::BorderBottom ) );
        }
        break;
      }
    case CE_PushButtonBevel:
      if ( const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt) ) {
        double aRad = model()->widgetRounding( Style_Model::ButtonRadius );
        bool aStateOn = opt->state & ( State_Sunken | State_On );
        bool isAutoRaising = model()->widgetEffect() == Style_Model::AutoRaiseEffect;
        bool isHighWdg     = model()->widgetEffect() == Style_Model::HighlightEffect;
        bool enabled = opt->state & State_Enabled;
        bool hover = hasHover() && enabled && ( opt->state & State_MouseOver );

        QColor aBtnCol = opt->palette.color( QPalette::Button );
        QColor top = aBtnCol.light( BUT_PERCENT_COL ),
               bottom = aBtnCol.dark( BUT_PERCENT_COL );
        QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
        QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
        QRect r = opt->rect;
        bool antialized = model()->antialiasing();
        if ( isAutoRaising && hover && !aStateOn )
          Style_Tools::shadowRect( p, r, aRad, -1, SHADOW, Style_Tools::All, bottom, top,
                                   aBrdTopCol, aBrdBotCol, antialized, true, aStateOn );
        else if ( isHighWdg && hover && !aStateOn )
          drawHoverRect( p, r, opt->palette.color( QPalette::Window ), aRad, Style_Tools::All, true );
        else
          Style_Tools::shadowRect( p, r, aRad, -1, SHADOW, Style_Tools::All, top, bottom,
                                   aBrdTopCol, aBrdBotCol, antialized, true, aStateOn );
	if (btn->features & QStyleOptionButton::HasMenu) {
	  int mbi = pixelMetric(PM_MenuButtonIndicator, btn, w);
	  QStyleOptionButton newBtn = *btn;
	  newBtn.rect = QRect(r.right() - mbi-2, r.height()/2 - mbi/2, mbi, mbi);
	  drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, w);
	}
        break;
      }
      case CE_DockWidgetTitle:
        if (const QStyleOptionDockWidget *dwOpt = qstyleoption_cast<const QStyleOptionDockWidget *>(opt)) {
          bool floating = false;
          int menuOffset = 0; //used to center text when floated
          QColor inactiveCaptionTextColor = opt->palette.color( QPalette::HighlightedText );
          const QDockWidget *dockWidget = qobject_cast<const QDockWidget *>(w);
          //Titlebar gradient
          if (dockWidget) {
            if ( dockWidget->isFloating() && dwOpt->movable) {
              floating = true;
              QColor top = opt->palette.color( QPalette::Highlight ).light();
              QColor bottom = opt->palette.color( QPalette::Highlight );
              menuOffset = 2;
              QBrush fillBrush(bottom);
              if (top != bottom) {
                QPoint p1(dwOpt->rect.x(), dwOpt->rect.y());
                QPoint p2(dwOpt->rect.x(), dwOpt->rect.bottom() );
                QLinearGradient lg(p1, p2);
                lg.setColorAt(0, top);
                lg.setColorAt(1, bottom);
                fillBrush = lg;
              }
              p->fillRect(dwOpt->rect.adjusted(0, 0, 0, -3), fillBrush);
            }
            else {
              QRect r = dwOpt->rect.adjusted(0, 0, -1, -1); 
              QColor bottom = opt->palette.color( QPalette::Window ),
                     top =    bottom.dark( BUT_PERCENT_COL );
              QRect aRect = dwOpt->rect;
              QLinearGradient gr( aRect.x(), aRect.y(), aRect.x(), aRect.y()+aRect.height() );
              gr.setColorAt( 0.0, top );
              gr.setColorAt( 0.4, bottom );
              gr.setColorAt( 0.6, bottom );
              gr.setColorAt( 1.0, top );
              p->fillRect( r, gr );

              QColor aBrdTopCol = model()->color( Style_Model::TabBorderTop );     // Style_Model::border_tab_top_clr
              QColor aBrdBotCol = model()->color( Style_Model::TabBorderBottom );  // Style_Model::border_tab_bot_clr
              p->setPen( aBrdTopCol );
              p->drawLine( r.x(), r.bottom(), r.x(), r.y() );
              p->drawLine( r.x(), r.y(), r.right(), r.y() );
              p->setPen( aBrdBotCol );
              p->drawLine( r.x(), r.bottom(), r.right(), r.bottom() );
              p->drawLine( r.right(), r.bottom(), r.right(), r.y() );
            }
          }
          p->setPen(dwOpt->palette.color(QPalette::Light));

          QString aTitle = dwOpt->title;
          if (!aTitle.isEmpty()) {
            int aMargin  = pixelMetric(QStyle::PM_DockWidgetTitleMargin, dwOpt, w);
            const int indent = p->fontMetrics().descent();
            QRect r = dwOpt->rect.adjusted(indent + 1, - menuOffset, -indent - 1, -1);
            QPixmap aPxm = standardPixmap( SP_DockWidgetCloseButton, opt, w);
            int aWidth = r.width() - aPxm.width()-2/*button margins*/;
            aPxm = standardPixmap( SP_TitleBarNormalButton, opt, w);
            aWidth = aWidth - aPxm.width()-2/*button margins*/;
            r = QRect( r.x(), r.y(), aWidth-aMargin-2/*buttons separator*/-2/*margin from text*/, r.height() );

            QFont oldFont = p->font();
            QFont font = oldFont;
            if (floating) {
              font.setBold(true);
              p->setFont(font);
            }
            aTitle = titleText( aTitle, r.width(), r.height(), font );

            QPalette palette = dwOpt->palette;
            palette.setColor(QPalette::Window, inactiveCaptionTextColor);
            bool active = dwOpt->state & State_Active;
            //const int indent = p->fontMetrics().descent();
            drawItemText(p, r, Qt::AlignLeft | Qt::AlignVCenter, palette,
                         dwOpt->state & State_Enabled, aTitle,
                         floating ? (active ? QPalette::BrightText : QPalette::Window) : QPalette::WindowText);
            p->setFont(oldFont);
          }
          break;
        }
    case CE_Splitter: {
      BaseStyle::drawControl( ce, opt, p, w );
      QRect r = opt->rect;
      bool horiz = r.width() > r.height();
      int aLen = model()->splitHandleLength();
      if ( horiz )
        r = QRect( r.x() +(int)((r.width()-aLen)/2), r.y(), aLen, r.height());
      else
        r = QRect( r.x(), r.y() +(int)((r.height()-aLen)/2), r.width(), aLen);
      drawHandle( p, r, horiz, true );
      break;
    }
    case CE_TabBarTabShape:
      if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
        if (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedEast ||
            tab->shape == QTabBar::RoundedSouth || tab->shape == QTabBar::RoundedWest) {
          p->save();
          QRect tabRect = opt->rect;
          // line under selected tab bar object
          bool isSelected = opt->state & State_Selected;
          bool isLast = tab->position == QStyleOptionTab::End ||
                        tab->position == QStyleOptionTab::OnlyOneTab;
          QColor aColor = opt->palette.color( QPalette::Window ),
                 aDarkColor = aColor.dark( BUT_PERCENT_ON );
          QColor aBrdTopCol = model()->color( Style_Model::TabBorderTop );     // Style_Model::border_tab_top_clr
          QColor aBrdBotCol = model()->color( Style_Model::TabBorderBottom );  // Style_Model::border_tab_bot_clr

          bool isHover = hasHover() && (opt->state & State_Enabled) &&
                                       (opt->state & State_MouseOver);
          if ( isHover && !isSelected && model()->widgetEffect() == Style_Model::HighlightEffect ) {
            aColor     = model()->color( Style_Model::HighlightWidget ); // Style_Model::high_wdg_clr
            aDarkColor = model()->color( Style_Model::HighlightBorder ); // Style_Model::high_brd_wdg_clr
          }
          Style_Tools::tabRect( p, tabRect, (int)tab->shape,
                                model()->widgetRounding( Style_Model::FrameRadius ),
                                DELTA_H_TAB, aColor, aDarkColor,
                                aBrdTopCol, aBrdBotCol, isSelected, isLast, isHover );
          p->restore();
        } else
           QCommonStyle::drawControl( ce, opt, p, w );
        break;
      }
    case CE_TabBarTabLabel:
      if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
        if ( tab->position == QStyleOptionTab::End ||
             tab->position == QStyleOptionTab::OnlyOneTab ) {
          QRect oldRect = opt->rect;
          int aDelta = 0;
          if ( tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedSouth ) {
            aDelta = (int)(opt->rect.height()*DELTA_H_TAB/2);
            oldRect = QRect( oldRect.topLeft(), QPoint( oldRect.right()-aDelta, oldRect.bottom() ) );
          }
          else {
            aDelta = (int)(opt->rect.width()*DELTA_H_TAB/2);
            oldRect = QRect( oldRect.topLeft(), QPoint( oldRect.right(), oldRect.bottom()-aDelta ) );
          }
          QStyleOptionTab* copyTab = (QStyleOptionTab*)tab;
          copyTab->rect = oldRect;
          BaseStyle::drawControl( ce, copyTab, p, w );
        }
        else
          BaseStyle::drawControl( ce, opt, p, w );
        break;
      }
    case CE_MenuBarItem:
      if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
        if ( w )
          drawBackground( p, w->rect(), opt->palette.color( QPalette::Window ), true, true, true );
        bool active = mbi->state & State_Selected;
        bool hasFocus = mbi->state & State_HasFocus;
        bool down = mbi->state & State_Sunken;
        QStyleOptionMenuItem newMbi = *mbi;
        if (active || hasFocus) {
          QBrush b( opt->palette.color( QPalette::Window ) );
          if (active && down)
            p->setBrushOrigin(p->brushOrigin() + QPoint(1, 1));
          if ( active && hasFocus) {
            bool aStateOn = opt->state & (State_Sunken | State_On);
            QColor aBtnCol = opt->palette.color( QPalette::Window ),
                   top =    aBtnCol.light( BUT_PERCENT_ON ),
                   bottom = aBtnCol.dark( BUT_PERCENT_ON );
            QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
            QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
            bool aHighWdg = model()->widgetEffect() == Style_Model::HighlightEffect;
            if ( !aStateOn && aHighWdg && (opt->state & State_Enabled) &&
                 (opt->state & State_Selected) )
              drawHoverRect(p, opt->rect, opt->palette.color( QPalette::Window ), 0, Style_Tools::All, true);
            else {
              Style_Tools::shadowRect( p, opt->rect, 0, 0., SHADOW, Style_Tools::All, top, bottom,
                                   aBrdTopCol, aBrdBotCol,
                                   model()->antialiasing(), true, aStateOn );
            }
          }
          if (active && down) {
            newMbi.rect.translate(pixelMetric(PM_ButtonShiftHorizontal, mbi, w),
                                  pixelMetric(PM_ButtonShiftVertical, mbi, w));
            p->setBrushOrigin(p->brushOrigin() - QPoint(1, 1));
          }
        }
        QCommonStyle::drawControl(ce, &newMbi, p, w);
        break;
      }
    case CE_MenuBarEmptyArea:
      drawBackground( p, opt->rect, opt->palette.color( QPalette::Window ), true, true, true );
      break;
    case CE_ProgressBarGroove: {
      QColor aBgColor = opt->palette.color( QPalette::Window ),
        top =    aBgColor.light( BUT_PERCENT_ON ),
        bottom = aBgColor.dark( BUT_PERCENT_ON );
      QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
      QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
      Style_Tools::shadowRect( p, opt->rect, model()->widgetRounding( Style_Model::EditRadius ), -1,
                               SHADOW, Style_Tools::All, top, bottom, aBrdTopCol, aBrdBotCol,
                               model()->antialiasing(), true );
      break;
      }
    case CE_ProgressBarLabel:
      if (const QStyleOptionProgressBar *bar = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
        // implementation from qplastiquestyle
        // The busy indicator doesn't draw a label
        if (bar->minimum == 0 && bar->maximum == 0)
          return;
        p->save();
        QRect rect = bar->rect;
        QRect leftRect;
        p->setPen(bar->palette.text().color());

        bool vertical = false;
        bool inverted = false;
        bool bottomToTop = false;
        // Get extra style options if version 2
        if (const QStyleOptionProgressBarV2 *bar2 = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(opt)) {
          vertical = (bar2->orientation == Qt::Vertical);
          inverted = bar2->invertedAppearance;
          bottomToTop = bar2->bottomToTop;
        }
        if (vertical) {
          rect = QRect(rect.left(), rect.top(), rect.height(), rect.width()); // flip width and height
          QMatrix m;
          if (bottomToTop) {
            m.translate(0.0, rect.width());
            m.rotate(-90);
          } else {
            m.translate(rect.height(), 0.0);
            m.rotate(90);
          }
          p->setMatrix(m);
        }
        int progressIndicatorPos = int(((bar->progress - bar->minimum) / double(bar->maximum - bar->minimum)) * rect.width());
        bool flip = (!vertical && (((bar->direction == Qt::RightToLeft) && !inverted)
                    || ((bar->direction == Qt::LeftToRight) && inverted))) || (vertical && ((!inverted && !bottomToTop) || (inverted && bottomToTop)));
        if (flip) {
          int indicatorPos = rect.width() - progressIndicatorPos;
          if (indicatorPos >= 0 && indicatorPos <= rect.width()) {
            p->setPen(bar->palette.base().color());
            leftRect = QRect(rect.left(), rect.top(), indicatorPos, rect.height());
          } else if (indicatorPos > rect.width()) {
            p->setPen(bar->palette.text().color());
          } else {
            p->setPen(bar->palette.base().color());
          }
        } else {
          if (progressIndicatorPos >= 0 && progressIndicatorPos <= rect.width()) {
            leftRect = QRect(rect.left(), rect.top(), progressIndicatorPos, rect.height());
          } else if (progressIndicatorPos > rect.width()) {
            p->setPen(bar->palette.base().color());
          } else {
            p->setPen(bar->palette.text().color());
          }
        }

        p->drawText(rect, bar->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
        if (!leftRect.isNull()) {
          p->setPen(flip ? bar->palette.text().color() : bar->palette.base().color());
          p->setClipRect(leftRect, Qt::IntersectClip);
          p->drawText(rect, bar->text, QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
        }
        p->restore();
      }
      break;
    case CE_ProgressBarContents:
      if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
        QRect rect = pb->rect;
        bool vertical = false;
        bool inverted = false;
        // Get extra style options if version 2
        const QStyleOptionProgressBarV2 *pb2 = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(opt);
        if (pb2) {
          vertical = (pb2->orientation == Qt::Vertical);
          inverted = pb2->invertedAppearance;
        }
        QMatrix m;
        if (vertical) {
          rect = QRect(rect.left(), rect.top(), rect.height(), rect.width()); // flip width and height
          m.translate(rect.height(), 0.0);
          m.rotate(90);
        }
        QPalette pal2 = pb->palette;
        // Correct the highlight color if it is the same as the background
        if (pal2.highlight() == pal2.background())
            pal2.setColor(QPalette::Highlight, pb->palette.color(QPalette::Active,
                                                                 QPalette::Highlight));
        bool reverse = ((!vertical && (pb->direction == Qt::RightToLeft)) || vertical);
        if (inverted)
          reverse = !reverse;
        int fw = 2;
        int width = rect.width() - 2 * fw;
        if (pb->minimum == 0 && pb->maximum == 0) {
          // draw busy indicator
          int x = (pb->progress - pb->minimum) % (width * 2);
          if (x > width)
            x = 2 * width - x;
          x = reverse ? rect.right() - x : x + rect.x();
          p->setPen(QPen(pal2.highlight().color(), 4));
          p->drawLine(x, rect.y() + 1, x, rect.height() - fw);
        } else {
          const int unit_width = pixelMetric(PM_ProgressBarChunkWidth, pb, w);
          int u;
          if (unit_width > 1)
            u = (rect.width() + unit_width / 3) / unit_width;
          else
            u = width / unit_width;

          int p_v = pb->progress - pb->minimum;
          int t_s = pb->maximum - pb->minimum ? pb->maximum - pb->minimum : 1;

          if (u > 0 && p_v >= INT_MAX / u && t_s >= u) {
            // scale down to something usable.
            p_v /= u;
            t_s /= u;
          }

          // nu < tnu, if last chunk is only a partial chunk
          int tnu, nu;
          tnu = nu = p_v * u / t_s;

          if (nu * unit_width > width)
            --nu;

          // Draw nu units out of a possible u of unit_width
          // width, each a rectangle bordered by background
          // color, all in a sunken panel with a percentage text
          // display at the end.
          int x = 0;
          int x0 = reverse ? rect.right() - ((unit_width > 1) ? unit_width : fw)
                           : rect.x() + fw;
          QStyleOptionProgressBarV2 pbBits = *pb;
          pbBits.rect = rect;
          pbBits.palette = pal2;
          int myY = pbBits.rect.y();
          int myHeight = pbBits.rect.height();
          pbBits.state = State_None;
          QRect aRect;
          QColor aColor = model()->color( Style_Model::ProgressBar );    // Style_Model::prbar_clr
          QColor top    = aColor.light( BUT_PERCENT_COL );
          QColor bottom = aColor.dark( BUT_PERCENT_COL );
          int aType;
          for (int i = 0; i <= nu; ++i) {
            aType = Style_Tools::None;
            if ( i < nu ) { // not last element
              aRect = QRect(x0 + x, myY, unit_width, myHeight);
              if ( i == 0 ) {
                if ( reverse )
                  aRect.setRight( aRect.right()-fw );
                if ( vertical )
                  aType = reverse ? Style_Tools::BottomLeft | Style_Tools::BottomRight
                                  : Style_Tools::TopLeft | Style_Tools::TopRight;
                else
                  aType = reverse ? Style_Tools::Right : Style_Tools::Left;
              }
            }
            else { // last element if it's necessary
              if ( nu >= tnu )
                break;
              int pixels_left = width - (nu * unit_width);
              int offset = reverse ? x0 + x + unit_width-pixels_left : x0 + x;
              aRect = QRect(offset, myY, pixels_left, myHeight);
              if ( vertical )
                aType = reverse ? Style_Tools::TopLeft | Style_Tools::TopRight
                                : Style_Tools::BottomLeft | Style_Tools::BottomRight;
              else
                aType = reverse ? Style_Tools::Left : Style_Tools::Right;
            }
            // display
            aRect = m.mapRect(aRect);
            if ( vertical )
              aRect = QRect(aRect.x(), aRect.y()+fw, aRect.width(), aRect.height());

            if ( !vertical )
              aRect = QRect(aRect.x()+1, aRect.y()+2, aRect.width()-1,
                            aRect.height()-4);
            else
              aRect = QRect(aRect.x()+1, aRect.y()-1, aRect.width()-5, aRect.height()-1);
            QColor aTopClr = aColor, aBotClr = aColor;
            if ( unit_width > 1 ) {
              aTopClr = aColor.light();
              aBotClr = aColor.dark();
            }
            Style_Tools::shadowRect( p, aRect, model()->widgetRounding( Style_Model::EditRadius ), -1, 0,
                                     aType, top, bottom, aTopClr, aBotClr, false, true );
            x += reverse ? -unit_width : unit_width;
          }
        }
      }
      break;
    case CE_MenuItem:
      if (const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
        const int windowsItemFrame    =  2; // definitions from qwindowstyle.cpp file
        const int windowsItemHMargin  =  3;
        const int windowsItemVMargin  =  2;
        const int windowsRightBorder  = 15;
        const int windowsArrowHMargin =  6;
        int x, y, width, h;
        menuitem->rect.getRect(&x, &y, &width, &h);
        int tab = menuitem->tabWidth;
        bool dis = !(menuitem->state & State_Enabled);
        bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable
                       ? menuitem->checked : false;
        bool act = menuitem->state & State_Selected;

        // windows always has a check column, regardless whether we have an icon or not
        int checkcol = qMax(menuitem->maxIconWidth, 20);

        QColor aBgColor = opt->palette.color( QPalette::Window );
        double aMargin = LINE_GR_MARGIN;
        QLinearGradient gr(x,y,menuitem->rect.right(),y);
        gr.setColorAt( 0.0, aBgColor );
        gr.setColorAt( aMargin/width, model()->color( Style_Model::FieldLight ) ); // Style_Model::fld_light_clr
        gr.setColorAt( 1.0, model()->color( Style_Model::FieldLight ) );
        QBrush fill;
        if ( act )
          fill = menuitem->palette.brush( QPalette::Highlight );
        else
          fill = QBrush( gr );
        p->fillRect(menuitem->rect, fill);
        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator){
          int yoff = y-1 + h / 2;
          QColor aBrdTopCol = model()->color( Style_Model::TabBorderTop );    // Style_Model::border_tab_top_clr
          QColor aBrdBotCol = model()->color( Style_Model::TabBorderBottom ); // Style_Model::border_tab_bot_clr
          p->setPen( aBrdBotCol );
          p->drawLine(x + 2, yoff, x + width - 4, yoff);
          p->setPen( aBrdTopCol );
          p->drawLine(x + 2, yoff + 1, x + width - 4, yoff + 1);
          return;
        }
        QRect vCheckRect = visualRect(opt->direction, menuitem->rect, QRect(menuitem->rect.x(),
                                      menuitem->rect.y(), checkcol, menuitem->rect.height()));
        if (checked) {
          if (act && !dis)
            qDrawShadePanel(p, vCheckRect, menuitem->palette, true, 1, new QBrush(aBgColor));
          else {
            QColor aCol = aBgColor;
            aCol.setAlpha( 80 );
            QBrush fill(aCol);
            qDrawShadePanel(p, vCheckRect, menuitem->palette, true, 1, &fill);
          }
        } else if (!act)
          p->fillRect(vCheckRect, QBrush( gr ) );
        // On Windows Style, if we have a checkable item and an icon we
        // draw the icon recessed to indicate an item is checked. If we
        // have no icon, we draw a checkmark instead.
        if (!menuitem->icon.isNull()) {
          QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
          if (act && !dis)
            mode = QIcon::Active;
          QPixmap pixmap;
          if (checked)
            pixmap = menuitem->icon.pixmap(pixelMetric(PM_SmallIconSize), mode, QIcon::On);
          else
            pixmap = menuitem->icon.pixmap(pixelMetric(PM_SmallIconSize), mode);
          int pixw = pixmap.width();
          int pixh = pixmap.height();
          if (act && !dis && !checked)
            qDrawShadePanel(p, vCheckRect,  menuitem->palette, false, 1,
                            new QBrush(aBgColor));
          QRect pmr(0, 0, pixw, pixh);
          pmr.moveCenter(vCheckRect.center());
          p->setPen(menuitem->palette.text().color());
          p->drawPixmap(pmr.topLeft(), pixmap);
        } else if (checked) {
          QStyleOptionMenuItem newMi = *menuitem;
          newMi.state = State_None;
          if (!dis)
            newMi.state |= State_Enabled;
          if (act)
            newMi.state |= State_On;
          newMi.rect = visualRect(opt->direction, menuitem->rect,
          QRect(menuitem->rect.x() + windowsItemFrame, menuitem->rect.y() + windowsItemFrame,
                checkcol - 2 * windowsItemFrame, menuitem->rect.height() - 2*windowsItemFrame));
          drawPrimitive(PE_IndicatorMenuCheckMark, &newMi, p, w);
        }
        p->setPen(act ? menuitem->palette.highlightedText().color() :
                        menuitem->palette.buttonText().color());
        QColor discol;
        if (dis) {
          discol = menuitem->palette.text().color();
          p->setPen(discol);
        }
        int xm = windowsItemFrame + checkcol + windowsItemHMargin;
        int xpos = menuitem->rect.x() + xm;
        QRect textRect(xpos, y + windowsItemVMargin, width - xm - windowsRightBorder - tab + 1,
                       h - 2 * windowsItemVMargin);
        QRect vTextRect = visualRect(opt->direction, menuitem->rect, textRect);
        QString s = menuitem->text;
        if (!s.isEmpty()) {                     // draw text
          p->save();
          int t = s.indexOf(QLatin1Char('\t'));
          int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip |
                           Qt::TextSingleLine;
          if (!styleHint(SH_UnderlineShortcut, menuitem, w))
            text_flags |= Qt::TextHideMnemonic;
          text_flags |= Qt::AlignLeft;
          if (t >= 0) {
            QRect vShortcutRect = visualRect(opt->direction, menuitem->rect, 
            QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));
            if (dis && !act) {
               p->setPen(menuitem->palette.light().color());
               p->drawText(vShortcutRect.adjusted(1,1,1,1), text_flags, s.mid(t + 1));
               p->setPen(discol);
             }
             p->drawText(vShortcutRect, text_flags, s.mid(t + 1));
             s = s.left(t);
          }
          QFont font = menuitem->font;
          if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
            font.setBold(true);
            p->setFont(font);
            if (dis && !act) {
              p->setPen(menuitem->palette.light().color());
              p->drawText(vTextRect.adjusted(1,1,1,1), text_flags, s.left(t));
              p->setPen(discol);
            }
            p->drawText(vTextRect, text_flags, s.left(t));
            p->restore();
          }
          if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu) {// draw sub menu arrow
            int dim = (h - 2 * windowsItemFrame) / 2;
            PrimitiveElement arrow;
            arrow = (opt->direction == Qt::RightToLeft) ? PE_IndicatorArrowLeft :
                     PE_IndicatorArrowRight;
            xpos = x + width - windowsArrowHMargin - windowsItemFrame - dim;
            QRect  vSubMenuRect = visualRect(opt->direction, menuitem->rect,
                                             QRect(xpos, y + h / 2 - dim / 2, dim, dim));
            QStyleOptionMenuItem newMI = *menuitem;
            newMI.rect = vSubMenuRect;
            newMI.state = dis ? State_None : State_Enabled;
            if (act)
              newMI.palette.setColor(QPalette::ButtonText,
                                     newMI.palette.highlightedText().color());
            drawPrimitive(arrow, &newMI, p, w);
          }
        }
      break;
      /*
    case CE_ToolBoxTab:
      QCommonStyle::drawControl( ce, opt, p, w );
      break;
      */
    case CE_HeaderSection: {
      bool aStateOn = opt->state & State_On;
      QColor aColor = model()->color( Style_Model::Header );    // Style_Model::header_clr
      QColor  top =    aColor.light( BUT_PERCENT_COL ),
             bottom =  aColor.dark( BUT_PERCENT_COL );
      QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
      QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
      Style_Tools::shadowRect( p, opt->rect, 0, -1, 0, Style_Tools::All, top, bottom, aBrdTopCol,
                       aBrdBotCol, model()->antialiasing(), true, aStateOn );
      break;
    }
    case CE_ComboBoxLabel:
      if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
        QRect editRect = subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, w);
        p->save();
        p->setClipRect(editRect);
        if (!cb->currentIcon.isNull()) {
          QIcon::Mode mode = cb->state & State_Enabled ? QIcon::Normal
                                                       : QIcon::Disabled;
          QPixmap pixmap = cb->currentIcon.pixmap(cb->iconSize, mode);
          QRect iconRect(editRect);
          iconRect.setWidth(cb->iconSize.width() + 4);
          QRect alignRect = editRect;
          if ( cb->editable ) {
            int aHalfRect = (int)Style_Tools::getMaxRect( iconRect, 
                               (int)model()->widgetRounding( Style_Model::EditRadius )/2 );
            alignRect.setLeft( alignRect.left() + aHalfRect );
            alignRect.setRight( alignRect.right() - aHalfRect );
          }
          iconRect = alignedRect(QApplication::layoutDirection(),
                                 Qt::AlignLeft | Qt::AlignVCenter,
                                 iconRect.size(), alignRect);
 
          // Here's absent filling of pixmap on basic color for editable comboBox
          drawItemPixmap(p, iconRect, Qt::AlignCenter, pixmap);

          if (cb->direction == Qt::RightToLeft)
            editRect.translate(-4 - cb->iconSize.width(), 0);
          else
            editRect.translate(cb->iconSize.width() + 4, 0);
        }
        if (!cb->currentText.isEmpty() && !cb->editable) {
          drawItemText(p, editRect.adjusted(1, 0, -1, 0), Qt::AlignLeft | Qt::AlignVCenter, cb->palette,
                       cb->state & State_Enabled, cb->currentText);
        }
        p->restore();
      }
      break;
    case CE_ScrollBarSubLine:
    case CE_ScrollBarAddLine: {
        bool aStateOn = opt->state & ( State_Sunken | State_On );
        QColor aBtnCol = opt->palette.color( QPalette::Button );
        QColor top =    aBtnCol.light( BUT_PERCENT_COL ),
               bottom = aBtnCol.dark( BUT_PERCENT_COL );
        QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
        QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
        Style_Tools::shadowRect( p, opt->rect, 0, -1, 0, Style_Tools::All, top, bottom, aBrdTopCol,
                                 aBrdBotCol, false, true, aStateOn, true );
      PrimitiveElement arrow;
      if (opt->state & State_Horizontal) {
        if (ce == CE_ScrollBarAddLine)
          arrow = opt->direction == Qt::LeftToRight ? PE_IndicatorArrowRight : PE_IndicatorArrowLeft;
        else
          arrow = opt->direction == Qt::LeftToRight ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
      } else {
        if (ce == CE_ScrollBarAddLine)
          arrow = PE_IndicatorArrowDown;
        else
          arrow = PE_IndicatorArrowUp;
      }
      drawPrimitive(arrow, opt, p, w);
      break;
    }
    case CE_ScrollBarSlider:
      if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
        p->save();
        p->setRenderHint( QPainter::Antialiasing, true );
        bool enabled = opt->state & State_Enabled;
        bool horiz = scrollbar->orientation == Qt::Horizontal;
        double aRad = model()->widgetRounding( Style_Model::ButtonRadius );
        if ( hasHover() && enabled && (opt->state & State_MouseOver) )
         drawHoverRect(p, opt->rect, opt->palette.color( QPalette::Window ), aRad, Style_Tools::All, false);
        else {
        QColor aColor = model()->color( Style_Model::Slider );      // Style_Model::slider_clr
          if ( !enabled )
            aColor = opt->palette.button().color();
          QColor top =    aColor.light( BUT_PERCENT_ON ),
                 bottom = aColor.dark( BUT_PERCENT_ON );
          QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
          QRect r = opt->rect;
          QPainterPath path = Style_Tools::roundRect( r, aRad,
                                                      Style_Tools::All );
          QLinearGradient gr;
          if (horiz)
            gr = QLinearGradient(r.x(), r.y(), r.x(), r.bottom());
          else
            gr = QLinearGradient(r.x(), r.y(), r.right(), r.y());
          gr.setColorAt( 0.0, bottom );
          gr.setColorAt( 0.5, top );
          gr.setColorAt( 1.0, bottom );

          p->fillPath( path, gr );
          p->strokePath( path, aBrdTopCol );
      }
      p->setRenderHint( QPainter::Antialiasing, false );
      // draw handle
      QRect aRect = opt->rect;
      int aWidth = aRect.width(), aHeight = aRect.height();
      if ( aWidth > aHeight + aRad )
        aRect = QRect( aRect.x() + (int)((aWidth-aHeight)/2),
                       aRect.y(), aHeight, aHeight );
      else if ( aHeight > aWidth + aRad )
        aRect = QRect( aRect.x(), aRect.y() + (int)((aHeight-aWidth)/2),
                       aWidth, aWidth );
      else {
        int aRad2 = (int)(aRad/3);
        aRect = QRect( aRect.x()+aRad2, aRect.y()+aRad2, aRect.width()-2*aRad2, aRect.height()-2*aRad2  );
      }
      drawHandle( p, aRect, horiz, true );

      p->restore();
      break;
    }
  case CE_ToolBar: {
    QRect r = w->rect();
    bool horiz = opt->state & State_Horizontal;
    drawBackground( p, r, opt->palette.color( QPalette::Window ), true, true, horiz );
    p->setRenderHint( QPainter::Antialiasing, false );
    drawBorder( p, r, horiz );
    break;
  }
  default:
    BaseStyle::drawControl( ce, opt, p, w );
    break;
  }
}

/*!
  \brief Draws the given primitive element with the provided painter \p using the style options specified by \a opt.
  \param pe primitive type
  \param opt style option
  \param p painter
  \param w widget (optional)
*/
void Style_Salome::drawPrimitive( PrimitiveElement pe, const QStyleOption* opt,
                                  QPainter* p, const QWidget* w ) const
{
  if ( checkDebugLevel(6) ) {
    BaseStyle::drawPrimitive( pe, opt, p, w );
    return;
  }

  const QPalette& pal = opt->palette;
  bool doRestore = false;
  switch ( pe ) {
    case PE_FrameMenu:
      if (qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
        QColor aBtnCol = opt->palette.color( QPalette::Window ),
               top =    aBtnCol.light( BUT_PERCENT_ON ),
               bottom = aBtnCol.dark( BUT_PERCENT_ON );
        QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
        QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
        Style_Tools::shadowRect( p, opt->rect, 0, 0., SHADOW, Style_Tools::All, top,
                                 bottom, aBrdTopCol, aBrdBotCol, false, false, false, false );
        break;
      }
    case PE_PanelButtonTool : {
      if ( w && ( opt->state & State_Enabled ) && 
           ( ( qobject_cast<QToolBar*>( w->parentWidget() ) ) ||
             ( w->inherits("QDockWidgetTitleButton") ) ||
             ( w->inherits("QtxWorkstackAreaTitleButton") ) ) ) {
        bool aStateOn = opt->state & (State_Sunken | State_On);
        bool aHighWdg = model()->widgetEffect() == Style_Model::HighlightEffect;
        if ( !aStateOn && aHighWdg && (opt->state & State_Enabled) &&
             (opt->state & State_MouseOver) )
          drawHoverRect(p, opt->rect, opt->palette.color( QPalette::Window ), 0, Style_Tools::All, true);
        else {
          QColor aBtnCol = opt->palette.color( QPalette::Window );
          QColor aBrdTopCol = model()->color( Style_Model::TabBorderTop );    // Style_Model::border_tab_top_clr
          QColor aBrdBotCol = model()->color( Style_Model::TabBorderBottom ); // Style_Model::border_tab_bot_clr
          QColor top =    aBtnCol.light( BUT_PERCENT_COL ),
                 bottom = aBtnCol.dark( BUT_PERCENT_COL );
          Style_Tools::shadowRect( p, opt->rect, 0.0, 0, SHADOW, Style_Tools::All, top, bottom,
           aBrdTopCol, aBrdBotCol, model()->antialiasing(), true, aStateOn );
        }
        break;
      }
      else
        BaseStyle::drawPrimitive( pe, opt, p, w );
    }
    break;
    case PE_FrameFocusRect: {
      if (w && qobject_cast<QTabBar*>((QWidget*)w)) {
        QTabBar* tabBar = qobject_cast<QTabBar*>((QWidget*)w);
        QColor aBrdTopCol = model()->color( Style_Model::TabBorderTop );    // Style_Model::border_tab_top_clr
        QColor aBrdBotCol = model()->color( Style_Model::TabBorderBottom ); // Style_Model::border_tab_bot_clr
        bool isHover = hasHover() && (opt->state & State_Enabled) && (opt->state & State_MouseOver);
        Style_Tools::tabRect( p, opt->rect, (int)tabBar->shape(),
                              model()->widgetRounding( Style_Model::FrameRadius ), DELTA_H_TAB,
                              pal.color( QPalette::Window ),
                              model()->color( Style_Model::BorderTop ), // Style_Model::border_bot_clr
                              aBrdTopCol, aBrdBotCol, false, false, isHover, true );
      }
      else {
        BaseStyle::drawPrimitive( pe, opt, p, w );
      }
      break;
    }
    case PE_IndicatorArrowRight:
    case PE_IndicatorArrowLeft:
    case PE_IndicatorArrowUp:
    case PE_IndicatorArrowDown:
    case PE_IndicatorSpinUp:
    case PE_IndicatorSpinDown:
    case PE_IndicatorSpinPlus:
    case PE_IndicatorSpinMinus: {
      QRect rect = opt->rect;
      QColor pen, brush;
      if ( opt->state & State_Enabled ) {
        pen = model()->color( Style_Model::Pointer );    // Style_Model::pointer_clr
        brush = opt->palette.color( QPalette::Button );
        if ( ( opt->state & State_Sunken ) && (opt->state & State_Enabled ) )
          rect.moveTo( rect.x()+1, rect.y()+1 );
      } else {
        pen = opt->palette.mid().color();
        brush = pen;
      }
      if ( pe == PE_IndicatorSpinPlus || pe == PE_IndicatorSpinMinus )
        Style_Tools::drawSign( pe, p, rect, pen, brush );
      else
        Style_Tools::drawArrow( pe, p, rect, pen, brush );
      break;
    }
    case PE_IndicatorCheckBox: {
      if ( hasHover() && (opt->state & State_Enabled) && (opt->state & State_MouseOver) )
        drawHoverRect(p, w->rect(), opt->palette.color( QPalette::Window ),
                      model()->widgetRounding( Style_Model::EditRadius ),
                      Style_Tools::All, false);
      QBrush fill;
      if (opt->state & State_NoChange)
        fill = QBrush( opt->palette.color( QPalette::Base ), Qt::Dense4Pattern);
      else if (opt->state & ( State_Sunken | ~State_Enabled ) )
        fill = opt->palette.color( QPalette::Window );
      else if (opt->state & State_Enabled) {
        if (!(opt->state & State_Off) )
          fill = QBrush( model()->color( Style_Model::Checked ) ); // Style_Model::checked_clr
        else
          fill = QBrush( opt->palette.color( QPalette::Base ) );
      }
      else
        fill = opt->palette.color( QPalette::Window );
      p->save();
      doRestore = true;
      QColor color = fill.color();
      QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
      QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
      if ( model()->antialiasing() )
        p->setRenderHint( QPainter::Antialiasing, true );

     // make sure the indicator is square
      QRect ir = opt->rect;
      if (opt->rect.width() < opt->rect.height()) {
        ir.setTop(opt->rect.top() + (opt->rect.height() - opt->rect.width()) / 2);
        ir.setHeight(opt->rect.width());
      } else if (opt->rect.height() < opt->rect.width()) {
        ir.setLeft(opt->rect.left() + (opt->rect.width() - opt->rect.height()) / 2);
        ir.setWidth(opt->rect.height());
      }

      Style_Tools::shadowCheck( p, ir, 2., Style_Tools::All,
                                color, color.dark( BUT_PERCENT_ON ), aBrdTopCol, aBrdBotCol );
      if ( model()->antialiasing() )
        p->setRenderHint( QPainter::Antialiasing, false );
      if (opt->state & State_NoChange)
        p->setPen(opt->palette.dark().color());
      else
        p->setPen(opt->palette.text().color());
     } // Fall through!
    case PE_IndicatorViewItemCheck:
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    case PE_Q3CheckListIndicator:
#endif
    {
      if (!doRestore) {
        p->save();
        doRestore = true;
      }
      const QStyleOptionViewItem *itemViewOpt = qstyleoption_cast<const QStyleOptionViewItem *>(opt);
      p->setPen(itemViewOpt && itemViewOpt->showDecorationSelected
                && opt->state & State_Selected ? opt->palette.highlightedText().color()
                : opt->palette.text().color());
      if (opt->state & State_NoChange)
        p->setBrush( opt->palette.color( QPalette::Button ) );
      p->drawRect(opt->rect.x() + 1, opt->rect.y() + 1, 11, 11);
      if (!(opt->state & State_Off)) {
        QLineF lines[11];
        int i, xx, yy;
        xx = opt->rect.x() + 4;
        yy = opt->rect.y() + 5;
        for (i = 0; i < 3; ++i) {
          lines[i] = QLineF(xx, yy, xx, yy + 2);
          ++xx;
          ++yy;
        }
        yy -= 2;
        for (i = 3; i < 11; ++i) {
          lines[i] = QLineF(xx, yy, xx, yy+2);
          ++xx;
          --yy;
        }
        QColor aColor = model()->color( Style_Model::Pointer ); // Style_Model::pointer_clr
        if ( !(opt->state & State_Enabled ) )
          aColor = opt->palette.mid().color();
        if ( opt->state & State_Selected && itemViewOpt && itemViewOpt->showDecorationSelected )
          aColor = opt->palette.highlightedText().color();

        p->setPen( QPen( aColor ) );
        p->drawLines(lines, 11);
      }

      if (doRestore)
          p->restore();
      break;
    }
    case PE_IndicatorRadioButton: {
      if ( hasHover() && (opt->state & State_Enabled) && (opt->state & State_MouseOver) )
        drawHoverRect(p, w->rect(), opt->palette.color( QPalette::Window ), 
                      model()->widgetRounding( Style_Model::ButtonRadius ),
                      Style_Tools::All, false);
#define PTSARRLEN(x) sizeof(x)/(sizeof(QPoint))
      static const QPoint pts_border[] = {              // border line
        QPoint(1, 9),  QPoint(1, 8),  QPoint(0, 7),  QPoint(0, 4),  QPoint(1, 3),  QPoint(1, 2),
        QPoint(2, 1),  QPoint(3, 1),  QPoint(4, 0),  QPoint(7, 0),  QPoint(8, 1),  QPoint(9, 1),
        QPoint(10, 2), QPoint(10, 3), QPoint(11, 4), QPoint(11, 7), QPoint(10, 8), QPoint(10, 9), 
        QPoint(9, 10), QPoint(8, 10), QPoint(7, 11), QPoint(4, 11), QPoint(3, 10), QPoint(2, 10)
      };
     // make sure the indicator is square
      QRect ir = opt->rect;
      if (opt->rect.width() < opt->rect.height()) {
        ir.setTop(opt->rect.top() + (opt->rect.height() - opt->rect.width()) / 2);
        ir.setHeight(opt->rect.width());
      } else if (opt->rect.height() < opt->rect.width()) {
        ir.setLeft(opt->rect.left() + (opt->rect.width() - opt->rect.height()) / 2);
        ir.setWidth(opt->rect.height());
      }
      p->save();
      bool down = opt->state & State_Sunken;
      bool enabled = opt->state & State_Enabled;
      bool on = opt->state & State_On;
      QPolygon a;
      p->translate(ir.x(), ir.y());

      if ( down || !enabled ) {
        QColor fillColor = opt->palette.color( QPalette::Window );
        p->setPen( fillColor );
        p->setBrush( fillColor );
      }
      else {
        QColor fillColor =  opt->palette.color( QPalette::Base );
        if ( enabled && on )
          fillColor = model()->color( Style_Model::Checked ); // Style_Model::checked_clr
        QLinearGradient gr( 3, 3, 8, 8 );
        gr.setColorAt( 0.0, fillColor.dark( BUT_PERCENT_ON ) );
        gr.setColorAt( 1.0, fillColor );
        p->setPen( fillColor.dark( BUT_PERCENT_ON ) );
        p->setBrush( gr );
      }
      p->drawPolygon(pts_border, PTSARRLEN(pts_border));
      int aSize = PTSARRLEN(pts_border),
          aHalfSize = (int)aSize/2;
      if ( model()->antialiasing() )
        p->setRenderHint( QPainter::Antialiasing, true );
      p->setPen( model()->color( Style_Model::BorderTop) );     // Style_Model::border_top_clr
      p->drawPolyline(pts_border, aHalfSize);
      p->setPen( model()->color( Style_Model::BorderBottom ) ); // Style_Model::border_bot_clr
      QPolygon aPolygon;
      for ( int i = aHalfSize; i < aSize; i++ )
        aPolygon << pts_border[i];
      p->drawPolyline( aPolygon );
      if ( model()->antialiasing() )
        p->setRenderHint( QPainter::Antialiasing, false );

      if (on) {
        QColor aPointerCol = model()->color( Style_Model::Pointer ); // Style_Model::pointer_clr
        if ( !enabled )
          aPointerCol = opt->palette.mid().color();
        p->setPen( Qt::NoPen );
        p->setBrush( aPointerCol );
        p->drawRect( 5, 4, 2, 4 );
        p->drawRect( 4, 5, 4, 2 );
      }

      p->translate(-ir.x(), -ir.y()); // restore translate
      p->restore();
      break;
    }
    case PE_FrameDockWidget:
      if ( qstyleoption_cast<const QStyleOptionFrame *>(opt))
        QCommonStyle::drawPrimitive( pe, opt, p, w );
      break;
    case PE_FrameLineEdit:
    case PE_PanelLineEdit: {
      if ( w ) {
        if ( qobject_cast<const QComboBox*>( w->parentWidget() ) ||
             qobject_cast<const QAbstractSpinBox*>( w->parentWidget() ) )
          break;
      }
      if ( pe == PE_FrameLineEdit ) {
        QColor aBrdTopCol = model()->color( Style_Model::BorderTop );    // Style_Model::border_top_clr
        QColor aBrdBotCol = model()->color( Style_Model::BorderBottom ); // Style_Model::border_bot_clr
        bool hover = hasHover() && (opt->state & State_Enabled) && (opt->state & State_MouseOver);
        double aRad = model()->widgetRounding( Style_Model::EditRadius );
        if ( hover )
          drawHoverRect(p, opt->rect, opt->palette.color( QPalette::Window ), aRad, Style_Tools::All, true);
        else {
          Style_Tools::shadowRect( p, opt->rect, aRad, LINE_GR_MARGIN, SHADOW,
                                   Style_Tools::All, opt->palette.color( QPalette::Base ), // getColor( Style_Model::fld_light_clr ),
                                   model()->color( Style_Model::FieldDark ), aBrdTopCol, aBrdBotCol,
                                   model()->antialiasing(), false );
        }
      }
      else {
        if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
          QRect rect = panel->rect.adjusted( panel->lineWidth,  panel->lineWidth,
                                             -panel->lineWidth, -panel->lineWidth);
          if ( w->inherits("QLineEdit") && panel->lineWidth > 0 ) {
            drawPrimitive( PE_FrameLineEdit, panel, p, w );
          }
          else {
            QColor c = panel->palette.color(QPalette::Base); c.setAlpha(255);
            p->fillRect( rect, c ); // panel->palette.brush(QPalette::Base)
          }
        }
      }
      break;
    }
    case PE_FrameTabWidget: {
      if (w && qobject_cast<QTabWidget*>((QWidget*)w)) {
        QTabWidget* tabW = qobject_cast<QTabWidget*>((QWidget*)w);
        int aRoundType = Style_Tools::BottomRight;
        QTabWidget::TabPosition aTabPos = tabW->tabPosition();
        if ( aTabPos != QTabWidget::North && aTabPos != QTabWidget::West )
          aRoundType = aRoundType | Style_Tools::TopLeft;
        if ( aTabPos != QTabWidget::South )
          aRoundType = aRoundType | Style_Tools::BottomLeft;
        if ( aTabPos != QTabWidget::East )
          aRoundType = aRoundType | Style_Tools::TopRight;
        QColor aBrdTopCol = model()->color( Style_Model::TabBorderTop );    // Style_Model::border_tab_top_clr
        QColor aBrdBotCol = model()->color( Style_Model::TabBorderBottom ); // Style_Model::border_tab_bot_clr
        Style_Tools::shadowRect( p, opt->rect, model()->widgetRounding( Style_Model::FrameRadius ),
                                 0., SHADOW, aRoundType,
                                 model()->color( Style_Model::FieldLight ),
                                 opt->palette.color( QPalette::Dark ),
                                 aBrdTopCol, aBrdBotCol, false, false, false, false );
        break;
      }
    }
    case PE_IndicatorToolBarHandle: {
      p->save();
      QRect r = opt->rect;
      bool horiz = opt->state & State_Horizontal;
      QLinearGradient gr( r.x(), r.y(), horiz ? r.x() : r.right(), horiz ? r.bottom() : r.y() );
      QColor aBgCol = opt->palette.color( QPalette::Window );
      gr.setColorAt( 0.0, aBgCol.light( BUT_PERCENT_ON ) );
      gr.setColorAt( 1.0, aBgCol );
      p->fillRect( r, gr );
      drawHandle( p, r, horiz, false );
      p->restore();
      break;
    }
    case PE_Widget: {
      BaseStyle::drawPrimitive( pe, opt, p, w );

      if ( !w )
        break;
      if( w->parent() && !qobject_cast<QMenuBar*>((QWidget*)w) )
         break;
      drawBackground( p, w->rect(), opt->palette.color( QPalette::Window ), false );
      break;
    }
    case PE_FrameTabBarBase:
      // for a tabbar that isn't part of a tab widget(dockWidgets for example).
      if (const QStyleOptionTabBarBase *tbb
          = qstyleoption_cast<const QStyleOptionTabBarBase *>(opt)) {
        if (tbb->shape != QTabBar::RoundedNorth && tbb->shape != QTabBar::RoundedEast &&
            tbb->shape != QTabBar::RoundedSouth && tbb->shape != QTabBar::RoundedWest) {
          BaseStyle::drawPrimitive( pe, opt, p, w );
          break;
        }
        QRect aSelRect = tbb->selectedTabRect;
        // line under selected tab bar object
        bool isSelected = opt->state & State_Selected;
        QTabBar* tabBar = qobject_cast<QTabBar*>((QWidget*)w);
        bool isLast = false;
        if ( tabBar )
          isLast = tabBar->currentIndex() == tabBar->count() -1;
        QColor aColor = opt->palette.color( QPalette::Window );
        bool isHover = hasHover() && (opt->state & State_Enabled) &&
                                     (opt->state & State_MouseOver);
        QPainterPath aSelPath = Style_Tools::tabRect( p, aSelRect, (int)tbb->shape,
                                                      model()->widgetRounding( Style_Model::FrameRadius ),
                                                      DELTA_H_TAB, aColor, aColor,
                            aColor, aColor, isSelected, isLast, isHover, false, false );
        if ( !aSelPath.isEmpty() )
          aSelRect = aSelPath.controlPointRect().toRect();
        QStyleOptionTabBarBase* copyOpt = (QStyleOptionTabBarBase*)tbb;
        copyOpt->selectedTabRect = aSelRect;
        QCommonStyle::drawPrimitive( pe, copyOpt, p, w );
        break;
      }
    case PE_IndicatorBranch: {
        // This is _way_ too similar to the common style.
        static const int decoration_size = 9;
        int mid_h = opt->rect.x() + opt->rect.width() / 2;
        int mid_v = opt->rect.y() + opt->rect.height() / 2;
        int bef_h = mid_h;
        int bef_v = mid_v;
        int aft_h = mid_h;
        int aft_v = mid_v;
        if (opt->state & State_Children) {
            int delta = decoration_size / 2;
            bef_h -= delta;
            bef_v -= delta;
            aft_h += delta;
            aft_v += delta;
            p->drawLine(bef_h + 2, bef_v + 4, bef_h + 6, bef_v + 4);
            if (!(opt->state & State_Open))
                p->drawLine(bef_h + 4, bef_v + 2, bef_h + 4, bef_v + 6);
            QPen oldPen = p->pen();
            p->setPen(opt->palette.dark().color());
            p->drawRect(bef_h, bef_v, decoration_size - 1, decoration_size - 1);
            p->setPen(oldPen);
        }
        QBrush brush(opt->palette.dark().color(), Qt::Dense4Pattern);
        if (opt->state & State_Item) {
            if (opt->direction == Qt::RightToLeft)
                p->fillRect(opt->rect.left(), mid_v, bef_h - opt->rect.left(), 1, brush);
            else
                p->fillRect(aft_h, mid_v, opt->rect.right() - aft_h + 1, 1, brush);
        }
        if (opt->state & State_Sibling)
            p->fillRect(mid_h, aft_v, 1, opt->rect.bottom() - aft_v + 1, brush);
        if (opt->state & (State_Open | State_Children | State_Item | State_Sibling))
            p->fillRect(mid_h, opt->rect.y(), 1, bef_v - opt->rect.y(), brush);
        break;
      }
    case PE_IndicatorDockWidgetResizeHandle: {
      QRect r = opt->rect;
      drawBorder( p, r, opt->state & State_Horizontal );
      bool hover = hasHover() && (opt->state & State_Enabled) && (opt->state & State_MouseOver);
      if ( hover )
          drawHoverRect(p, r, opt->palette.color( QPalette::Window ), 0, Style_Tools::All, false);
      bool horiz = r.width() > r.height();
      int aLen = model()->splitHandleLength();
      if ( horiz )
        r = QRect( r.x() +(int)((r.width()-aLen)/2), r.y(), aLen, r.height());
      else
        r = QRect( r.x(), r.y() +(int)((r.height()-aLen)/2), r.width(), aLen);
      drawHandle( p, r, horiz, true );
      break;
    }
    case PE_Frame: {
      QWidget* aWdg = (QWidget*)w;
      if ( qobject_cast<QTextEdit*>(aWdg) || qobject_cast<QTreeView*>(aWdg) ||
           qobject_cast<QListView*>(aWdg) ) {
        QRect r = opt->rect;
        if ( qobject_cast<QTreeView*>(aWdg) ) {
          QTreeView* trView = qobject_cast<QTreeView*>(aWdg);
          QHeaderView* aHeader = trView->header();
          if ( aHeader && aHeader->isVisible() ) {
            int aHeight = aHeader->contentsRect().height();
            r = QRect( r.x(), r.y()+aHeight, r.width(), r.height()-aHeight );
          }
        }
        QPalette aPal = aWdg->palette();
        double aMarg = LINE_GR_MARGIN;
        QColor base = model()->color( Style_Model::Base ), // Style_Model::pal_base_clr
          light = base,
          light_alt = base.dark(110),//AlternateBase color
          dark  = model()->color( Style_Model::FieldDark ); // Style_Model::fld_dark_clr
        //light.setAlpha( 0 ); // VSR commented: IPAL19262
        QLinearGradient gr_h(r.x(), r.y(), r.right(), r.y());
        gr_h.setColorAt( 0.0, dark );
        gr_h.setColorAt( aMarg / r.width(), light );
        gr_h.setColorAt( 1.0, light );
        QLinearGradient gr_v(r.x(), r.y(), r.x(), r.bottom() );
        gr_v.setColorAt( 0.0, dark );
        gr_v.setColorAt( aMarg / r.height(), light );
        gr_v.setColorAt( 1.0, light );
        // draw frame
        p->fillRect( r, base );
        p->fillRect( r, gr_h );
        p->fillRect( r, gr_v );
        aPal.setBrush( QPalette::Base, QBrush( light ) );

        QLinearGradient gr_alt(r.x(), r.y(), r.right(), r.y());
        gr_alt.setColorAt( 0.0, dark );
        gr_alt.setColorAt( (aMarg)*2/3/r.width(), light_alt );
        gr_alt.setColorAt( 1.0, light_alt );
        aPal.setBrush( QPalette::AlternateBase, QBrush( gr_alt ) );
        aWdg->setPalette( aPal );
      }
      BaseStyle::drawPrimitive( pe, opt, p, w );
      break;
    }
    default:
      BaseStyle::drawPrimitive( pe, opt, p, w );
  }
}

/*!
  \brief Returns the value of the given pixel \a metric.
  \param metric metric type
  \param opt style option
  \param w widget
  \return metric value
*/
int Style_Salome::pixelMetric( PixelMetric metric, const QStyleOption* opt,
                               const QWidget* w ) const
{
  if ( checkDebugLevel(7) ) {
    return BaseStyle::pixelMetric( metric, opt, w );
  }
  int aRes = BaseStyle::pixelMetric( metric, opt, w );
  switch( metric ) {
    case PM_SliderLength: {
      aRes += (int)((double)model()->sliderSize()/2);
      break;
    }
    case PM_DockWidgetFrameWidth:
      aRes = 1;
    break;
    case PM_DockWidgetSeparatorExtent:
      aRes = 8;
    break;
    case PM_DockWidgetTitleMargin:
      aRes = 2;
    break;
    case PM_SplitterWidth:
      aRes = 6;
    break;
    default:
    break;
  }
  return aRes;
}

/*!
  \brief Returns the size of the element described by the specified option \a opt 
  and type \a ct, based on the provided \a contentsSize.
  \param ct contents type
  \param opt style option
  \param contentsSize contents size
  \param w widget (optional)
  \return size of the element
*/
QSize Style_Salome::sizeFromContents( ContentsType ct, const QStyleOption* opt,
                                      const QSize& contentsSize, const QWidget* w ) const
{
  if ( checkDebugLevel(8) ) {
    return BaseStyle::sizeFromContents( ct, opt,contentsSize, w );
  }
  QSize sz = BaseStyle::sizeFromContents( ct, opt, contentsSize, w );
  switch (ct) {
    case CT_TabBarTab:
      if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
        if ( tab->position == QStyleOptionTab::End ||
             tab->position == QStyleOptionTab::OnlyOneTab ) {
          if ( tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedSouth ) {
            int aDelta = (int)(opt->rect.height()*DELTA_H_TAB/2);
            sz.setWidth( sz.width() + aDelta );
          }
          if ( tab->shape == QTabBar::RoundedEast || tab->shape == QTabBar::RoundedWest ) {
            int aDelta = (int)(opt->rect.width()*DELTA_H_TAB/2);
            sz.setHeight( sz.height() + aDelta );
          }
        }
        break;
      }
      break;
      case CT_Slider: {
        int aValue = model()->sliderSize();
        sz.setWidth( sz.width() + aValue );
        sz.setHeight( sz.height() + aValue );
        break;
      }
      case CT_ComboBox:
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
          QRect res = QRect( 0, 0, sz.width(), sz.height() );
          int aHalfRect = (int)Style_Tools::getMaxRect( res, 
                             (int)model()->widgetRounding( Style_Model::EditRadius )/2 ); // left value

          QRect old_arrow = BaseStyle::subControlRect( CC_ComboBox, cmb,
                                                           SC_ComboBoxArrow, w );
          int aDelta = res.height() - old_arrow.width(); // right value
          if ( cmb->editable )
            aDelta += aHalfRect; // for right of line edit internal
          sz.setWidth( res.width() + aDelta + aHalfRect );
        }
        break;
      default:
      break;
  }
  return sz;
}

/*!
  \brief Returns a pixmap for the given \a standardPixmap.
  \param stPixmap standard pixmap type
  \param opt style option
  \param w widget (optional)
  \return standard pixmap
*/
QPixmap Style_Salome::standardPixmap(StandardPixmap stPixmap, const QStyleOption *opt,
                                     const QWidget *w) const
{
  if ( checkDebugLevel(9) ) {
    return BaseStyle::standardPixmap( stPixmap, opt, w );
  }

  switch ( stPixmap )
  {
  case SP_DockWidgetCloseButton:
  case SP_TitleBarCloseButton:
    return QPixmap( cross_xpm );
  case SP_TitleBarMaxButton:
    return QPixmap( maximize_xpm );
  case SP_TitleBarNormalButton:
    return QPixmap( normal_xpm );
  case SP_TitleBarMinButton:
    return QPixmap( minimize_xpm );
  default:
    return BaseStyle::standardPixmap( stPixmap, opt, w );
  }
}

/*!
  \brief Returns an icon for the given \a standardIcon.
  \param standardIcon standard icon type
  \param opt style option
  \param w widget (optional)
  \return standard icon
*/
QIcon Style_Salome::standardIconImplementation( StandardPixmap standardIcon, 
                                                const QStyleOption* opt,
                                                const QWidget* w ) const
{
  if ( checkDebugLevel(10) ) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    return BaseStyle::standardIconImplementation( standardIcon, opt, w );
#else
    return QCommonStyle::standardIcon( standardIcon, opt, w );
#endif
  }

  switch ( standardIcon )
  {
  case SP_MessageBoxInformation:
    return QPixmap( ":/images/information.png" );
  case SP_MessageBoxWarning:
    return QPixmap( ":/images/warning.png" );
  case SP_MessageBoxCritical:
    return QPixmap( ":/images/critical.png" );
  case SP_MessageBoxQuestion:
    return QPixmap( ":/images/question.png" );
  default:
    break;
  }
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  return BaseStyle::standardIconImplementation( standardIcon, opt, w );
#else
  return BaseStyle::standardIcon( standardIcon, opt, w );
#endif
}

/*!
  \brief Returns an integer representing the specified style \a hint for the
  given widget \a w described by the provided style option \a opt.
  \param hint hint type
  \param opt style option
  \param w widget (optional)
  \param returnData (currently not used)
  \return style hint value
*/
int Style_Salome::styleHint( StyleHint hint, const QStyleOption* opt, const QWidget* w,
                            QStyleHintReturn* returnData ) const
{
  if ( checkDebugLevel(11) ) {
    return BaseStyle::styleHint( hint, opt, w, returnData );
  }
  int aRes = BaseStyle::styleHint( hint, opt, w, returnData );
  switch( hint ) {
    case SH_Table_GridLineColor: {
      if ( opt )
        aRes = model()->color( Style_Model::GridLine ).rgb(); // Style_Model::tbl_grline_clr
      else
        return aRes;
      break;
    }
    default:
      break;
  }
  return aRes;
}

/*!
  \brief Get the rectangle containing the specified subcontrol \a sc of the given
  complex control \a cc (with the style specified by option \a opt). 
  The rectangle is defined in screen coordinates.
  \param cc complex control type
  \param opt style option
  \param sc subcontrol type
  \param wid widget (optional)
  \return subcontrol rectangle
*/
QRect Style_Salome::subControlRect( ComplexControl cc, const QStyleOptionComplex* opt,
                                    SubControl sc, const QWidget* wid ) const
{
  if ( checkDebugLevel(12) ) {
    return BaseStyle::subControlRect( cc, opt, sc, wid );
  }
  QRect res = BaseStyle::subControlRect( cc, opt, sc, wid );
  switch ( cc ) {
    case CC_SpinBox: {
      int x = res.x(), w = res.width(), h = res.height();
      if ( sc==SC_SpinBoxUp || sc==SC_SpinBoxDown ) {
        QRect frame_r = BaseStyle::subControlRect( cc, opt, SC_SpinBoxFrame, wid );
        h = frame_r.height();
        res.setX( x+w-h );
        res.setWidth( h );
      }
      else if ( sc==QStyle::SC_SpinBoxEditField ) {
        res.setWidth( w-h );
        res.setTopLeft( QPoint( res.x(), res.y()-SHADOW ) );
      }
      break;
    }
    case CC_ComboBox: {
      if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
        res = cb->rect;
        int aHalfRect = (int)Style_Tools::getMaxRect( res, (int)model()->widgetRounding( Style_Model::EditRadius )/2 );
        int x = res.x(), w = res.width(), h = res.height();
        switch( sc ) {
          case SC_ComboBoxEditField: {
            res.setWidth( w-h );
            int aX = res.x();
            if ( !cb->editable )
              aX += aHalfRect;
            res.setTopLeft( QPoint( aX, res.y()-SHADOW ) );
            break;
          }
          case SC_ComboBoxArrow: {
            res.setX( x+w-h );
            res.setWidth( h );
            break;
          }
        }
      }
      break;
    }
    case CC_ScrollBar:
      if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
        QRect slider_r = BaseStyle::subControlRect( cc, opt, SC_ScrollBarSlider, wid );
        int aRect = Style_Tools::getMaxRect( slider_r, (int)model()->widgetRounding( Style_Model::ButtonRadius ) );
        switch( sc ) {
          case SC_ScrollBarSubPage:            // between top/left button and slider
            if (scrollbar->orientation == Qt::Horizontal)
              res.setRight( res.right()+aRect+1 );
            else
              res.setBottom( res.bottom()+aRect+1 );
            break;
          case SC_ScrollBarAddPage:            // between bottom/right button and slider
            if (scrollbar->orientation == Qt::Horizontal)
              res.setLeft( res.left() - aRect - 1 );
            else
              res.setTop( res.top() - aRect - 1);
          break;
          default:
            break;
        }
        break;
      }
    case CC_Slider: {
      if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
        switch ( sc ) {
          case SC_SliderGroove: {
            if ( slider->orientation == Qt::Horizontal ) {
              res.setLeft( res.left()+DELTA_SLIDER );
              res.setRight( res.right()-DELTA_SLIDER );
            }
            else {
              res.setTop( res.top()+DELTA_SLIDER );
              res.setBottom( res.bottom()-DELTA_SLIDER );
            }
            break;
          }
        }
      }
    }
  }
  return res;
}

/*!
  \brief Returns the sub-area for the given element \a se as described 
  in the provided style option \a opt.
  The returned rectangle is defined in screen coordinates.
  \param se subelement type
  \param opt style option
  \param wid widget (optional)
  \return subelement rectangle
*/
QRect Style_Salome::subElementRect( SubElement se, const QStyleOption* opt,
                                   const QWidget* wid ) const
{
  if ( checkDebugLevel(13) ) {
    return BaseStyle::subElementRect( se, opt, wid );
  }

  QRect res = BaseStyle::subElementRect( se, opt, wid );
  int aHalfRect = (int)Style_Tools::getMaxRect( res, (int)model()->widgetRounding( Style_Model::EditRadius )/2 );
  int w = res.width(), h = res.height();
  switch ( se ) {
    case SE_ComboBoxFocusRect: {

      QRect old_r = BaseStyle::subControlRect( CC_ComboBox,
                     qstyleoption_cast<const QStyleOptionComplex*>( opt ),
                     SC_ComboBoxArrow, wid );
      int old_w = old_r.width();
      res.setWidth( w-h+old_w-2 );
      break;
    }
    case SE_LineEditContents: {
      res.setTopLeft( QPoint( res.x()+aHalfRect, res.y()-SHADOW ) );
      res.setTopRight( QPoint( res.right()-aHalfRect, res.y() ) );
      break;
    }
    case SE_ProgressBarLabel:
    case SE_ProgressBarContents:
    case SE_ProgressBarGroove:
      return opt->rect;
  }
  if( qobject_cast<const QRadioButton*>(wid) ||
      qobject_cast<const QCheckBox*>(wid) ) {
      if( hasHover() )
        res = res.adjusted(0, 0, 2, 0);
  }
  return res;
}

/*!
  \brief Update palette colors from style model
*/
void Style_Salome::updatePaletteColors()
{
  QPalette pal = QApplication::palette();
  // colors
  for ( int i = (int)QPalette::Active; i <= (int)QPalette::Inactive; i++ ) {
    for ( int j = (int)Style_Model::WindowText; j < (int)Style_Model::NColorRoles; j++ ) {
      if ( j == QPalette::NoRole ) continue;
      pal.setColor( (QPalette::ColorGroup)i, (QPalette::ColorRole)j,
                    model()->color( (Style_Model::ColorRole)j, (QPalette::ColorGroup)i ) );
    }
  }
  QPixmapCache::clear();
  QApplication::setPalette( pal );
}

/*!
  \brief Update all widgets with the current style properties.
  \param app application object
*/
void Style_Salome::updateAllWidgets( QApplication* app )
{
  if ( !app )
    return;

  QWidgetList all = app->allWidgets();
  QWidget* w;
  for (QWidgetList::ConstIterator it2 = all.constBegin(); it2 != all.constEnd(); ++it2) {
    w = *it2;
    if (w->windowType() != Qt::Desktop && w->testAttribute(Qt::WA_WState_Polished)
        && !w->testAttribute(Qt::WA_SetStyle)) {
      QEvent e(QEvent::StyleChange);
      QApplication::sendEvent(w, &e);
      polish( w );
      w->update();
    }
  }
}

/*!
  \brief Check if any widget effect is currently used
  \return \c true if any widget effect is currently used
*/
bool Style_Salome::hasHover() const
{
  return model()->widgetEffect() != Style_Model::NoEffect;
}

/*!
  \brief Draw rectangle for the hovered widget
  \param p painter
  \param r rectangle
  \param bgCol background color
  \param rad corners rounding radius
  \param type shadow type
  \param border if \c true border is colored with specific color
*/
void Style_Salome::drawHoverRect( QPainter* p, const QRect& r, const QColor& bgCol, const double rad,
                                  const int type, const bool border ) const
{
  if ( !hasHover() )
    return;
  bool isAutoRaising = model()->widgetEffect() == Style_Model::AutoRaiseEffect;
  bool isHighWdg     = model()->widgetEffect() == Style_Model::HighlightEffect;
  QColor aBorder = model()->color( Style_Model::BorderTop ), // Style_Model::border_bot_clr
         aCol, aBrdCol;
  double aMargin = HIGH_WDG_MARGIN;
  if  ( isAutoRaising ) {
    aCol = bgCol;
    aBrdCol = aCol.dark(BUT_PERCENT_ON);
    if ( !border )
      aBorder = aCol;
    aMargin = 0;
  }
  else if ( isHighWdg ) {
    aCol    = model()->color( Style_Model::HighlightWidget ); // Style_Model::high_wdg_clr
    aBrdCol = model()->color( Style_Model::HighlightBorder ); // Style_Model::high_brd_wdg_clr
    if ( !border )
      aBorder = aBrdCol;
  }
  Style_Tools::highlightRect( p, r, rad, type, aMargin, aCol, aBrdCol, aBorder );
}

/*!
  \brief Draw widget handle
  \param p painter
  \param r rectangle
  \param horiz if \c true draw horizontal handle, otherwise draw vertical handle
  \param isRect if \c true surrounding rectangle is also drawn
*/
void Style_Salome::drawHandle( QPainter* p, const QRect& r, bool horiz, bool isRect ) const
{
  QPixmap hole( (const char**)hole_xpm );
  int i, j;
  double d_hor = model()->handleDelta( Qt::Horizontal );
  double d_ver = model()->handleDelta( Qt::Vertical );
  if ( !d_hor || !d_ver || !r.width() || !r.height() )
    return;
  int c_hor = (int)(r.width()/d_hor)-1;
  int c_ver = (int)(r.height()/d_ver)-1;
  if ( c_hor+1 <= 0 || c_ver+1 <= 0 )
    return;
  // correction for delta value
  d_hor = r.width()/(c_hor+1);
  d_ver = r.height()/(c_ver+1);

  double dd_hor = 0, dd_ver = 0;
  if ( horiz ) {
    for ( i = 0; i < c_hor; i++ ) {
      for ( j = 0; j < c_ver; j++ ) {
          p->drawPixmap( (int)( r.x() + dd_hor + ( i + 1 ) * d_hor  - 1 ),
                         (int)( r.y() + dd_ver + ( j + 1 ) * d_ver - 1 ), hole );
      }
      if ( !isRect ) {
        dd_ver += (int(d_ver)/2) + (int(d_ver)%2);
        c_ver = c_ver - 1;
      }
    }
  }
  else {
    for ( j = 0; j < c_ver; j++ ) {
      for ( i = 0; i < c_hor; i++ ) {
          p->drawPixmap( (int)( r.x() + dd_hor + ( i + 1 ) * d_hor  - 1 ),
                         (int)( r.y() + dd_ver + ( j + 1 ) * d_ver - 1 ), hole );
      }
      if ( !isRect ) {
        dd_hor += (int(d_hor)/2) + (int(d_hor)%2);
        c_hor = c_hor - 1;
      }
    }
  }
}

/*!
  \brief Draw background.
  \param p painter
  \param r rectangle
  \param bgCol background color
  \param fill 'fill rectangle' flag
  \param grad 'draw gradient' flag
  \param horix 'draw horizontal item' flag (usefull for gradient background)
*/
void Style_Salome::drawBackground( QPainter* p, const QRect& r, const QColor& bgCol,
                                   const bool fill, const bool grad, const bool horiz ) const
{
  if ( fill ) {
    if ( !grad )
      p->fillRect( r, bgCol );
    else {
      QLinearGradient gr( r.x(), r.y(), horiz ? r.x() : r.right(), horiz ? r.bottom() : r.y() );
      gr.setColorAt( 0.0, bgCol.light( BUT_PERCENT_ON ) );
      gr.setColorAt( 1.0, bgCol );
      p->fillRect( r, gr );
    }
  }

  int x = r.x(), y = r.y(), left = r.left(), top = r.top();
  int w = r.width(), h = r.height();

  QVector<QLine> lines;

  switch ( model()->linesType() ) {
  case Style_Model::Horizontal:
    {
      const int d = 3;
      for ( int i = 0; i <= h; i += d )
        lines.append( QLine( x, y+i, w, r.y()+i ) );
    }
    break;
  case Style_Model::Inclined:
    {
      const int d = 5;
      w = w/d*d;
      h = h/d*d;
      for ( int i = 0; i <= w; i += d )
        lines.append( QLine( x+i, y, x, y+i ) );
      for ( int i = 0; i < h; i += d )
        lines.append( QLine( left+w-i, top+h, left+w, top+h-i ) );
    }
    break;
  default:
    break;
  }

  if ( !lines.isEmpty() ) {
    QColor c = model()->color( Style_Model::Lines );
    int anAlpha = (int)( 255*( 1 - model()->linesTransparency()/100 ) ); 
    c.setAlpha( anAlpha );
    p->setPen( c );
    p->setRenderHint( QPainter::Antialiasing );
    p->drawLines( lines );
  }
}

/*!
  \brief Draw border
  \param p painter
  \param r rectangle
  \param horiz 'draw horizontal item' flag
*/
void Style_Salome::drawBorder( QPainter* p, const QRect& r, bool horiz ) const 
{
  QPen oldPen = p->pen();
  QColor aBrdTopCol = model()->color( Style_Model::TabBorderTop );    // Style_Model::border_tab_top_clr
  QColor aBrdBotCol = model()->color( Style_Model::TabBorderBottom ); // Style_Model::border_tab_bot_clr
  p->setPen( aBrdTopCol );
  if (horiz) {
    p->drawLine(r.left(),  r.top(), r.right(), r.top());
    p->setPen(aBrdBotCol);
    p->drawLine(r.left(), r.bottom(), r.right(), r.bottom());
  }
  else {
    p->drawLine(r.left(), r.top(), r.left(), r.bottom());
    p->setPen(aBrdBotCol);
    p->drawLine(r.right(), r.top(), r.right(), r.bottom());
  }
  p->setPen(oldPen);
}

/*!
  \brief Get corrected title text
  \param txt original text
  \param W possible width
  \param H possible height
  \param f used font
  \return corrected title text
*/
QString Style_Salome::titleText( const QString& txt, const int W, const int H, QFont& f ) const
{
  QString res = txt.trimmed();

  QFontMetrics fm( f );
  while( fm.height() > H && f.pointSize()>1 )
  {
    f.setPointSize( f.pointSize()-1 );
    fm = QFontMetrics( f );
  }

  if ( fm.width( res ) > W )
  {
    QString end( "..." );
    while ( !res.isEmpty() && fm.width( res + end ) > W )
      res.remove( res.length() - 1, 1 );

    if ( !res.isEmpty() )
      res += end;
  }

  return res;
}
