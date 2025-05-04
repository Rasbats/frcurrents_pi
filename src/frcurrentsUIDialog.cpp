/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  frcurrents Object
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 *
 */
#include "wx/datetime.h"
#include "wx/sound.h"
#include "wx/tokenzr.h"
#include "wx/wx.h"
#include <wx/debug.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/graphics.h>
#include <wx/intl.h>
#include <wx/wfstream.h>
#include "qtstylesheet.h"
#include <wx/stdpaths.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "frcurrents_pi.h"
#include "icons.h"

#include <vector>
#include <wx/arrimpl.cpp>

#include "tcmgr.h"
#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

#ifdef __ANDROID__
wxWindow* g_Window;
#endif

enum { TIDE_PLOT, CURRENT_PLOT };

constexpr double radToDeg(float rad) { return rad * (180 / M_PI); }
constexpr double DegToRad(float deg) { return deg * (M_PI / 180); }

double vectorAngle(double x, double y) {
  if (x == 0)  // special cases
    return (y > 0) ? 90 : (y == 0) ? 0 : 270;
  else if (y == 0)  // special cases
    return (x >= 0) ? 0 : 180;
  double ret = radToDeg(atanf(y / x));
  if (x < 0 && y < 0)  // quadrant Ⅲ
    ret = 180 + ret;
  else if (x < 0)            // quadrant Ⅱ
    ret = 180 + ret;         // it actually substracts
  else if (y < 0)            // quadrant Ⅳ
    ret = 270 + (90 + ret);  // it actually substracts
  return ret;
}

double vectorSize(double x, double y) {
  double Hypotenuse1 = (pow(x, 2)) + (pow(y, 2));
  Hypotenuse1 = sqrt(Hypotenuse1);
  return Hypotenuse1;
}

enum {
  FORWARD_ONE_HOUR_STEP = 3600,
  FORWARD_TEN_MINUTES_STEP = 600,
  FORWARD_ONE_MINUTES_STEP = 60,
  BACKWARD_ONE_HOUR_STEP = -3600,
  BACKWARD_TEN_MINUTES_STEP = -600,
  BACKWARD_ONE_MINUTES_STEP = -60,
  FIRST_DAY_IN_MONTH = 1,
  FIRST_MONTH_IN_YEAR = 0,  //  base 0
  LAST_MONTH_IN_YEAR = 11   //  to 11
};

wxString m_Areas[] = {_T("557"), _T("564"), _T("561"), _T("562"), _T("563"),
                      _T("560"), _T("558"), _T("559"), _T("565")};

// Handle to DLL
using namespace std;

/*
#if defined (_WIN32)
int round (double x) {
        int i = (int) x;
        if (x >= 0.0) {
                return ((x-i) >= 0.5) ? (i + 1) : (i);
        } else {
                return (-x+i >= 0.5) ? (i - 1) : (i);
        }
}
#endif
*/
#define FAIL(X)  \
  do {           \
    error = X;   \
    goto failed; \
  } while (0)

// date/time in the desired time zone format
static wxString TToString(const wxDateTime date_time, const int time_zone) {
  wxDateTime t(date_time);
  t.MakeFromTimezone(wxDateTime::UTC);
  if (t.IsDST()) t.Subtract(wxTimeSpan(1, 0, 0, 0));
  switch (time_zone) {
    case 0:
      return t.Format(" %a %d-%b-%Y  %H:%M ", wxDateTime::Local) + "LOC";  //:%S
    case 1:
    default:
      return t.Format(" %a %d-%b-%Y %H:%M  ", wxDateTime::UTC) + "UTC";
  }
}

frcurrentsUIDialog::frcurrentsUIDialog(wxWindow* parent, frcurrents_pi* ppi)
    : frcurrentsUIDialogBase(parent) {
  this->Fit();
  pParent = parent;
  pPlugIn = ppi;

  //  Set date picker's limits from 1st January 1980 to 31th December 2037
  //  to prevent crashes beyong time_t 32 bits capacity overtake during jan
  //  2038.
  m_datePicker1->SetRange(
      wxDateTime((time_t)1, static_cast<wxDateTime::Month>(0), 1970),
      wxDateTime((time_t)31, static_cast<wxDateTime::Month>(11), 2037));

#ifdef __ANDROID__

  m_binResize = false;

  g_Window = this;
  GetHandle()->setStyleSheet(qtStyleSheet);
  Connect(wxEVT_LEFT_DOWN,
          wxMouseEventHandler(frcurrentsUIDialog::OnMouseEvent));
  Connect(wxEVT_LEFT_UP, wxMouseEventHandler(frcurrentsUIDialog::OnMouseEvent));

  Connect(wxEVT_MOTION, wxMouseEventHandler(frcurrentsUIDialog::OnMouseEvent));

#endif

  wxFileConfig* pConf = GetOCPNConfigObject();

  if (pConf) {
    pConf->SetPath("/PlugIns/frcurrents");

    pConf->Read("frcurrentsUseRate", &m_bUseRate);
    pConf->Read("frcurrentsUseDirection", &m_bUseDirection);
    pConf->Read("frcurrentsUseFillColour", &m_bUseFillColour);
    pConf->Read("frcurrentsUseHighResolution", &m_bUseHighRes);

    pConf->Read("frcurrentsUseArrowStyle", &m_UseArrowStyle);

    pConf->Read("frcurrentsAreaID", &m_AreaIDSelected, 0.);
    pConf->Read("frcurrentsPort", &m_PortSelected);

    pConf->Read("frcurrentsFolder", &m_FolderSelected);

    pConf->Read("VColour0", &myVColour[0], myVColour[0]);
    pConf->Read("VColour1", &myVColour[1], myVColour[1]);
    pConf->Read("VColour2", &myVColour[2], myVColour[2]);
    pConf->Read("VColour3", &myVColour[3], myVColour[3]);
    pConf->Read("VColour4", &myVColour[4], myVColour[4]);

    myUseColour[0] = myVColour[0];
    myUseColour[1] = myVColour[1];
    myUseColour[2] = myVColour[2];
    myUseColour[3] = myVColour[3];
    myUseColour[4] = myVColour[4];

  }
  ptcmgr = NULL;
  m_choiceArea->SetSelection(m_AreaIDSelected);
  m_dirPicker1->SetValue(m_FolderSelected);
  m_bOnStart = false;
  m_myChoice = 0;

  LoadStandardPorts();  // From StandardPorts.xml Load the port choice control
  LoadTCMFile();
  LoadHarmonics();

  OpenFile();        // Set up variables
  OnStartSetupHW();  // Set up the HW control and information text controls
}

frcurrentsUIDialog::~frcurrentsUIDialog() {
  wxFileConfig* pConf = GetOCPNConfigObject();
  ;

  if (pConf) {
    pConf->SetPath("/PlugIns/frcurrents");

    pConf->Write("frcurrentsUseRate", m_bUseRate);
    pConf->Write("frcurrentsUseDirection", m_bUseDirection);
    pConf->Write("frcurrentsUseHighResolution", m_bUseHighRes);
    pConf->Write("frcurrentsUseFillColour", m_bUseFillColour);

    pConf->Write(_T ( "frcurrentsUseArrowStyle" ), m_UseArrowStyle);

    pConf->Write("VColour0", myVColour[0]);
    pConf->Write("VColour1", myVColour[1]);
    pConf->Write("VColour2", myVColour[2]);
    pConf->Write("VColour3", myVColour[3]);
    pConf->Write("VColour4", myVColour[4]);

    /*
   int b = m_choiceArea->GetCurrentSelection();
    pConf->Write("frcurrentsAreaID", b);
    int c = m_choice1->GetCurrentSelection();
    wxString myP = m_choice1->GetString(c);
    pConf->Write("frcurrentsPort", myP);
    //wxMessageBox(myP);
    */

    pConf->Write("frcurrentsFolder", m_FolderSelected);
    
  }
  
}

#ifdef __ANDROID__
wxPoint g_startPos;
wxPoint g_startMouse;
wxPoint g_mouse_pos_screen;

void frcurrentsUIDialog::OnPopupClick(wxCommandEvent& evt) {
  switch (evt.GetId()) {
    case ID_DASH_RESIZE:
      m_binResize = true;
      break;
      // case ID_SOMETHING_ELSE:
      //   break;
  }
}

void frcurrentsUIDialog::OnDLeftClick(wxMouseEvent& event) {
  wxMenu mnu;
  mnu.Append(ID_DASH_RESIZE, "Resize...");
  // mnu.Append(ID_SOMETHING_ELSE, "Do something else");
  mnu.Connect(wxEVT_COMMAND_MENU_SELECTED,
              wxCommandEventHandler(frcurrentsUIDialog::OnPopupClick), NULL,
              this);
  PopupMenu(&mnu);
}

void frcurrentsUIDialog::OnMouseEvent(wxMouseEvent& event) {
  if (m_binResize) {
    wxSize currentSize = g_Window->GetSize();
    double aRatio = (double)currentSize.y / (double)currentSize.x;

    wxSize par_size = GetOCPNCanvasWindow()->GetClientSize();
    wxPoint par_pos = GetOCPNCanvasWindow()->GetPosition();

    if (event.LeftDown()) {
      m_resizeStartPoint = event.GetPosition();
      m_resizeStartSize = currentSize;
      m_binResize2 = true;
    }

    if (m_binResize2) {
      if (event.Dragging()) {
        wxPoint p = event.GetPosition();

        wxSize dragSize = m_resizeStartSize;

        dragSize.y += p.y - m_resizeStartPoint.y;
        dragSize.x += p.x - m_resizeStartPoint.x;
        ;

        if ((par_pos.y + dragSize.y) > par_size.y)
          dragSize.y = par_size.y - par_pos.y;

        if ((par_pos.x + dragSize.x) > par_size.x)
          dragSize.x = par_size.x - par_pos.x;

        /// vertical
        // dragSize.x = dragSize.y / aRatio;

        // not too small
        dragSize.x = wxMax(dragSize.x, 150);
        dragSize.y = wxMax(dragSize.y, 150);

        g_Window->SetSize(dragSize);
      }

      if (event.LeftUp()) {
        wxPoint p = event.GetPosition();

        wxSize dragSize = m_resizeStartSize;

        dragSize.y += p.y - m_resizeStartPoint.y;
        dragSize.x += p.x - m_resizeStartPoint.x;
        ;

        if ((par_pos.y + dragSize.y) > par_size.y)
          dragSize.y = par_size.y - par_pos.y;

        if ((par_pos.x + dragSize.x) > par_size.x)
          dragSize.x = par_size.x - par_pos.x;

        // not too small
        dragSize.x = wxMax(dragSize.x, 150);
        dragSize.y = wxMax(dragSize.y, 150);

        g_Window->SetSize(dragSize);

        m_binResize = false;
        m_binResize2 = false;
      }
    }
  } else {
    if (event.Dragging()) {
      m_resizeStartPoint = event.GetPosition();
      int x = wxMax(0, m_resizeStartPoint.x);
      int y = wxMax(0, m_resizeStartPoint.y);
      int xmax = ::wxGetDisplaySize().x - GetSize().x;
      x = wxMin(x, xmax);
      int ymax =
          ::wxGetDisplaySize().y - (GetSize().y);  // Some fluff at the bottom
      y = wxMin(y, ymax);

      g_Window->Move(x, y);
    }
  }
}

