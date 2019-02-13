// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

#ifndef __QREMOTECOPYWIDGET__
#define __QREMOTECOPYWIDGET__

#include "RemoteFileBrowser.h"

#include "QDialog"
#include "QMutex"
#include "QThread"
#include "QTreeView"
#include "QTableView"
#include "QItemDelegate"
#include "QPointer"
#include "QProcess"

class QRemoteFileSystemModel;
class QFilesDirsCopierModel;
class DataStructure;
class QTableView;

void PerformCopy(QWidget *parent, QRemoteFileSystemModel *srcModel, const QModelIndexList& srcSelectedFiles, DataStructure *ds);

class QREMOTEFILEBROWSER_EXPORT CopierThread : public QThread
{
public:
  CopierThread(QObject *parent, QFilesDirsCopierModel *model):QThread(parent),_model(model) { }
  void stopRequested();
protected:
  void run();
private:
  QFilesDirsCopierModel *_model;
};

class QREMOTEFILEBROWSER_EXPORT QFilesDirsCopierModel : public QAbstractListModel
{
  Q_OBJECT
public:
  QFilesDirsCopierModel(QObject *parent, const QList<const DataStructure *>& srcFiles, DataStructure *destLoc);
  int nbOfRows() const;
  int rowCount(const QModelIndex&) const;
  int columnCount(const QModelIndex&) const;
  QVariant data(const QModelIndex&, int) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  const QString& getErrorStr() const { return _error; }
  int getProgressOf(int srcFileId) const;
  QString getFullNameOf(int srcFileId) const;
  QString getNameOf(int srcFileId) const;
  QString getPrettyText(int srcFileId) const;
  QSize sizeHint() const;
  //
  void launchCopy();
  void stopCurrentCopy();
public slots:
  void newOutputAvailable();
private:
  void fillArgsForRSync(const DataStructure *srcFile, QStringList& args) const;
private:
  QList<const DataStructure *> _srcFiles;
  //
  mutable QMutex _mutOnProc;
  volatile int _currentElt;
  QPointer<QProcess> _curProc;
  QString _error;
  //
  QVector<int> _progress;
  DataStructure *_destLoc;
public:
  static constexpr int PROGRESS_STATUS_START=-1;
  static constexpr int PROGRESS_STATUS_OVER=101;
  static const char ATOMIC_STOP_MSG[];
};

class QREMOTEFILEBROWSER_EXPORT ProgressDelegate : public QItemDelegate
{
public:
  ProgressDelegate(QObject *parent, QFilesDirsCopierModel *model):QItemDelegate(parent),_model(model) { }
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
  QFilesDirsCopierModel *_model;
};

class QREMOTEFILEBROWSER_EXPORT CopierTableView : public QTableView
{
public:
  CopierTableView(QWidget *parent);
  int sizeHintForColumn(int column) const;
  int sizeHintForRow(int row) const;
  void resizeEvent(QResizeEvent *event);
  QSize sizeHint() const;
};
   
class QREMOTEFILEBROWSER_EXPORT FilesDirsCopier : public QDialog
{
  Q_OBJECT
public:
  FilesDirsCopier(QWidget *parent, const QList<const DataStructure *>& srcFiles, DataStructure *destLoc);
public slots:
  void cancelRequested();
  void myAccept(bool);
  int exec();
  const QString& getErrorStr() const { return _model->getErrorStr(); }
signals:
  void myAcceptSignal(bool);
private:
  CopierTableView *_table;
  QPushButton *_cancel;
  CopierThread *_th;
  QFilesDirsCopierModel *_model;
};

#endif
