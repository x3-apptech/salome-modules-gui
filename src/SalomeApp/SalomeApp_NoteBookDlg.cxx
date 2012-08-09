// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:    SalomeApp_NoteBookDlg.cxx
// Author : Roman NIKOLAEV, Open CASCADE S.A.S.
// Module : GUI
//
#include <PyConsole_Interp.h> // this include must be first (see PyInterp_base.h)!
#include <PyConsole_Console.h>

#include "SalomeApp_NoteBookDlg.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_VisualState.h"

#include <Qtx.h>

#include <CAM_Module.h>

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include <SALOMEDS_Tool.hxx>

#include <QWidget>
#include <QDialog>
#include <QGridLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QFont>
#include <QGroupBox>
#include <QList>
#include <QApplication>
#include <QDir>

#include <string>
#include <vector>

#define DEFAULT_MARGIN  11
#define DEFAULT_SPACING 6
#define SPACER_SIZE     120
#define COLUMN_SIZE     180

#define NAME_COLUMN  0
#define VALUE_COLUMN 1


///////////////////////////////////////////////////////////////////////////
//                 NoteBook_TableRow class                               //
///////////////////////////////////////////////////////////////////////////
//============================================================================
/*! Function : NoteBook_TableRow
 *  Purpose  : Constructor
 */
//============================================================================
NoteBook_TableRow::NoteBook_TableRow(int index, NoteBook_Table* parentTable, QWidget* parent):
  QWidget(parent),
  myParentTable(parentTable),
  myIndex(index),
  myRowHeader(new QTableWidgetItem()),
  myVariableName(new QTableWidgetItem()),
  myVariableValue(new QTableWidgetItem())
{
}

//============================================================================
/*! Function : ~NoteBook_TableRow
 *  Purpose  : Destructor
 */
//============================================================================
NoteBook_TableRow::~NoteBook_TableRow()
{
}

//============================================================================
/*! Function : AddToTable
 *  Purpose  : Add this row to the table theTable
 */
//============================================================================
void NoteBook_TableRow::AddToTable(QTableWidget *theTable)
{
  int aPosition = theTable->rowCount();
  int aRowCount = aPosition+1;
  theTable->setRowCount(aRowCount);
  myRowHeader->setText(QString::number(aRowCount));

  theTable->setVerticalHeaderItem(aPosition,myRowHeader);
  theTable->setItem(aPosition, NAME_COLUMN, myVariableName);
  theTable->setItem(aPosition, VALUE_COLUMN, myVariableValue);
}

//============================================================================
/*! Function : SetName
 *  Purpose  : 
 */
//============================================================================
void NoteBook_TableRow::SetName(const QString theName)
{
  myVariableName->setText(theName);
}

//============================================================================
/*! Function : SetValue
 *  Purpose  : 
 */
//============================================================================
void NoteBook_TableRow::SetValue(const QString theValue)
{
  myVariableValue->setText(theValue);
}

//============================================================================
/*! Function : GetName
 *  Purpose  : Return variable name
 */
//============================================================================
QString NoteBook_TableRow::GetName() const
{
  return myVariableName->text();
}

//============================================================================
/*! Function : GetValue
 *  Purpose  : Return variable value
 */
//============================================================================
QString NoteBook_TableRow::GetValue() const
{
  return myVariableValue->text(); 
}

//============================================================================
/*! Function : CheckName
 *  Purpose  : Return true if variable name correct, otherwise return false
 */
//============================================================================
bool NoteBook_TableRow::CheckName()
{
  QString aName = GetName();
  int aPos = 0;
  QRegExpValidator aValidator( QRegExp("^([a-zA-Z]+)([a-zA-Z0-9_]*)$"), 0 );
  if( aName.isEmpty() || !aValidator.validate( aName, aPos ) )
    return false;
  return true;
}

//============================================================================
/*! Function : CheckValue
 *  Purpose  : Return true if variable value correct, otherwise return false
 */
//============================================================================
bool NoteBook_TableRow::CheckValue()
{
  bool aResult = false;
  QString aValue = GetValue();
  if(!aValue.isEmpty() && 
     (IsRealValue(aValue) ||
      IsIntegerValue(aValue) ||
      IsBooleanValue(aValue) ||
      IsValidStringValue(aValue)))
    aResult = true;
  
  return aResult;
}

//============================================================================
/*! Function : GetVariableItem
 *  Purpose  : 
 */
//============================================================================
QTableWidgetItem* NoteBook_TableRow::GetVariableItem()
{
  return myVariableValue;
}

