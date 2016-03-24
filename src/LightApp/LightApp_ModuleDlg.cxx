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

// File   : LightApp_ModuleDlg.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "LightApp_ModuleDlg.h"

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>

/*!
  \brief Pixmap used as default icon for the module.
  \internal
*/
static const char* const default_icon[] = { 
"48 48 17 1",
". c None",
"# c #161e4c",
"b c #1d3638",
"e c #2f585b",
"i c #345b5e",
"c c #386266",
"g c #3f7477",
"d c #4d8589",
"m c #519099",
"o c #6abbc1",
"a c #70c9d3",
"f c #79ddea",
"n c #7adff2",
"k c #7ce2f4",
"j c #993550",
"h c #d84b71",
"l c #ef537d",
"................................................",
"................................................",
"................................................",
"................................................",
"................................................",
"................########.########.########......",
"...............#aaaaaa###aaaaaa###aaaaaa##......",
"..............#aaaaaa#b#aaaaaa#b#aaaaaa#c#......",
".............########b########b########cc#......",
".............#dddddd#b#dddddd#b#dddddd#cc#......",
"...........########d########d########d#cc#......",
"..........#aaaaaa###aaaaaa###aaaaaa##d#cc#......",
".........#aaaaaa#b#aaaaaa#b#aaaaaa#c#d#cc#......",
"........########b########e########cc#d#c#.......",
"........#dddddd#b#dddddd#e#ffffff#cc#d####......",
"......########d########d########f#cc###g##......",
".....#aaaaaa###aaaaaa###hhhhhh##f#cc#gg#c#......",
"....#aaaaaa#b#aaaaaa#i#hhhhhh#j#f#cc###cc#......",
"...########b########i########jj#f#c#gg#cc#......",
"...#kkkkkk#b#kkkkkk#i#llllll#jj#f####g#cc#......",
"...#kkkkkk#b#kkkkkk#i#llllll#jj###m##g#cc#......",
"...#knnkkk#b#kkkkkk#i#llllll#jj#mm#c#g#cc#......",
"...#knnkkk#b#kkkkkk#i#llllll#jj###cc#g#c#.......",
"...#kkkkkk#b#kkkkkk#i#llllll#j#dd#cc#g####......",
"...#kkkkkk###kkkkkk###llllll####d#cc###g##......",
"...########g########g########o##d#cc#gg#c#......",
"....#gggggg#b#gggggg#b#oooooo#c#d#cc###cc#......",
"...########b########b########cc#d#c#gg#cc#......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#d####g#cc#......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc###g##g#cc#......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#gg#c#g#cc#......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc###cc#g#c#.......",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#c#gg#cc#g##........",
"...#kkkkkk###kkkkkk###kkkkkk####g#cc###.........",
"...########g########g########g##g#cc#...........",
"....#gggggg#b#gggggg#b#gggggg#c#g#cc#...........",
"...########b########b########cc#g#c#............",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#g##.............",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc###..............",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#................",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#cc#................",
"...#kkkkkk#b#kkkkkk#b#kkkkkk#c#.................",
"...#kkkkkk###kkkkkk###kkkkkk##..................",
"...########.########.########...................",
"................................................",
"................................................",
"................................................",
"................................................"};

/*!
  \class LightApp_ModuleDlg
  \brief A dialog box allowing to select study operation to be performed
  on the module activating.

  The dialog box is shown when the user tries to activate any module
  while there is no opened study. The dialog box proposes user to select
  one of the possible operations which should be done before module activating,
  for example, create new study or open study from the file.
  The available operations are assigned by adding the buttons with the unique
  identifier to the dialog box. When the user clicks any operation button,
  the dialog box sets its identifier as the return code and closes.

  The typical usage of the dialog box:
  \code
  LightApp_ModuleDlg dlg( desktop() );
  dlg.addButton( "New study", NewStudyId );
  dlg.addButton( "Open study...", OpenStudyId );
  int ret = dlg.exec();
  switch( ret ) {
  case NewStudyId:
    // create new study
    createNewStudy();
    break;
  case OpenStudyId:
    // open study from the file
    // ... show dialog box to choose the file
    QString fileName = QFileDialog::getOpenFileName( desktop(), "Open File" );
    if ( !fileName.isEmpty() )
      openStudy( fileName );
    break;
  default:
    // operation is cancelled
    break;
  }
  \endcode

  \sa addButton()
*/

