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

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include <wx/glcanvas.h>
#endif  // precompiled headers

#include <wx/fileconf.h>
#include <wx/stdpaths.h>

#include "frcurrents_pi.h"

wxString myVColour[] = {"rgb(127, 0, 255)", "rgb(0, 166, 80)",
                        "rgb(253, 184, 19)", "rgb(248, 128, 64)",
                        "rgb(248, 0, 0)"};

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin *create_pi(void *ppimgr) {
  return new frcurrents_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin *p) { delete p; }

//---------------------------------------------------------------------------------------------------------
//
//    frcurrents PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

static wxBitmap load_plugin(const char *icon_name, const char *api_name) {
  wxBitmap bitmap;
  wxFileName fn;
  auto path = GetPluginDataDir(api_name);
  fn.SetPath(path);
  fn.AppendDir("data");
  fn.SetName(icon_name);
#ifdef ocpnUSE_SVG
  wxLogDebug("Loading SVG icon");
  fn.SetExt("svg");
  const static int ICON_SIZE = 48;  // FIXME: Needs size from GUI
  bitmap = GetBitmapFromSVGFile(fn.GetFullPath(), ICON_SIZE, ICON_SIZE);
#else
  wxLogDebug("Loading png icon");
  fn.SetExt("png");
  path = fn.GetFullPath();
  if (!wxImage::CanRead(path)) {
    wxLogDebug("Initiating image handlers.");
    wxInitAllImageHandlers();
  }
  wxImage panelIcon(path);
  bitmap = wxBitmap(panelIcon);
#endif
  wxLogDebug("Icon loaded, result: %s", bitmap.IsOk() ? "ok" : "fail");
  return bitmap;
}
frcurrents_pi *g_pi;
//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

frcurrents_pi::frcurrents_pi(void *ppimgr) : opencpn_plugin_118(ppimgr) {
  // Create the PlugIn icons
  initialize_images();

  m_panelBitmap = load_plugin("frcurrents_panel_icon", "frcurrents_pi");

  m_bShowfrcurrents = false;

  g_pi = this;
}

frcurrents_pi::~frcurrents_pi(void) {
  if (_xpm_frcurrents_pi) delete _xpm_frcurrents_pi;
}

int frcurrents_pi::Init(void) {
  AddLocaleCatalog("opencpn-frcurrents_pi");

  // Set some default private member parameters
  m_frcurrents_dialog_x = 40;
  m_frcurrents_dialog_y = 80;
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

  // Get a pointer to the opencpn display canvas, to use as a parent for the
  // frcurrents dialog
  m_parent_window = GetOCPNCanvasWindow();

  //    This PlugIn needs a toolbar icon, so request its insertion if
  //    enabled locally
  if (m_bfrcurrentsShowIcon) {
#ifdef ocpnUSE_SVG
    m_leftclick_tool_id = InsertPlugInToolSVG(
        "frcurrents", _svg_frcurrents, _svg_frcurrents_rollover,
        _svg_frcurrents_toggled, wxITEM_CHECK, _("frcurrents"), _T( "" ), NULL,
        frcurrents_TOOL_POSITION, 0, this);
#else
    m_leftclick_tool_id = InsertPlugInTool(
        "", _xpm_frcurrents_pi, _xpm_frcurrents_pi, wxITEM_CHECK,
        _("frcurrents"), "", NULL, frcurrents_TOOL_POSITION, 0, this);
#endif
  }
  return (WANTS_OVERLAY_CALLBACK | WANTS_OPENGL_OVERLAY_CALLBACK |
          WANTS_CURSOR_LATLON | WANTS_TOOLBAR_CALLBACK | INSTALLS_TOOLBAR_TOOL |
          WANTS_CONFIG | WANTS_PREFERENCES
          // WANTS_PLUGIN_MESSAGING
  );
}

