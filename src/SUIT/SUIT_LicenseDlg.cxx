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

#include "SUIT_LicenseDlg.h"

#include <QApplication>
#include <QTextEdit>
#include <QLayout> 
#include <QPushButton>
#include <QTextStream> 
#include <QFile>
#include <QPrinter>
//#include <QSimpleRichText>
#include <QTextDocument>
#include <QPainter>

#include <math.h>

#ifdef WIN32
#include <UserEnv.h>
#endif


/*!
  Constructor
  Construct a dialog with specified parent and name.
  \param modal define modal status of dialog (default modal dialog created).
*/
SUIT_LicenseDlg::SUIT_LicenseDlg( bool firstShow, QWidget* parent, const char* name, bool modal )
 : QDialog( parent )
{
  setObjectName( name );
  setModal( modal );
  QString env;
  if ( ::getenv( "SALOME_LICENSE_FILE" ) )
    env = ::getenv( "SALOME_LICENSE_FILE" );
 
  QFile file( env ); // Read the text from a file
  
  if ( !file.exists() || !file.open( QIODevice::ReadOnly ) )
    return;
  
  setWindowTitle( tr( "License" ) );
  
  // Create text editor
  myTextEdit = new QTextEdit( this );
  QTextStream stream( &file );
  myTextEdit->setText( stream.readAll() );
  file.close();
  myTextEdit->setReadOnly( true );
  //myTextEdit->ensureVisible(0, 0);
  
  // Create buttons
  QPushButton* anAgreeBtn = new QPushButton( tr( "Agree" ), this );
  if (firstShow) {
    anAgreeBtn->setAutoDefault( true );
    anAgreeBtn->setFocus();
  }
  else
    anAgreeBtn->hide();
 
  QPushButton* aCancelBtn = new QPushButton( this );
  aCancelBtn->setText( firstShow ? tr( "Cancel" ) : tr( "Close" ) );
  if ( !firstShow )
    aCancelBtn->setFocus();

  QPushButton* aPrintBtn = new QPushButton( tr( "Print..." ), this );

  // Layouting
  QVBoxLayout* aBaseLayout = new QVBoxLayout( this );
  aBaseLayout->setMargin( 5 );
  aBaseLayout->setSpacing( 5 );
  aBaseLayout->addWidget( myTextEdit );
  
  QHBoxLayout* aButtonsLayout = new QHBoxLayout();
  aBaseLayout->addLayout( aButtonsLayout );
  if (firstShow)
    aButtonsLayout->addWidget( anAgreeBtn );
  
  aButtonsLayout->addWidget( aCancelBtn );
  aButtonsLayout->addStretch();
  aButtonsLayout->addWidget( aPrintBtn );
 
  // Connections
  connect( anAgreeBtn, SIGNAL( clicked() ), this, SLOT( onAgree( ) ) );
  connect( aCancelBtn, SIGNAL( clicked() ), this, SLOT( onCancel( ) ) );
  connect( aPrintBtn,  SIGNAL( clicked() ), this, SLOT( onPrint( ) ) );
  
  resize( 640, 480 );
}

/*!
        Name: ~SUIT_LicenseDlg [public]
        Desc: Destructor
*/

SUIT_LicenseDlg::~SUIT_LicenseDlg()
{
}

void SUIT_LicenseDlg::onAgree()
{
  QString env;
#ifdef WIN32
    DWORD aLen=1024;
  char aStr[1024];
  HANDLE aToken=0;
  HANDLE hProcess = GetCurrentProcess();
  OpenProcessToken(hProcess,TOKEN_QUERY,&aToken);
  if( ! GetUserProfileDirectory( aToken, aStr, &aLen ) )
    reject();

  env = aStr;
#else
  if( ! ::getenv( "HOME" ) )
    reject();
  env = ::getenv( "HOME" );
#endif
 
  QFile file( env + "/ReadLicense.log" ); // Read the text from a file

  file.open( QIODevice::WriteOnly );

  QTextStream ts( &file );
  ts << "OK" << endl;
  //file.writeBlock( "OK", (Q_ULONG)qstrlen( "OK" ) );
  file.close();

  accept();
}

void SUIT_LicenseDlg::onCancel()
{
  reject();
}

void SUIT_LicenseDlg::onPrint()
{
  QPrinter aPrinter( QPrinter::HighResolution );
  aPrinter.setFullPage(true);
  
  if ( true /*aPrinter.pageSetup( this )*/ ) {
    QPainter aPainter( &aPrinter );
    if( !aPainter.isActive() ) // starting printing failed
      return;
        
    // define fonts
    QFont aBodyFont = myTextEdit->currentFont();
    QFont aFooterFont = aBodyFont;

    // calculate margin
    QPaintDevice* aMetrics = aPainter.device();
    int aDpiY = aMetrics->logicalDpiY();
    int aMargin = (int) ( (2/2.54)*aDpiY ); // 2 cm margins

    QRect aBody( aMargin, aMargin, aMetrics->width() - 2*aMargin, aMetrics->height() - 2*aMargin );

    // convert text to rich text format
    QString aFormattedText = Qt::convertFromPlainText( myTextEdit->toPlainText() );
        
    QTextDocument aRichText( aFormattedText );
    aRichText.setDefaultFont( aBodyFont );


    /*QSimpleRichText aRichText( aFormattedText,
                               aBodyFont,
                               myTextEdit->context(),
                               myTextEdit->styleSheet(),
                               myTextEdit->mimeSourceFactory(),
                               aBody.height() );
    */
    aRichText.setPageSize( QSize( aBody.width(), aRichText.pageSize().height() ) );
        //aRichText.setWidth( &aPainter, aBody.width() );
    
    QRect aView( aBody );
    
    int aPageIndex = 1;
    
    do {
      // print page text
      aRichText.drawContents( &aPainter, aView );
          //aRichText.draw( &aPainter, aBody.left(), aBody.top(), aView, colorGroup() );
      aView.translate( 0, aBody.height() );
      aPainter.translate( 0 , -aBody.height() );
      
      // print page number
      aPainter.setPen(Qt::gray);
      aPainter.setFont(aFooterFont);
      QString aFooter = QString("Page ") + QString::number(aPageIndex);
      aPainter.drawText( aView.right() - aPainter.fontMetrics().width( aFooter ),
                         aView.bottom() + aPainter.fontMetrics().ascent() + 5, aFooter );
      
      if ( aView.top() >= aRichText.size().height() )
        break;
      aPrinter.newPage();
      aPageIndex++;
    } while (true);
  }
}