//============================================================================
/*! Function : GetNameItem
 *  Purpose  : 
 */
//============================================================================
QTableWidgetItem* NoteBook_TableRow::GetNameItem()
{
  return myVariableName;
}

//============================================================================
/*! Function : GetHeaderItem
 *  Purpose  : 
 */
//============================================================================
QTableWidgetItem* NoteBook_TableRow::GetHeaderItem()
{
  return myRowHeader;
}

//============================================================================
/*! Function : IsRealValue
 *  Purpose  : Return true if theValue string is real value, otherwise return 
 *             false
 */
//============================================================================
bool NoteBook_TableRow::IsRealValue(const QString theValue, double* theResult)
{
  bool aResult = false;
  double aDResult = theValue.toDouble(&aResult);
  if(aResult && theResult)
    *theResult = aDResult;
  
  return aResult;
}

//============================================================================
/*! Function : IsBooleanValue
 *  Purpose  : Return true if theValue String is boolean value, otherwise return 
 *             false
 */
//============================================================================
bool NoteBook_TableRow::IsBooleanValue(const QString theValue, bool* theResult){
  bool aResult = false;
  bool aBResult; 
  if(theValue.compare("True") == 0) {
    aBResult = true;
    aResult = true;
  }
  else if(theValue.compare("False") == 0) {
    aBResult = false;
    aResult = true;
  }
  if(aResult && theResult)
    *theResult = aBResult;
  
  return aResult;
}

//============================================================================
/*! Function : IsIntegerValue
 *  Purpose  : Return true if theValue string is integer value, otherwise return 
 *             false
 */
//============================================================================
bool NoteBook_TableRow::IsIntegerValue(const QString theValue, int* theResult)
{
  bool aResult = false;
  int anIResult;
  anIResult = theValue.toInt(&aResult);

  if(aResult && theResult)
    *theResult = anIResult;  
  
  return aResult;
}

//============================================================================
/*! Function : IsValidStringValue
 *  Purpose  : Return true if theValue string is valid, otherwise return 
 *             false
 *             The string are always valid for the moment
 *             The whole notebook is verified on apply
 */
//============================================================================
bool NoteBook_TableRow::IsValidStringValue(const QString theValue)
{
  int aNumRows = myParentTable->myRows.count();
  if( aNumRows == 0 )
    return true;

  bool aLastRowIsEmpty = myParentTable->myRows[ aNumRows - 1 ]->GetName().isEmpty() &&
                         myParentTable->myRows[ aNumRows - 1 ]->GetValue().isEmpty();

  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  PyConsole_Console* pyConsole = app->pythonConsole();
  PyConsole_Interp* pyInterp = pyConsole->getInterp();
  PyLockWrapper aLock = pyInterp->GetLockWrapper();
  std::string command = "import salome_notebook ; ";
  command += "salome_notebook.checkThisNoteBook(";
  for( int i = 0, n = aLastRowIsEmpty ? aNumRows - 1 : aNumRows; i < n; i++ ) {
    command += myParentTable->myRows[i]->GetName().toStdString();
    command += "=\"";
    command += myParentTable->myRows[i]->GetValue().toStdString();
    command += "\", ";
  }
  command += ") ";

  //rnv: fix for bug 21947 WinTC5.1.4: Wrong error management of "Salome NoteBook"
  bool oldSuppressValue = pyConsole->isSuppressOutput();
  pyConsole->setIsSuppressOutput(true);	
  bool aResult = pyInterp->run(command.c_str());
  pyConsole->setIsSuppressOutput(oldSuppressValue);	
  return !aResult;
}

///////////////////////////////////////////////////////////////////////////
//                      NoteBook_Table class                             //
///////////////////////////////////////////////////////////////////////////
//============================================================================
/*! Function : NoteBook_Table
 *  Purpose  : Constructor
 */
//============================================================================
NoteBook_Table::NoteBook_Table(QWidget * parent)
  :QTableWidget(parent),
   isProcessItemChangedSignal(false),
   myIsModified(false)
{
  setColumnCount(2);
  setSelectionMode(QAbstractItemView::SingleSelection);
  
  //Add Headers Columns
  QFont aFont = QFont();
  aFont.setBold(true);
  aFont.setPointSize(10);
  
  //"Name" column
  QTableWidgetItem * aNameHeader = new QTableWidgetItem();
  aNameHeader->setText(tr("VARNAME_COLUMN"));
  aNameHeader->setFont(aFont);
  setHorizontalHeaderItem(0,aNameHeader);
  setColumnWidth ( 0, COLUMN_SIZE);

  //"Value" Column
  QTableWidgetItem * aValueHeader = new QTableWidgetItem();
  aValueHeader->setText(tr("VARVALUE_COLUMN"));
  aValueHeader->setFont(aFont);
  setHorizontalHeaderItem(1,aValueHeader);
  setColumnWidth ( 1, COLUMN_SIZE);
  setSortingEnabled(false);
  
  connect(this,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onItemChanged(QTableWidgetItem*)));
}

