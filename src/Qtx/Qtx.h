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

// File:      Qtx.h
// Author:    Sergey TELKOV
//
#ifndef QTX_H
#define QTX_H

#if defined WIN32
#  if defined QTX_EXPORTS || defined qtx_EXPORTS
#    define QTX_EXPORT _declspec( dllexport )
#  else
#    define QTX_EXPORT _declspec( dllimport )
#  endif
#else
#  define QTX_EXPORT  
#endif

#if defined SOLARIS
#define bool  int
#define false 0
#define true  1
#endif

#include <QString>
#include <QList>
#include <QFont>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QGradient>

class QObject;
class QWidget;
class QCompleter;

typedef QList<int>    QIntList;       //!< list of int values
typedef QList<short>  QShortList;     //!< list of short int values
typedef QList<double> QDoubleList;    //!< list of double values
typedef QList<QColor> QColorList;     //!< list of colors

class QTX_EXPORT Qtx
{
public:
  //! Widget alignment flags
  typedef enum
  {
    AlignLeft            = Qt::AlignLeft,            //!< align left side of one widget to the left side of another widget
    AlignLeading         = Qt::AlignLeading,         //!< synonim for AlignLeft
    AlignRight           = Qt::AlignRight,           //!< align right side of one widget to the right side of another widget
    AlignTrailing        = Qt::AlignTrailing,        //!< synonim for AlignRight
    AlignHCenter         = Qt::AlignHCenter,         //!< align one widget to the center of another widget in horizontal dimension
    AlignJustify         = Qt::AlignJustify,         //!< synonym of Qt::AlignJustify
    AlignAbsolute        = Qt::AlignAbsolute,        //!< synonym of Qt::AlignAbsolute
    AlignHorizontal_Mask = Qt::AlignHorizontal_Mask, //!< synonym of Qt::AlignHorizontal_Mask

    AlignTop             = Qt::AlignTop,             //!< align top side of one widget to the top side of another widget
    AlignBottom          = Qt::AlignBottom,          //!< align bottom side of one widget to the bottom side of another widget
    AlignVCenter         = Qt::AlignVCenter,         //!< align one widget to the center of another widget in vertical dimension
    AlignVertical_Mask   = Qt::AlignVertical_Mask,   //!< synonym of Qt::AlignVertical_Mask

    AlignCenter          = Qt::AlignCenter,          //!< align one widget to the center of another widget in both dimensions

    AlignOutLeft         = Qt::AlignVCenter  << 2,   //!< align right side of one widget to the left side of another widget
    AlignOutRight        = AlignOutLeft      << 2,   //!< align left side of one widget to the right side of another widget
    AlignOutTop          = AlignOutRight     << 2,   //!< align bottom side of one widget to the top side of another widget
    AlignOutBottom       = AlignOutTop       << 2    //!< align top side of one widget to the bottom side of another widget
  } AlignmentFlags;

  //! Path type, indicates required directory/file operation
  typedef enum { 
    PT_OpenFile,      //!< the file is opened
    PT_SaveFile,      //!< the file is saved
    PT_Directory      //!< the directory path is required
  } PathType;

  //! Custom data roles
  enum { 
    AppropriateRole = Qt::UserRole + 100   //!< can be used to return \c true if data is appropriate
  };

  typedef enum {
        Shown,   //!< column should be always visible
        Hidden,  //!< column should be always hidden
        Toggled  //!< it should be possible to show/hide the column with help of popup menu
  } Appropriate;  //!< appropriate status

  //! Environment variables substitution mode
  typedef enum {
        Always, //!< substitute environment variable by it's value if variable exists, and "" otherwise
        Never,  //!< keep environment variable as is without any substitution
        Auto    //!< substitute environment variable by it's value if variable exists, and keep it as is otherwise
  } SubstMode;

  //! object visibility state
  typedef enum {
    ShownState,             //!< Object is shown in viewer
    HiddenState,            //!< Object is hidden in viewer
    UnpresentableState,     //!< Unpresentable object
  } VisibilityState;

  //! Header view flags
  typedef enum {
    ShowText = 0x001,                //!< Show only text in the header
    ShowIcon = 0x010,                //!< Show only icon in the header
    ShowAll  = ShowText | ShowIcon   //!< Show icon and text in the header
  } HeaderViewFlags;

  //! Type of the custom data (for custom tree model)
  typedef enum {
    IdType
  } CustomDataType;
  
  //! Background mode
  typedef enum { 
    NoBackground,              // no (invalid) background data
    ColorBackground,           // single color
    SimpleGradientBackground,  // simple two-color gradient
    CustomGradientBackground   // custom (complex) gradient
  } BackgroundMode;

