//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "SalomeApp_CheckFileDlg.h"

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

/*!
Constructor
*/
SalomeApp_CheckFileDlg::SalomeApp_CheckFileDlg( QWidget* parent, bool open, const QString& theCheckBoxName, bool showQuickDir, bool modal) :
SUIT_FileDlg( parent, open, showQuickDir, modal )
{    
  QGridLayout* grid = ::qobject_cast<QGridLayout*>( layout() );
  if ( grid )
  {
    myCheckBox = new QCheckBox( theCheckBoxName, this );
    QLabel* label = new QLabel("", this);
    QPushButton* pb = new QPushButton(this);        
    
    int row = grid->rowCount();
    grid->addWidget( label, row, 0 );
    grid->addWidget( myCheckBox, row, 1 );
    grid->addWidget( pb, row, 2 );
      
    pb->hide();
  }
}

/*!
Destructor
*/
SalomeApp_CheckFileDlg::~SalomeApp_CheckFileDlg() 
{

}

/*!Sets checked.*/
void SalomeApp_CheckFileDlg::SetChecked( bool check )
{
  myCheckBox->setChecked(check);
}

/*!Is checked?
 *\retval boolean - true, check box is checked, else false.
 */
bool SalomeApp_CheckFileDlg::IsChecked() const
{
  return myCheckBox->isChecked();
}
