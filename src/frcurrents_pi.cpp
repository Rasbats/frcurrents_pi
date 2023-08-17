/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  frcurrents Plugin
 * Author:   David Register, Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
  #include <wx/glcanvas.h>
#endif //precompiled headers

#include <wx/fileconf.h>
#include <wx/stdpaths.h>

#include "frcurrents_pi.h"

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new frcurrents_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    frcurrents PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"


//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

frcurrents_pi::frcurrents_pi(void *ppimgr)
      :opencpn_plugin_116(ppimgr)
{
      // Create the PlugIn icons
      initialize_images();

	  wxFileName fn;
      wxString tmp_path;

      tmp_path = GetPluginDataDir("frcurrents_pi");
      fn.SetPath(tmp_path);
      fn.AppendDir(_T("data"));
      fn.SetFullName("frcurrents_panel_icon.png");

      wxString shareLocn = fn.GetFullPath();

      wxInitAllImageHandlers();

      wxLogDebug(wxString("Using icon path: ") + shareLocn);
      if (!wxImage::CanRead(shareLocn)) {
          wxLogDebug("Initiating image handlers.");
          wxInitAllImageHandlers();
      }

      wxImage panelIcon(shareLocn);

      if (panelIcon.IsOk())
          m_panelBitmap = wxBitmap(panelIcon);
      else
          wxLogMessage(_("frcurrents panel icon has NOT been loaded"));



      m_bShowfrcurrents = false;
}

frcurrents_pi::~frcurrents_pi(void)
{
      delete _img_frcurrents_pi;
      delete _img_frcurrents;
}

int frcurrents_pi::Init(void)
{
      AddLocaleCatalog( _T("opencpn-frcurrents_pi") );

      // Set some default private member parameters
      m_frcurrents_dialog_x = 0;
      m_frcurrents_dialog_y = 0;
      m_frcurrents_dialog_sx = 200;
      m_frcurrents_dialog_sy = 400;
      m_pfrcurrentsDialog = NULL;
      m_pfrcurrentsOverlayFactory = NULL;
			m_bfrcurrentsShowIcon = true;

      ::wxDisplaySize(&m_display_width, &m_display_height);

      //    Get a pointer to the opencpn configuration object
      m_pconfig = GetOCPNConfigObject();
	 
      //    And load the configuration items
      LoadConfig();

      // Get a pointer to the opencpn display canvas, to use as a parent for the frcurrents dialog
      m_parent_window = GetOCPNCanvasWindow();

      //    This PlugIn needs a toolbar icon, so request its insertion if
      //    enabled locally
      if (m_bfrcurrentsShowIcon) {
#ifdef ocpnUSE_SVG
          m_leftclick_tool_id = InsertPlugInToolSVG(_T( "frcurrents" ),
              _svg_frcurrents, _svg_frcurrents_rollover,
              _svg_frcurrents_toggled, wxITEM_CHECK, _("frcurrents"), _T( "" ),
              NULL, frcurrents_TOOL_POSITION, 0, this);
#else
          m_leftclick_tool_id = InsertPlugInTool(_T(""), _img_frcurrents,
              _img_frcurrents, wxITEM_CHECK, _("frcurrents"), _T(""), NULL,
              frcurrents_TOOL_POSITION, 0, this);
#endif
      }
      return (WANTS_OVERLAY_CALLBACK |
              WANTS_OPENGL_OVERLAY_CALLBACK |
              WANTS_CURSOR_LATLON       |
              WANTS_TOOLBAR_CALLBACK    |
              INSTALLS_TOOLBAR_TOOL     |
              WANTS_CONFIG              |
              WANTS_PREFERENCES         
              //WANTS_PLUGIN_MESSAGING
            );
}

bool frcurrents_pi::DeInit(void)
{
    if(m_pfrcurrentsDialog) {
        m_pfrcurrentsDialog->Close();
        delete m_pfrcurrentsDialog;
        m_pfrcurrentsDialog = NULL;
    }

    delete m_pfrcurrentsOverlayFactory;
    m_pfrcurrentsOverlayFactory = NULL;

		m_bShowfrcurrents = false;
    SetToolbarItemState(m_leftclick_tool_id, m_bShowfrcurrents);

    return true;
}

int frcurrents_pi::GetAPIVersionMajor()
{ 
    return atoi(API_VERSION);
}

int frcurrents_pi::GetAPIVersionMinor()
{ 
    std::string v(API_VERSION);
    size_t dotpos = v.find('.');
    return atoi(v.substr(dotpos + 1).c_str());
}

int frcurrents_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int frcurrents_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *frcurrents_pi::GetPlugInBitmap()
{
      return _img_frcurrents_pi;
}

wxString frcurrents_pi::GetCommonName()
{
      return _T("frcurrents");
}


wxString frcurrents_pi::GetShortDescription()
{
      return _("frcurrents PlugIn for OpenCPN");
}