void frcurrentsUIDialog::OnContextMenuSelect(wxCommandEvent& event) {
  //
  switch (event.GetId()) {
    case ID_DASH_RESIZE: {
      /*
                  for( unsigned int i=0; i<m_ArrayOfInstrument.size(); i++ ) {
                      DashboardInstrument* inst =
         m_ArrayOfInstrument.Item(i)->m_pInstrument; inst->Hide();
                  }
      */
      m_binResize = true;
      wxMessageBox("here");

      return;
    }
  }
  pPlugIn->SaveConfig();
}

void frcurrentsUIDialog::OnContextMenu(wxContextMenuEvent& event) {
  wxMenu* contextMenu = new wxMenu();
  wxFont* pf = OCPNGetFont(_T("Menu"), 0);

  // add stuff
  wxMenuItem* item1 =
      new wxMenuItem(contextMenu, ID_DASH_PREFS, _("Preferences..."));
  item1->SetFont(*pf);
  contextMenu->Append(item1);

  wxMenuItem* item2 =
      new wxMenuItem(contextMenu, ID_DASH_RESIZE, _("Resize..."));
  item2->SetFont(*pf);
  contextMenu->Append(item2);

  PopupMenu(contextMenu);
  delete contextMenu;
}
#endif  // End of Android functions for move/resize

void frcurrentsUIDialog::SetScaledBitmaps(double scalefactor) {
  //  Round to the nearest "quarter", to avoid rendering artifacts
  double myscaledFactor = wxRound(scalefactor * 4.0) / 4.0;
  int w, h;
  w = 32 * scalefactor;  // 32x32 is the standard bitmap's size
  h = 32 * scalefactor;

#ifdef ocpnUSE_SVG
  wxBitmap bitmap = GetBitmapFromSVGFile(_svg_frcurrents_info, w, h);
  m_button8->SetBitmap(bitmap);
  bitmap = GetBitmapFromSVGFile(_svg_frcurrents_next, w, h);
  m_bpNext->SetBitmap(bitmap);
  bitmap = GetBitmapFromSVGFile(_svg_frcurrents_prev, w, h);
  m_bpPrev->SetBitmap(bitmap);
  bitmap = GetBitmapFromSVGFile(_svg_frcurrents_now, w, h);
  m_bpNow->SetBitmap(bitmap);
#else
  wxImage im0 =
      wxBitmap(prev_blue).ConvertToImage().Scale(w, h, wxIMAGE_QUALITY_HIGH);
  m_bpPrev->SetBitmap(wxBitmap(im0));
  wxImage im1 =
      wxBitmap(next_blue).ConvertToImage().Scale(w, h, wxIMAGE_QUALITY_HIGH);
  m_bpNext->SetBitmap(wxBitmap(im1));
  wxImage im2 =
      wxBitmap(info_blue).ConvertToImage().Scale(w, h, wxIMAGE_QUALITY_HIGH);
  m_button8->SetBitmap(wxBitmap(im2));
  wxImage im3 =
      wxBitmap(now_blue).ConvertToImage().Scale(w, h, wxIMAGE_QUALITY_HIGH);
  m_bpNow->SetBitmap(wxBitmap(im3));
#endif

  this->Refresh();
}
void frcurrentsUIDialog::SetCursorLatLon(double lat, double lon) {
  m_cursor_lon = lon;
  m_cursor_lat = lat;
}

void frcurrentsUIDialog::SetViewPort(PlugIn_ViewPort* vp) {
  if (m_vp == vp) return;

  m_vp = new PlugIn_ViewPort(*vp);
}

void frcurrentsUIDialog::OnClose(wxCloseEvent& event) {
  m_FolderSelected = m_dirPicker1->GetValue();

  
  //m_AreaIDSelected = m_choiceArea->GetCurrentSelection();

  //int c = m_choice1->GetCurrentSelection();
  //m_PortSelected = m_choice1->GetString(c);

  pPlugIn->m_CopyArea = m_AreaIDSelected;
  pPlugIn->m_CopyPort = m_PortSelected;
  
  pPlugIn->m_CopyFolderSelected = m_FolderSelected;
  pPlugIn->OnfrcurrentsDialogClose();
}

void frcurrentsUIDialog::OnMove(wxMoveEvent& event) {
  //    Record the dialog position
  wxPoint p = GetPosition();
  pPlugIn->SetfrcurrentsDialogX(p.x);
  pPlugIn->SetfrcurrentsDialogY(p.y);

  event.Skip();
}
/*
void frcurrentsUIDialog::OnSize( wxSizeEvent& event )
{
    //    Record the dialog size
    wxSize p = event.GetSize();
    pPlugIn->SetfrcurrentsDialogSizeX( p.x );
    pPlugIn->SetfrcurrentsDialogSizeY( p.y );

    event.Skip();
}*/

void frcurrentsUIDialog::OpenFile(bool newestFile) {
  m_bUseRate = pPlugIn->GetCopyRate();
  m_bUseDirection = pPlugIn->GetCopyDirection();
  m_bUseHighRes = pPlugIn->GetCopyResolution();
  m_bUseFillColour = pPlugIn->GetCopyColour();
  m_UseArrowStyle = pPlugIn->GetCopyArrowStyle();

  button_id = 6;  // High Water
  m_myChoice = 0;

  label_array[0] = _("HW-6");
  label_array[1] = _("HW-5");
  label_array[2] = _("HW-4");
  label_array[3] = _("HW-3");
  label_array[4] = _("HW-2");
  label_array[5] = _("HW-1");
  label_array[6] = _("HW");
  label_array[7] = _("HW+1");
  label_array[8] = _("HW+2");
  label_array[9] = _("HW+3");
  label_array[10] = _("HW+4");
  label_array[11] = _("HW+5");
  label_array[12] = _("HW+6");

  label_lw[0] = _("LW-6");
  label_lw[1] = _("LW-5");
  label_lw[2] = _("LW-4");
  label_lw[3] = _("LW-3");
  label_lw[4] = _("LW-2");
  label_lw[5] = _("LW-1");
  label_lw[6] = _("LW");
  label_lw[7] = _("LW+1");
  label_lw[8] = _("LW+2");
  label_lw[9] = _("LW+3");
  label_lw[10] = _("LW+4");
  label_lw[11] = _("LW+5");
  label_lw[12] = _("LW+6");
}

void frcurrentsUIDialog::OnStartSetupHW() {
  //  find area ID and select it
  //int id;
  //int count = m_choiceArea->GetCount() -1;
  //if (m_AreaIDSelected < 0 || m_AreaIDSelected > count)
  //  m_AreaIDSelected = 0;
  m_choiceArea->SetSelection(m_AreaIDSelected);
  wxString s = m_Areas[m_AreaIDSelected];

  FindTidePortUsingChoice(s);  // populate m_choice1 (this area's ports list)

  //  find port ID and select it
  //id = m_choice1->FindString(m_PortSelected, true);
  //if (id == wxNOT_FOUND) id = 0;
  m_choice1->SetSelection(0);

  SetNow();
}

void frcurrentsUIDialog::OnNow(wxCommandEvent& event) {
  if (!SetDateForNowButton()) return;

  // calc coefficient
  BrestRange = CalcRange_Brest();
  m_textCtrlCoefficient->SetValue(CalcCoefficient());

  if (m_bUseBM) {
    button_id = 6 + CalcHoursFromLWNow();
  } else
    button_id = 6 + CalcHoursFromHWNow();

  if (button_id == 13) {
    button_id = 12;
  }
  if (button_id == -1) {
    button_id = 0;
  }
  wxString s2;
  if (m_bUseBM) {
    s2 = label_lw[button_id];
  } else {
    s2 = label_array[button_id];
  }
  wxDateTime this_now = wxDateTime::Now().ToGMT();
  wxString s0 = this_now.Format("%a %d %b %Y");
  wxString s1 = this_now.Format("%H:%M");

  m_staticText2->SetLabel(s0);
  m_staticText211->SetLabel(s1 + "    " + s2);

  SetCorrectHWSelection();

  m_myChoice = m_choice2->GetSelection();

  RequestRefresh(pParent);
}

void frcurrentsUIDialog::SetNow() {
  if (!SetDateForNowButton()) return;

  // calc coefficient
  BrestRange = CalcRange_Brest();
  m_textCtrlCoefficient->SetValue(CalcCoefficient());

  if (m_bUseBM) {
    button_id = 6 + CalcHoursFromLWNow();
  } else
    button_id = 6 + CalcHoursFromHWNow();

  if (button_id == 13) {
    button_id = 12;
  }
  if (button_id == -1) {
    button_id = 0;
  }
  wxString s2;
  if (m_bUseBM) {
    s2 = label_lw[button_id];
  } else {
    s2 = label_array[button_id];
  }
  wxDateTime this_now = wxDateTime::Now().ToGMT();
  wxString s0 = this_now.Format("%a %d %b %Y");
  wxString s1 = this_now.Format("%H:%M");

  m_staticText2->SetLabel(s0);
  m_staticText211->SetLabel(s1 + "    " + s2);

  SetCorrectHWSelection();

  m_myChoice = m_choice2->GetSelection();

  RequestRefresh(pParent);
}

