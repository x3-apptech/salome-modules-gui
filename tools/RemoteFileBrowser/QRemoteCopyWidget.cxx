// Copyright (C) 2017  CEA/DEN, EDF R&D
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

#include "QRemoteCopyWidget"
#include "QRemoteFileBrowser"
#include "QVBoxLayout"
#include "QPushButton"
#include "QHeaderView"
#include "QResizeEvent"
#include "QProcess"
#include "QPainter"
#include "QPaintDevice"
#include "QMetaType"
#include "QApplication"
#include "QMessageBox"

#include <iostream>

const char QFilesDirsCopierModel::ATOMIC_STOP_MSG[]="Clean interruption";

void FilterEntries(const TopDirDataStructure *tpds, QList<const DataStructure *>& listToFilter)
{
  for(QList<const DataStructure *>::iterator it(listToFilter.begin());it!=listToFilter.end();it++)
    {
      const DataStructure *elt(*it);
      std::vector<const DataStructure *> toKill(elt->getItermediateElts(tpds));
      for(QList<const DataStructure *>::iterator it2(listToFilter.begin());it2!=listToFilter.end();)
        {
          if(it==it2)
            {
              it2++;
              continue;
            }
          if(std::find(toKill.begin(),toKill.end(),*it2)!=toKill.end())
            it2=listToFilter.erase(it2);
          else
            it2++;
        }
    }
}

void PerformCopy(QWidget *parent, QRemoteFileSystemModel *srcModel, const QModelIndexList& srcSelectedFiles, DataStructure *ds)
{
  QStringList sl;
  if(srcSelectedFiles.isEmpty())
    return ;
  // Filtering
  const TopDirDataStructure *tpdsSrc(nullptr);
  QList<const DataStructure *> listOfEntries;
  foreach(const QModelIndex& srcItem, srcSelectedFiles)
    {
      quintptr pt(srcItem.internalId());
      const DataStructure *srcDS(reinterpret_cast<const DataStructure *>(pt));
      if(!srcDS)
        continue;
      if(!tpdsSrc)
        tpdsSrc=srcDS->getRoot();
      listOfEntries.push_back(srcDS);
    }
  FilterEntries(tpdsSrc,listOfEntries);
  // End filtering
  FilesDirsCopier fdc(parent,listOfEntries,ds);
  int res(fdc.exec());
  if(res!=QDialog::Accepted)
    {
      QMessageBox mb(QMessageBox::Warning,"Copy status",fdc.getErrorStr());
      mb.exec();
    }
  
}

void CopierThread::run()
{
  _model->launchCopy();
  FilesDirsCopier *par(qobject_cast<FilesDirsCopier *>(parent()));
  if(!par)
    return ;
  emit par->myAcceptSignal(_model->getErrorStr().isEmpty());//emit signal to notify main thread. Not direct invocation because executed in different thread
}

void CopierThread::stopRequested()
{
  _model->stopCurrentCopy();
  requestInterruption();
}

QFilesDirsCopierModel::QFilesDirsCopierModel(QObject *parent2, const QList<const DataStructure *>& srcFiles, DataStructure *destLoc):QAbstractListModel(parent2),_srcFiles(srcFiles),_currentElt(0),_curProc(nullptr),_destLoc(destLoc),_progress(_srcFiles.size(),0)
{
  QTableView *par(qobject_cast<QTableView *>(parent()));
  par->resizeColumnToContents(0);
  qRegisterMetaType<QVector<int>>();//to be able to send 3th arg on dataChanged signal
}

int QFilesDirsCopierModel::nbOfRows() const
{
  return _srcFiles.size();
}

int QFilesDirsCopierModel::rowCount(const QModelIndex&) const
{
  return nbOfRows();
}

int QFilesDirsCopierModel::columnCount(const QModelIndex&) const
{
  return 1;
}

QVariant QFilesDirsCopierModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
    return QVariant();
  if(role==Qt::DisplayRole || role==Qt::EditRole)
    {
      return _srcFiles[index.row()]->fullName();
    }
  return QVariant();
}

QVariant QFilesDirsCopierModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role==Qt::DisplayRole || role==Qt::EditRole)
    {
      if(orientation==Qt::Horizontal)
        {
          return QString("Files to be copied");
        }
    }
  return QAbstractListModel::headerData(section,orientation,role);
}

int QFilesDirsCopierModel::getProgressOf(int srcFileId) const
{
  QMutexLocker locker(&_mutOnProc);
  return _progress[srcFileId];
}

QString QFilesDirsCopierModel::getFullNameOf(int srcFileId) const
{
  return _srcFiles[srcFileId]->fullName();
}

