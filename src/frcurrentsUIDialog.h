/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  frcurrents Plugin Friends
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
 */

#ifndef __frcurrentsUIDIALOG_H__
#define __frcurrentsUIDIALOG_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif  // precompiled headers
#include <wx/fileconf.h>
#include <wx/glcanvas.h>

#include "frcurrentsUIDialogBase.h"
#include "tinyxml.h"
#include <list>
#include <vector>
#include <wx/progdlg.h>

#include "ocpn_plugin.h"
#include "tcmgr.h"
#include "wx/dateevt.h"
#include "wx/process.h"
#include "wx/stattext.h"
#include "wx/utils.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <wx/dir.h>
#include <wx/numdlg.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <wx/statbox.h>
#include <wx/event.h>
#include "ocpn_plugin.h"


enum { ID_DASH_PREFS = 999, ID_DASH_RESIZE };


using namespace std;

#ifndef PI
#define PI 3.1415926535897931160E0 /* pi */
#endif

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif

#define RT_RCDATA2 MAKEINTRESOURCE(999)

class frcurrentsOverlayFactory;
class PlugIn_ViewPort;
class PositionRecordSet;

class wxFileConfig;
class frcurrents_pi;
class wxGraphicsContext;

class SHOMport {
public:
  wxString name;
  wxString lat, lon;
  wxString PMVE, BMVE, PMME, BMME;
  wxArrayString PMVEew, PMVEns, PMMEew, PMMEns, BMVEew, BMVEns, BMMEew, BMMEns;
};

class Position {
public:
  double latD, lonD;
  wxString lat, lon;
  wxString stat_num, port_num;
  wxString minus_plus[13];
  double spRate[13];
  double npRate[13];
  double spDir[13];
  double npDir[13];
  wxString minus_6, minus_5, minus_4, minus_3, minus_2, minus_1, zero;
  wxString plus_1, plus_2, plus_3, plus_4, plus_5, plus_6;
  Position *prev, *next; /* doubly linked circular list of positions */
};

class PositionSHOM {
public:
  double latD, lonD;
  wxString lat, lon;
};

class PortTides {
public:
  double latlon;
  wxString m_portID, m_portName, m_portLat, m_portLon, m_IDX;
  double m_spRange, m_npRange;
  double PMVE, PMME, BMVE, BMME;
  double dirVE[13], dirME[13], rateVE[13], rateME[13];
};

class StandardPort {
public:
  wxString PORT_NUMBER, PORT_NAME, LAT, LON, IDX, PMVE, PMME, BMVE, BMME;
  double spRange, npRange;
};

class frcurrentsUIDialog : public frcurrentsUIDialogBase {
public:
  frcurrentsUIDialog(wxWindow* parent, frcurrents_pi* ppi);
  ~frcurrentsUIDialog();

  void OpenFile(bool newestFile = false);

  void SetCursorLatLon(double lat, double lon);
  void SetFactoryOptions(bool set_val = false);

  void SetViewPort(PlugIn_ViewPort* vp);
  PlugIn_ViewPort* vp;

  bool m_bUseRate;
  bool m_bUseDirection;
  bool m_bUseHighRes;
  bool m_bUseFillColour;
  int m_UseArrowStyle;

  wxString myUseColour[5];

  vector<Position> my_positions;
  vector<StandardPort> my_ports;
  wxString wxPortName[100][5];
  wxString selectedPort;

  SHOMport myPort;

  vector<Position> my_points;
  vector<Position> OnRecord();
  void SetFromHW(int fromHW);

  int press_next_id;
  int press_prev_id;
  int button_id;
  wxString m_portXML;
  double myRange;
  double BrestRange;
  float myHeightHW;
  float myHeightLW;
  float BrestHeightHW;
  float BrestHeightLW;
  StandardPort PopulatePortTides(wxString PortName);
  double CalcCurrent(double VE, double ME, double spRate, double npRate,
                     double coefficient);
  double CalcCoefficient();
  int CalcHoursFromHWNow();
  int CalcHoursFromLWNow();
  wxString nearestHW[8];
  wxString nearestLW[8];
  bool m_bUseBM;
  int round(double c);

