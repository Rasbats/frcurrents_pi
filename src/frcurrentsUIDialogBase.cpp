///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "frcurrentsUIDialogBase.h"

///////////////////////////////////////////////////////////////////////////

frcurrentsUIDialogBase::frcurrentsUIDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Standard Port") ), wxVERTICAL );

	wxArrayString m_choice1Choices;
	m_choice1 = new wxChoice( sbSizer6->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	sbSizer6->Add( m_choice1, 0, wxALL|wxEXPAND, 5 );


	bSizerMain->Add( sbSizer6, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizerDateTime;
	sbSizerDateTime = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Choose Date") ), wxHORIZONTAL );

	m_datePicker1 = new wxDatePickerCtrl( sbSizerDateTime->GetStaticBox(), wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT );
	m_datePicker1->SetFont( wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial") ) );

	sbSizerDateTime->Add( m_datePicker1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_textCtrlCoefficient = new wxTextCtrl( sbSizerDateTime->GetStaticBox(), wxID_ANY, _("   Coeff"), wxDefaultPosition, wxDefaultSize, wxTE_BESTWRAP );
	m_textCtrlCoefficient->SetFont( wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial") ) );

	sbSizerDateTime->Add( m_textCtrlCoefficient, 0, wxALIGN_CENTER_VERTICAL, 5 );


	sbSizerDateTime->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button55 = new wxButton( sbSizerDateTime->GetStaticBox(), wxID_ANY, _("???"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerDateTime->Add( m_button55, 0, wxALL, 5 );


	bSizerMain->Add( sbSizerDateTime, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizerFolder;
	sbSizerFolder = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Select High Water") ), wxVERTICAL );

	wxArrayString m_choice2Choices;
	m_choice2 = new wxChoice( sbSizerFolder->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice2Choices, 0 );
	m_choice2->SetSelection( 0 );
	sbSizerFolder->Add( m_choice2, 0, wxALL|wxEXPAND, 5 );


	bSizerMain->Add( sbSizerFolder, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizerControls;
	sbSizerControls = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxHORIZONTAL );

	m_bpNow = new wxBitmapButton( sbSizerControls->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_bpNow->SetToolTip( _("Now") );
	m_bpNow->SetMinSize( wxSize( 45,45 ) );

	sbSizerControls->Add( m_bpNow, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );


	sbSizerControls->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText1 = new wxStaticText( sbSizerControls->GetStaticBox(), wxID_ANY, _("Time Zone"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetFont( wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Arial") ) );

	sbSizerControls->Add( m_staticText1, 0, wxALL, 5 );


	sbSizerControls->Add( 5, 0, 1, wxEXPAND, 5 );

	m_bpPrev = new wxBitmapButton( sbSizerControls->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW|0 );
	m_bpPrev->SetToolTip( _("Previous") );
	m_bpPrev->SetMinSize( wxSize( 40,40 ) );

	sbSizerControls->Add( m_bpPrev, 0, wxALL|wxEXPAND, 5 );

	m_bpNext = new wxBitmapButton( sbSizerControls->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW|0 );
	m_bpNext->SetToolTip( _("Next") );
	m_bpNext->SetMinSize( wxSize( 40,40 ) );

	sbSizerControls->Add( m_bpNext, 0, wxALL|wxEXPAND, 5 );


	bSizerMain->Add( sbSizerControls, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );


	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button5 = new wxButton( this, 6, _("HW"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button5, 0, wxALL, 5 );


	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button4 = new wxButton( this, 0, _("HW -6"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button4, 0, wxALL, 5 );


	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button6 = new wxButton( this, 12, _("HW +6"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button6, 0, wxALL, 5 );


	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizerMain->Add( bSizer4, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Display date/time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer5->Add( m_staticText2, 0, wxALL, 5 );

	m_staticText211 = new wxStaticText( this, wxID_ANY, _("High Water +-"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText211->Wrap( -1 );
	bSizer5->Add( m_staticText211, 0, wxALL, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer5->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );


	bSizerMain->Add( bSizer5, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer61;
	sbSizer61 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Tide Harmonics IDX Directory") ), wxVERTICAL );

	m_button7 = new wxButton( sbSizer61->GetStaticBox(), wxID_ANY, _("Select Directory"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer61->Add( m_button7, 0, wxALL, 5 );

	m_dirPicker1 = new wxTextCtrl( sbSizer61->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer61->Add( m_dirPicker1, 0, wxALL|wxEXPAND, 5 );


	bSizerMain->Add( sbSizer61, 1, wxEXPAND, 5 );

	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Tests") ), wxHORIZONTAL );

	m_bXML = new wxBitmapButton( sbSizer7->GetStaticBox(), wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,-1 ), wxBU_AUTODRAW|0 );
	m_bXML->SetToolTip( _("XML") );
	m_bXML->SetMinSize( wxSize( 40,40 ) );

	sbSizer7->Add( m_bXML, 0, wxALL, 5 );

	m_button51 = new wxButton( sbSizer7->GetStaticBox(), wxID_ANY, _("Read File"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer7->Add( m_button51, 0, wxALL, 5 );

	m_button61 = new wxButton( sbSizer7->GetStaticBox(), wxID_ANY, _("Load filenames"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer7->Add( m_button61, 0, wxALL, 5 );


	bSizerMain->Add( sbSizer7, 0, wxEXPAND, 5 );


	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( frcurrentsUIDialogBase::OnClose ) );
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( frcurrentsUIDialogBase::OnSize ) );
	m_choice1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( frcurrentsUIDialogBase::OnPortChanged ), NULL, this );
	m_datePicker1->Connect( wxEVT_DATE_CHANGED, wxDateEventHandler( frcurrentsUIDialogBase::OnDateSelChanged ), NULL, this );
	m_button55->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::About ), NULL, this );
	m_bpNow->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnNow ), NULL, this );
	m_bpPrev->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnPrev ), NULL, this );
	m_bpNext->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnNext ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnChooseTideButton ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnChooseTideButton ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnChooseTideButton ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnSelectData ), NULL, this );
	m_bXML->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnXML ), NULL, this );
	m_button51->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnFile ), NULL, this );
	m_button61->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnFileNames ), NULL, this );
}

frcurrentsUIDialogBase::~frcurrentsUIDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( frcurrentsUIDialogBase::OnClose ) );
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( frcurrentsUIDialogBase::OnSize ) );
	m_choice1->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( frcurrentsUIDialogBase::OnPortChanged ), NULL, this );
	m_datePicker1->Disconnect( wxEVT_DATE_CHANGED, wxDateEventHandler( frcurrentsUIDialogBase::OnDateSelChanged ), NULL, this );
	m_button55->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::About ), NULL, this );
	m_bpNow->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnNow ), NULL, this );
	m_bpPrev->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnPrev ), NULL, this );
	m_bpNext->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnNext ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnChooseTideButton ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnChooseTideButton ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnChooseTideButton ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnSelectData ), NULL, this );
	m_bXML->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnXML ), NULL, this );
	m_button51->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnFile ), NULL, this );
	m_button61->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnFileNames ), NULL, this );

}

