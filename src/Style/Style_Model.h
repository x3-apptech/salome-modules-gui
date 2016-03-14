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

// File   : Style_Model.h
// Author : Vadim SANDLER, Open CASCADE S.A.S (vadim.sandler@opencascade.com)
//
#ifndef STYLE_MODEL_H
#define STYLE_MODEL_H

#include "Style.h"

#include <QMap>
#include <QString>
#include <QPalette>
#include <QFont>

class QStyle;
class QtxResourceMgr;

//
// This class is private for Style package.
// Normally it should not be exported.
//
class STYLE_SALOME_EXPORT Style_Model
{
public:
  //! Color palette instance enumeration
  typedef enum {
    WindowText      = QPalette::WindowText,      //!< A general foreground color
    Button          = QPalette::Button,          //!< The general button background color
    Light           = QPalette::Light,           //!< Usually lighter than Button color
    Midlight        = QPalette::Midlight,        //!< Usually between Button and Light
    Dark            = QPalette::Dark,            //!< Usually darker than Button
    Mid             = QPalette::Mid,             //!< Usualliy between Button and Dark
    Text            = QPalette::Text,            //!< The foreground color used with Base
    BrightText      = QPalette::BrightText,      //!< A text color that is very different from WindowText, and contrasts well with e.g. Dark
    ButtonText      = QPalette::ButtonText,      //!< A foreground color used with the Button color
    Base            = QPalette::Base,            //!< Used mostly as the background color for text entry widgets
    Window          = QPalette::Window,          //!< A general background color
    Shadow          = QPalette::Shadow,          //!< Usually a very dark palette color
    Highlight       = QPalette::Highlight,       //!< A color to indicate a selected item or the current item
    HighlightedText = QPalette::HighlightedText, //!< A text color that contrasts with Highlight
    Link            = QPalette::Link,            //!< A text color used for unvisited hyperlinks
    LinkVisited     = QPalette::LinkVisited,     //!< A text color used for already visited hyperlinks
    AlternateBase   = QPalette::AlternateBase,   //!< Used as the alternate background color in views with alternating row colors
    NoRole          = QPalette::NoRole,          //!< No role; this special role is often used to indicate that a role has not been assigned
    ToolTipBase     = QPalette::ToolTipBase,     //!< Used as the background color for QToolTip and QWhatsThis
    ToolTipText     = QPalette::ToolTipText,     //!< Used as the foreground color for QToolTip and QWhatsThis
    NColorRoles     = QPalette::NColorRoles,     //!< Used to indicate last standard palette color
    BorderTop       = NColorRoles,               //!< Used to draw top-left borders of the widgets
    BorderBottom,                                //!< Used to draw bottom-right borders of the widgets
    TabBorderTop,                                //!< Used to draw top-left borders of the tab panels
    TabBorderBottom,                             //!< Used to draw bottom-right borders of the tab panels
    FieldLight,                                  //!< Light component of the text entry widgets
    FieldDark,                                   //!< Dark component of the text entry widgets
    Slider,                                      //!< Used to draw sliders
    Lines,                                       //!< Used to draw lines throughout all the widgets
    HighlightWidget,                             //!< Used to draw widget background when widget is hovered
    HighlightBorder,                             //!< Used to draw widget borders when widget is hovered
    Header,                                      //!< Used to draw header of tab panels
    ProgressBar,                                 //!< Main progress bar color
    Pointer,                                     //!< Used to draw different widgets indicators like spin box arrows etc
    Checked,                                     //!< Check box indicator color
    GridLine,                                    //!< Used to draw table widgets grid
    LastColor                                    //!< Points to the last color; no specific meaning
  } ColorRole;

  //! Lines type
  typedef enum {
    NoLines,                     //!< Do not draw lines
    Horizontal,                  //!< Draw horozontal lines
    Inclined,                    //!< Draw inclined lines
  } LineType;

  //!< Widget roundings
  typedef enum {
    ButtonRadius,               //!< Buttons rounding
    EditRadius,                 //!< Text entry widgets rounding
    FrameRadius,                //!< Frames rounding
    SliderRadius,               //!< Sliders rounding
  } WidgetRounding;

  //! Widget effect
  typedef enum {
    NoEffect,                   //!< Do not use widget effects
    HighlightEffect,            //!< Highlight widget when it is hovered
    AutoRaiseEffect             //!< Raise widget when it is hovered
  } WidgetEffect;

  Style_Model();
  virtual ~Style_Model();

  void                fromApplication( bool = false );
  void                fromResources( QtxResourceMgr*, const QString& = QString() );

  void                save( QtxResourceMgr* = 0, const QString& = QString() );
  void                update();
  void                restore();

  QtxResourceMgr*     resourceMgr() const;
  QString             resourceSection() const;


  QColor              color( ColorRole, QPalette::ColorGroup = QPalette::Active ) const;
  void                setColor( ColorRole, const QColor&, const QColor& = QColor(), const QColor& = QColor() );
  void                setColor( ColorRole, QPalette::ColorGroup, const QColor& );

  bool                isAutoPalette() const;
  void                setAutoPalette( bool );

  LineType            linesType() const;
  void                setLinesType( LineType );

  int                 linesTransparency() const;
  void                setLinesTransparency( int );

  QFont               applicationFont() const;
  void                setApplicationFont( const QFont& );

  double              widgetRounding( WidgetRounding ) const;
  void                setWidgetRounding( WidgetRounding, double );

  bool                antialiasing() const;
  void                setAntialiasing( bool );
  
  WidgetEffect        widgetEffect() const;
  void                setWidgetEffect( WidgetEffect );

  int                 handleDelta( Qt::Orientation ) const;
  void                setHandleDelta( Qt::Orientation, int );

  int                 splitHandleLength() const;
  void                setSplitHandleLength( int );

  int                 sliderSize() const;
  void                setSliderSize( int );

private:
  void                initDefaults();
  void                readColorValue( ColorRole, const QString& );
  void                writeColorValue( ColorRole, const QString&, QtxResourceMgr*, const QString& ) const;

private:
  typedef QMap<ColorRole, QColor>              ColorMap;
  typedef QMap<QPalette::ColorGroup, ColorMap> ColorGroupMap;
  typedef QMap<WidgetRounding, double>         RoundMap;
  typedef QMap<Qt::Orientation, int>           DeltaMap;

  QtxResourceMgr*     myResourceMgr;
  QString             myResourceSection;

  ColorGroupMap       myColors;
  bool                myAutoPalette;
  RoundMap            myWidgetRounding;
  DeltaMap            myHandleDelta;
  QFont               myFont;
  LineType            myLinesType;
  WidgetEffect        myWidgetEffect;
  bool                myAntiAliasing;
  int                 myLinesTransparency;
  int                 mySplitHandleLength;
  int                 mySliderSize;

  struct AppData {
    QStyle*  myStyle;
    QPalette myPalette;
    QFont    myFont;
  };
  
  static AppData*     myAppData;
};

#endif // STYLE_MODEL_H
