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

#ifndef _frcurrentsPI_H_
#define _frcurrentsPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
  #include <wx/glcanvas.h>
#endif //precompiled headers

#include "ocpn_plugin.h"
#include "frcurrentsOverlayFactory.h"
#include "frcurrentsUIDialog.h"
#include "pidc.h"
#include "wx/string.h"
#include <wx/window.h>

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#include "config.h"

extern wxString myVColour[5]; 

#define frcurrents_TOOL_POSITION    -1          // Request default positioning of toolbar tool

class frcurrents_pi : public opencpn_plugin_118
{
public:
      frcurrents_pi(void *ppimgr);
      ~frcurrents_pi(void);

//    The required PlugIn Methods
      int Init(void);
      bool DeInit(void);

      int GetAPIVersionMajor();
      int GetAPIVersionMinor();
      int GetPlugInVersionMajor();
      int GetPlugInVersionMinor();
      int GetPlugInVersionPatch() { return PLUGIN_VERSION_PATCH; }
      int GetPlugInVersionPost() { return PLUGIN_VERSION_TWEAK; }
      const char* GetPlugInVersionPre() { return PKG_PRERELEASE; }
      const char* GetPlugInVersionBuild() { return PKG_BUILD_INFO; }
      wxBitmap* GetPlugInBitmap();
      wxString GetCommonName();
      wxString GetShortDescription();
      wxString GetLongDescription();

//    The override PlugIn Methods
      bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp);
	  bool RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
      void SetCursorLatLon(double lat, double lon);
      void SendTimelineMessage(wxDateTime time);
      void SetDefaults(void);
      int GetToolbarToolCount(void);
      void SetDialogFont(wxWindow* dialog, wxFont* font);
      void ShowPreferencesDialog( wxWindow* parent );
      void OnToolbarToolCallback(int id);

// Other public methods
      void SetfrcurrentsDialogX    (int x){ m_frcurrents_dialog_x = x;};
      void SetfrcurrentsDialogY    (int x){ m_frcurrents_dialog_y = x;}
      void SetfrcurrentsDialogSizeX(int x){ m_frcurrents_dialog_sx = x;}
      void SetfrcurrentsDialogSizeY(int x){ m_frcurrents_dialog_sy = x;}
      void SetColorScheme(PI_ColorScheme cs);

      void OnfrcurrentsDialogClose();

      bool GetCopyRate() { return  m_bCopyUseRate; }
      bool GetCopyDirection() { return  m_bCopyUseDirection; }
	  bool GetCopyResolution() { return  m_bCopyUseHighRes; }
	  bool GetCopyColour() { return m_bfrcurrentsUseHiDef ; }
	  int GetCopyArrowStyle() { return  m_CopyArrowStyle; }


	  wxString GetFolderSelected() {return m_CopyFolderSelected;}
     // wxString GetCopyPort() { return  m_bCopyusePort; }
      frcurrentsOverlayFactory *GetfrcurrentsOverlayFactory(){ return m_pfrcurrentsOverlayFactory; }
	  
	  wxString          m_CopyFolderSelected;

      int m_frcurrents_dialog_x, m_frcurrents_dialog_y;
      int m_frcurrents_dialog_sx, m_frcurrents_dialog_sy;

      frcurrentsUIDialog* m_pfrcurrentsDialog;

      double            m_IconsScaleFactor;
      double            my_IconsScaleFactor;
      int               my_FontpointSizeFactor;

private:
      bool LoadConfig(void);
      bool SaveConfig(void);

      wxFileConfig     *m_pconfig;
      wxWindow         *m_parent_window;

      frcurrentsOverlayFactory *m_pfrcurrentsOverlayFactory;

      int              m_display_width, m_display_height;
      int              m_leftclick_tool_id;
	  
	  //wxString myPort;

	 // int myNewPort; 
	 // 

      //    Controls added to Preferences panel
      wxCheckBox              *m_pfrcurrentsUseHiDef;
      wxCheckBox              *m_pfrcurrentsUseGradualColors;

     // frcurrentsTimelineRecordSet *m_pLastTimelineSet;

      // preference data

      bool              m_bfrcurrentsUseHiDef;
      bool              m_bfrcurrentsUseGradualColors;
	  bool              m_bCopyUseRate;
      bool              m_bCopyUseDirection;
	  bool				m_bCopyUseHighRes;
	  int               m_CopyArrowStyle;

      int              m_bTimeZone;
   
      int              m_bStartOptions;
      wxString         m_RequestConfig;
     
      
      bool             m_bfrcurrentsShowIcon;

      int              m_height;

      bool        m_bShowfrcurrents;
      wxBitmap    m_panelBitmap;
};

//----------------------------------------------------------------------------------------
// Prefrence dialog definition
//----------------------------------------------------------------------------------------

class frcurrentsPreferencesDialog : public frcurrentsPreferencesDialogBase
{
public:
    frcurrentsPreferencesDialog( wxWindow *pparent)
    : frcurrentsPreferencesDialogBase(pparent) {}
    ~frcurrentsPreferencesDialog() {}

private:
  void OnIconsSlidersChange(wxCommandEvent& event);
  void OnFontSlidersChange(wxCommandEvent& event);
};
#endif
