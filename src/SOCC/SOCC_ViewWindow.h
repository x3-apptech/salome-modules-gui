#ifndef SOCC_VIEWWINDOW_H
#define SOCC_VIEWWINDOW_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SOCC.h"
#include "OCCViewer_ViewWindow.h"

class SOCC_EXPORT SOCC_ViewWindow : public OCCViewer_ViewWindow
{
  Q_OBJECT;

public:
  SOCC_ViewWindow( SUIT_Desktop*, OCCViewer_Viewer* );
  virtual ~SOCC_ViewWindow();
  
protected:
  virtual void      action( const int );

};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
