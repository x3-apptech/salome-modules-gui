#include "SalomeApp_CheckFileDlg.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>

/*!
Constructor
*/
SalomeApp_CheckFileDlg::SalomeApp_CheckFileDlg( QWidget* parent, bool open, const QString& theCheckBoxName, bool showQuickDir, bool modal) :
SUIT_FileDlg( parent, open, showQuickDir, modal )
{    
  myCheckBox = new QCheckBox( theCheckBoxName, this );
  QLabel* label = new QLabel("", this);
  QPushButton* pb = new QPushButton(this);               
  addWidgets( label, myCheckBox, pb );
  pb->hide();
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
