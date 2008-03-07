
#ifndef OBJECT_BROWSER_OBJECT_SEARCH_HEADER
#define OBJECT_BROWSER_OBJECT_SEARCH_HEADER

#include <qstring.h>

class OB_ListItem;
class OB_Browser;
class SUIT_DataObject;

class OB_ObjSearch
{
public:
  OB_ObjSearch( OB_Browser* );
  virtual ~OB_ObjSearch();

  void setPattern( const QString&, const bool, const bool );
  OB_Browser* browser() const;

  OB_ListItem* findFirst() const;
  OB_ListItem* findLast() const;
  OB_ListItem* findNext() const;
  OB_ListItem* findPrev() const;

protected:
  virtual OB_ListItem* current() const;
  virtual OB_ListItem* tail( const bool ) const;
  virtual OB_ListItem* next( OB_ListItem*, const bool ) const;
  virtual SUIT_DataObject* data( OB_ListItem* ) const;
  virtual bool matches( SUIT_DataObject* ) const;
  OB_ListItem* find( OB_ListItem*, const bool, const bool, const bool ) const;

private:
  OB_Browser* myBrowser;
  QString myData;
  bool myIsRegExp, myIsCaseSens;
};

#endif