QString QFilesDirsCopierModel::getNameOf(int srcFileId) const
{
  return _srcFiles[srcFileId]->name();
}

QString QFilesDirsCopierModel::getPrettyText(int srcFileId) const
{
  int progress(getProgressOf(srcFileId));
  QString name(getNameOf(srcFileId));
  switch(progress)
    {
    case PROGRESS_STATUS_START:
      return QString("Copy of %1 has started").arg(name);
    case PROGRESS_STATUS_OVER:
      return QString("Copy of %1 has finished").arg(name);
    default:
      return QString("%1 (%2%)").arg(name).arg(progress);
    }
}

QSize QFilesDirsCopierModel::sizeHint() const
{
  int w(0),h(0);
  for(int zePos=0;zePos<_srcFiles.size();zePos++)
    {
      int progress(getProgressOf(zePos));
      QString txt(getPrettyText(zePos));
      QFont ft;
      QFontMetrics fm(ft);
      QSize sz(fm.boundingRect(txt).size());
      w=std::max(sz.width(),w);
      h+=sz.height();
    }
  return QSize(2*w,2*h);
}

void QFilesDirsCopierModel::launchCopy()
{
  foreach(const DataStructure *srcFile,_srcFiles)
    {
      {
        QMutexLocker locker(&_mutOnProc);
        _progress[_currentElt]=PROGRESS_STATUS_START;
        QModelIndex ind(this->index(_currentElt,0));
        emit this->dataChanged(ind,ind,QVector<int>());
      }
      if(QThread::currentThread()->isInterruptionRequested())
        {
          _error=QString("%1 just before %2 (%3/%4)").arg(ATOMIC_STOP_MSG).arg(srcFile->fullName()).arg(_currentElt).arg(_srcFiles.size());
          return ;
        }
      {
        QMutexLocker locker(&_mutOnProc);
        _curProc=new QProcess;
        QStringList args;
        args << "--progress" << "-r";
        fillArgsForRSync(srcFile,args);
        _curProc->start("rsync",args);
        connect(_curProc,SIGNAL(readyReadStandardOutput()),this,SLOT(newOutputAvailable()));
      }
      bool s1(_curProc->waitForFinished(-1));
      bool s2(_curProc->exitStatus()==QProcess::NormalExit && _curProc->exitCode()==0);
      if(s1 && s2)
        {
          QMutexLocker locker(&_mutOnProc);
          _progress[_currentElt]=PROGRESS_STATUS_OVER;
          QModelIndex ind(this->index(_currentElt,0));
          emit this->dataChanged(ind,ind,QVector<int>());
        }
      else
        {
          QMutexLocker locker(&_mutOnProc);
          _error=QString("The copy of %1 has not finished correctly (%2/%3)").arg(srcFile->fullName()).arg(_currentElt).arg(_srcFiles.size());
          return ;
        }
      if(QThread::currentThread()->isInterruptionRequested())
        {
          QMutexLocker locker(&_mutOnProc);
          if(s1 && s2)
            _error=QString("%1 right after %2 (%3/%4)").arg(ATOMIC_STOP_MSG).arg(srcFile->fullName()).arg(_currentElt).arg(_srcFiles.size());
          else
            _error=QString("Interrupted during copy of %1 (%2/%3)").arg(srcFile->fullName()).arg(_currentElt).arg(_srcFiles.size());
          return ;
        }
      {
        QMutexLocker locker(&_mutOnProc);
        _curProc.clear();
        _currentElt++;
      }
    }
}
        
void QFilesDirsCopierModel::stopCurrentCopy()
{
  QMutexLocker locker(&_mutOnProc);
  if(!_curProc.isNull())
    _curProc->kill();
}

void QFilesDirsCopierModel::newOutputAvailable()
{
  QMutexLocker locker(&_mutOnProc);
  QByteArray ba(_curProc->readAllStandardOutput());
  QString str(QString::fromLocal8Bit(ba));
  QRegularExpression re("[\\s]*([\\d\\,]+)[\\s]+([\\d]+)\\%[\\s]+([\\d]+\\.[\\d]{2}[Mk]B/s)[\\s]+([\\d]{1,2}:[\\d]{2}:[\\d]{2})");
  QRegularExpressionMatch rem(re.match(str,0,QRegularExpression::PartialPreferFirstMatch));
  if(rem.isValid())
    {
      QString s(rem.captured(2));
      bool isOK(false);
      int prog(s.toInt(&isOK));
      if(isOK)
        {
          _progress[_currentElt]=prog;
          QModelIndex ind(this->index(_currentElt,0));
          emit this->dataChanged(ind,ind,QVector<int>());
        }
    }
}