wxString frcurrents_pi::GetLongDescription()
{
      return _("frcurrents PlugIn for OpenCPN\nProvides an overlay of Tidal Current (Stream) Arrows.\n\n\
			   ");
}

void frcurrents_pi::SetDefaults(void)
{
}


int frcurrents_pi::GetToolbarToolCount(void)
{
      return 1;
}

void frcurrents_pi::ShowPreferencesDialog( wxWindow* parent )
{
    frcurrentsPreferencesDialog *Pref = new frcurrentsPreferencesDialog(parent);

    Pref->m_cbUseRate->SetValue(m_bCopyUseRate);
    Pref->m_cbUseDirection->SetValue(m_bCopyUseDirection);
	Pref->m_cbFillColour->SetValue(m_bfrcurrentsUseHiDef);

 if( Pref->ShowModal() == wxID_OK ) {

	 //bool copyFillColour = true;


     bool copyrate = Pref->m_cbUseRate->GetValue();
     bool copydirection = Pref->m_cbUseDirection->GetValue();
	 bool FillColour = Pref->m_cbFillColour->GetValue();

		 if (m_bfrcurrentsUseHiDef != FillColour){		 
			 m_bfrcurrentsUseHiDef = FillColour;
		 }
	 
        if( m_bCopyUseRate != copyrate || m_bCopyUseDirection != copydirection ) {
             m_bCopyUseRate = copyrate;
             m_bCopyUseDirection = copydirection;           
         }

		
         if(m_pfrcurrentsDialog )
		 {
			 m_pfrcurrentsDialog->OpenFile(true);
			 m_pfrcurrentsDialog->m_bUseRate = m_bCopyUseRate;
			 m_pfrcurrentsDialog->m_bUseDirection = m_bCopyUseDirection;	
			 m_pfrcurrentsDialog->m_bUseFillColour = m_bfrcurrentsUseHiDef;
		 }

		 if (m_pfrcurrentsOverlayFactory)
		 {
			 m_pfrcurrentsOverlayFactory->m_bShowRate = m_bCopyUseRate;
			 m_pfrcurrentsOverlayFactory->m_bShowDirection = m_bCopyUseDirection;
			 m_pfrcurrentsOverlayFactory->m_bShowFillColour = m_bfrcurrentsUseHiDef;
		 }

         SaveConfig();
     }

	
}

void frcurrents_pi::OnToolbarToolCallback(int id)
{
    if(NULL == m_pfrcurrentsDialog)
    {
		
        		
		m_pfrcurrentsDialog = new frcurrentsUIDialog(m_parent_window, this);
        wxPoint p = wxPoint(m_frcurrents_dialog_x, m_frcurrents_dialog_y);
        m_pfrcurrentsDialog->Move(p);

        // Create the drawing factory
        m_pfrcurrentsOverlayFactory = new frcurrentsOverlayFactory( *m_pfrcurrentsDialog );
        m_pfrcurrentsOverlayFactory->SetParentSize( m_display_width, m_display_height);
		
		 m_pfrcurrentsDialog->Show();
        
    }

    m_pfrcurrentsDialog->Fit();
    m_pfrcurrentsDialog->OpenFile(true);
      // Qualify the frcurrents dialog position
            //bool b_reset_pos = false;
/*
#ifdef __WXMSW__
        //  Support MultiMonitor setups which an allow negative window positions.
        //  If the requested window does not intersect any installed monitor,
        //  then default to simple primary monitor positioning.
            RECT frame_title_rect;
            frame_title_rect.left =   m_frcurrents_dialog_x;
            frame_title_rect.top =    m_frcurrents_dialog_y;
            frame_title_rect.right =  m_frcurrents_dialog_x + m_frcurrents_dialog_sx;
            frame_title_rect.bottom = m_frcurrents_dialog_y + 30;


            if(NULL == MonitorFromRect(&frame_title_rect, MONITOR_DEFAULTTONULL))
                  b_reset_pos = true;
#else
       //    Make sure drag bar (title bar) of window on Client Area of screen, with a little slop...
            wxRect window_title_rect;                    // conservative estimate
            window_title_rect.x = m_frcurrents_dialog_x;
            window_title_rect.y = m_frcurrents_dialog_y;
            window_title_rect.width = m_frcurrents_dialog_sx;
            window_title_rect.height = 30;

            wxRect ClientRect = wxGetClientDisplayRect();
            ClientRect.Deflate(60, 60);      // Prevent the new window from being too close to the edge
            if(!ClientRect.Intersects(window_title_rect))
                  b_reset_pos = true;

#endif

            if(b_reset_pos)
            {
                  m_frcurrents_dialog_x = 20;
                  m_frcurrents_dialog_y = 170;
                  m_frcurrents_dialog_sx = 300;
                  m_frcurrents_dialog_sy = 540;
            }
*/
      //Toggle frcurrents overlay display
      m_bShowfrcurrents = !m_bShowfrcurrents;

      //    Toggle dialog?
      if(m_bShowfrcurrents) {
          m_pfrcurrentsDialog->Move(wxPoint(m_frcurrents_dialog_x, m_frcurrents_dialog_y));
          m_pfrcurrentsDialog->SetSize(m_frcurrents_dialog_sx, m_frcurrents_dialog_sy);
          m_pfrcurrentsDialog->Show();
      } else {
          m_pfrcurrentsDialog->Hide();         
          }

      // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
      // to actual status to ensure correct status upon toolbar rebuild
      SetToolbarItemState( m_leftclick_tool_id, m_bShowfrcurrents );

      wxPoint p = m_pfrcurrentsDialog->GetPosition();
      wxRect r = m_pfrcurrentsDialog->GetRect();
      SetfrcurrentsDialogX(p.x);
      SetfrcurrentsDialogY(p.y);
      SetfrcurrentsDialogSizeX(r.GetWidth());
      SetfrcurrentsDialogSizeY(r.GetHeight());

      RequestRefresh(m_parent_window); // refresh main window
}