//============================================================================
/*! Function : ~NoteBook_Table
 *  Purpose  : Destructor
 */
//============================================================================
NoteBook_Table::~NoteBook_Table(){}

//============================================================================
/*! Function : getUniqueIndex
 *  Purpose  : Get a unique index for the new row
 */
//============================================================================
int NoteBook_Table::getUniqueIndex() const
{
  int anIndex = 0;
  if( !myRows.isEmpty() )
    if( NoteBook_TableRow* aRow = myRows.last() )
      anIndex = aRow->GetIndex();

  int aMaxRemovedRow = 0;
  for( QListIterator<int> anIter( myRemovedRows ); anIter.hasNext(); )
  {
    int aRemovedRow = anIter.next();
    aMaxRemovedRow = qMax( aRemovedRow, aMaxRemovedRow );
  }

  anIndex = qMax( anIndex, aMaxRemovedRow ) + 1;
  return anIndex;
}

//============================================================================
/*! Function : Init
 *  Purpose  : Add variables in the table from theStudy
 */
//============================================================================
void NoteBook_Table::Init(_PTR(Study) theStudy)
{
  isProcessItemChangedSignal = false;

  int aNumRows = myRows.count();
  if( aNumRows > 0 )
  {
    for( int i = 0; i < myRows.size(); i++ )
    {
      NoteBook_TableRow* aRow = myRows[ i ];
      if( aRow )
      {
        delete aRow;
        aRow = 0;
      }
    }
    myRows.clear();
  }
  setRowCount( 0 );

  myRemovedRows.clear();
  myVariableMapRef.clear();
  myVariableMap.clear();

  //Add all variables into the table
  std::vector<std::string> aVariables = theStudy->GetVariableNames();
  for(int iVar = 0; iVar < aVariables.size(); iVar++ ) {
    AddRow(QString(aVariables[iVar].c_str()),
           Variable2String(aVariables[iVar],theStudy));
  }

  //Add empty row
  AddEmptyRow();
  isProcessItemChangedSignal = true;

  ResetMaps();

  myStudy = theStudy;
}

//============================================================================
/*! Function : Variable2String
 *  Purpose  : Convert variable values to QString
 */
//============================================================================
QString NoteBook_Table::Variable2String(const std::string& theVarName,
                                        _PTR(Study) theStudy)
{
  QString aResult;
  if( theStudy->IsReal(theVarName) )
    aResult = QString::number(theStudy->GetReal(theVarName));
  else if( theStudy->IsInteger(theVarName) )
    aResult = QString::number(theStudy->GetInteger(theVarName));
  else if( theStudy->IsBoolean(theVarName) )
    aResult = theStudy->GetBoolean(theVarName) ? QString("True") : QString("False");
  else if( theStudy->IsString(theVarName) )
    aResult = theStudy->GetString(theVarName).c_str();
  
  return aResult;
}

//============================================================================
/*! Function : IsValid
 *  Purpose  : Check validity of the table data
 */
//============================================================================
bool NoteBook_Table::IsValid() const
{
  int aNumRows = myRows.count();
  if( aNumRows == 0 )
    return true;

  bool aLastRowIsEmpty = myRows[ aNumRows - 1 ]->GetName().isEmpty() &&
                         myRows[ aNumRows - 1 ]->GetValue().isEmpty();

  for( int i = 0, n = aLastRowIsEmpty ? aNumRows - 1 : aNumRows; i < n; i++ )
    if( !myRows[i]->CheckName() || !IsUniqueName( myRows[i] ) || !myRows[i]->CheckValue() )
      return false;

  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  PyConsole_Console* pyConsole = app->pythonConsole();
  PyConsole_Interp* pyInterp = pyConsole->getInterp();
  PyLockWrapper aLock = pyInterp->GetLockWrapper();
  std::string command = "import salome_notebook ; ";
  command += "salome_notebook.checkThisNoteBook(";
  for( int i = 0, n = aLastRowIsEmpty ? aNumRows - 1 : aNumRows; i < n; i++ )
    {
      command += myRows[i]->GetName().toStdString();
      command += "=\"";
      command += myRows[i]->GetValue().toStdString();
      command += "\",";
    }
  command += ")";

  //rnv: fix for bug 21947 WinTC5.1.4: Wrong error management of "Salome NoteBook"
  bool oldSuppressValue = pyConsole->isSuppressOutput();
  pyConsole->setIsSuppressOutput(true);	
  bool aResult = pyInterp->run(command.c_str());
  pyConsole->setIsSuppressOutput(oldSuppressValue);	

  return !aResult;
}