void QFilesDirsCopierModel::fillArgsForRSync(const DataStructure *srcFile, QStringList& args) const
{
  QString src(srcFile->entryForRSyncSrc()),dest(_destLoc->entryForRSyncDest());
  args << src << dest;
}

void ProgressDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  int zePos(index.row());
  QWidget *wid( dynamic_cast<QWidget *>(painter->device()) );
  int progress(_model->getProgressOf(zePos));
  QString txt(_model->getPrettyText(zePos));
  QRect refRect(option.rect);
  QFont ft(wid->font());
  QFontMetrics fm(ft);
  QSize refRect2(fm.boundingRect(txt).size());
  if(progress==QFilesDirsCopierModel::PROGRESS_STATUS_OVER)
    {
      QFont ft2(ft);
      ft.setBold(true);
      painter->setFont(ft);
      QPen p(painter->pen());
      painter->setPen(QPen(Qt::green));
      painter->drawText(QPoint(refRect.x()+(refRect.width()-refRect2.width())/2,
                               refRect.y()+refRect.height()/2+refRect2.height()/2-fm.descent()),txt);
      painter->setPen(p);
      painter->setFont(ft2);
      return ;
    }
  if(progress==QFilesDirsCopierModel::PROGRESS_STATUS_START)
    {
      QFont ft2(ft);
      ft.setBold(true);
      painter->setFont(ft);
      QPen p(painter->pen());
      painter->setPen(QPen(Qt::red));
      QString txt2(QString("Copy in progress of %1").arg(txt));
      painter->drawText(QPoint(refRect.x()+(refRect.width()-refRect2.width())/2,
                               refRect.y()+refRect.height()/2+refRect2.height()/2-fm.descent()),txt);
      painter->setPen(p);
      painter->setFont(ft2);
      return ;
    }
  {
    QBrush brush(Qt::green),b2(painter->brush());
    painter->setBrush(brush);
    painter->drawRect(refRect.x(),refRect.y(),int((float)refRect.width()*float(progress/100.f)),refRect.height());
    painter->drawText(QPoint(refRect.x()+(refRect.width()-refRect2.width())/2,
                             refRect.y()+refRect.height()/2+refRect2.height()/2-fm.descent()),txt);
    painter->setBrush(b2);
  }
}

CopierTableView::CopierTableView(QWidget *parent):QTableView(parent)
{
  setSelectionMode(QAbstractItemView::NoSelection);
  setFocusPolicy(Qt::NoFocus);
}

int CopierTableView::sizeHintForColumn(int column) const
{
  return width();
}

int CopierTableView::sizeHintForRow(int row) const
{
  QFilesDirsCopierModel *mod(qobject_cast<QFilesDirsCopierModel *>(model()));
  int nbElts(mod->nbOfRows());
  int sz((height()-horizontalHeader()->height())/(nbElts>0?nbElts:1));
  return sz;
}

void CopierTableView::resizeEvent(QResizeEvent *event)
{
  resizeColumnToContents(0);
  resizeRowsToContents();
  QTableView::resizeEvent(event);
}

QSize CopierTableView::sizeHint() const
{
  QFilesDirsCopierModel *model2(qobject_cast<QFilesDirsCopierModel *>(model()));
  if(model2)
    return model2->sizeHint();
  return CopierTableView::sizeHint();
}

FilesDirsCopier::FilesDirsCopier(QWidget *parent, const QList<const DataStructure *>& srcFiles, DataStructure *destLoc):QDialog(parent),_table(new CopierTableView(this)),_cancel(new QPushButton(this)),_model(nullptr)
{
  QVBoxLayout *vb(new QVBoxLayout(this));
  _cancel->setText("Cancel");
  vb->addWidget(_table);
  vb->addWidget(_cancel);
  _model=new QFilesDirsCopierModel(_table,srcFiles,destLoc);
  _th=new CopierThread(this,_model);
  _table->setModel(_model);
  _table->setShowGrid(false);
  _table->setItemDelegate(new ProgressDelegate(_table,_model));
  connect(_cancel,SIGNAL(clicked()),this,SLOT(cancelRequested()));
  connect(this,SIGNAL(myAcceptSignal(bool)),this,SLOT(myAccept(bool)));
}

void FilesDirsCopier::cancelRequested()
{
  _th->stopRequested();
  _th->wait();
  reject();
}

void FilesDirsCopier::myAccept(bool isOK)
{
  _th->wait();
  if(isOK)
    accept();
  else
    reject();
}

int FilesDirsCopier::exec()
{
  _th->start();
  return QDialog::exec();
}
