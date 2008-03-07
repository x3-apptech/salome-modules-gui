// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
// File   : SUIT_MsgDlg.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "SUIT_MsgDlg.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>

/*!
  \class SUIT_MsgDlg
  \brief Information message dialog box with custom number of buttons.

  The class provides a functionality to display message box with the custom number
  of buttons. Each button is identified by the unique non-zero number which can 
  be tested after dialog box finishes its execution. Pressing each button except
  causes dialog box to finish execution with return status equal to the button
  identifier. In addition, pressing "Cancel" button finishes dialog box execution
  wih return status 0.

  It is also possible to display custom pixmap or icon at the left side of the
  dialog box. Pass the required pixmap to the constructor or use method setPixmap().

  The typical usage of the dialog box:
  \code
  SUIT_MsgDlg dlg( this, tr( "Warning!" ), 
                   tr( "File %s exists. Overwrite?" ).arg( files[i] ),
		   QMessageBox::standardIcon( QMessageBox::Warning ) );
  dlg.addButton( "Yes", YesId );
  dlg.addButton( "No", NoId );
  dlg.addButton( "Yes to all", YesAllId );
  dlg.addButton( "No to all", NoAllId );
  int ret = dlg.exec();
  switch( ret ) {
  case YesId:
    // process one file
    processOneFile();
    break;
  case NoId:
    // skip current file processing and proceed to the next file
    break;
  case YesAllId:
    // process all files
    processAllFiles();
    stopped = true;
    break;
  case NoAllId:
    // skip all process all files
    processAllFiles();
    break;
  default:
    // operation is cancelled
    break;
  }
  \endcode

  \sa addButton(), setPixmap()
*/

/*!
  \brief Constructor.
  \param parent parent widget
  \param title dialog box caption
  \param msg dialog box message
  \param icon dialog box icon
*/
SUIT_MsgDlg::SUIT_MsgDlg( QWidget*       parent, 
			  const QString& title, 
			  const QString& msg, 
			  const QPixmap& icon )
: QDialog ( parent, "SUIT_MsgDlg", true )
{
  // title
  setCaption( title );
  
  // icon
  myIconLab = new QLabel( this );
  myIconLab->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  myIconLab->setScaledContents( false );
  myIconLab->setAlignment( Qt::AlignCenter );

  if ( !icon.isNull() )
    myIconLab->setPixmap( icon );

  // info message
  myMsgLab = new QLabel( msg, this );
  myMsgLab->setTextFormat( Qt::RichText );
  myMsgLab->setAlignment( Qt::AlignCenter );
  
  // Buttons
  myButtonLayout = new QHBoxLayout(); 
  myButtonLayout->setMargin( 0 ); 
  myButtonLayout->setSpacing( 6 );

  // <Cancel>
  QPushButton* cancelBtn = new QPushButton( tr( "CANCEL" ), this );
  myButtonLayout->addSpacing( 20 );
  myButtonLayout->addStretch();
  myButtonLayout->addWidget( cancelBtn );

  QGridLayout* layout = new QGridLayout( this ); 
  layout->setMargin( 11 );
  layout->setSpacing( 6 );

  layout->addWidget( myIconLab, 0, 0 );
  layout->addWidget( myMsgLab,  0, 1 );
  layout->addMultiCellLayout( myButtonLayout, 1, 1, 0, 1 );

  // signals and slots connections
  connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*!
  \brief Destructor.
*/
SUIT_MsgDlg::~SUIT_MsgDlg()
{
}

/*!
  \brief Add operation button to the dialog box.

  If the parameter \a id is equal to -1, then the 
  button identifier is generated automatically.

  \param btext button text
  \param id button identifier
  \return button identifier
*/
int SUIT_MsgDlg::addButton( const QString& btext, const int id )
{
  static int lastId = 0;
  int bid = id == -1 ? --lastId : id;

  QPushButton* b = button( bid );
  if ( b ) {
    myButtons.remove( b );
    delete b;
  }

  QPushButton* newButton = new QPushButton( btext, this );

  myButtonLayout->insertWidget( myButtons.count(), newButton );
  myButtons.insert( newButton, bid );
  connect( newButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

  return bid;
}

/*!
  \brief Set icon to the dialog box
  \param icon pixmap
*/
void SUIT_MsgDlg::setPixmap( const QPixmap& icon )
{
  myIconLab->setPixmap( icon );
}


/*!
  \brief Called when any dialog button (except \c Cancel) 
  is clicked.
  
  Closes the dialog and sets its result code to the identifier
  of the button clicked by the user.
*/
void SUIT_MsgDlg::accept()
{
  QPushButton* btn = ( QPushButton* )sender();
  done( myButtons[ btn ] );
}

/*!
  \brief Search button with the specified identifier.
  \param id button identifier
  \return button or 0 if \a id is invalid
*/
QPushButton* SUIT_MsgDlg::button( const int id ) const
{
  QPushButton* btn = 0;
  for ( ButtonMap::ConstIterator it = myButtons.begin(); 
	it != myButtons.end() && !btn; ++it ) {
    if ( it.data() == id )
      btn = it.key();
  }
  return btn;
}

/*!
  \brief Get information icon label.
  \return information icon label
*/
QLabel* SUIT_MsgDlg::iconLabel() const
{
  return myIconLab;
}

/*!
  \brief Get information message label.
  \return information message label
*/
QLabel* SUIT_MsgDlg::messageLabel() const
{
  return myMsgLab;
}
