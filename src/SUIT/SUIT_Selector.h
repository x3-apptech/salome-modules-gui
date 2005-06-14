#ifndef SUIT_SELECTOR_H
#define SUIT_SELECTOR_H

#include "SUIT.h"

#include <qvaluelist.h>
#include <qobject.h>

class SUIT_SelectionMgr;
class SUIT_DataOwnerPtrList;

class SUIT_EXPORT SUIT_Selector : public QObject
{
  Q_OBJECT
public:
  SUIT_Selector( SUIT_SelectionMgr*, QObject* = 0 );
  virtual ~SUIT_Selector();

  virtual QString    type() const = 0;

  SUIT_SelectionMgr* selectionMgr() const;

  bool               isEnabled() const;
  virtual void       setEnabled( const bool );

  bool               autoBlock() const;
  virtual void       setAutoBlock( const bool );

  void               selected( SUIT_DataOwnerPtrList& ) const;
  void               setSelected( const SUIT_DataOwnerPtrList& );

  bool               hasSelectionMode( const int ) const;
  void               selectionModes( QValueList<int>& ) const;

protected:
  void               selectionChanged();
  virtual void       getSelection( SUIT_DataOwnerPtrList& ) const = 0;
  virtual void       setSelection( const SUIT_DataOwnerPtrList& ) = 0;

private:
  bool               myBlock;
  SUIT_SelectionMgr* mySelMgr;
  bool               myEnabled;
  bool               myAutoBlock;
};

#endif
