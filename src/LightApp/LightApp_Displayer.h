
#ifndef LIGHTAPP_DISPLAYER_HEADER
#define LIGHTAPP_DISPLAYER_HEADER

#include <SALOME_Prs.h>

class QString;

class LightApp_Displayer : public SALOME_Displayer
{
public:
  LightApp_Displayer();
  virtual ~LightApp_Displayer();

  void Display( const QString&, const bool = true, SALOME_View* = 0 );
  void Redisplay( const QString&, const bool = true );
  void Erase( const QString&, const bool forced = false, const bool updateViewer = true, SALOME_View* = 0 );
  void EraseAll( const bool forced = false, const bool updateViewer = true, SALOME_View* = 0 ) const;
  bool IsDisplayed( const QString&, SALOME_View* = 0 ) const;
  void UpdateViewer() const;

  static SALOME_View*        GetActiveView();
  static LightApp_Displayer* FindDisplayer( const QString&, const bool );

  virtual bool canBeDisplayed( const QString& /*entry*/, const QString& /*viewer_type*/ ) const;
          bool canBeDisplayed( const QString& /*entry*/ ) const;

protected:
  virtual SALOME_Prs* buildPresentation( const QString&, SALOME_View* = 0 );
};

#endif
