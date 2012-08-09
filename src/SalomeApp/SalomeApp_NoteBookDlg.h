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

// File:    SalomeApp_NoteBookDlg.h
// Author : Roman NIKOLAEV, Open CASCADE S.A.S.
// Module : GUI
//
#ifndef SALOMEAPP_NOTEBOOKDLG_H
#define SALOMEAPP_NOTEBOOKDLG_H

#include "SalomeApp.h"

#include "SALOMEDSClient_ClientFactory.hxx" 
#include CORBA_SERVER_HEADER(SALOMEDS)

#include <QDialog>
#include <QTableWidget>
#include <QList>

class QWidget;
class QPushButton;
class QTableWidgetItem;
class NoteBook_Table;

struct NoteBoox_Variable
{
  NoteBoox_Variable() {}
  NoteBoox_Variable( const QString& theName, const QString& theValue )
  {
    Name = theName;
    Value = theValue;
  }
  QString Name;
  QString Value;
};

typedef QMap< int, NoteBoox_Variable > VariableMap;

class SALOMEAPP_EXPORT NoteBook_TableRow : public QWidget
{
 public:
  NoteBook_TableRow(int, NoteBook_Table* parentTable, QWidget* parent=0 );
  virtual ~NoteBook_TableRow();

  int  GetIndex() const { return myIndex; }
  
  void AddToTable(QTableWidget *theTable);
  
  void SetName(const QString theName);
  void SetValue(const QString theValue);

  QString GetValue() const;
  QString GetName() const;

  bool CheckName();
  bool CheckValue();

  QTableWidgetItem* GetVariableItem();
  QTableWidgetItem* GetNameItem();
  QTableWidgetItem* GetHeaderItem();

  static bool IsRealValue(const QString theValue, double* theResult = 0);
  static bool IsIntegerValue(const QString theValue, int* theResult = 0);
  static bool IsBooleanValue(const QString theValue, bool* theResult = 0);
  bool IsValidStringValue(const QString theName);
  
 private:
  int               myIndex;
  NoteBook_Table*   myParentTable;
  QTableWidgetItem* myRowHeader;
  QTableWidgetItem* myVariableName;
  QTableWidgetItem* myVariableValue;
};

class SALOMEAPP_EXPORT NoteBook_Table : public QTableWidget
{
  Q_OBJECT
 public:
  NoteBook_Table(QWidget * parent = 0);
  virtual ~NoteBook_Table();

  void Init(_PTR(Study) theStudy);
  static QString Variable2String(const std::string& theVarName,
                                 _PTR(Study) theStudy);

  bool IsValid() const;

  void AddRow( const QString& theName = QString::null, const QString& theValue = QString::null );
  void AddEmptyRow();
  NoteBook_TableRow* GetRowByItem(const QTableWidgetItem* theItem) const;
  bool IsLastRow(const NoteBook_TableRow* aRow) const;

  void RemoveSelected();

  void SetProcessItemChangedSignalFlag(const bool enable);
  bool GetProcessItemChangedSignalFlag()const;

  bool IsUniqueName(const NoteBook_TableRow* theRow) const;
  QList<NoteBook_TableRow*> GetRows() const;

  const bool         IsModified() const { return myIsModified; }
  const QList<int>&  GetRemovedRows() const { return myRemovedRows; }
  const VariableMap& GetVariableMap() const { return myVariableMap; }
  const VariableMap& GetVariableMapRef() const { return myVariableMapRef; }
  void  RenamberRowItems();

  void ResetMaps();

  QList<NoteBook_TableRow*>          myRows;

  public slots:
    void onItemChanged(QTableWidgetItem* theItem);

 private:
    int  getUniqueIndex() const;
    
 private:
  bool isProcessItemChangedSignal;

  bool        myIsModified;
  QList<int>  myRemovedRows;
  VariableMap myVariableMapRef;
  VariableMap myVariableMap;

  _PTR(Study)      myStudy;
};

class SALOMEAPP_EXPORT SalomeApp_NoteBookDlg : public QDialog 
{
  Q_OBJECT
 public:
  SalomeApp_NoteBookDlg(QWidget * parent , _PTR(Study) theStudy);
  virtual ~SalomeApp_NoteBookDlg();

  void Init(_PTR(Study) theStudy);
  
 public slots:
   void onOK();
   void onApply();
   void onCancel();
   void onRemove();
   void onUpdateStudy();
   void onHelp();

 protected:
   bool updateStudy();
   void clearStudy();

 private:
  NoteBook_Table*  myTable;
  QPushButton*     myRemoveButton;
  QPushButton*     myUpdateStudyBtn;
  QPushButton*     myOkBtn;
  QPushButton*     myApplyBtn;
  QPushButton*     myCancelBtn;
  QPushButton*     myHelpBtn;
  
  _PTR(Study)      myStudy;
};

#endif //SALOMEAPP_NOTEBOOKDLG_H