//============================================================================
/*! Function : RenamberRowItems
 *  Purpose  : renumber row items
 */
//============================================================================
void NoteBook_Table::RenamberRowItems() {
  for(int i=0; i<myRows.size();i++){
    myRows[i]->GetHeaderItem()->setText(QString::number(i+1));
  }
}

//============================================================================
/*! Function : AddRow
 *  Purpose  : Add a row into the table
 */
//============================================================================
void NoteBook_Table::AddRow(const QString& theName, const QString& theValue)
{
  int anIndex = getUniqueIndex();
  NoteBook_TableRow* aRow = new NoteBook_TableRow(anIndex, this, this);
  aRow->SetName(theName);
  aRow->SetValue(theValue);
  aRow->AddToTable(this);
  myRows.append(aRow);

  myVariableMap.insert( anIndex, NoteBoox_Variable( theName, theValue ) );
}

//============================================================================
/*! Function : AddEmptyRow
 *  Purpose  : Add an empty row into the end of the table
 */
//============================================================================
void NoteBook_Table::AddEmptyRow()
{
  isProcessItemChangedSignal = false;
  AddRow();
  isProcessItemChangedSignal = true;
}

//============================================================================
/*! Function : GetRowByItem
 *  Purpose  : 
 */
//============================================================================
NoteBook_TableRow* NoteBook_Table::GetRowByItem(const QTableWidgetItem* theItem) const
{
  int aCurrentRow = row(theItem);
  
  if( (myRows.size() <= aCurrentRow ) && (aCurrentRow < 0))
    return NULL;
  else
    return myRows.at(aCurrentRow);
}

//============================================================================
/*! Function : IsLastRow
 *  Purpose  : Return true if theRow is last row in the table
 */
//============================================================================
bool NoteBook_Table::IsLastRow(const NoteBook_TableRow* theRow) const
{
  return (myRows.last() == theRow);
}

//============================================================================
/*! Function : onItemChanged
 *  Purpose  : [slot] called then table item changed
 */