void frcurrents_pi::OnfrcurrentsDialogClose()
{
    m_bShowfrcurrents = false;
    SetToolbarItemState( m_leftclick_tool_id, m_bShowfrcurrents );

    m_pfrcurrentsDialog->Hide();
    //if(m_pfrcurrentsDialog->pReq_Dialog) m_pfrcurrentsDialog->pReq_Dialog->Hide();

    SaveConfig();

    RequestRefresh(m_parent_window); // refresh mainn window

}

bool frcurrents_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
    if(!m_pfrcurrentsDialog ||
       !m_pfrcurrentsDialog->IsShown() ||
       !m_pfrcurrentsOverlayFactory)
        return false;

    piDC pidc(dc);
    m_pfrcurrentsOverlayFactory->RenderOverlay (pidc, *vp );
    return true;
}

bool frcurrents_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
    if(!m_pfrcurrentsDialog ||
       !m_pfrcurrentsDialog->IsShown() ||
       !m_pfrcurrentsOverlayFactory)
        return false;

    piDC piDC;
    glEnable( GL_BLEND );
    piDC.SetVP(vp);
    
    m_pfrcurrentsOverlayFactory->RenderOverlay ( piDC, *vp );
    return true;
}


void frcurrents_pi::SetCursorLatLon(double lat, double lon)
{
    if(m_pfrcurrentsDialog)
        m_pfrcurrentsDialog->SetCursorLatLon(lat, lon);
}

bool frcurrents_pi::LoadConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if(!pConf)
        return false;

    pConf->SetPath ( _T( "/PlugIns/frcurrents" ) );

	  m_bCopyUseRate = pConf->Read ( _T ( "frcurrentsUseRate" ),1);
    m_bCopyUseDirection = pConf->Read ( _T ( "frcurrentsUseDirection" ), 1);
	  m_bfrcurrentsUseHiDef = pConf->Read ( _T ( "frcurrentsUseFillColour" ), 1);
	  m_CopyFolderSelected = pConf->Read ( _T( "frcurrentsFolder" ), "");	

    m_frcurrents_dialog_sx = pConf->Read ( _T ( "frcurrentsDialogSizeX" ), 300L );
    m_frcurrents_dialog_sy = pConf->Read ( _T ( "frcurrentsDialogSizeY" ), 540L );
    m_frcurrents_dialog_x =  pConf->Read ( _T ( "frcurrentsDialogPosX" ), 20L );
    m_frcurrents_dialog_y =  pConf->Read ( _T ( "frcurrentsDialogPosY" ), 170L );



	
    return true;
}

bool frcurrents_pi::SaveConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if(!pConf)
        return false;

    pConf->SetPath ( _T( "/PlugIns/frcurrents" ) );
    pConf->Write ( _T ( "frcurrentsUseRate" ), m_bCopyUseRate );
    pConf->Write ( _T ( "frcurrentsUseDirection" ), m_bCopyUseDirection );
	pConf->Write ( _T ( "frcurrentsUseFillColour" ), m_bfrcurrentsUseHiDef );
	pConf->Write ( _T( "frcurrentsFolder" ), m_CopyFolderSelected); 

    pConf->Write ( _T ( "frcurrentsDialogSizeX" ),  m_frcurrents_dialog_sx );
    pConf->Write ( _T ( "frcurrentsDialogSizeY" ),  m_frcurrents_dialog_sy );
    pConf->Write ( _T ( "frcurrentsDialogPosX" ),   m_frcurrents_dialog_x );
    pConf->Write ( _T ( "frcurrentsDialogPosY" ),   m_frcurrents_dialog_y );

	
    return true;
}

void frcurrents_pi::SetColorScheme(PI_ColorScheme cs)
{
    DimeWindow(m_pfrcurrentsDialog);
}

