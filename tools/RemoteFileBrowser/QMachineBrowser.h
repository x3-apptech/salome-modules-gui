// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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
// Author : Anthony GEAY (EDF R&D)

#ifndef __QMACHINEBROWSER__
#define __QMACHINEBROWSER__

#include "RemoteFileBrowser.h"

#include <QWidget>
#include <QComboBox>

class QPushButton;
class QLineEdit;

class QREMOTEFILEBROWSER_EXPORT QMachineSelector : public QComboBox
{
  Q_OBJECT
public:
  QMachineSelector(QWidget *parent);
  void initLocation();
public slots:
  void appendEntry(const QString& entry);
private:
  void fillMachines();
  void fillMachinesFromCatalog();
  void fillMachinesFromSettings();
  void assignToLocalhost();
};

class QREMOTEFILEBROWSER_EXPORT QMachineManager : public QWidget
{
  Q_OBJECT
public:
  QMachineManager(QWidget *parent);
  void initLocation();
  QString getSelectedHost() const;
public slots:
  void newEntryRequested();
private:
  QPushButton *_pb;
  QMachineSelector *_ms;
};

class QRemoteFileSystemModel;
class FileLoader;

class QREMOTEFILEBROWSER_EXPORT QMachineBrowser : public QWidget
{
  Q_OBJECT
public:
  QMachineBrowser(QWidget *parent=NULL);
  void initLocation();
  QRemoteFileSystemModel *generateModel();
  FileLoader *generateFileLoader();
signals:
  void locationChanged();
private:
  QMachineManager *_msel;
  QLineEdit *_le;
};

#endif