//============================================================================
void NoteBook_Table::onItemChanged(QTableWidgetItem* theItem)
{
  if(isProcessItemChangedSignal) {
    bool isModified = true;
    NoteBook_TableRow* aRow = GetRowByItem(theItem);
    if(aRow) {
      int aCurrentColumn = column(theItem);
      bool IsCorrect = true, IsVariableComplited = false;
      QString aMsg;

      if(aCurrentColumn == NAME_COLUMN) {
        int anIndex = aRow->GetIndex();
        if( myVariableMap.contains( anIndex ) )
        {
          const NoteBoox_Variable& aVariable = myVariableMap[ anIndex ];
          if( !aVariable.Name.isEmpty() && myStudy->IsVariableUsed( std::string( aVariable.Name.toLatin1().constData() ) ) )
          {
            if( QMessageBox::warning( parentWidget(), tr( "WARNING" ),
                                      tr( "RENAME_VARIABLE_IS_USED" ).arg( aVariable.Name ),
                                      QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
            {
              bool isBlocked = blockSignals( true );
              aRow->SetName( aVariable.Name );
              blockSignals( isBlocked );
              return;
            }
          }
        }
      }

      //Case then varible name changed. 
      if(aCurrentColumn == NAME_COLUMN) {
        if(!aRow->CheckName()) {
          IsCorrect = false;
          aMsg = tr( "VARNAME_INCORRECT" ).arg(aRow->GetName());
        }
        else if(!IsUniqueName(aRow)) {
          IsCorrect = false;
          aMsg = tr( "VARNAME_EXISTS" ).arg(aRow->GetName());
        }
        else
          IsVariableComplited = aRow->CheckValue();
      }
      
      //Case then varible value changed. 
      else if(aCurrentColumn == VALUE_COLUMN){
        if(!aRow->CheckValue()) {
          IsCorrect = false;
          aMsg = tr( "VARVALUE_INCORRECT" ).arg(aRow->GetName());
        }
        else
          IsVariableComplited = aRow->CheckName() && IsUniqueName(aRow);
      }

      if(!IsCorrect && !aMsg.isEmpty())
        SUIT_MessageBox::warning( parentWidget(), tr( "WARNING" ), aMsg );

      bool isBlocked = blockSignals( true );
      theItem->setForeground( QBrush( IsCorrect ? Qt::black : Qt::red ) );
      blockSignals( isBlocked );

      int anIndex = aRow->GetIndex();
      if( myVariableMap.contains( anIndex ) )
      {
        NoteBoox_Variable& aVariable = myVariableMap[ anIndex ];
        if( aVariable.Name.compare( aRow->GetName() ) != 0 ||
            aVariable.Value.compare( aRow->GetValue() ) != 0 )
        {
          aVariable.Name = aRow->GetName();
          aVariable.Value = aRow->GetValue();
        }
        else
          isModified = false;
      }

      if(IsCorrect && IsVariableComplited && IsLastRow(aRow))
        AddEmptyRow();
    }

    if( !myIsModified )
      myIsModified = isModified;
  }
}

//============================================================================
/*! Function : IsUniqueName
 *  Purpose  : Return true if theName is unique name of the Variable
 */
//============================================================================
bool NoteBook_Table::IsUniqueName(const NoteBook_TableRow* theRow) const
{
  for(int i=0; i<myRows.size();i++) {
    if(myRows[i] == theRow ) 
      continue;
    if(myRows[i]->GetName().compare(theRow->GetName()) == 0)
      return false;
  }
  return true;
}

//============================================================================
/*! Function : RemoveSelected
 *  Purpose  : Remove selected rows in the table
 */
//============================================================================
void NoteBook_Table::RemoveSelected()
{
  isProcessItemChangedSignal = false;
  QList<QTableWidgetItem*> aSelectedItems = selectedItems();
  if( !(aSelectedItems.size() > 0)) {
    isProcessItemChangedSignal = true;
    return;
  }
  bool removedFromStudy = false;
  for(int i=0; i < aSelectedItems.size(); i++ ) {
    NoteBook_TableRow* aRow = GetRowByItem(aSelectedItems[i]);
    if(aRow) {
      if(IsLastRow(aRow)) {
        aRow->SetName(QString());
        aRow->SetValue(QString());
      }
      else {
        int nRow = row(aSelectedItems[i]);

        if( myStudy->IsVariableUsed( std::string( aRow->GetName().toLatin1().constData() ) ) )
        {
          if( QMessageBox::warning( parentWidget(), tr( "WARNING" ),
                                    tr( "REMOVE_VARIABLE_IS_USED" ).arg( aRow->GetName() ),
                                    QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
          {
            isProcessItemChangedSignal = true;
            return;
          }
        }

        int index = aRow->GetIndex();
        QString aVarName = aRow->GetName();
        myRemovedRows.append( index );
        if( myVariableMap.contains( index ) )
          myVariableMap.remove( index );
        removeRow(nRow);
        myRows.removeAt(nRow);
        if(myStudy->IsVariable(aVarName.toLatin1().constData()))
          removedFromStudy = true;
      }
    }
  }
  if(removedFromStudy)
    myIsModified = true;
  RenamberRowItems();
  isProcessItemChangedSignal = true;
}

//============================================================================
/*! Function : SetProcessItemChangedSignalFlag
 *  Purpose  : 
 */
//============================================================================
void NoteBook_Table::SetProcessItemChangedSignalFlag(const bool enable)
{
  isProcessItemChangedSignal = enable;
}

//============================================================================
/*! Function : GetProcessItemChangedSignalFlag
 *  Purpose  : 
 */
//============================================================================
bool NoteBook_Table::GetProcessItemChangedSignalFlag() const
{
  return isProcessItemChangedSignal;
}

//============================================================================
/*! Function : GetRows
 *  Purpose  : 
 */
//============================================================================
QList<NoteBook_TableRow*> NoteBook_Table::GetRows() const
{
  return myRows;
}

//============================================================================
/*! Function : ResetMaps
 *  Purpose  : Reset variable maps
 */
//============================================================================
void NoteBook_Table::ResetMaps()
{
  myIsModified = false;
  myVariableMapRef = myVariableMap;
  myRemovedRows.clear();
}

///////////////////////////////////////////////////////////////////////////
//                  SalomeApp_NoteBookDlg class                          //
///////////////////////////////////////////////////////////////////////////
//============================================================================
/*! Function : SalomeApp_NoteBookDlg
 *  Purpose  : Constructor
 */
//============================================================================
SalomeApp_NoteBookDlg::SalomeApp_NoteBookDlg(QWidget * parent, _PTR(Study) theStudy):
  QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
  myStudy(theStudy)
{
  setModal(false);
  setObjectName("SalomeApp_NoteBookDlg");
  setWindowTitle(tr("NOTEBOOK_TITLE"));
  QGridLayout* aLayout = new QGridLayout(this);
  aLayout->setMargin(DEFAULT_MARGIN);
  aLayout->setSpacing(DEFAULT_SPACING);

  //Table
  myTable = new NoteBook_Table(this);
  aLayout->addWidget(myTable, 0, 0, 1, 3);
  
  //Buttons
  myRemoveButton = new QPushButton(tr("BUT_REMOVE"));
  aLayout->addWidget(myRemoveButton, 1, 0, 1, 1);

  QSpacerItem* spacer =
    new QSpacerItem(DEFAULT_SPACING, 5 , QSizePolicy::Expanding, QSizePolicy::Minimum);
  aLayout->addItem(spacer, 1, 1, 2, 1);

  myUpdateStudyBtn = new QPushButton(tr("BUT_UPDATE_STUDY"));
  aLayout->addWidget(myUpdateStudyBtn, 1, 2, 1, 1);
  
  QGroupBox* groupBox = new QGroupBox(this);

  QGridLayout* aLayout1 = new QGridLayout(groupBox);

  aLayout1->setMargin(DEFAULT_MARGIN);
  aLayout1->setSpacing(DEFAULT_SPACING);

  myOkBtn = new QPushButton(tr("BUT_APPLY_AND_CLOSE"));
  aLayout1->addWidget(myOkBtn, 0, 0, 1, 1);
  
  myApplyBtn = new QPushButton(tr("BUT_APPLY"));
  aLayout1->addWidget(myApplyBtn, 0, 1, 1, 1);  

  QSpacerItem* spacer1 =
    new QSpacerItem(DEFAULT_SPACING, 5, QSizePolicy::Expanding, QSizePolicy::Minimum);
  aLayout1->addItem(spacer1, 0, 2, 1, 1);

  myCancelBtn = new QPushButton(tr("BUT_CLOSE"));
  aLayout1->addWidget(myCancelBtn, 0, 3, 1, 1);

  myHelpBtn = new QPushButton(tr("BUT_HELP"));
  aLayout1->addWidget(myHelpBtn, 0, 4, 1, 1);
  
  aLayout->addWidget(groupBox, 2, 0, 1, 3);

  QWidgetList aWidgetList;
  aWidgetList.append( myTable );
  aWidgetList.append( myOkBtn );
  aWidgetList.append( myApplyBtn );
  aWidgetList.append( myCancelBtn );
  aWidgetList.append( myHelpBtn );
  aWidgetList.append( myUpdateStudyBtn );
  aWidgetList.append( myRemoveButton );
  Qtx::setTabOrder( aWidgetList );

  connect( myOkBtn, SIGNAL(clicked()), this, SLOT(onOK()) );
  connect( myApplyBtn, SIGNAL(clicked()), this, SLOT(onApply()) );
  connect( myCancelBtn, SIGNAL(clicked()), this, SLOT(onCancel()) );
  connect( myUpdateStudyBtn, SIGNAL(clicked()), this, SLOT(onUpdateStudy()) );
  connect( myRemoveButton, SIGNAL(clicked()), this, SLOT(onRemove()));
  connect( myHelpBtn, SIGNAL(clicked()), this, SLOT(onHelp()));
  
  myTable->Init(myStudy);
}

//============================================================================
/*! Function : ~SalomeApp_NoteBookDlg
 *  Purpose  : Destructor
 */
//============================================================================
SalomeApp_NoteBookDlg::~SalomeApp_NoteBookDlg(){}


//============================================================================
/*! Function : Init()
 *  Purpose  : init variable table
 */
//============================================================================
void SalomeApp_NoteBookDlg::Init(_PTR(Study) theStudy){
  if(myStudy!= theStudy)
    myStudy = theStudy;
  myTable->Init(myStudy);
}


//============================================================================
/*! Function : onOK
 *  Purpose  : [slot]
 */
//============================================================================
void SalomeApp_NoteBookDlg::onOK()
{
  onApply();
  if( myTable->IsValid() )
    accept();
}

//============================================================================
/*! Function : onHelp
 *  Purpose  : [slot]
 */
//============================================================================
void SalomeApp_NoteBookDlg::onHelp()
{
  QString aHelpFileName("using_notebook.html");
  LightApp_Application* app = (LightApp_Application*)(SUIT_Session::session()->activeApplication());
  if (app)
    app->onHelpContextModule("GUI",aHelpFileName);
  else {
    QString platform;
#ifdef WIN32
    platform = "winapplication";
#else
    platform = "application";
#endif
    SUIT_MessageBox::warning(this, tr("WRN_WARNING"),
                             tr("EXTERNAL_BROWSER_CANNOT_SHOW_PAGE").
                             arg(app->resourceMgr()->stringValue("ExternalBrowser",
                                                                 platform)).
                             arg(aHelpFileName));
  }

}
//============================================================================
/*! Function : onApply
 *  Purpose  : [slot]
 */
//============================================================================
void SalomeApp_NoteBookDlg::onApply()
{
  if( !myTable->IsValid() )
  {
    SUIT_MessageBox::warning( this, tr( "WARNING" ), tr( "INCORRECT_DATA" ) );
    return;
  }

  double aDVal;
  int    anIVal;
  bool   aBVal;

  const QList<int>& aRemovedRows = myTable->GetRemovedRows();
  const VariableMap& aVariableMap = myTable->GetVariableMap();
  const VariableMap& aVariableMapRef = myTable->GetVariableMapRef();

  for( QListIterator<int> anIter( aRemovedRows ); anIter.hasNext(); )
  {
    int anIndex = anIter.next();
    if( aVariableMapRef.contains( anIndex ) )
    {
      QString aRemovedVariable = aVariableMapRef[ anIndex ].Name;
      myStudy->RemoveVariable( std::string( aRemovedVariable.toLatin1().constData() ) );
    }
  }

  VariableMap::const_iterator it = aVariableMap.constBegin(), itEnd = aVariableMap.constEnd();
  for( ; it != itEnd; ++it )
  {
    int anIndex = it.key();
    const NoteBoox_Variable& aVariable = it.value();
    QString aName = aVariable.Name;
    QString aValue = aVariable.Value;

    if( !aName.isEmpty() && !aValue.isEmpty() )
    {
      if( aVariableMapRef.contains( anIndex ) )
      {
        const NoteBoox_Variable& aVariableRef = aVariableMapRef[ anIndex ];
        QString aNameRef = aVariableRef.Name;
        QString aValueRef = aVariableRef.Value;

        if( !aNameRef.isEmpty() && !aValueRef.isEmpty() && aNameRef != aName )
        {
          myStudy->RenameVariable( std::string( aNameRef.toLatin1().constData() ),
                                   std::string( aName.toLatin1().constData() ) );
        }
      }

      if( NoteBook_TableRow::IsIntegerValue(aValue,&anIVal) )
        myStudy->SetInteger(std::string(aName.toLatin1().constData()),anIVal);

      else if( NoteBook_TableRow::IsRealValue(aValue,&aDVal) )
        myStudy->SetReal(std::string(aName.toLatin1().constData()),aDVal);
    
      else if( NoteBook_TableRow::IsBooleanValue(aValue,&aBVal) )
        myStudy->SetBoolean(std::string(aName.toLatin1().constData()),aBVal);
    
      else
        myStudy->SetString(std::string(aName.toLatin1().constData()),aValue.toStdString());
    }
  }
  myTable->ResetMaps();

  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if(app)
    app->updateActions();

}

//============================================================================
/*! Function : onCancel
 *  Purpose  : [slot]
 */
//============================================================================
void SalomeApp_NoteBookDlg::onCancel()
{
  if( myTable->IsModified() )
  {
    int answer = QMessageBox::question( this, tr( "CLOSE_CAPTION" ), tr( "CLOSE_DESCRIPTION" ),
                                        QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel );
    switch( answer )
    {
      case QMessageBox::Yes    : onOK(); return;
      case QMessageBox::No     : break;
      case QMessageBox::Cancel : return;
      default : break;
    }
  }
  reject();
}

//============================================================================
/*! Function : onRemove
 *  Purpose  : [slot]
 */
//============================================================================
void SalomeApp_NoteBookDlg::onRemove()
{
  myTable->RemoveSelected();
}

//============================================================================
/*! Function : onUpdateStudy
 *  Purpose  : [slot]
 */
//============================================================================
void SalomeApp_NoteBookDlg::onUpdateStudy()
{
  onApply();
  if( !myTable->IsValid() )
    return;

  QApplication::setOverrideCursor( Qt::WaitCursor );

  if( !updateStudy() )
    SUIT_MessageBox::warning( this, tr( "ERROR" ), tr( "ERR_UPDATE_STUDY_FAILED" ) );
    
  QApplication::restoreOverrideCursor();
}

//============================================================================
/*! Function : updateStudy
 *  Purpose  : 
 */
//============================================================================
bool SalomeApp_NoteBookDlg::updateStudy()
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( !app )
    return false;

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if( !study )
    return false;

  bool isStudySaved = study->isSaved();
  QString aStudyName = study->studyName();

  _PTR(Study) studyDS = study->studyDS();

  // get unique temporary directory name
  QString aTmpDir = QString::fromStdString( SALOMEDS_Tool::GetTmpDir() );
  if( aTmpDir.isEmpty() )
    return false;

  if( aTmpDir.right( 1 ).compare( QDir::separator() ) == 0 )
    aTmpDir.remove( aTmpDir.length() - 1, 1 );

  // dump study to the temporary directory
  QString aFileName( "notebook" );
  bool toPublish = true;
  bool isMultiFile = false;
  bool toSaveGUI = true;

  int savePoint;
  _PTR(AttributeParameter) ap;
  _PTR(IParameters) ip = ClientFactory::getIParameters(ap);
  if(ip->isDumpPython(studyDS)) ip->setDumpPython(studyDS); //Unset DumpPython flag.
  if ( toSaveGUI ) { //SRN: Store a visual state of the study at the save point for DumpStudy method
    ip->setDumpPython(studyDS);
    savePoint = SalomeApp_VisualState( app ).storeState(); //SRN: create a temporary save point
  }
  bool ok = studyDS->DumpStudy( aTmpDir.toStdString(), aFileName.toStdString(), toPublish, isMultiFile );
  if ( toSaveGUI )
    study->removeSavePoint(savePoint); //SRN: remove the created temporary save point.

  if( !ok )
    return false;

  // clear a study (delete all objects)
  clearStudy();

  // get active application
  app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );

  // load study from the temporary directory
  QString command = QString( "execfile(r\"%1\")" ).arg( aTmpDir + QDir::separator() + aFileName + ".py" );

  PyConsole_Console* pyConsole = app->pythonConsole();
  if ( pyConsole )
    pyConsole->execAndWait( command );

  // remove temporary directory
  QDir aDir( aTmpDir );
  QStringList aFiles = aDir.entryList( QStringList( "*.py*" ) );
  for( QStringList::iterator it = aFiles.begin(), itEnd = aFiles.end(); it != itEnd; ++it )
    ok = aDir.remove( *it ) && ok;
  if( ok )
    ok = aDir.rmdir( aTmpDir );

  if( SalomeApp_Study* newStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) )
  {
    myStudy = newStudy->studyDS();
    myTable->Init(myStudy);
    if(isStudySaved) {
      newStudy->markAsSavedIn(aStudyName);
    }
  }
  else
    ok = false;

  return ok;
}

//============================================================================
/*! Function : clearStudy
 *  Purpose  : 
 */
//============================================================================
void SalomeApp_NoteBookDlg::clearStudy()
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( !app )
    return;

  QList<SUIT_Application*> aList = SUIT_Session::session()->applications();
  int anIndex = aList.indexOf( app );

  //Store position and size of the this dialog
  int aW = width();
  int aH = height();
  int aX = x();
  int aY = y();

  // Disconnect dialog from application desktop in case if:
  // 1) Application is not the first application in the session 
  // 2) Application is the first application in session but not the only.
  bool changeDesktop = ((anIndex > 0) || (anIndex == 0 && aList.count() > 1));

  if( changeDesktop )
    setParent( 0 );

  app->onCloseDoc( false );
  
  if( anIndex > 0 && anIndex < aList.count() )
    app = dynamic_cast<SalomeApp_Application*>( aList[ anIndex - 1 ] );
  else if(anIndex == 0 && aList.count() > 1)
    app = dynamic_cast<SalomeApp_Application*>( aList[ 1 ] );

  if( !app )
    return;

  app->onNewDoc();

  app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( changeDesktop && app ) {
    setParent( app->desktop(), Qt::Dialog );
    app->setNoteBook(this);
  }
  //Set position and size of the this dialog
  resize( aW, aH );
  move( aX, aY );
  show();
}
