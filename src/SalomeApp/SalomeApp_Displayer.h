
#ifndef SALOMEAPP_DISPLAYER_HEADER
#define SALOMEAPP_DISPLAYER_HEADER

#include <SALOME_Prs.h>

class QString;

class SalomeApp_Displayer : public SALOME_Displayer
{
public:
  SalomeApp_Displayer();
  virtual ~SalomeApp_Displayer();

  void Display( const QString&, const bool = true, SALOME_View* = 0 );
  void Redisplay( const QString&, const bool = true );
  void Erase( const QString&, const bool forced = false, const bool updateViewer = true, SALOME_View* = 0 );
  void EraseAll( const bool forced = false, const bool updateViewer = true, SALOME_View* = 0 ) const;
  bool IsDisplayed( const QString&, SALOME_View* = 0 ) const;
  void UpdateViewer() const;

  static SALOME_View*  GetActiveView();

protected:
  virtual SALOME_Prs* buildPresentation( const QString&, SALOME_View* = 0 );
};

#endif
