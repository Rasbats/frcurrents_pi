///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/choice.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/bmpbuttn.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/clrpicker.h>
#include <wx/event.h>
#include <wx/slider.h>
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class frcurrentsUIDialogBase
///////////////////////////////////////////////////////////////////////////////
class frcurrentsUIDialogBase : public wxDialog {
private:
protected:
  wxBitmapButton* m_button8;
  wxDatePickerCtrl* m_datePicker1;
  wxStaticText* m_staticTextHW;
  wxChoice* m_choice2;
  wxBitmapButton* m_bpNow;
  wxBitmapButton* m_bpPrev;
  wxBitmapButton* m_bpNext;
  wxButton* m_button5;
  wxButton* m_button4;
  wxButton* m_button6;
  wxButton* m_button7;

  // Virtual event handlers, override them in your derived class
  virtual void OnClose(wxCloseEvent& event) { event.Skip(); }
  virtual void OnDLeftClick(wxMouseEvent& event) { event.Skip(); }
  virtual void OnMove(wxMoveEvent& event) { event.Skip(); }
  virtual void OnSize(wxSizeEvent& event) { event.Skip(); }
  virtual void OnAreaSelected(wxCommandEvent& event) { event.Skip(); }
  virtual void OnInformation(wxCommandEvent& event) { event.Skip(); }
  virtual void OnPortChanged(wxCommandEvent& event) { event.Skip(); }
  virtual void OnDateSelChanged(wxDateEvent& event) { event.Skip(); }
  virtual void OnNow(wxCommandEvent& event) { event.Skip(); }
  virtual void OnPrev(wxCommandEvent& event) { event.Skip(); }
  virtual void OnNext(wxCommandEvent& event) { event.Skip(); }
  virtual void OnChooseTideButton(wxCommandEvent& event) { event.Skip(); }
  virtual void OnSelectData(wxCommandEvent& event) { event.Skip(); }
  virtual void OnContextMenu(wxContextMenuEvent& event) { event.Skip(); }
  virtual void OnContextMenuSelect(wxCommandEvent& event) { event.Skip(); }
  virtual void OnMouseEvent(wxMouseEvent& event) { event.Skip(); }

public:
  wxChoice* m_choiceArea;
  wxChoice* m_choice1;
  wxTextCtrl* m_textCtrlCoefficient;
  wxStaticBoxSizer* sbSizerFolder;
  wxTextCtrl* m_dirPicker1;
  wxStaticText* m_staticText2;
  wxStaticText* m_staticText211;
  wxStaticText* m_staticText1;

  frcurrentsUIDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY,
                         const wxString& title = _("French Tidal Streams"),
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxSize(-1, -1),
                         long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

  ~frcurrentsUIDialogBase();
};

///////////////////////////////////////////////////////////////////////////////
/// Class frcurrentsPreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class frcurrentsPreferencesDialogBase : public wxDialog {
private:
  wxStaticText* staticTextLess05;
  wxStaticText* staticText0515;
  wxStaticText* staticText1525;
  wxStaticText* staticText2535;
  wxStaticText* m_staticTextMore35;

protected:
  wxStaticText* m_staticText8;
  wxStaticText* m_staticText13;
  wxStdDialogButtonSizer* m_sdbSizerButtons;
  wxButton* m_sdbSizerButtonsOK;
  wxButton* m_sdbSizerButtonsCancel;

  // Virtual event handlers, override them in your derived class
  virtual void OnChoice(wxCommandEvent& event) { event.Skip(); }
  virtual void OnIconsSlidersChange(wxCommandEvent& event) { event.Skip(); }
  virtual void OnFontSlidersChange(wxCommandEvent & event) { event.Skip(); }
  virtual void OnColourChanged(wxColourPickerEvent & event) {
    event.Skip();
  }


public:
  wxCheckBox* m_cbUseRate;
  wxCheckBox* m_cbUseDirection;
  wxCheckBox* m_cbFillColour;
  wxCheckBox* m_cbUseHighRes;
  wxColourPickerCtrl* myColourPicker0;
  wxColourPickerCtrl* myColourPicker1;
  wxColourPickerCtrl* myColourPicker2;
  wxColourPickerCtrl* myColourPicker3;
  wxColourPickerCtrl* myColourPicker4;
  wxChoice* m_cStyle;
  wxSlider* m_sIconSizeFactor;
  wxSlider * m_sFontSizeFactor;

  frcurrentsPreferencesDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY,
                                  const wxString& title = _("Preferences"),
                                  const wxPoint& pos = wxDefaultPosition,
                                  const wxSize& size = wxSize(-1, -1),
                                  long style = wxCAPTION | wxRESIZE_BORDER);

  ~frcurrentsPreferencesDialogBase();
};