void frcurrentsUIDialog::SetCorrectHWSelection() {
  int i, c, s, t, t1, t2, t10, t20;
  wxDateTime d1, d2, d10, d20;

  c = m_choice2->GetCount();
  wxDateTime myChoiceDates[6], m_dt;

  c = m_choice2->GetCount();
  for (i = 0; i < c; i++) {
    myChoiceDates[i] = m_choice2_dt[i];
  }

  wxDateTime this_now = wxDateTime::Now().ToGMT();
  t = this_now.GetTicks();

  for (i = 0; i < c; i++) {
    d1 = myChoiceDates[i];
    t1 = d1.GetTicks() - 6 * 60 * 60;
    d2 = myChoiceDates[i];
    t2 = d2.GetTicks() + 6 * 60 * 60;

    if ((t > t1) && (t < t2)) {
      s = i;
      i = 100;
      m_choice2->SetSelection(s);
      return;
    } else {
      s = 0;
    }
  }
  // Arrived here because time in between +6/-6
  for (i = 0; i < c - 1; i++) {
    d10 = myChoiceDates[i];
    t10 = d10.GetTicks();
    d20 = myChoiceDates[i + 1];
    t20 = d20.GetTicks();
    if ((t > t10) && (t < t20)) {
      m_choice2->SetSelection(i);
      return;
    }
  }
  // Arrived here because date/time outside the day shown on calendar
  // Date after dateCal
  wxDateTime datepick = m_datePicker1->GetValue();
  if (this_now.GetDayOfYear() > datepick.GetDayOfYear()) {
    m_choice2->SetSelection(c - 1);
    return;
  }
  // Date before dateCal
  if (this_now.GetDayOfYear() < datepick.GetDayOfYear()) {
    m_choice2->SetSelection(0);
    return;
  }
  // Default
  m_choice2->SetSelection(0);
}

