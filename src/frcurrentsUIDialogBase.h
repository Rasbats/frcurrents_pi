///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
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
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>

///////////////////////////////////////////////////////////////////////////

#define 6 1000
#define 0 1001
#define 12 1002

///////////////////////////////////////////////////////////////////////////////
/// Class frcurrentsUIDialogBase
///////////////////////////////////////////////////////////////////////////////
class frcurrentsUIDialogBase : public wxDialog
{
	private:

	protected:
		wxDatePickerCtrl* m_datePicker1;
		wxButton* m_button55;
		wxChoice* m_choice2;
		wxBitmapButton* m_bpNow;
		wxStaticText* m_staticText1;
		wxBitmapButton* m_bpPrev;
		wxBitmapButton* m_bpNext;
		wxButton* m_button5;
		wxButton* m_button4;
		wxButton* m_button6;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText211;
		wxStaticLine* m_staticline1;
		wxButton* m_button7;
		wxBitmapButton* m_bXML;
		wxButton* m_button51;
		wxButton* m_button61;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnPortChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDateSelChanged( wxDateEvent& event ) { event.Skip(); }
		virtual void About( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNow( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPrev( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNext( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChooseTideButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectData( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnXML( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileNames( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxChoice* m_choice1;
		wxTextCtrl* m_textCtrlCoefficient;
		wxTextCtrl* m_dirPicker1;
		wxStaticBoxSizer* sbSizer7;

		frcurrentsUIDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

		~frcurrentsUIDialogBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class frcurrentsPreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class frcurrentsPreferencesDialogBase : public wxDialog
{
	private:

	protected:
		wxStdDialogButtonSizer* m_sdbSizerButtons;
		wxButton* m_sdbSizerButtonsOK;
		wxButton* m_sdbSizerButtonsCancel;

	public:
		wxCheckBox* m_cbUseRate;
		wxCheckBox* m_cbUseDirection;
		wxCheckBox* m_cbFillColour;

		frcurrentsPreferencesDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 235,442 ), long style = wxCAPTION );

		~frcurrentsPreferencesDialogBase();

};