/*!
  \brief Constructor.
  \param parent parent widget
  \param component module name
  \param icon module icon
*/
LightApp_ModuleDlg::LightApp_ModuleDlg( QWidget*       parent, 
                                        const QString& component, 
                                        const QPixmap& icon )
: QDialog ( parent )
{
  setModal( true );

  QPixmap defaultIcon( default_icon );
  setWindowTitle( tr( "CAPTION" ) );
  
  // icon
  QLabel* iconLab = new QLabel( this );
  iconLab->setFrameStyle( QFrame::Box | QFrame::Sunken );
  iconLab->setMinimumSize( 70, 70 );
  iconLab->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  iconLab->setPixmap( !icon.isNull() ? icon : defaultIcon );
  iconLab->setScaledContents( false );
  iconLab->setAlignment( Qt::AlignCenter );

  // info message
  QLabel* infoLab = new QLabel( tr ( "DESCRIPTION" ).arg( component ), this );
  infoLab->setTextFormat( Qt::RichText );
  infoLab->setAlignment( Qt::AlignCenter );
  
  // Buttons
  myButtonLayout = new QHBoxLayout(); 
  myButtonLayout->setMargin( 0 ); 
  myButtonLayout->setSpacing( 6 );

  // <Cancel>
  QPushButton* cancelBtn = new QPushButton( tr( "CANCEL" ), this );
  cancelBtn->setAutoDefault( true );

  myButtonLayout->addSpacing( 70 );
  myButtonLayout->addStretch();
  myButtonLayout->addWidget( cancelBtn );

  QGridLayout* layout = new QGridLayout( this ); 
  layout->setMargin( 11 );
  layout->setSpacing( 6 );

  layout->addWidget( iconLab, 0, 0 );
  layout->addWidget( infoLab, 0, 1 );
  layout->addLayout( myButtonLayout, 1, 0, 1, 2 );

  // signals and slots connections
  connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*!
  \brief Destructor.
*/
LightApp_ModuleDlg::~LightApp_ModuleDlg()
{
}

/*!
  \brief Add operation button to the dialog box.

  If the parameter \a id is equal to -1, then the 
  button identifier is generated automatically.

  \param button button text
  \param id button identifier
  \return button identifier
*/
int LightApp_ModuleDlg::addButton( const QString& button, const int id )
{
  static int lastId = 0;
  int bid = id == -1 ? --lastId : id;

  QPushButton* b = findButton( bid );
  if ( b ) {
    myButtons.remove( b );
    delete b;
  }

  QPushButton* newButton = new QPushButton( button, this );
  newButton->setAutoDefault( true );

  if ( myButtons.empty() ) {
	newButton->setDefault( true );
	newButton->setFocus();
  }

  myButtons.insert( newButton, bid );
  myButtonLayout->insertWidget( myButtonLayout->count()-3, newButton );
  connect( newButton, SIGNAL( clicked() ), this, SLOT( accept() ) );

  return bid;
}

/*!
  \brief Search button with the specified identifier.
  \param id button identifier
  \return button or 0 if \a id is invalid
*/
QPushButton* LightApp_ModuleDlg::findButton( const int id ) const
{
  QPushButton* btn = 0;
  for ( ButtonMap::ConstIterator it = myButtons.begin(); 
        it != myButtons.end() && !btn; ++it ) {
    if ( it.value() == id )
      btn = it.key();
  }
  return btn;
}

/*!
  \brief Called when any dialog button (except \c Cancel) 
  is clicked.
  
  Closes the dialog and sets its result code to the identifier
  of the button clicked by the user.
*/
void LightApp_ModuleDlg::accept()
{
  QPushButton* btn = ( QPushButton* )sender();
  done( myButtons[ btn ] );
}