  //! Texture mode
  typedef enum { 
    CenterTexture,             // center texture
    TileTexture,               // tile texture
    StretchTexture,            // stretch texture
  } TextureMode;

  class QTX_EXPORT Localizer
  {
  public:
    Localizer();
    ~Localizer();
  private:
    QString myCurLocale;
  };

  class QTX_EXPORT CmdLineArgs
  {
  public:
    CmdLineArgs();
    ~CmdLineArgs();
    
    int argc() const;
    char** argv() const;

  private:
    int    myArgc;
    char** myArgv;
  };

  class QTX_EXPORT BackgroundData
  {
  public:
    BackgroundData();
    BackgroundData( const QColor& );
    BackgroundData( int, const QColor&, const QColor& );
    BackgroundData( const QGradient& );
    virtual ~BackgroundData();

    bool operator==( const BackgroundData&) const;
    inline bool operator!=( const BackgroundData& other ) const
    { return !operator==( other ); }

    bool             isValid() const;
    
    BackgroundMode   mode() const;
    void             setMode( const BackgroundMode );
    
    TextureMode      texture( QString& ) const;
    void             setTexture( const QString&, TextureMode = Qtx::CenterTexture );
    bool             isTextureShown() const;
    void             setTextureShown( bool );

    QColor           color() const;
    void             setColor( const QColor& );

    int              gradient( QColor&, QColor& ) const;
    void             setGradient( int, const QColor&, const QColor& );

    const QGradient* gradient() const;
    void             setGradient( const QGradient& );
  
  private:
    BackgroundMode   myMode;
    TextureMode      myTextureMode;
    QString          myFileName;
    QColorList       myColors;
    int              myGradientType;
    QGradient        myGradient;
    bool             myTextureShown;
  };

  static QString     toQString( const char*, const int = -1 );
  static QString     toQString( const short*, const int = -1 );
  static QString     toQString( const unsigned char*, const int = -1 );
  static QString     toQString( const unsigned short*, const int = -1 );

  static void        setTabOrder( QWidget*, ... );
  static void        setTabOrder( const QWidgetList& );
  static void        alignWidget( QWidget*, const QWidget*, const int );

  static void        simplifySeparators( QWidget*, const bool = true );

  static bool        isParent( QObject*, QObject* );
  static QObject*    findParent( QObject*, const char* );

  static QString     dir( const QString&, const bool = true );
  static QString     file( const QString&, const bool = true );
  static QString     extension( const QString&, const bool = false );

  static QString     library( const QString& );

  static QString     tmpDir();
  static bool        mkDir( const QString& );
  static bool        rmDir( const QString& );
  static bool        dos2unix( const QString& );
  static QString     addSlash( const QString& );

  static QCompleter* pathCompleter( const PathType, const QString& = QString() );
  static QString     findEnvVar( const QString&, int&, int& );
  static QString     makeEnvVarSubst( const QString&, const SubstMode = Auto );

  static int         rgbSet( const QColor& );
  static int         rgbSet( const int, const int, const int );

  static QColor      rgbSet( const int );
  static void        rgbSet( const int, int&, int&, int& );

  static QColor      scaleColor( const int, const int, const int );
  static void        scaleColors( const int, QColorList& );

  static QPixmap     scaleIcon( const QPixmap&, const unsigned, const unsigned = 0 );
  static QImage      grayscale( const QImage& );
  static QPixmap     grayscale( const QPixmap& );
  static QImage      transparentImage( const int, const int, const int = -1 );
  static QPixmap     transparentPixmap( const int, const int, const int = -1 );
  static QPixmap     composite( const QPixmap&, const int, const int, const QPixmap& = QPixmap() );

  static QString     colorToString( const QColor& );
  static bool        stringToColor( const QString&, QColor& );
  static QString     biColorToString( const QColor&, const int );
  static bool        stringToBiColor( const QString&, QColor&, int& );
  static QColor      mainColorToSecondary( const QColor&, int );

  static QString     gradientToString( const QLinearGradient& );
  static QString     gradientToString( const QRadialGradient& );
  static QString     gradientToString( const QConicalGradient& );
  static bool        stringToLinearGradient( const QString&, QLinearGradient& );
  static bool        stringToRadialGradient( const QString&, QRadialGradient& );
  static bool        stringToConicalGradient( const QString&, QConicalGradient& );

  static QString        backgroundToString( const BackgroundData& );
  static BackgroundData stringToBackground( const QString& );

  static long        versionToId( const QString& );

  static QString     qtDir( const QString& = QString());

  static QFont   stringToFont( const QString& fontDescription );

#ifndef WIN32
  static void*       getDisplay();
  static Qt::HANDLE  getVisual();
#endif
};

#endif
