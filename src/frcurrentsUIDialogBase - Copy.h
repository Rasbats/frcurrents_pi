///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __frcurrentsUIDIALOGBASE_H__
#define __frcurrentsUIDIALOGBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/slider.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/radiobox.h>
#include <wx/datectrl.h>
#include "wx/dateevt.h"
#include <wx/event.h>
#include <wx/listbox.h>

///////////////////////////////////////////////////////////////////////////

#define ID_TIMELINE 1000
#define ID_CB_WIND_SPEED 1001
#define ID_CB_SIG_WAVE_HEIGHT 1002
#define ID_CB_CURRENT_VELOCITY 1003
#define ID_CB_PRESSURE 1004
#define ID_CB_SEA_TEMPERATURE 1005

///////////////////////////////////////////////////////////////////////////////
/// Class frcurrentsUIDialogBase
///////////////////////////////////////////////////////////////////////////////
class frcurrentsUIDialogBase : public wxDialog 
{
	private:
	
	protected:
		//wxBitmapButton* m_bpStart;
		wxBitmapButton* m_bpPrev;
		//wxStaticText* m_staticText24;
		wxChoice* m_choice2;
		wxDatePickerCtrl* m_datePicker1;
		
		//wxComboBox* m_cRecordForecast;
		wxBitmapButton* m_bpNext;
		//wxStaticText* m_staticText21; 
		wxBitmapButton* m_bpNow;
		wxBitmapButton* m_bpPlay;
        wxButton* m_bXML;

		wxButton* m_button33;
		wxBitmapButton* m_bpButton8;
		wxBitmapButton* m_bpButton9;
		//wxListBox* m_pitemCurrentKMLDirectoryCtrl;
		wxButton* m_button5;
		wxButton* m_button4;
		wxButton* m_button6;
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText211;
		wxFlexGridSizer* m_fgTrackingControls;
		wxTextCtrl* m_tcWindSpeed;
		wxTextCtrl* m_tcWindDirection;
		wxTextCtrl* m_tcWaveHeight;
		wxTextCtrl* m_tcWaveDirection;
		wxTextCtrl* m_tcCurrentVelocity;
		wxTextCtrl* m_tcCurrentDirection;
		wxTextCtrl* m_tcWindGust;
		wxTextCtrl* m_tcPressure;
		wxTextCtrl* m_tcPrecipitation;
		wxTextCtrl* m_tcCloud;
		wxTextCtrl* m_tcAirTemperature;
		wxTextCtrl* m_tcSeaTemperature;
		wxTextCtrl* m_tcCAPE;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void OnDateSelChanged(wxDateEvent& event) { event.Skip(); }
		virtual void OnPortChanged(wxCommandEvent& event) { event.Skip(); }
		virtual void OnChooseTideButton(wxCommandEvent& event) { event.Skip(); }
		virtual void OnPrev( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNext( wxCommandEvent& event ) { event.Skip(); }
        virtual void OnXML(wxCommandEvent& event) { event.Skip(); }

		virtual void About( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNow( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOpenFile( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxSlider* m_sTimeline;
		wxCheckBox* m_cbWind;
		wxCheckBox* m_cbWave;
		wxCheckBox* m_cbCurrent;
		wxCheckBox* m_cbWindGust;
		wxCheckBox* m_cbPressure;
		wxCheckBox* m_cbPrecipitation;
		wxCheckBox* m_cbCloud;
		wxCheckBox* m_cbAirTemperature;
		wxCheckBox* m_cbSeaTemperature;
		wxCheckBox* m_cbCAPE;
	
		wxChoice* m_choice1;

		frcurrentsUIDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tidal Current Control"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxCLOSE_BOX|wxDIALOG_NO_PARENT|wxRESIZE_BORDER|wxSYSTEM_MENU|wxSTAY_ON_TOP); 
		~frcurrentsUIDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class frcurrentsSettingsDialogBase
///////////////////////////////////////////////////////////////////////////////
class frcurrentsSettingsDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxSpinCtrl* m_sUpdatesPerSecond;
		wxStaticText* m_tSlicesPerUpdate;
		wxSpinCtrl* m_sSlicesPerUpdate;
		wxStaticText* m_tHourDivider;
		wxChoice* m_cDataType;
		wxChoice* m_cDataUnits;
		wxCheckBox* m_cbBarbedArrows;
		wxStaticText* m_tBarbedRange;
		wxSpinCtrl* m_sBarbedRange;
		wxCheckBox* m_cbIsoBars;
		wxStaticText* m_tIsoBarSpacing;
		wxSpinCtrl* m_sIsoBarSpacing;
		wxCheckBox* m_cbDirectionArrows;
		wxStaticText* m_tDirectionArrowSize;
		wxSpinCtrl* m_sDirectionArrowSize;
		wxCheckBox* m_cbOverlayMap;
		wxStaticText* m_tOverlayColors;
		wxChoice* m_cOverlayColors;
		wxCheckBox* m_cbNumbers;
		wxSpinCtrl* m_sNumbersSpacing;
		wxStaticLine* m_staticline1;
		wxStaticLine* m_staticline2;
		wxStaticLine* m_staticline3;
		wxStaticText* m_staticText24;
		wxSlider* m_sTransparency;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnIntepolateChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDataTypeChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTransparencyChange( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnApply( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxCheckBox* m_cLoopMode;
		wxCheckBox* m_cInterpolate;
		wxSpinCtrl* m_sHourDivider;
		wxStdDialogButtonSizer* m_sButton;
		wxButton* m_sButtonOK;
		wxButton* m_sButtonApply;
		wxButton* m_sButtonCancel;
		
		frcurrentsSettingsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("frcurrents Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~frcurrentsSettingsDialogBase();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class frcurrentsPreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class frcurrentsPreferencesDialogBase : public wxDialog 
{
	private:
	
	protected:
		
		// Virtual event handlers, overide them in your derived class
		
		
	
	public:
		wxCheckBox* m_cbUseRate;
		wxCheckBox* m_cbUseDirection;
		wxCheckBox* m_cbFillColour;

		wxCheckBox* m_cbCopyFirstCumulativeRecord;
		wxCheckBox* m_cbCopyMissingWaveRecord;
		wxRadioBox* m_rbLoadOptions;
		wxRadioBox* m_rbStartOptions;
		wxRadioBox* m_rbTimeFormat;
		
		frcurrentsPreferencesDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE ); 
		~frcurrentsPreferencesDialogBase();
	
};


#endif //__frcurrentsUIDIALOGBASE_H__
