#include "SUIT_Tools.h"

#include <qdir.h>

#include <stdio.h>
#include <stdarg.h>

/*!
  Traces output to log-file.
  If log is NULL, 'Salome_trace' file is created in temp directory.
  Log file is written in 'append' mode.
*/
void SUIT_Tools::trace( const char* lpszLog, const char* lpszFormat, ... )
{
  QString tmpPath = tmpDir();
  if ( !tmpPath.isEmpty() )
    tmpPath += QDir::separator();

  tmpPath += QString( "Salome_trace" );

  FILE*	pStream;
  pStream = fopen( lpszLog ? lpszLog : tmpPath.latin1(), "a" );
  if ( pStream ) 
  {	
    va_list argptr;
    va_start( argptr, lpszFormat );
    fprintf( pStream, "- Trace %s [%d] : %s", __FILE__, __LINE__, lpszFormat );
    va_end( argptr );

    fclose( pStream );
  }
}

/*! 
    Creates a rect with TopLeft = ( min(x1,x2), min(y1,y2) )
    and BottomRight = ( TopLeft + (x2-x1)(y2-y1) )    
*/	
QRect SUIT_Tools::makeRect( const int x1, const int y1, const int x2, const int y2 )
{  
  QRect aRect;
  aRect.setRect( QMIN( x1, x2 ), QMIN( y1, y2 ), QABS( x2 - x1 ), QABS( y2 - y1 ) );
  return aRect;
}

/*!
  Creates font from string description
*/
QFont SUIT_Tools::stringToFont( const QString& fontDescription )
{
  QFont font;
  if ( fontDescription.stripWhiteSpace().isEmpty() || !font.fromString( fontDescription ) )
    font = QFont( "Courier", 11 );
  return font;
}

/*!
  Creates font's string description
*/
QString SUIT_Tools::fontToString( const QFont& font )
{
  return font.toString();
}

void SUIT_Tools::centerWidget( QWidget* src, const QWidget* ref )
{
  SUIT_Tools::alignWidget(src, ref, Qt::AlignCenter);
}

/*!
  Aligns widget 'w' as refered to widget 'ref' [ static ]
*/
void SUIT_Tools::alignWidget ( QWidget* src, const QWidget* ref, int alignFlags )
{
  if ( !src || !ref || !alignFlags ) return;

  QPoint srcOri = src->mapToGlobal( QPoint( 1, 1 ) );
  QPoint refOri = ref->mapToGlobal( QPoint( 1, 1 ) );

  int x = srcOri.x(), y = srcOri.y();
  int refWidth = ref->width(), refHei = ref->height();
  int srcWidth = src->width(), srcHei = src->height();
  if ( srcWidth <= 1 )
    srcWidth = src->sizeHint().width();
  if ( srcHei <= 1 )
    srcHei = src->sizeHint().height();

  if ( alignFlags & AlignLeft )
    x = refOri.x();
  if ( alignFlags & AlignRight )
    x = refOri.x() + refWidth - srcWidth;
  if ( alignFlags & AlignTop )
    y = refOri.y();
  if ( alignFlags & AlignBottom )
    y = refOri.y() + refHei - srcHei;
  if ( alignFlags & AlignHCenter )
    x = refOri.x() + ( refWidth - srcWidth ) / 2;
  if ( alignFlags & AlignVCenter )
    y = refOri.y() + ( refHei - srcHei ) / 2;

  if ( src->parentWidget() &&        /* we move a widget inside its parent */
      !src->inherits( "QDialog" ))   /* dialogs use global coordinates  */
    {
      QPoint pos = src->parentWidget()->mapFromGlobal( QPoint(x,y) );
      x = pos.x(); y = pos.y();
    }
#ifdef WNT
  x -= 4;                             /* - frame border width ( approx. ) */
  y -= 30;                            /* - caption height ( approx. ) */
#endif

  src->move( x, y );
}