  wxString m_PortSelected;
  wxString m_AreaSelected;
  wxArrayString TideCurrentDataSet;
  wxString* pTC_Dir;
  vector<SHOMport> portLines;
  wxString g_SData_Locn;
  TCMgr* ptcmgr;
  wxString m_FolderSelected;
  double m_coeff;
  double m_jumpLat, m_jumpLon;
  
 // void OnMouseEvent(wxMouseEvent& event);
  /*
  wxPoint m_resizeStartPoint;
  wxSize m_resizeStartSize;
  bool m_binResize;
  bool m_binResize2;
  int eventid;*/

#ifdef __ANDROID__
  void OnContextMenu(wxContextMenuEvent& event);
  void OnContextMenuSelect(wxCommandEvent& event);


  void OnMouseEvent(wxMouseEvent& event);
  wxPoint m_resizeStartPoint;
  wxSize m_resizeStartSize;
  bool m_binResize;
  bool m_binResize2;

  void OnPopupClick(wxCommandEvent& evt);
  void OnRightClick(wxMouseEvent& event);

#endif

private:
  void OnClose(wxCloseEvent& event);
  void OnMove(wxMoveEvent& event);
  // void OnSize( wxSizeEvent& event );
  bool m_bOnStart;
  void OnStartSetupHW();
  void OnNow(wxCommandEvent& event);

  void CalcHW(int PortCode);
  void CalcLW(int PortCode);
  double CalcRange_Brest();
  void SetNow();
  void SetCorrectHWSelection();
  void OnDateSelChanged(wxDateEvent& event);
  void OnPortChanged(wxCommandEvent& event);
  void OnPortListed();
  void SetDateForNowButton();

  void OnInformation(wxCommandEvent& event);
  wxString FindPortXMLUsingChoice(wxString inPortName);

  int FindPortIDUsingChoice(wxString inPortName);
  int FindTidePortUsingChoice(wxString inAreaNumber);

  void OnSelectData(wxCommandEvent& event);
  void LoadTCMFile();
  void LoadHarmonics();
  int FindPortID(wxString myPort);
  bool LoadStandardPorts();
  bool OpenXML();
  void OnAreaSelected(wxCommandEvent& event);
  void GetCurrentsData(wxString areaFolder);
  void ParseCurrentsFile(wxString infile);
  void ParseCurrentsVE(wxString inCurrents);
  void ParseCurrentsME(wxString inCurrents);
  void OnChooseTideButton(wxCommandEvent& event);
  void OnPrev(wxCommandEvent& event);
  void OnNext(wxCommandEvent& event);
  void About(wxCommandEvent& event);
  // Time/HW
  int myPortCode;
  int BrestID;
  void SetTimeFactors();
  bool btc_valid;
  int m_stationOffset_mins;
  int m_diff_mins;
  wxString m_stz;
  int m_t_graphday_GMT;
  wxDateTime m_graphday;
  int m_tzoneDisplay;

  //
  //    Data
  wxWindow* pParent;
  frcurrents_pi* pPlugIn;

  PlugIn_ViewPort* m_vp;

  // preference data
  bool m_bfrcurrentsUseHiDef;
  bool m_bfrcurrentsUseGradualColors;
  bool m_bCopyUseRate;
  bool m_bCopyUseDirection;
  bool m_bCopyUseHighRes;

  int m_lastdatatype;

  double m_cursor_lat, m_cursor_lon;

  int next_id;
  int back_id;
  // For the tide bit
  // wxDateTime m_dt;
  int m_myChoice;
  wxString label_array[13];
  wxString label_lw[13];
  float tcv[26];

  int myDateSelection;
  wxDateTime m_SelectedDate; //  to store the current selected date
  wxString euTC[8][4];  // Date.Time, Height, Units, HW.LW
  wxDateTime m_dt;
  wxDateTime choice_dt;
  wxDateTime back_dt;
  wxDateTime next_dt;
  wxTimeSpan m_ts;
  int m_plot_type;
  void JumpToPort();

  bool isNowButton;
  bool m_bAtLastChoice;
  bool m_bAtLastPrev;
  bool m_bPrev, m_bNext;
  bool m_bChooseTide;
  int track_id;

  double PMVEew[13], PMVEns[13], PMMEew[13], PMMEns[13], BMVEew[13], BMVEns[13],
      BMMEew[13], BMMEns[13];
  void GetCurrents(wxString dirname, wxString filename);
};

#endif