bool frcurrents_pi::DeInit(void) {
  if (m_pfrcurrentsDialog) {
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

int frcurrents_pi::GetAPIVersionMajor() { return atoi(API_VERSION); }

int frcurrents_pi::GetAPIVersionMinor() {
  std::string v(API_VERSION);
  size_t dotpos = v.find('.');
  return atoi(v.substr(dotpos + 1).c_str());
}

int frcurrents_pi::GetPlugInVersionMajor() { return PLUGIN_VERSION_MAJOR; }

int frcurrents_pi::GetPlugInVersionMinor() { return PLUGIN_VERSION_MINOR; }

int GetPlugInVersionPatch() { return PLUGIN_VERSION_PATCH; }

int GetPlugInVersionPost() { return PLUGIN_VERSION_TWEAK; }
const char *GetPlugInVersionPre() { return PKG_PRERELEASE; }
const char *GetPlugInVersionBuild() { return PKG_BUILD_INFO; }

wxBitmap *frcurrents_pi::GetPlugInBitmap() { return &m_panelBitmap; }

wxString frcurrents_pi::GetCommonName() { return "frcurrents"; }

wxString frcurrents_pi::GetShortDescription() {
  return _("frcurrents PlugIn for OpenCPN");
}

wxString frcurrents_pi::GetLongDescription() {
  return _(
      "frcurrents PlugIn for OpenCPN\nProvides an overlay of Tidal Current (Stream) Arrows.\n\n\
			   ");
}

void frcurrents_pi::SetDefaults(void) {}

int frcurrents_pi::GetToolbarToolCount(void) { return 1; }

void frcurrents_pi::ShowPreferencesDialog(wxWindow *parent) {
  frcurrentsPreferencesDialog *Pref = new frcurrentsPreferencesDialog(parent);

  Pref->m_cbUseRate->SetValue(m_bCopyUseRate);
  Pref->m_cbUseDirection->SetValue(m_bCopyUseDirection);
  Pref->m_cbFillColour->SetValue(m_bCopyUseFillColour);
  Pref->m_cbUseHighRes->SetValue(m_bCopyUseHighRes);
  Pref->m_sIconSizeFactor->SetValue(my_IconsScaleFactor);
  Pref->m_sFontSizeFactor->SetValue(my_FontpointSizeFactor);
  Pref->m_rTimeZoneOptions->SetSelection(g_pi->GetTZoptionID());

  wxColour myC0 = wxColour(myVColour[0]);
  Pref->myColourPicker0->SetColour(myC0);

  wxColour myC1 = wxColour(myVColour[1]);
  Pref->myColourPicker1->SetColour(myC1);

  wxColour myC2 = wxColour(myVColour[2]);
  Pref->myColourPicker2->SetColour(myC2);

  wxColour myC3 = wxColour(myVColour[3]);
  Pref->myColourPicker3->SetColour(myC3);

  wxColour myC4 = wxColour(myVColour[4]);
  Pref->myColourPicker4->SetColour(myC4);

  Pref->m_cStyle->SetSelection(m_CopyArrowStyle);

  if (Pref->ShowModal() == wxID_OK) {
// bool copyFillColour = true;
#ifndef __ANDROID__
    myVColour[0] = Pref->myColourPicker0->GetColour().GetAsString();
    myVColour[1] = Pref->myColourPicker1->GetColour().GetAsString();
    myVColour[2] = Pref->myColourPicker2->GetColour().GetAsString();
    myVColour[3] = Pref->myColourPicker3->GetColour().GetAsString();
    myVColour[4] = Pref->myColourPicker4->GetColour().GetAsString();
#endif

    bool copyrate = Pref->m_cbUseRate->GetValue();
    bool copydirection = Pref->m_cbUseDirection->GetValue();
    bool copyresolution = Pref->m_cbUseHighRes->GetValue();
    bool copyFillColour = Pref->m_cbFillColour->GetValue();

    int ArrowStyle = Pref->m_cStyle->GetSelection();

    if (m_bCopyUseRate != copyrate) {
      m_bCopyUseRate = copyrate;
    }

    if (m_bCopyUseDirection != copydirection) {
      m_bCopyUseDirection = copydirection;
    }

    if (m_bCopyUseHighRes != copyresolution) {
      m_bCopyUseHighRes = copyresolution;
    }

    if (m_bCopyUseFillColour != copyFillColour) {
      m_bCopyUseFillColour = copyFillColour;
    }

    if (m_CopyArrowStyle != ArrowStyle) {
      m_CopyArrowStyle = ArrowStyle;
    }

    if (m_pfrcurrentsDialog) {
      m_pfrcurrentsDialog->m_bUseRate = m_bCopyUseRate;
      m_pfrcurrentsDialog->m_bUseDirection = m_bCopyUseDirection;
      m_pfrcurrentsDialog->m_bUseHighRes = m_bCopyUseHighRes;
      m_pfrcurrentsDialog->m_bUseFillColour = m_bCopyUseFillColour;
      m_pfrcurrentsDialog->m_UseArrowStyle = m_CopyArrowStyle;

      m_pfrcurrentsDialog->myUseColour[0] = myVColour[0];
      m_pfrcurrentsDialog->myUseColour[1] = myVColour[1];
      m_pfrcurrentsDialog->myUseColour[2] = myVColour[2];
      m_pfrcurrentsDialog->myUseColour[3] = myVColour[3];
      m_pfrcurrentsDialog->myUseColour[4] = myVColour[4];
    }

    if (m_pfrcurrentsOverlayFactory) {
      m_pfrcurrentsOverlayFactory->m_bShowRate = m_bCopyUseRate;
      m_pfrcurrentsOverlayFactory->m_bShowDirection = m_bCopyUseDirection;
      m_pfrcurrentsOverlayFactory->m_bShowFillColour = m_bCopyUseFillColour;
      m_pfrcurrentsOverlayFactory->m_bHighResolution = m_bCopyUseHighRes;
      m_pfrcurrentsOverlayFactory->m_ShowArrowStyle = m_CopyArrowStyle;
    }

    SaveConfig();
    RequestRefresh(m_parent_window);  // refresh main window
  }
}
#ifdef __WXMSW__
void frcurrents_pi::SetDialogFont(wxWindow *dialog, wxFont *font) {
  // We have to go down to all necessary windows levels. In this case
  //+  two levels are enough
  dialog->SetFont(*font);  // dialog level
  wxFont &ft = dialog->GetFont();
  ft.SetNumericWeight(wxFONTWEIGHT_BOLD);
  wxWindowList list = dialog->GetChildren();  // first level
  for (wxWindowList::iterator it = list.begin(); it != list.end(); ++it) {
    wxWindow *win = *it;
    win->GetId() == wxID_FIND ? win->SetFont(ft) : win->SetFont(*font);
    wxWindowList list1 = win->GetChildren();  // second level
    for (wxWindowList::iterator it = list1.begin(); it != list1.end(); ++it) {
      wxWindow *win1 = *it;
      win1->GetId() == wxID_FIND ? win1->SetFont(ft) : win1->SetFont(*font);
    }
  }
}
#endif
void frcurrents_pi::OnToolbarToolCallback(int id) {
  //  get icons scale factor
  double scalefactor = GetOCPNGUIToolScaleFactor_PlugIn();
  scalefactor *=
      OCPN_GetWinDIPScaleFactor() * (1. + (my_IconsScaleFactor / 10.));
  //  get font scale factor
  wxFont f = *OCPNGetFont(_("Dialog"), 10);
  f.SetPointSize(f.GetPointSize() + my_FontpointSizeFactor);
  wxFont *font = &f;
  if (NULL == m_pfrcurrentsDialog) {
    m_pfrcurrentsDialog = new frcurrentsUIDialog(m_parent_window, this);
    wxPoint p = wxPoint(m_frcurrents_dialog_x, m_frcurrents_dialog_y);
    m_pfrcurrentsDialog->Move(p);
    m_pfrcurrentsDialog->SetSize(m_frcurrents_dialog_sx,
                                 m_frcurrents_dialog_sy);

    // Create the drawing factory
    m_pfrcurrentsOverlayFactory =
        new frcurrentsOverlayFactory(*m_pfrcurrentsDialog);
    m_pfrcurrentsOverlayFactory->SetParentSize(m_display_width,
                                               m_display_height);
  }

  // Toggle frcurrents overlay display
  m_bShowfrcurrents = !m_bShowfrcurrents;

  //    Toggle dialog?
  if (m_bShowfrcurrents) {
    m_pfrcurrentsDialog->SetScaledBitmaps(scalefactor);

#ifdef __WXMSW__
    wxFont f = *OCPNGetFont(_("Dialog"), 10);
    f.SetPointSize(f.GetPointSize() + g_pi->my_FontpointSizeFactor);
    SetDialogFont(g_pi->m_pfrcurrentsDialog, &f);
/*                                                        \
#else                                                     \
wxFont f = m_pfrcurrentsDialog->m_staticText1->GetFont(); \
f.SetNumericWeight(wxFONTWEIGHT_BOLD);                    \
m_pfrcurrentsDialog->m_staticText1->SetFont(f);           \
m_pfrcurrentsDialog->m_staticText2->SetFont(f);           \
m_pfrcurrentsDialog->m_staticText211->SetFont(f);         \
*/                                                        \
#endif

    m_pfrcurrentsDialog->Move(
        wxPoint(m_frcurrents_dialog_x, m_frcurrents_dialog_y));
    m_pfrcurrentsDialog->SetSize(m_frcurrents_dialog_sx,
                                 m_frcurrents_dialog_sy);
    m_pfrcurrentsDialog->Show();
  } else {
    m_pfrcurrentsDialog->Hide();
  }

  // Toggle is handled by the toolbar but we must keep plugin manager b_toggle
  // updated to actual status to ensure correct status upon toolbar rebuild
  SetToolbarItemState(m_leftclick_tool_id, m_bShowfrcurrents);

  // Capture dialog position
  wxPoint p = m_pfrcurrentsDialog->GetPosition();
  wxRect r = m_pfrcurrentsDialog->GetRect();
  SetfrcurrentsDialogX(p.x);
  SetfrcurrentsDialogY(p.y);
  SetfrcurrentsDialogSizeX(r.GetWidth());
  SetfrcurrentsDialogSizeY(r.GetHeight());

  RequestRefresh(m_parent_window);  // refresh main window
}

int frcurrents_pi::VerifyTimeZoneID( int id) {
  if (id <= 0 || id > 1) return 0; // only 0 or 1 supported; 0 always valid
  wxTimeSpan diff = wxDateTime::Now().Subtract(wxDateTime::Now().ToGMT());
  int diffs = wxDateTime::Now().IsDST() ? diff.GetMinutes() - 60 : diff.GetMinutes();
  if (abs(diffs) > 60 || abs(diffs) < 0) {    //  only UTC and UTC+1 time zones supported
    wxString s = _("The Time Zone UTC");
    s << ((diffs > 0) ? _T(" +") : _T(" -"));
    int h = abs(diffs / 60);
    int h1 = abs(diffs % 60);
    s << wxString::Format("%01d", h);
    if(h1 != 0) s << wxString::Format(":%01d", h1);
    s << (" ") << _("set on your device is not supported!\nTime will be displayed in UTC");
    wxMessageBox(s, _("Warning"));
    return 0;
  }
  return id;
}

void frcurrents_pi::OnfrcurrentsDialogClose() {
  m_bShowfrcurrents = false;
  SetToolbarItemState(m_leftclick_tool_id, m_bShowfrcurrents);

  m_pfrcurrentsDialog->Hide();

  // Capture dialog position
  wxPoint p = m_pfrcurrentsDialog->GetPosition();
  wxRect r = m_pfrcurrentsDialog->GetRect();
  SetfrcurrentsDialogX(p.x);
  SetfrcurrentsDialogY(p.y);
  SetfrcurrentsDialogSizeX(r.GetWidth());
  SetfrcurrentsDialogSizeY(r.GetHeight());

  SaveConfig();

  RequestRefresh(m_parent_window);  // refresh mainn window
}

bool frcurrents_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp) {
  if (!m_pfrcurrentsDialog || !m_pfrcurrentsDialog->IsShown() ||
      !m_pfrcurrentsOverlayFactory)
    return false;
  m_pfrcurrentsDialog->SetViewPort(vp);
  piDC pidc(dc);
  m_pfrcurrentsOverlayFactory->RenderOverlay(pidc, *vp);
  return true;
}

bool frcurrents_pi::RenderGLOverlay(wxGLContext *pcontext,
                                    PlugIn_ViewPort *vp) {
  if (!m_pfrcurrentsDialog || !m_pfrcurrentsDialog->IsShown() ||
      !m_pfrcurrentsOverlayFactory)
    return false;

  m_pfrcurrentsDialog->SetViewPort(vp);
  piDC piDC;
  glEnable(GL_BLEND);
  piDC.SetVP(vp);

  m_pfrcurrentsOverlayFactory->RenderOverlay(piDC, *vp);
  return true;
}

bool frcurrents_pi::LoadConfig(void) {
  wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

  if (!pConf) return false;

  pConf->SetPath(_T( "/PlugIns/frcurrents" ));

  m_bCopyUseRate = pConf->Read("frcurrentsUseRate", 1);
  m_bCopyUseDirection = pConf->Read("frcurrentsUseDirection", 1);
  m_bCopyUseHighRes = pConf->Read("frcurrentsUseHighResolution", 1);
  m_bCopyUseFillColour = pConf->Read("frcurrentsUseFillColour", 1);

  m_CopyArrowStyle = pConf->Read("frcurrentsUseArrowStyle", 1);

  my_IconsScaleFactor = pConf->Read("frcurrentsIconscalefactor", 1.);
  my_FontpointSizeFactor = pConf->Read("frcurrentsFontpointsizefactor", 0.);
  int idx = pConf->Read("frcurrentsTimeZoneID", 0.);  // 0 = UTC; 1 = Local TZ
  m_bTimeZoneID = VerifyTimeZoneID(idx); // verify validity
  m_CopyFolderSelected = pConf->Read("frcurrentsFolder", "");

  m_frcurrents_dialog_sx = pConf->Read("frcurrentsDialogSizeX", 300L);
  m_frcurrents_dialog_sy = pConf->Read("frcurrentsDialogSizeY", 540L);
  m_frcurrents_dialog_x = pConf->Read("frcurrentsDialogPosX", 20L);
  m_frcurrents_dialog_y = pConf->Read("frcurrentsDialogPosY", 170L);

  if ((m_frcurrents_dialog_x < 0) || (m_frcurrents_dialog_x > m_display_width))
    m_frcurrents_dialog_x = 40;
  if ((m_frcurrents_dialog_y < 0) || (m_frcurrents_dialog_y > m_display_height))
    m_frcurrents_dialog_y = 140;

  pConf->Read("VColour0", &myVColour[0], myVColour[0]);
  pConf->Read("VColour1", &myVColour[1], myVColour[1]);
  pConf->Read("VColour2", &myVColour[2], myVColour[2]);
  pConf->Read("VColour3", &myVColour[3], myVColour[3]);
  pConf->Read("VColour4", &myVColour[4], myVColour[4]);

  return true;
}

bool frcurrents_pi::SaveConfig(void) {
  wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

  if (pConf) {
    pConf->SetPath("/PlugIns/frcurrents");
    pConf->Write("frcurrentsUseRate", m_bCopyUseRate);
    pConf->Write("frcurrentsUseDirection", m_bCopyUseDirection);
    pConf->Write("frcurrentsUseHighResolution", m_bCopyUseHighRes);
    pConf->Write("frcurrentsUseFillColour", m_bCopyUseFillColour);

    pConf->Write("frcurrentsUseArrowStyle", m_CopyArrowStyle);

    pConf->Write("frcurrentsIconscalefactor", my_IconsScaleFactor);
    pConf->Write("frcurrentsFontpointsizefactor", my_FontpointSizeFactor);
    pConf->Write("frcurrentsTimeZoneID", m_bTimeZoneID);

    pConf->Write("frcurrentsFolder", m_CopyFolderSelected);

    pConf->Write("frcurrentsDialogSizeX", m_frcurrents_dialog_sx);
    pConf->Write("frcurrentsDialogSizeY", m_frcurrents_dialog_sy);
    pConf->Write("frcurrentsDialogPosX", m_frcurrents_dialog_x);
    pConf->Write("frcurrentsDialogPosY", m_frcurrents_dialog_y);

    pConf->Write("VColour0", myVColour[0]);
    pConf->Write("VColour1", myVColour[1]);
    pConf->Write("VColour2", myVColour[2]);
    pConf->Write("VColour3", myVColour[3]);
    pConf->Write("VColour4", myVColour[4]);

    return true;

  } else
    return true;
}

void frcurrents_pi::SetColorScheme(PI_ColorScheme cs) {
  DimeWindow(m_pfrcurrentsDialog);
}

// ------------------------------------------------------------------------
//                 Preferences Dialog Implementation
// ------------------------------------------------------------------------
void frcurrentsPreferencesDialog::OnIconsSlidersChange(wxCommandEvent &event) {
  if (g_pi) {
    g_pi->my_IconsScaleFactor = (double)event.GetInt();
    if (g_pi->m_pfrcurrentsDialog) {
      double scalefactor = GetOCPNGUIToolScaleFactor_PlugIn() *
                           OCPN_GetWinDIPScaleFactor() *
                           (1. + ((double)event.GetInt() / 10.));
      g_pi->m_pfrcurrentsDialog->SetScaledBitmaps(scalefactor);
      g_pi->m_pfrcurrentsDialog->Fit();
    }
  }
}
void frcurrentsPreferencesDialog::OnFontSlidersChange(wxCommandEvent &event) {
  if (g_pi) {
    g_pi->my_FontpointSizeFactor = event.GetInt();
    if (g_pi->m_pfrcurrentsDialog) {
      wxFont f = *OCPNGetFont(_("Dialog"), 10);
      f.SetPointSize(f.GetPointSize() + g_pi->my_FontpointSizeFactor);
      wxFont *font = &f;
#ifdef __WXMSW__
      g_pi->SetDialogFont(g_pi->m_pfrcurrentsDialog, font);
#endif
      g_pi->m_pfrcurrentsDialog->Fit();
    }
  }
}

void frcurrentsPreferencesDialog::OnTimeZoneChange(wxCommandEvent& event) {
  if (g_pi) {
    int id = g_pi->GetTZoptionID();
    int idx = g_pi->VerifyTimeZoneID(m_rTimeZoneOptions->GetSelection());
    g_pi->SetTZoptionID(idx);
    if (id != idx) {
      m_rTimeZoneOptions->SetSelection(idx);
      if (g_pi->m_pfrcurrentsDialog)
        g_pi->m_pfrcurrentsDialog->SetNow(); // force returning to now to activate new TZ chosen
    }
  }
}