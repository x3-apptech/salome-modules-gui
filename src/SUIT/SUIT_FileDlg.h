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
#ifndef SUIT_FILEDIALOG_H
#define SUIT_FILEDIALOG_H

#include "SUIT.h"

#include <qfiledialog.h>

class QLabel;
class QComboBox;
class QPushButton;
class SUIT_FileValidator;

/*! \class QFileDialog
 *  For more information see <a href="http://doc.trolltech.com">QT documentation</a>.
*/
class SUIT_EXPORT SUIT_FileDlg : public QFileDialog
{
  Q_OBJECT

public:
  SUIT_FileDlg( QWidget*, bool open, bool showQuickDir = true, bool modal = true );
  virtual ~SUIT_FileDlg();

public:    
  bool                isOpenDlg()    const;    
  QString             selectedFile() const;

  void                setValidator( SUIT_FileValidator* );

  QString             dirPath() const; // QFileDialog::dirPath() has a bug on Linux Debian (1 level up from correct 
                                       // directory is returned).  This redefinition fixes the bug.  

  static QString      getFileName( QWidget* parent, const QString& initial, const QStringList& filters, 
				   const QString& caption, const bool open, const bool showQuickDir = true,
                                   SUIT_FileValidator* validator = 0 );
  static QStringList  getOpenFileNames( QWidget* parent, const QString& initial, const QStringList& filters, 
				        const QString& caption, bool showQuickDir = true, 
				        SUIT_FileValidator* validator = 0 );
  static QString      getExistingDirectory( QWidget* parent, const QString& initial,
                                            const QString& caption, const bool showQuickDir = true );

public slots:
  void                polish();

private:
  bool                acceptData();
  void                addExtension();
  bool                processPath( const QString& path );

protected slots:
  void                accept();        
  void                reject(); 
  void                quickDir( const QString& );
  void                addQuickDir();

protected:
  bool                myOpen;             //!< open/save selector
  QString             mySelectedFile;     //!< selected filename
  SUIT_FileValidator* myValidator;        //!< file validator
  QLabel*             myQuickLab;         //!< quick dir combo box
  QComboBox*          myQuickCombo;       //!< quick dir combo box
  QPushButton*        myQuickButton;      //!< quick dir add button
  
  /*! \var myAccepted
   * \brief flag is used to warkaround the Qt 2.2.2
   * \bug accept() method is called twice if user presses 'Enter' key 
   * in file name editor while file name is not acceptable by acceptData()
   * (e.g. permission denied)
   */
//  bool                myAccepted;
  /*! ASL: this bug can be fixed with help of call setDefault( false ) 
   *       and setAutoDefault( false ) methods for all QPushButtons of this dialog
   */

  static QString      myLastVisitedPath;  //!< last visited path
};

#endif
