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
    QCheckBox* myCheckBox = new QCheckBox( theCheckBoxName, this );
    QLabel*         label = new QLabel("", this);
    QPushButton*       pb = new QPushButton(this);        
    myCheckBoxes.append( myCheckBox );

    int row = grid->rowCount();
    grid->addWidget( label, row, 0 );
    grid->addWidget( myCheckBox, row, 1 );
    grid->addWidget( pb, row, 2 );

    pb->hide();
  }
}

/*!
  Constructor
*/
SalomeApp_CheckFileDlg::SalomeApp_CheckFileDlg( QWidget*                 parent,
                                                bool                     open,
                                                const QStringList&       theCheckBoxNames,
                                                bool                     showQuickDir,
                                                bool                     modal,
                                                const QList< QWidget* >& wdgList,
                                                const bool               wdgAfter) :
  SUIT_FileDlg( parent, open, showQuickDir, modal )
{
  if ( theCheckBoxNames.count() > 0 || wdgList.count() > 0 )
  {
    QGridLayout* grid = ::qobject_cast<QGridLayout*>( layout() );
    if ( grid )
    {
      if ( !wdgAfter )
        for ( int i = 0; i < wdgList.count(); ++i )
        {
          if ( wdgList[i] )
          {
            int row = grid->rowCount();
            grid->addWidget( wdgList[i], row, 1 );
          }
        }

      for ( int i = 0; i < theCheckBoxNames.count(); ++i )
      {
        QCheckBox* myCheckBox = new QCheckBox( theCheckBoxNames.at(i), this );
        myCheckBoxes.append( myCheckBox );

        int row = grid->rowCount();
        grid->addWidget( myCheckBox, row, 1 );
      }

      if ( wdgAfter )
        for ( int i = 0; i < wdgList.count(); ++i )
        {
          if ( wdgList[i] )
          {
            int row = grid->rowCount();
            grid->addWidget( wdgList[i], row, 1 );
          }
        }
    }
  }
}

/*!
  Destructor
*/
SalomeApp_CheckFileDlg::~SalomeApp_CheckFileDlg() 
{

}

/*!Sets checked.*/
void SalomeApp_CheckFileDlg::SetChecked( bool check, int checkBoxId/*=0*/ )
{
  if ( checkBoxId >=0 && checkBoxId < myCheckBoxes.count() )
    myCheckBoxes.at( checkBoxId )->setChecked(check);
}

/*!Is checked?
 *\retval boolean - true, check box is checked, else false.
 */
bool SalomeApp_CheckFileDlg::IsChecked( int checkBoxId ) const
{
  if ( checkBoxId >=0 && checkBoxId < myCheckBoxes.count() )
    return myCheckBoxes.at( checkBoxId )->isChecked();
  return false;
}
