#ifndef SALOMESTYLE_H
#define SALOMESTYLE_H

#define parent_style qwindowsstyle
#define PARENT_STYLE QWindowsStyle

#define QUOTE(x) #x
#define PARENT_INC(x) QUOTE(x.h)

//#include PARENT_INC(parent_style)
#include <qwindowsstyle.h>

#include <qpixmap.h>
#include <qdockwindow.h>
#include <qstyleplugin.h>

#if defined SALOMESTYLE_EXPORTS
#if defined WIN32
#define SALOMESTYLE_EXPORT _declspec( dllexport )
#else
#define SALOMESTYLE_EXPORT  
#endif
#else
#if defined WIN32
#define SALOMESTYLE_EXPORT _declspec( dllimport )
#else
#define SALOMESTYLE_EXPORT  
#endif
#endif

class QFontMetrics;

class SALOMESTYLE_EXPORT SalomeStyle : public PARENT_STYLE
{
public:
  SalomeStyle();
  virtual ~SalomeStyle();

  virtual void       polish( QWidget* );

  virtual bool       eventFilter( QObject*, QEvent* );

  static void        mix( const double, QRgb&, const QRgb& );
  static void        mix( const double, QPixmap&, const QColor& );
  static void        mix( const double, const QColor&, const QColor&, QColor& );

  static void        toGrayscale( QPixmap&, double = 1.0 );

  virtual void       drawPrimitive( PrimitiveElement, QPainter*, const QRect&,
				    const QColorGroup&, SFlags = Style_Default,
				    const QStyleOption& = QStyleOption::Default ) const;

  virtual void       drawControl( ControlElement, QPainter*, const QWidget*,
				  const QRect&, const QColorGroup&, SFlags = Style_Default,
				  const QStyleOption& = QStyleOption::Default ) const; 

  virtual QRect      subRect( SubRect, const QWidget* ) const;

  virtual void       drawComplexControl( ComplexControl, QPainter*, const QWidget*,
					 const QRect&, const QColorGroup&, SFlags = Style_Default,
					 SCFlags = SC_All, SCFlags = SC_None,
					 const QStyleOption& = QStyleOption::Default ) const;

  virtual QPixmap    stylePixmap( StylePixmap, const QWidget* = 0,
				  const QStyleOption& = QStyleOption::Default ) const;

  virtual int        pixelMetric( PixelMetric, const QWidget* widget = 0 ) const;

protected:
  typedef enum { LeftToRight, RightToLeft, UpToDown, DownToUp } Direction;

  typedef double (*gradient_func)( double );

  void               drawGradient( QPainter*, const QRect&, const QColor&,
				   const QColor&, const Direction, gradient_func ) const;

  QString            titleText( const QString&, const int, const QFontMetrics& ) const;

private:
  QWidget*           myTitleParent;
};

class SalomeStylePlugin : public QStylePlugin
{
public:
  SalomeStylePlugin();
  virtual ~SalomeStylePlugin();

  virtual QStringList keys() const;
  virtual QStyle* create( const QString& );
};

#endif