void frcurrentsUIDialog::OnDateSelChanged(wxDateEvent& event) {
  if (event.GetDate().IsSameDate(m_SelectedDate)) return;

  //  calc new date and apply
  //  get days, months & Year
  int selectedDateTicks = m_SelectedDate.GetTicks();
  int currentDay = m_SelectedDate.GetDay();
  int newDay = event.GetDate().GetDay();
  int currentMonth = static_cast<int>(m_SelectedDate.GetMonth());
  int newMonth = static_cast<int>(event.GetDate().GetMonth());
  int newYear = event.GetDate().GetYear();

  //  years scrolled or changed
  if (m_SelectedDate.GetYear() != newYear) m_SelectedDate.SetYear(newYear);

  //   months scrolled or changed
  else if (currentMonth != newMonth) {
    if (newMonth == FIRST_MONTH_IN_YEAR &&
        currentMonth == LAST_MONTH_IN_YEAR)  // 1 year later
      currentMonth = newMonth - 1;
    else if (newMonth == LAST_MONTH_IN_YEAR &&
             currentMonth == FIRST_MONTH_IN_YEAR)  // 1 year earlier
      currentMonth = newMonth + 1;

    if (newMonth > currentMonth)
      m_SelectedDate.Add(wxDateSpan::Months(newMonth - currentMonth));
    else
      m_SelectedDate.Subtract(wxDateSpan::Months(currentMonth - newMonth));
  }
  //   days scrolled or changed
  else if (currentDay != newDay) {
    // find last day of the month
    int feb = wxDateTime::IsLeapYear(m_SelectedDate.GetYear()) ? 29 : 28;
    int lastDays[] = {31, feb, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int lastDaysINMonth = lastDays[currentMonth];
    if (newDay == FIRST_DAY_IN_MONTH &&
        currentDay == lastDaysINMonth)  // 1 month later
      currentDay = newDay - 1;
    else if (newDay == lastDaysINMonth &&
             currentDay == FIRST_DAY_IN_MONTH)  // 1 month earlier
      currentDay = newDay + 1;

    if (newDay > currentDay)
      m_SelectedDate.Add(wxDateSpan::Days(newDay - currentDay));
    else
      m_SelectedDate.Subtract(wxDateSpan::Days(currentDay - newDay));
  }
  //   update picker & save the new selected date ticks
  m_datePicker1->SetValue(m_SelectedDate);
  bool later = m_SelectedDate.GetTicks() > selectedDateTicks;
  //  end new date

  m_staticText2->SetLabel("");
  m_staticText211->SetLabel("");

  //    Clear the Choice ListBox
  m_choice2->Clear();

  int m = m_choice1->GetCurrentSelection();
  wxString s = m_choice1->GetString(m);

  int i = FindPortIDUsingChoice(s);

  if (i == 0) {
    wxMessageBox(_("No Tidal Data"), _("Tidal Data Finder"));
    return;
  }
  if (m_bUseBM) {
    CalcLW(i);
  } else {
    CalcHW(i);
  }

  BrestRange = CalcRange_Brest();
  m_textCtrlCoefficient->SetValue(CalcCoefficient());

  button_id = 6;  //  in another days as today, set to HW/LW

  if (later)
    m_myChoice = 0;  // first HW/LW of the later day
  else {
    int c = m_choice2->GetCount();
    m_myChoice = c - 1;  // last HW/LW of the earlier day
  }

  m_choice2->SetSelection(m_myChoice);

  m_dt = m_choice2_dt[m_myChoice];
  m_dt.Subtract(wxTimeSpan::Hours(6));
  m_dt.Add(wxTimeSpan::Hours(button_id));
  m_staticText2->SetLabel(m_dt.Format("%a %d %b %Y"));
  wxString t = m_dt.Format("%H:%M") + "    ";
  if (m_bUseBM)
    m_staticText211->SetLabel(t + label_lw[button_id]);
  else
    m_staticText211->SetLabel(t + label_array[button_id]);

  RequestRefresh(pParent);
}

void frcurrentsUIDialog::OnPortChanged(wxCommandEvent& event) { SetNow(); }

bool frcurrentsUIDialog::SetDateForNowButton() {
  m_staticText2->SetLabel("");
  m_staticText211->SetLabel("");

  wxDateTime this_now = wxDateTime::Now().ToGMT();
  m_datePicker1->SetValue(this_now);
  m_SelectedDate = this_now.GetDateOnly();

  int ma = m_choiceArea->GetCurrentSelection();
  wxString sa = m_Areas[ma];

  int m = m_choice1->GetSelection();
  wxString string = m_choice1->GetString(m);
  m_portXML = FindPortXMLUsingChoice(string);

  if (m_portXML == "") {
    wxMessageBox(_("Port not found"), _("Port finder"));
    return false;
  }

  int id = FindPortIDUsingChoice(string);

  if (id == 0) {
    wxMessageBox(_("No Tidal Data"), _("Tidal Data Finder"));
    button_id = 6;
    RequestRefresh(pParent);
    return false;
  } else {
    GetCurrentsData(sa);
    if (m_bUseBM) {
      CalcLW(id);
    } else {
      CalcHW(id);
    };
    int i, c, s, t, t1, t2, t10, t20;
    wxDateTime d1, d2, d10, d20;

    t1 = 0;
    t2 = 0;

    c = m_choice2->GetCount();
    wxDateTime myChoiceDates[6], m_dt;

    c = m_choice2->GetCount();
    for (i = 0; i < c; i++) {
      myChoiceDates[i] = m_choice2_dt[i];
    }

    t = this_now.GetTicks();

    for (i = 0; i < c; i++) {
      d1 = myChoiceDates[i];
      t1 = d1.GetTicks() - 6 * 60 * 60;
      d2 = myChoiceDates[i];
      t2 = d2.GetTicks() + 6 * 60 * 60;

      if ((t > t1) && (t < t2)) {
        s = i;
        i = 100;
        m_choice2->SetSelection(s);
        return true;
      } else {
        s = 999;
      }
    }

    if (s == 999) {
      // Deal with date time between two high waters >+6 <-6
      for (i = 0; i < c - 1; i++) {
        d10 = myChoiceDates[i];
        t10 = d10.GetTicks();
        d20 = myChoiceDates[i + 1];
        t20 = d20.GetTicks();
        if ((t > t10) && (t < t20)) {
          m_choice2->SetSelection(i + 1);
          return true;
        }
      }

      // 6 hours less than first HW. Reset day before.
      if (t > t2) {
        wxDateSpan myOneDay = wxDateSpan::Days(1);
        wxDateTime myDate = m_datePicker1->GetValue();
        myDate.Add(myOneDay);
        m_datePicker1->SetValue(myDate);
        m_SelectedDate = myDate.GetDateOnly();
        if (m_bUseBM) {
          CalcLW(id);
        } else {
          CalcHW(id);
        }
        m_choice2->SetSelection(0);
        return true;
      }

      // 6 hours greater than last HW. Reset day later.
      if (t < t1) {
        wxDateSpan myOneDay = wxDateSpan::Days(1);
        wxDateTime myDate = m_datePicker1->GetValue();
        myDate.Subtract(myOneDay);
        m_datePicker1->SetValue(myDate);
        m_SelectedDate = myDate.GetDateOnly();

        if (m_bUseBM) {
          CalcLW(id);
        } else {
          CalcHW(id);
        }
        c = m_choice2->GetCount();
        m_choice2->SetSelection(c - 1);
        return true;
      }
    }
  }
}

StandardPort frcurrentsUIDialog::PopulatePortTides(wxString PortName) {
  StandardPort myCPort;
  int m = m_choice1->GetSelection();
  wxString s = m_choice1->GetString(m);

  int i = 0;
  for (std::vector<StandardPort>::iterator it = my_ports.begin();
       it != my_ports.end(); it++) {
    myCPort.PORT_NUMBER = (*it).PORT_NUMBER;
    myCPort.PORT_NAME = (*it).PORT_NAME;

    myCPort.BMME = (*it).BMME;
    myCPort.BMVE = (*it).BMVE;
    myCPort.PMME = (*it).PMME;
    myCPort.PMVE = (*it).PMVE;

    myCPort.LAT = (*it).LAT;
    myCPort.LON = (*it).LON;

    double value, dBMME, dBMVE, dPMME, dPMVE;
    myCPort.BMME.ToDouble(&value);
    dBMME = value;

    myCPort.BMVE.ToDouble(&value);
    dBMVE = value;

    myCPort.PMME.ToDouble(&value);
    dPMME = value;

    myCPort.PMVE.ToDouble(&value);
    dPMVE = value;

    myCPort.spRange = dPMVE - dBMVE;
    myCPort.npRange = dPMME - dBMME;

    if (myCPort.PORT_NAME == s) {
      wxString getvalue;
      double value;

      getvalue = myCPort.LAT;
      getvalue.ToDouble(&value);
      m_jumpLat = value;

      getvalue = myCPort.LON;
      getvalue.ToDouble(&value);
      m_jumpLon = value;
      return myCPort;
    }
    i++;
  }
  return myCPort;
}

void frcurrentsUIDialog::OnInformation(wxCommandEvent& WXUNUSED(event)) {
  // wxString s = wxFileName::GetPathSeparator();
  // wxString infolocation = GetPluginDataDir("frcurrents_pi") + s + "data" + s
  // +
  //  "Information" + s + "frcurrentsInformation.html";

  wxString help =
      "For a source of harmonics and other information: "
      "\n\nhttps://opencpn-manuals.github.io/main/frcurrents/index.html";
  wxMessageBox(help);
}

wxString frcurrentsUIDialog::FindPortXMLUsingChoice(wxString inPortName) {
  PortTides myPortTides;

  wxString s = inPortName;

  int i = 0;
  for (std::vector<StandardPort>::iterator it = my_ports.begin();
       it != my_ports.end(); it++) {
    myPortTides.m_portID = (*it).PORT_NUMBER;
    myPortTides.m_portName = (*it).PORT_NAME;
    myPortTides.m_IDX = (*it).IDX;

    if (myPortTides.m_portName == s) {
      return myPortTides.m_IDX;
    }

    i++;
  }

  return "";
}

int frcurrentsUIDialog::FindTidePortUsingChoice(wxString inAreaNumber) {
  int i;
  wxString s = inAreaNumber;
  PortTides myPortTides;

  m_choice1->Clear();

  i = 0;
  for (std::vector<StandardPort>::iterator it = my_ports.begin();
       it != my_ports.end(); it++) {
    myPortTides.m_portID = (*it).PORT_NUMBER;
    myPortTides.m_portName = (*it).PORT_NAME;
    myPortTides.m_IDX = (*it).IDX;

    if (myPortTides.m_portID == s) {
      m_choice1->Append(myPortTides.m_portName);
    }
    i++;
  }
  m_choice1->Fit();
  return 0;
}

int frcurrentsUIDialog::FindPortIDUsingChoice(wxString inPortName) {
  int i;
  wxString s = inPortName;
  PortTides myPortTides;
  // wxMessageBox(inPortName);
  i = 0;
  for (std::vector<StandardPort>::iterator it = my_ports.begin();
       it != my_ports.end(); it++) {
    myPortTides.m_portID = (*it).PORT_NUMBER;
    myPortTides.m_portName = (*it).PORT_NAME;
    myPortTides.m_portLat = (*it).LAT;
    myPortTides.m_portLon = (*it).LON;

    myPortTides.m_IDX = (*it).IDX;
    // wxMessageBox(myPortTides.m_portName);
    if (myPortTides.m_portName == s) {
      // wxMessageBox(myPortTides.m_portName, "in findPortID");
      i = FindPortID(myPortTides.m_portName);
      return i;
    }
  }

  return 0;
}

void frcurrentsUIDialog::OnSelectData(wxCommandEvent& event) {
#ifndef __ANDROID__
  wxDirDialog* d = new wxDirDialog(this, _("Choose Harmonics Directory"), "", 0,
                                   wxDefaultPosition);
  if (d->ShowModal() == wxID_OK) {
    m_dirPicker1->SetValue(d->GetPath());

    m_FolderSelected = m_dirPicker1->GetValue();
    pPlugIn->m_CopyFolderSelected = m_FolderSelected;
  }
#else
  wxString dir_spec;
  int response = PlatformDirSelectorDialog(g_Window, &dir_spec,
                                           _("Choose Harmonics Directory"),
                                           m_dirPicker1->GetValue());
  if (response == wxID_OK) {
    m_dirPicker1->SetValue(dir_spec);
    m_FolderSelected = dir_spec;
    pPlugIn->m_CopyFolderSelected = m_FolderSelected;
  }
#endif

  LoadTCMFile();
  LoadHarmonics();
  RequestRefresh(pParent);
}

void frcurrentsUIDialog::LoadHarmonics() {
  if (!ptcmgr) {
    ptcmgr = new TCMgr;
    ptcmgr->LoadDataSources(TideCurrentDataSet);

  }

  else {
    bool b_newdataset = false;

    //      Test both ways
    wxArrayString test = ptcmgr->GetDataSet();
    for (unsigned int i = 0; i < test.GetCount(); i++) {
      bool b_foundi = false;
      for (unsigned int j = 0; j < TideCurrentDataSet.GetCount(); j++) {
        if (TideCurrentDataSet[j] == test[i]) {
          b_foundi = true;
          break;  // j loop
        }
      }
      if (!b_foundi) {
        b_newdataset = true;
        break;  //  i loop
      }
    }

    test = TideCurrentDataSet;
    for (unsigned int i = 0; i < test.GetCount(); i++) {
      bool b_foundi = false;
      for (unsigned int j = 0; j < ptcmgr->GetDataSet().GetCount(); j++) {
        if (ptcmgr->GetDataSet()[j] == test[i]) {
          b_foundi = true;
          break;  // j loop
        }
      }
      if (!b_foundi) {
        b_newdataset = true;
        break;  //  i loop
      }
    }

    if (b_newdataset) ptcmgr->LoadDataSources(TideCurrentDataSet);
  }
}

void frcurrentsUIDialog::LoadTCMFile() {
  wxString TCDir = m_FolderSelected;

  if (TCDir == wxEmptyString) {
    wxMessageBox(
        _("No Harmonics\nSelect the directory containing "
          "HARMONIC.IDX\n Using the dialog that follows"));

#ifndef __ANDROID__
    TCDir = pPlugIn->GetFolderSelected();
    m_dirPicker1->SetValue(TCDir);
    wxDirDialog* d = new wxDirDialog(this, _("Choose Harmonics Directory"), "",
                                     0, wxDefaultPosition);
    if (d->ShowModal() == wxID_OK) {
      m_dirPicker1->SetValue(d->GetPath());
      TCDir = m_dirPicker1->GetValue();
    }
#else
    wxString dir_spec;
    int response = PlatformDirSelectorDialog(g_Window, &dir_spec,
                                             _("Choose Harmonics Directory"),
                                             m_dirPicker1->GetValue());
    if (response == wxID_OK) {
      m_dirPicker1->SetValue(dir_spec);
      m_FolderSelected = dir_spec;
      pPlugIn->m_CopyFolderSelected = m_FolderSelected;
      TCDir = m_dirPicker1->GetValue();
    }

#endif
  }
  TCDir.Append(wxFileName::GetPathSeparator());
  wxLogMessage(_("Using Tide/Current data from:  ") + TCDir);

  wxString default_tcdata1 = TCDir + "HARMONIC.IDX";
  wxLogMessage(default_tcdata1);

  TideCurrentDataSet.Add(default_tcdata1);
}

int frcurrentsUIDialog::FindPortID(wxString myPort) {
  const IDX_entry* pIDX;
  for (int i = 1; i < ptcmgr->Get_max_IDX() + 1; i++) {
    pIDX = ptcmgr->GetIDX_entry(i);
    wxString locnx(pIDX->IDX_station_name, wxConvUTF8);
    if (locnx == myPort) {
      wxString ind = wxString::Format("%i", i);
      return i;
    }
  }
  return 0;
}

void frcurrentsUIDialog::SetTimeFactors() {
  //    Figure out this computer timezone minute offset

  /*  wxDatePicker's Time is O0h, but the summer time occurs at 00h +/- TZ
   * offset. What's more, the change is effective at time decided by contries
   * (ex: England and France make the change at the same time whereas they are
   * not on the same meridien) Thus to be sure to spot the change the first day,
   * the best way is to look at the end of the day;  */

  wxDateTime this_now = m_datePicker1->GetValue();
  this_now.Add(wxTimeSpan().Minutes(
      (22 * 60) + 59));  //	set the time very near the end of the day
  wxDateTime this_gmt = this_now.ToGMT();

#if wxCHECK_VERSION(2, 6, 2)
  wxTimeSpan diff = this_now.Subtract(this_gmt);
#else
  wxTimeSpan diff = this_gmt.Subtract(this_now);
#endif

  m_diff_mins = diff.GetMinutes();

  //  Correct a bug in wx3.0.2
  //  If the system TZ happens to be GMT, with DST active (e.g.summer in
  //  London), then wxDateTime returns incorrect results for toGMT() method
#if wxCHECK_VERSION(3, 0, 2)
  if (m_diff_mins == 0 && this_now.IsDST()) m_diff_mins += 60;
#endif
  //   End of this computer timezone minute offset calculation

  // find the first day with summer time
  m_diff_first_dst_sec = 0;
  wxDateTime dtn = this_now;
  if (this_now.IsDST() && !dtn.Subtract(wxTimeSpan().Days(1)).IsDST())
    m_diff_first_dst_sec = 3600;

  const IDX_entry* pIDX = ptcmgr->GetIDX_entry(myPortCode);

  IDX_entry IDX = *pIDX;

  int station_offset = ptcmgr->GetStationTimeOffset(&IDX);

  m_stationOffset_mins = station_offset;
  if (this_now.IsDST()) m_stationOffset_mins += 60;

    //  Correct a bug in wx3.0.2
    //  If the system TZ happens to be GMT, with DST active (e.g.summer in
    //  London), then wxDateTime returns incorrect results for toGMT()
    //  method
#if wxCHECK_VERSION(3, 0, 2)
//    if(  this_now.IsDST() )
//        m_corr_mins +=60;
#endif

  //    Establish the inital drawing day as today, in the timezone of the
  //    station
  m_graphday = m_datePicker1->GetValue();

  // m_graphday = this_gmt;

  time_t t_time_gmt = m_graphday.GetTicks();

  time_t ttNow = this_now.GetTicks();
  time_t tt_at_station =
      ttNow - (m_diff_mins * 60) + (m_stationOffset_mins * 60);
  /*
    if (t_time_gmt > tt_at_station) {
      wxTimeSpan dt((t_time_gmt - tt_at_station) / 3600, 0, 0, 0);
      m_graphday.Subtract(dt);
    } else if (t_time_gmt < tt_at_station) {
      wxTimeSpan dt((tt_at_station - t_time_gmt) / 3600, 0, 0, 0);
      m_graphday.Add(dt);
    }
  */
  wxDateTime graphday_00 = m_graphday;  // this_gmt;
  graphday_00.ResetTime();
  time_t t_graphday_00 = graphday_00.GetTicks();

  //    Correct a Bug in wxWidgets time support
  //    if( !graphday_00.IsDST() && m_graphday.IsDST() ) t_graphday_00 -=
  //    3600; if( graphday_00.IsDST() && !m_graphday.IsDST() ) t_graphday_00
  //    += 3600;

  m_t_graphday_GMT = t_graphday_00;

  btc_valid = false;  // Force re-calculation
}

double frcurrentsUIDialog::CalcRange_Brest() {
  int BrestID = FindPortID("BREST, France");

  if (BrestID == 0) {
    return 999;
  }
  myPortCode = BrestID;
  SetTimeFactors();

  const IDX_entry* pIDX = ptcmgr->GetIDX_entry(BrestID);

  m_plot_type = TIDE_PLOT;

  // Establish the inital drawing day as today
  m_graphday = m_datePicker1->GetValue();

  float dir;
  float tcmax, tcmin;
  tcmax = -10;
  tcmin = 10;
  float val = -100;
  int list_index = 0;
  int array_index = 0;
  wxString sHWLW = "";
  int e = 0;
  double myLW, myHW;
  bool wt = false;
  bool gotHW = false;
  int i;
  float tcv[26];
  time_t tt_tcv[26];
  myHeightHW = 0;
  myHeightLW = 0;

  // The tide/current modules calculate values based on PC local time
  // We need  LMT at station, so adjust accordingly
  int tt_localtz = m_t_graphday_GMT + (m_diff_mins * 60);
  // tt_localtz -= m_stationOffset_mins * 60;  // LMT at station

  // get tide flow sens ( flood or ebb ? )
  ptcmgr->GetTideFlowSens(tt_localtz, BACKWARD_TEN_MINUTES_STEP,
                          pIDX->IDX_rec_num, tcv[0], val, wt);

  for (i = 0; i < 26; i++) {
    int tt = tt_localtz + (i * FORWARD_ONE_HOUR_STEP);

    ptcmgr->GetTideOrCurrent(tt, pIDX->IDX_rec_num, tcv[i], dir);
    tt_tcv[i] = tt;  // store the corresponding time_t value
    if (tcv[i] > tcmax) tcmax = tcv[i];

    if (tcv[i] < tcmin) tcmin = tcv[i];
    if (TIDE_PLOT == m_plot_type) {
      if (!((tcv[i] > val) == wt) && (i > 0))  // if tide flow sense change
      {
        float tcvalue;  // look backward for HW or LW
        time_t tctime;
        ptcmgr->GetHightOrLowTide(tt, BACKWARD_TEN_MINUTES_STEP,
                                  BACKWARD_ONE_MINUTES_STEP, tcv[i], wt,
                                  pIDX->IDX_rec_num, tcvalue, tctime);
        if (tctime > tt_localtz) {  // Only show events visible in graphic
          // presently shown
          wxDateTime tcd;  // write date
          wxString s, s1, s2;
          tcd.Set(tctime - (m_diff_mins * 60));

          (wt) ? sHWLW = "HW" : sHWLW = "LW";  // write HW or LT
          // Fill the array with tide data
          euTC[array_index][3] = sHWLW;

          if (euTC[array_index][3] == "LW") {
            myLW = tcvalue;
            if (gotHW) {  // We use the BM after PM
              BrestHeightHW = myHW;
              BrestHeightLW = myLW;
              BrestRange = myHW - myLW;  // Used for
                                         // CalcCoefficient
            }
          }

          if (euTC[array_index][3] == "HW") {
            gotHW = true;
            myHW = tcvalue;

            // nearestHW for the now button
            e++;
            list_index++;
          }
        }
        array_index++;
        wt = !wt;  // change tide flow sens
      }
      val = tcv[i];
    }
  }
  return BrestRange;
}

void frcurrentsUIDialog::CalcHW(int PortCode) {
  m_choice2->Clear();
  m_choice2_dt.clear();

  if (PortCode == 0) return;

  myPortCode = PortCode;
  SetTimeFactors();

  const IDX_entry* pIDX = ptcmgr->GetIDX_entry(PortCode);

  m_plot_type = TIDE_PLOT;

  // Establish the inital drawing day as today
  m_graphday = m_datePicker1->GetValue();
  // Show the timezones
  m_stz = _("Display Time: UTC");
  int diffloc = m_diff_mins;
  if (diffloc != 0) {
    m_stz << ("  (") << _("Local");
    if (diffloc > 0)
      m_stz << (" -");
    else {
      m_stz << (" +");
      diffloc = -diffloc;
    }
    int h = abs(diffloc / 60);
    int h1 = abs(diffloc % 60);
    if (h1 == 0)
      m_stz << wxString::Format("%01d", h) << (")");
    else
      m_stz << wxString::Format("%01d:%02d", h, h1) << (")");
  }
  m_staticText1->SetLabel(m_stz);
  //
  float dir;
  float tcmax, tcmin;
  tcmax = -10;
  tcmin = 10;
  float val = -100;
  int list_index = 0;
  int array_index = 0;
  wxString sHWLW = "";
  int e = 0;
  double myLW, myHW;
  bool wt = false;
  bool gotHW = false;
  Station_Data* pmsd;
  int i;
  wxDateTime euDT[8];
  float tcv[26];
  time_t tt_tcv[26];
  myHeightHW = 0;
  myHeightLW = 0;

  // The tide/current modules calculate values based on PC local time
  // We need  UTC, so adjust accordingly
  int tt_localtz = m_t_graphday_GMT + (m_diff_mins * 60) - m_diff_first_dst_sec;
  //  Get the day after
  int tt_nextlocaltzday = tt_localtz + (24 * 3600);

  // get tide flow sens ( flood or ebb ? )
  ptcmgr->GetTideFlowSens(tt_localtz, BACKWARD_TEN_MINUTES_STEP,
                          pIDX->IDX_rec_num, tcv[0], val, wt);

  for (i = 0; i < 26; i++) {
    int tt = tt_localtz + (i * FORWARD_ONE_HOUR_STEP);

    ptcmgr->GetTideOrCurrent(tt, pIDX->IDX_rec_num, tcv[i], dir);
    tt_tcv[i] = tt;  // store the corresponding time_t value
    if (tcv[i] > tcmax) tcmax = tcv[i];

    if (tcv[i] < tcmin) tcmin = tcv[i];
    if (TIDE_PLOT == m_plot_type) {
      if (!((tcv[i] > val) == wt) && (i > 0))  // if tide flow sense change
      {
        float tcvalue;  // look backward for HW or LW
        time_t tctime;
        ptcmgr->GetHightOrLowTide(tt, BACKWARD_TEN_MINUTES_STEP,
                                  BACKWARD_ONE_MINUTES_STEP, tcv[i], wt,
                                  pIDX->IDX_rec_num, tcvalue, tctime);
        if (tctime >= tt_localtz &&
            tctime <
                tt_nextlocaltzday) {  // Only show events visible in graphic
          /* tweak to correct tide on the first hour of the day thrown back to
           * the previous day by the summer time offset  */
          if (tctime < (tt_localtz + m_diff_first_dst_sec))
            tctime += m_diff_first_dst_sec;

          // presently shown
          wxDateTime tcd;  // write date
          wxString s, s1;
          tcd.Set(tctime - (m_diff_mins * 60));
          s = tcd.Format("%a %d %b %Y  %H:%M  ");
          s1.Printf("%05.2f ", tcvalue);  // write value
          pmsd = pIDX->pref_sta_data;     // write unit
          if (pmsd) s1.Append(wxString(pmsd->units_abbrv, wxConvUTF8));

          (wt) ? sHWLW = "HW" : sHWLW = "LW";  // write HW or LT
          // Fill the array with tide data
          euTC[array_index][0] = s + s1;
          euTC[array_index][1] = s1;
          euTC[array_index][2] = wxString(pmsd->units_abbrv, wxConvUTF8);
          euTC[array_index][3] = sHWLW;
          euDT[array_index] = tcd;
          if (euTC[array_index][3] == "LW") {
            if (gotHW) myLW = tcvalue;
          }

          if (euTC[array_index][3] == "HW") {
            gotHW = true;
            myHW = tcvalue;
            myHeightHW = myHW;
            myHeightLW = myLW;

            myRange = myHW - myLW;

            m_choice2->Insert(euTC[array_index][0], list_index);
            m_choice2_dt.push_back(euDT[array_index]);
            // nearestHW for the now button
            nearestHW[e] = euDT[array_index];
            e++;
            list_index++;
          }
        }
        array_index++;
        wt = !wt;  // change tide flow sens
      }
      val = tcv[i];
    }
  }
}

void frcurrentsUIDialog::CalcLW(int PortCode) {
  m_choice2->Clear();
  m_choice2_dt.clear();

  if (PortCode == 0) return;

  myPortCode = PortCode;
  SetTimeFactors();

  const IDX_entry* pIDX = ptcmgr->GetIDX_entry(PortCode);

  // if (strchr("Tt", pIDX->IDX_type))
  m_plot_type = TIDE_PLOT;

  // Establish the inital drawing day as today
  m_graphday = m_datePicker1->GetValue();
  // Get the timezones
  m_stz = _("Display Time: UTC");
  int diffloc = m_diff_mins;
  if (diffloc != 0) {
    m_stz << ("  (") << _("Local");
    if (diffloc > 0)
      m_stz << (" -");
    else {
      m_stz << (" +");
      diffloc = -diffloc;
    }
    int h = abs(diffloc / 60);
    int h1 = abs(diffloc % 60);
    if (h1 == 0)
      m_stz << wxString::Format("%01d", h) << (")");
    else
      m_stz << wxString::Format("%01d:%02d", h, h1) << (")");
  }
  m_staticText1->SetLabel(m_stz);
  //
  float dir;
  float tcmax, tcmin;
  tcmax = -10;
  tcmin = 10;
  float val = -100;
  int list_index = 0;
  int array_index = 0;
  wxString sHWLW = "";
  int e = 0;
  double myLW, myHW;
  myHW = 0.0;
  bool wt = false;
  bool gotHW = false;
  bool gotLW = false;
  Station_Data* pmsd;
  int i;
  wxDateTime euDT[8];
  float tcv[26];
  time_t tt_tcv[26];
  myHeightHW = 0;
  myHeightLW = 0;

  // The tide/current modules calculate values based on PC local time
  // We need  UTC, so adjust accordingly
  int tt_localtz = m_t_graphday_GMT + (m_diff_mins * 60) - m_diff_first_dst_sec;
  //  Get the day after
  int tt_nextlocaltzday = tt_localtz + (24 * 3600);

  // get tide flow sens ( flood or ebb ? )
  ptcmgr->GetTideFlowSens(tt_localtz, BACKWARD_TEN_MINUTES_STEP,
                          pIDX->IDX_rec_num, tcv[0], val, wt);

  for (i = 0; i < 26; i++) {
    int tt = tt_localtz + (i * FORWARD_ONE_HOUR_STEP);

    ptcmgr->GetTideOrCurrent(tt, pIDX->IDX_rec_num, tcv[i], dir);
    tt_tcv[i] = tt;  // store the corresponding time_t value
    if (tcv[i] > tcmax) tcmax = tcv[i];

    if (tcv[i] < tcmin) tcmin = tcv[i];
    if (TIDE_PLOT == m_plot_type) {
      if (!((tcv[i] > val) == wt) && (i > 0))  // if tide flow sense change
      {
        float tcvalue;  // look backward for HW or LW
        time_t tctime;
        ptcmgr->GetHightOrLowTide(tt, BACKWARD_TEN_MINUTES_STEP,
                                  BACKWARD_ONE_MINUTES_STEP, tcv[i], wt,
                                  pIDX->IDX_rec_num, tcvalue, tctime);
        if (tctime >= tt_localtz &&
            tctime <
                tt_nextlocaltzday) {  // Only show events visible in graphic
          /* tweak to correct tide on the first hour of the day thrown back to
           * the previous day by the summer time offset  */
          if (tctime < (tt_localtz + m_diff_first_dst_sec))
            tctime += m_diff_first_dst_sec;

          // presently shown
          wxDateTime tcd;  // write date
          wxString s, s1;
          tcd.Set(tctime - (m_diff_mins * 60));
          s = tcd.Format("%a %d %b %Y  %H:%M  ");
          s1.Printf("%05.2f ",
                    tcvalue);          // write value
          pmsd = pIDX->pref_sta_data;  // write unit
          if (pmsd) s1.Append(wxString(pmsd->units_abbrv, wxConvUTF8));

          (!wt) ? sHWLW = "LW" : sHWLW = "HW";  // write HW or LT
          // Fill the array with tide data
          euTC[array_index][0] = s + s1;
          euTC[array_index][1] = s1;
          euTC[array_index][2] = wxString(pmsd->units_abbrv, wxConvUTF8);
          euTC[array_index][3] = sHWLW;
          euDT[array_index] = tcd;

          if (euTC[array_index][3] == "LW") {
            gotLW = true;
            myLW = tcvalue;
            myHeightLW = myLW;
            myHeightHW = myHW;

            myRange = myHW - myLW;  // Used for
                                    // CalcCoefficient

            m_choice2->Insert(euTC[array_index][0], list_index);
            m_choice2_dt.push_back(euDT[array_index]);
            // nearestLW for the now button
            nearestLW[e] = euDT[array_index];
            e++;
            list_index++;
            wxDateTime t;
          } else

              if (euTC[array_index][3] == "HW") {
            if (gotLW) myHW = tcvalue;
          }
        }
        array_index++;
        wt = !wt;  // change tide flow sens
      }
      val = tcv[i];
    }
  }
}

double frcurrentsUIDialog::CalcCurrent(double VE, double ME, double spRate,
                                       double npRate, double coefficient) {
  // y = mx + c

  double m, c, x;
  m = (VE - ME) / (spRate - npRate);
  c = 95 - (m * spRate);
  x = (coefficient - c) / m;
  if (isnan(x)) x = 0.01;
  return x;
}

wxString frcurrentsUIDialog::CalcCoefficient() {
  if (BrestRange == 999) return wxString(_("No Tidal Data for BREST"));

  double PMVE, PMME, BMVE, BMME;

  BMME = 2.70;

  BMVE = 1.15;

  PMME = 5.50;

  PMVE = 7.05;

  double U;

  U = 100 * (PMVE - BMVE) / 95;

  m_coeff = 100 * (BrestRange) / U;

  wxString cf = _("Coeff:");
  cf.Append(wxString::Format("  %3.0f", m_coeff));
  return cf;
}

void frcurrentsUIDialog::JumpToPort() {
  JumpToPosition(m_jumpLat, m_jumpLon, m_vp->view_scale_ppm);
}

int frcurrentsUIDialog::CalcHoursFromHWNow() {
  wxDateTime myDateTime;
  wxTimeSpan diff;
  double myDiff, myTest;

  myTest = 26;

  wxDateTime this_now = wxDateTime::Now().ToGMT();
  int t = this_now.GetTicks();
  int i = 0;
  int m;
  double d;

  for (i; i < 8; i++) {
    myDateTime = nearestHW[i];
    m = myDateTime.GetTicks();

    d = t - m;
    myDiff = (d / 60) / 60;

    if (abs(myDiff) < abs(myTest)) {
      myTest = myDiff;
    }
  }

  int c = m_choice2->GetCount();
  for (c = 0; c < 8; c++) {
    for (i = 0; i < 8; i++) {
      if (m_choice2_dt[c] == nearestHW[i]) {
        m_choice2->SetSelection(c);
      }
    }
  }
  int f = round(myTest);

  return f;
}

int frcurrentsUIDialog::CalcHoursFromLWNow() {
  wxDateTime myDateTime;
  wxTimeSpan diff;
  double myDiff, myTest;

  myTest = 26;

  wxDateTime this_now = wxDateTime::Now().ToGMT();
  int t = this_now.GetTicks();
  int i = 0;
  int m;
  double d;

  for (i; i < 8; i++) {
    myDateTime = nearestLW[i];
    m = myDateTime.GetTicks();

    d = t - m;
    myDiff = (d / 60) / 60;

    if (abs(myDiff) < abs(myTest)) {
      myTest = myDiff;
    }
  }

  int c = m_choice2->GetCount();
  for (c = 0; c < 8; c++) {
    for (i = 0; i < 8; i++) {
      if (m_choice2_dt[c] == nearestLW[i]) {
        m_choice2->SetSelection(c);
      }
    }
  }
  int f = round(myTest);

  return f;
}

int frcurrentsUIDialog::round(double c) {
  // c = -0.52
  int a = c;  // a =  0
  int b = 0;
  double input = c;

  if (a == 0) {
    if (c < 0) {
      c = c + a;  // -0.52
    } else {
      c = c - a;  //
    }
  } else {
    c = c - a;  //-2.6 --2 c = -0.6
  }

  if (abs(c) > 0.5) {
    b = 1;  // 1
  } else {
    b = 0;
  }

  if (a > 0)  // a -2
  {
    c = a + b;
  } else {
    if (a == 0) {
      if (input >= 0) {
        c = b;
      } else {
        c -= b;
      }
    } else {
      c = a - b;
    }
  }
  return c;
}

bool frcurrentsUIDialog::LoadStandardPorts() {
  wxFileName fn;
  wxString tmp_path;

  wxString m_gpx_path = fn.GetFullPath();
  int response = wxID_CANCEL;
  int my_count = 0;

  wxString filename;

  tmp_path = GetPluginDataDir("frcurrents_pi");
  fn.SetPath(tmp_path);
  fn.AppendDir("data");
  fn.SetFullName("StandardPorts.xml");

  filename = fn.GetFullPath();

  TiXmlDocument doc;

  if (!doc.LoadFile(filename)) {
    wxMessageBox(_("Failed to load file: ") + filename);
  }

  TiXmlElement* root = doc.RootElement();

  StandardPort my_port;
  my_ports.clear();

  int count = 0;

  wxString wxPORT_NUMBER, wxPORT_NAME, wxPORT_LAT, wxPORT_LON, PMVE, PMME, BMVE,
      BMME, IDX;

  int i = 0;
  for (TiXmlElement* e = root->FirstChildElement(); e;
       e = e->NextSiblingElement(), i++) {
    for (TiXmlElement* f = e->FirstChildElement(); f;
         f = f->NextSiblingElement()) {
      if (!strcmp(f->Value(), "PORT_NUMBER")) {
        wxPORT_NUMBER = wxString::FromUTF8(f->GetText());
        my_port.PORT_NUMBER = wxPORT_NUMBER;
      }

      if (!strcmp(f->Value(), "PORT_NAME")) {
        wxPORT_NAME = wxString::FromUTF8(f->GetText());
        my_port.PORT_NAME = wxPORT_NAME;
      }

      if (!strcmp(f->Value(), "LAT")) {
        wxPORT_LAT = wxString::FromUTF8(f->GetText());
        my_port.LAT = wxPORT_LAT;
      }

      if (!strcmp(f->Value(), "LON")) {
        wxPORT_LON = wxString::FromUTF8(f->GetText());
        my_port.LON = wxPORT_LON;
      }

      if (!strcmp(f->Value(), "PMVE")) {
        PMVE = wxString::FromUTF8(f->GetText());
        my_port.PMVE = PMVE;
        // wxMessageBox(my_port.PMVE,PMVE);
      }

      if (!strcmp(f->Value(), "PMME")) {
        PMME = wxString::FromUTF8(f->GetText());
        my_port.PMME = PMME;
      }

      if (!strcmp(f->Value(), "BMVE")) {
        BMVE = wxString::FromUTF8(f->GetText());
        my_port.BMVE = BMVE;
      }

      if (!strcmp(f->Value(), "BMME")) {
        BMME = wxString::FromUTF8(f->GetText());
        my_port.BMME = BMME;
      }

      if (!strcmp(f->Value(), "IDX")) {
        IDX = wxString::FromUTF8(f->GetText());
        my_port.IDX = IDX;

        // wxMessageBox(my_port.IDX,_("myPort"));
        my_ports.push_back(my_port);
      }

      // end for
    }  // end for
  }  // end for

  int c = my_ports.size();
  return true;
}

bool frcurrentsUIDialog::OpenXML() {
  wxFileName fn;
  wxString tmp_path;

  wxString m_gpx_path = fn.GetFullPath();
  int response = wxID_CANCEL;
  int my_count = 0;

  wxString filename;

  tmp_path = GetPluginDataDir("frcurrents_pi");
  fn.SetPath(tmp_path);
  fn.AppendDir("data");
  fn.SetFullName("tcSample.xml");

  filename = fn.GetFullPath();

  TiXmlDocument doc;
  /*  // Saved for writing to file method
      std::ofstream f("C:\\old\\x.txt",std::ios::out | std::ios::binary);
  // write to outfile
*/
  doc.LoadFile(filename);
  TiXmlElement* root = doc.RootElement();

  Position my_position;

  my_positions.clear();  // Clear all the diamonds of information

  int count = 0;

  wxString rte_lat, rte_lon;
  wxString stat_num, port_num, minus_6, minus_5, minus_4, minus_3, minus_2,
      minus_1, zero;
  wxString plus_1, plus_2, plus_3, plus_4, plus_5, plus_6;

  for (TiXmlElement* e = root->FirstChildElement(); e;
       e = e->NextSiblingElement())
    count++;
  int i = 0;
  for (TiXmlElement* e = root->FirstChildElement(); e;
       e = e->NextSiblingElement(), i++) {
    for (TiXmlElement* f = e->FirstChildElement(); f;
         f = f->NextSiblingElement()) {
      if (!strcmp(f->Value(), "LATITUDE")) {
        rte_lat = wxString::FromUTF8(f->GetText());
        my_position.lat = rte_lat;
      }

      if (!strcmp(f->Value(), "LONGITUDE")) {
        rte_lon = wxString::FromUTF8(f->GetText());
        my_position.lon = rte_lon;
      }
      if (!strcmp(f->Value(), "PORT_NUMBER")) {
        port_num = wxString::FromUTF8(f->GetText());
        my_position.port_num = port_num;
      }
      if (!strcmp(f->Value(), "MINUS_6")) {
        minus_6 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[0] = minus_6;
      }
      if (!strcmp(f->Value(), "MINUS_5")) {
        minus_5 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[1] = minus_5;
      }
      if (!strcmp(f->Value(), "MINUS_4")) {
        minus_4 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[2] = minus_4;
      }
      if (!strcmp(f->Value(), "MINUS_3")) {
        minus_3 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[3] = minus_3;
      }
      if (!strcmp(f->Value(), "MINUS_2")) {
        minus_2 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[4] = minus_2;
      }
      if (!strcmp(f->Value(), "MINUS_1")) {
        minus_1 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[5] = minus_1;
      }
      if (!strcmp(f->Value(), "ZERO")) {
        zero = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[6] = zero;
      }
      if (!strcmp(f->Value(), "PLUS_1")) {
        plus_1 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[7] = plus_1;
      }
      if (!strcmp(f->Value(), "PLUS_2")) {
        plus_2 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[8] = plus_2;
      }
      if (!strcmp(f->Value(), "PLUS_3")) {
        plus_3 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[9] = plus_3;
      }
      if (!strcmp(f->Value(), "PLUS_4")) {
        plus_4 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[10] = plus_4;
      }
      if (!strcmp(f->Value(), "PLUS_5")) {
        plus_5 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[11] = plus_5;
      }
      if (!strcmp(f->Value(), "PLUS_6")) {
        plus_6 = wxString::FromUTF8(f->GetText());
        my_position.minus_plus[12] = plus_6;

        my_positions.push_back(my_position);  // End of diamond information.
                                              // Push to diamond object
      }

      // PORT_NUMBER

    }  // end for

  }  // end for

  RequestRefresh(pParent);
  return true;
}

void frcurrentsUIDialog::OnAreaSelected(wxCommandEvent& event) {
  int a = m_choiceArea->GetSelection();
  m_AreaIDSelected = a;

  wxString s = m_Areas[a];

  int id = FindTidePortUsingChoice(s);  // populate m_choice1 (this area's ports list)

  m_choice1->SetSelection(id);
  wxString myP = m_choice1->GetString(id);
  m_PortSelected = myP;

  SetNow();
}

void frcurrentsUIDialog::GetCurrentsData(wxString areaFolder) {
  wxString area = areaFolder;
  GetCurrents(area, "");
}

void frcurrentsUIDialog::GetCurrents(wxString dirname, wxString filename) {
  wxArrayString filenames;
  wxFileName fn;
  wxString tmp_path;

  tmp_path = GetPluginDataDir("frcurrents_pi");
  fn.SetPath(tmp_path);
  fn.AppendDir("data");
  fn.SetFullName(dirname);

  wxDir dir(fn.GetFullPath());
  if (!dir.IsOpened()) {
    // deal with the error here - wxDir would already log an error message
    // explaining the exact reason of the failure
    return;
  }
  // puts("Enumerating object files in current directory:");
  wxString ffilename;
  bool cont = dir.GetFirst(&ffilename, "*", wxDIR_FILES);
  while (cont) {
    filenames.Add(ffilename);
    cont = dir.GetNext(&ffilename);
  }

  wxString result;
  my_positions.clear();
  int linenum = 0;

  int p = m_choice1->GetCurrentSelection();  // Get the port selected
  wxString s = m_choice1->GetString(p);

  int t = 0;
  wxString token[2];
  wxStringTokenizer tokenizer(s.Trim(), ",");
  while (tokenizer.HasMoreTokens()) {
    token[t] = tokenizer.GetNextToken();
    t++;
  }
  wxString tidePort = token[0];

  m_bUseBM = false;

  if (tidePort == "ILE de GROIX (Port Tudy)") {
    tidePort = "PORT-TUDY";
  } else if (tidePort == "LES SABLES-D'OLONNE") {
    tidePort = "LES SABLES D'OLONNE";
  } else if (tidePort == "LE HAVRE") {
    tidePort = "LE HAVRE.BM";
    m_bUseBM = true;
  } else if (tidePort == "LA ROCHELLE - LA PALLICE") {
    tidePort = "LA_ROCHELLE_BM";
    m_bUseBM = true;
  }
  if (m_bUseBM) {
    m_button5->SetLabel(_("LW"));
    m_staticTextHW->SetLabel(_("Low Water"));
    m_button4->SetLabel(_("LW-6"));
    m_button6->SetLabel(_("LW+6"));
    m_choice2->SetToolTip("Select Low Water");
  } else {
    m_button5->SetLabel(_("HW"));
    m_staticTextHW->SetLabel(_("High Water"));
    m_button4->SetLabel(_("HW-6"));
    m_button6->SetLabel(_("HW+6"));
    m_choice2->SetToolTip("Select High Water");
  }

  wxString filePort;
  // wxMessageBox(tidePort);

  for (size_t i = 0; i < filenames.size(); i++) {
    result = filenames.Item(i);
    wxString shareLocn = fn.GetFullPath() + fn.GetPathSeparator() + result;
    wxFileInputStream input(shareLocn);
    wxTextInputStream text(input);
    wxString line = text.ReadLine();
    filePort = line.MakeUpper().Trim();
    input.GetFile()->Close();
    if (filePort == tidePort) {
      ParseCurrentsFile(shareLocn);
    }
  }
}

void frcurrentsUIDialog::ParseCurrentsFile(wxString infile) {
  // wxMessageBox(infile);
  int linenum = 0;
  int line2 = 2;
  int line3 = 3;
  int line4 = 4;
  int s = 0;
  wxString token[4];
  Position testPosition;
  wxFileInputStream input(infile);
  wxTextInputStream text(input);
  while (input.IsOk() && !input.Eof()) {
    linenum++;
    wxString line = text.ReadLine();
    if (linenum == line2) {
      s = 0;
      wxStringTokenizer tokenizer(line.Trim());
      while (tokenizer.HasMoreTokens()) {
        token[s] = tokenizer.GetNextToken();
        s++;
      }
      testPosition.lat = token[0];
      testPosition.lon = token[1];
    }
    if (linenum == line3) {
      ParseCurrentsVE(line);
      for (int z = 0; z < 13; z++) {
        testPosition.spDir[z] = vectorAngle(PMVEns[z], PMVEew[z]);
        testPosition.spRate[z] = vectorSize(PMVEew[z], PMVEns[z]);
        if (isnan(testPosition.spRate[z])) {
          testPosition.spRate[z] = 0.01;
        }
      }
    }
    if (linenum == line4) {
      ParseCurrentsME(line);

      for (int z = 0; z < 13; z++) {
        testPosition.npDir[z] = vectorAngle(PMMEns[z], PMMEew[z]);
        testPosition.npRate[z] = vectorSize(PMMEew[z], PMMEns[z]);
        if (isnan(testPosition.npRate[z])) {
          testPosition.npRate[z] = .01;
        }
      }
      my_positions.push_back(testPosition);
    }

    if (linenum == line4) {
      line2 += 3;
      line3 += 3;
      line4 += 3;
    }

  }  // end of while loop
  input.GetFile()->Close();
}

void frcurrentsUIDialog::ParseCurrentsVE(wxString inCurrents) {
  wxString token[40];
  wxString s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
  token[0] = "";
  wxString currentN;
  wxString splits[2];
  int t = 0;
  wxStringTokenizer tokenizerS(inCurrents, "*");
  int is = 0;
  while (tokenizerS.HasMoreTokens()) {
    splits[is] = tokenizerS.GetNextToken();
    is++;
  }
  // 1 is ouest/est, 2 is nord/sud
  for (int n = 0; n < 2; n++) {
    currentN = splits[n];
    int i = 0;
    int ii = 0;
    wxStringTokenizer tokenizer(currentN, " ");
    while (tokenizer.HasMoreTokens()) {
      token[i] = tokenizer.GetNextToken();
      int len = token[i].length();
      wxString curr = token[i].Left(1);
      if (len > 2 && curr == "-") {
        //

        wxStringTokenizer tokenizer2(token[i], "-");
        while (tokenizer2.HasMoreTokens()) {
          token[ii] = tokenizer2.GetNextToken();
          if (token[ii].Trim() != "-") {
            token[ii].Prepend("-");
          }
          if (token[ii].Trim() != "-") {
            if (n == 0) {
              token[ii].Trim().ToDouble(&PMVEew[t]);
            } else if (n == 1) {
              token[ii].Trim().ToDouble(&PMVEns[t]);
            }
            t++;
          }
          ii++;
        }
      } else {
        if (token[i].Trim() != "-") {
          if (n == 0) {
            token[i].Trim().ToDouble(&PMVEew[t]);
          } else if (n == 1) {
            token[i].Trim().ToDouble(&PMVEns[t]);
          }
          t++;
        }
      }
      ii = 0;
      i++;
    }
    t = 0;
  }
}

void frcurrentsUIDialog::ParseCurrentsME(wxString inCurrents) {
  wxString token[40];
  wxString s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
  token[0] = "";
  wxString currentN;
  wxString splits[2];
  int t = 0;
  wxStringTokenizer tokenizerS(inCurrents, "*");
  int is = 0;
  while (tokenizerS.HasMoreTokens()) {
    splits[is] = tokenizerS.GetNextToken();
    is++;
  }
  // 1 is ouest/est, 2 is nord/sud
  for (int n = 0; n < 2; n++) {
    currentN = splits[n];
    int i = 0;
    int ii = 0;
    wxStringTokenizer tokenizer(currentN, " ");
    while (tokenizer.HasMoreTokens()) {
      token[i] = tokenizer.GetNextToken();
      int len = token[i].length();
      wxString curr = token[i].Left(1);
      // Test for long - string
      if (len > 2 && curr == "-") {
        //

        wxStringTokenizer tokenizer2(token[i], "-");
        while (tokenizer2.HasMoreTokens()) {
          token[ii] = tokenizer2.GetNextToken();
          if (token[ii].Trim() != "-") {
            token[ii].Prepend("-");
          }
          if (token[ii].Trim() != "-") {
            if (n == 0) {
              token[ii].Trim().ToDouble(&PMMEew[t]);
            } else if (n == 1) {
              token[ii].Trim().ToDouble(&PMMEns[t]);
            }
            t++;
          }
          ii++;
        }
      } else {
        if (token[i].Trim() != "-") {
          if (n == 0) {
            token[i].Trim().ToDouble(&PMMEew[t]);
          } else if (n == 1) {
            token[i].Trim().ToDouble(&PMMEns[t]);
          }
          t++;
        }
      }
      ii = 0;
      i++;
    }
    t = 0;
  }
}

vector<Position> frcurrentsUIDialog::OnRecord() { return my_positions; }

void frcurrentsUIDialog::SetFromHW(int fromHW) { button_id = fromHW; }

void frcurrentsUIDialog::OnChooseTideButton(wxCommandEvent& event) {
  m_staticText2->SetLabel("");
  m_staticText211->SetLabel("");

  int m = m_choice1->GetSelection();
  wxString s = m_choice1->GetString(m);
  m_portXML = FindPortXMLUsingChoice(s);

  if (m_portXML == "") {
    wxMessageBox(_("Port not found"), _("Port finder"));
    return;
  }

  wxString st_mydate;
  int i = FindPortIDUsingChoice(s);

  if (i == 0) {
    wxMessageBox(_("No Tidal Data"), _("Tidal Data Finder"));
    button_id = 6;
    return;
  }
  m_myChoice = m_choice2->GetSelection();
  st_mydate = m_choice2->GetString(m_myChoice);
  m_dt = m_choice2_dt[m_myChoice];
  m_ts = wxTimeSpan::Hours(0);
  wxString t1 = ("");
  button_id = event.GetId();  // Find which button was pushed (HW, HW-6, HW+6)`
  switch (button_id) {        // And make the label depending HW+6, HW-6 etc
    case 6: {
      m_myChoice = m_choice2->GetSelection();
      m_dt.Add(m_ts);
      if (st_mydate != "") {
        m_staticText2->SetLabel(m_dt.Format("%a %d %b %Y"));
        t1 = m_dt.Format("%H:%M") + "    ";
      }
      break;
    }
    case 0: {
      m_myChoice = m_choice2->GetSelection();
      m_ts = wxTimeSpan::Hours(6);
      m_dt.Subtract(m_ts);
      if (st_mydate != "") {
        m_staticText2->SetLabel(m_dt.Format("%a %d %b %Y"));
        t1 = m_dt.Format("%H:%M") + "    ";
      }
      break;
    }
    case 12: {
      m_myChoice = m_choice2->GetSelection();
      m_ts = wxTimeSpan::Hours(6);
      m_dt.Add(m_ts);
      if (st_mydate != "") {
        m_staticText2->SetLabel(m_dt.Format("%a %d %b %Y"));
        t1 = m_dt.Format("%H:%M") + "    ";
      }
      break;
    }
  }
  if (m_bUseBM) {
    m_staticText211->SetLabel(t1 + label_lw[button_id]);
  } else {
    m_staticText211->SetLabel(t1 + label_array[button_id]);
  }

  RequestRefresh(pParent);
}

void frcurrentsUIDialog::OnPrev(wxCommandEvent& event) {
  wxString st_mydate;

  int ma = m_choiceArea->GetCurrentSelection();
  wxString sa = m_Areas[ma];

  int p = m_choice1->GetSelection();  // Get the port selected
  wxString s = m_choice1->GetString(p);
  m_portXML = FindPortXMLUsingChoice(s);

  int f = FindPortIDUsingChoice(s);
  if (f == 0) return;

  int c = m_choice2->GetCount();
  if (c == 0) return;

  button_id--;

  // Test if we have gone beyond the current list of HW
  if (button_id == -1) {
    if (m_myChoice > 0) {  //  we stay in the same tide day
      m_myChoice--;
    } else {
      // we have gone beyond the current list of HW. so go to previous day
      wxDateTime myDate;
      wxDateSpan myOneDay = wxDateSpan::Days(1);
      myDate = m_datePicker1->GetValue();
      myDate.Subtract(myOneDay);
      m_datePicker1->SetValue(myDate);
      m_SelectedDate = myDate.GetDateOnly();

      if (m_bUseBM)
        CalcLW(f);
      else
        CalcHW(f);

      BrestRange = CalcRange_Brest();
      m_textCtrlCoefficient->SetValue(CalcCoefficient());

      c = m_choice2->GetCount();
      if (c == 0) return;
      m_myChoice = c - 1;
    }
    button_id = 12;
  }
  m_choice2->SetSelection(m_myChoice);
  // End of test.

  m_staticText2->SetLabel("");
  wxString t1 = _T("");
  st_mydate = m_choice2->GetString(m_myChoice);
  m_dt = m_choice2_dt[m_myChoice];
  m_dt.Subtract(wxTimeSpan::Hours(6));
  m_dt.Add(wxTimeSpan::Hours(button_id));
  if (st_mydate != "") {
    m_staticText2->SetLabel(m_dt.Format("%a %d %b %Y"));
    t1 = m_dt.Format("%H:%M") + "    ";
  }

  if (m_bUseBM) {
    m_staticText211->SetLabel(t1 + label_lw[button_id]);
  } else {
    m_staticText211->SetLabel(t1 + label_array[button_id]);
  }
  RequestRefresh(pParent);
}

void frcurrentsUIDialog::OnNext(wxCommandEvent& event) {
  wxString st_mydate;

  int ma = m_choiceArea->GetCurrentSelection();
  wxString sa = m_Areas[ma];
  int p = m_choice1->GetSelection();  // Get the port selected
  wxString s = m_choice1->GetString(p);

  int f = FindPortIDUsingChoice(s);
  if (f == 0) return;

  int c = m_choice2->GetCount();
  if (c == 0) return;

  button_id++;

  // Test if we have gone beyond the current list of HW
  if (button_id == 13) {
    if (m_myChoice < c - 1) {  //  we stay in the same tide day
      m_myChoice++;
    } else if (m_myChoice == c - 1) {
      // we have gone beyond the current list of HW. so go to next day
      wxDateTime myDate;
      wxDateSpan myOneDay = wxDateSpan::Days(1);
      myDate = m_datePicker1->GetValue();
      myDate.Add(myOneDay);
      m_datePicker1->SetValue(myDate);
      m_SelectedDate = myDate.GetDateOnly();

      if (m_bUseBM)
        CalcLW(f);
      else
        CalcHW(f);

      BrestRange = CalcRange_Brest();
      m_textCtrlCoefficient->SetValue(CalcCoefficient());

      c = m_choice2->GetCount();
      if (c == 0) return;
      m_myChoice = 0;
    }
    button_id = 0;
  }
  m_choice2->SetSelection(m_myChoice);
  // End of test.

  m_staticText2->SetLabel("");
  wxString t1 = _T("");
  st_mydate = m_choice2->GetString(m_myChoice);
  m_dt = m_choice2_dt[m_myChoice];
  m_dt.Subtract(wxTimeSpan::Hours(6));
  m_dt.Add(wxTimeSpan::Hours(button_id));
  if (st_mydate != "") {
    m_staticText2->SetLabel(m_dt.Format("%a %d %b %Y"));
    t1 = m_dt.Format("%H:%M") + "    ";
  }
  if (m_bUseBM) {
    m_staticText211->SetLabel(t1 + label_lw[button_id]);
  } else {
    m_staticText211->SetLabel(t1 + label_array[button_id]);
  }

  RequestRefresh(pParent);
}

void frcurrentsUIDialog::About(wxCommandEvent& event) {
  /*
 wxMessageBox(
_("Tidal Data for UKHO Tidal
Diamonds\n--------------------------------------------------------------\nThe
standard OpenCPN distribution has tidal data for the\nfollowing ports, which
this plugin uses:\n\nPLYMOUTH
(DEVONPORT)\nPORTSMOUTH\nDOVER\nSHEERNESS\nLOWESTOFT\nIMMINGHAM\nLEITH\nABERDEEN\nWICK\nLERWICK\nULLAPOOL\nLIVERPOOL
(GLADSTONE DOCK)\nHOLYHEAD\nMILFORD HAVEN\nPORT OF BRISTOL (AVONMOUTH)\nST.
HELIER\n\nUse this data with caution.\nUse in conjunction with UKHO Tidal Stream
Atlases and tidal
diamonds\n\n--------------------------------------------------------------------\n\nNote:
1 Rates shown are for a position corresponding to the centre\nof the base of the
arrow. Tidal rate is shown as knots.\nNote: 2 Rates are calculated by using the
method shown in UKHO Tidal\nStream Atlases. This is based on the average range
for the day\n") , _("About Tidal Arrows"), wxOK | wxICON_INFORMATION, this);*/

  wxFileName fn;

  auto path1 = GetPluginDataDir("frcurrents_pi");
  fn.SetPath(path1);
  fn.AppendDir("data");
  fn.SetFullName("\\console.exe");

  wxString g_sencutil_bin = fn.GetPath() + "\\console.exe";

  wxString cmd = g_sencutil_bin;
  cmd += " -c";
  wxMessageBox(cmd);

  wxProcess* process{new wxProcess(wxPROCESS_REDIRECT)};
  long pid = wxExecute(cmd, wxEXEC_ASYNC, process);
  if (!pid) {
    wxMessageBox("fail");
    delete process;
  } else {
    wxInputStream* iStream{process->GetInputStream()};
    wxString output;
    wxTextInputStream tiStream{*iStream};
    output = tiStream.ReadLine();

    while (!iStream->Eof()) {
      output += tiStream.ReadLine();
    }
    wxMessageBox(output);
  }
}