frcurrentsPreferencesDialogBase::frcurrentsPreferencesDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizerCheckboxes;
	bSizerCheckboxes = new wxBoxSizer( wxVERTICAL );

	m_cbUseRate = new wxCheckBox( this, wxID_ANY, _("Display tidal rates (knots)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerCheckboxes->Add( m_cbUseRate, 0, wxALL, 5 );

	m_cbUseDirection = new wxCheckBox( this, wxID_ANY, _("Display tidal direction"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerCheckboxes->Add( m_cbUseDirection, 0, wxALL, 5 );

	m_cbFillColour = new wxCheckBox( this, wxID_ANY, _("Fill Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerCheckboxes->Add( m_cbFillColour, 0, wxALL, 5 );


	bSizerMain->Add( bSizerCheckboxes, 0, wxALL|wxEXPAND, 5 );

	m_sdbSizerButtons = new wxStdDialogButtonSizer();
	m_sdbSizerButtonsOK = new wxButton( this, wxID_OK );
	m_sdbSizerButtons->AddButton( m_sdbSizerButtonsOK );
	m_sdbSizerButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizerButtons->AddButton( m_sdbSizerButtonsCancel );
	m_sdbSizerButtons->Realize();

	bSizerMain->Add( m_sdbSizerButtons, 0, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizerMain );
	this->Layout();

	this->Centre( wxBOTH );
}

frcurrentsPreferencesDialogBase::~frcurrentsPreferencesDialogBase()
{
}
