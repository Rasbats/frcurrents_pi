///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "frcurrentsUIDialogBase.h"

///////////////////////////////////////////////////////////////////////////

frcurrentsUIDialogBase::frcurrentsUIDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Standard Port") ), wxHORIZONTAL );
	
	sbSizer2->SetMinSize( wxSize( 215, -1 ) ); 

	wxArrayString m_choice1Choices ;
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1Choices, 0);	
		
	sbSizer2->Add( m_choice1, 0, wxALL, 5 );
	
	bSizer1->Add( sbSizer2, 0, 0, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Choose Date/Time") ), wxHORIZONTAL );
	
	sbSizer3->SetMinSize( wxSize( -1,30 ) ); 
	m_datePicker1 = new wxDatePickerCtrl( this, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN );
	sbSizer3->Add( m_datePicker1, 0, wxALL, 5 );

	sbSizer3->Add( 15, 0, 1, wxEXPAND, 5 );
		// Start About Information Button
	m_button33 = new wxButton( this, wxID_ANY , _("???"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_button33->SetToolTip(_("About these Tidal Arrows"));	
	sbSizer3->Add( m_button33, 0, wxALL, 5 );

	bSizer1->Add( sbSizer3, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY,_("Select High Water")), wxVERTICAL );
	
	sbSizer4->SetMinSize( wxSize( -1,50 ) ); 
	
	wxArrayString m_choiceHWTime;
	m_choice2 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceHWTime, 0 );
	m_choice2->SetSelection( -1 );
	sbSizer4->Add( m_choice2, 0, wxALL, 5 );
	
	bSizer1->Add( sbSizer4, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	bSizer1->Add( bSizer2, 0, 0, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_button5 = new wxButton( this, 6, _("HW"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	bSizer3->Add( m_button5, 0, wxALL, 5 );
	
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
           wxCommandEventHandler( frcurrentsUIDialogBase::OnChooseTideButton), NULL, this );
   	
	m_button4 = new wxButton( this, 0 , _("HW -6"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	bSizer3->Add( m_button4, 0, wxALL, 5 );

	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler( frcurrentsUIDialogBase::OnChooseTideButton), NULL, this );
	
	m_button6 = new wxButton( this, 12 , _("HW +6"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	bSizer3->Add( m_button6, 0, wxALL, 5 );

	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler( frcurrentsUIDialogBase::OnChooseTideButton), NULL, this );
	
	bSizer1->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bpNow = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpNow->SetToolTip( _("Now") );
	
	bSizer7->Add( m_bpNow, 0, wxALL, 1 );

	bSizer7->Add( 20, 0, 1, wxEXPAND, 5 );

	m_bpPrev = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 30,-1 ), 0  );
	m_bpPrev->SetToolTip( _("Previous") );	
	bSizer7->Add( m_bpPrev, 0, wxALL, 1 );
	
	bSizer7->Add( 20, 0, 1, wxEXPAND, 5 );

	m_bpNext = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 30,-1 ), 0  );
	m_bpNext->SetToolTip( _("Next") );
	
	bSizer7->Add( m_bpNext, 0, wxALL, 1 );

	m_bXML = new wxButton(this, wxID_ANY, "testXML",  wxDefaultPosition, wxSize(40, -1), 0);
    m_bXML->SetToolTip(_("XML"));

    bSizer7->Add(m_bXML, 0, wxALL, 5);


	
	bSizer1->Add( bSizer7, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize( -1,-1 ), wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	m_staticText1->Wrap( -1 );
	m_staticText1->SetFont( wxFont( 10, 74, 90, 90, false, wxT("Tahoma") ) );
	
	bSizer1->Add( m_staticText1, 0, wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize( -1,-1 ), wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( 10, 74, 90, 90, false, wxT("Tahoma") ) );
	
	bSizer1->Add( m_staticText2, 0, wxEXPAND, 5 );
	
	m_staticText211 = new wxStaticText( this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize( -1,-1 ), wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	m_staticText211->Wrap( -1 );
	m_staticText211->SetFont( wxFont( 12, 74, 90, 92, false, wxT("Tahoma") ) );
	
	bSizer1->Add( m_staticText211, 0, wxEXPAND, 5 );


	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( frcurrentsUIDialogBase::OnClose ) );
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( frcurrentsUIDialogBase::OnSize ) );
	
    m_choice1->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( frcurrentsUIDialogBase::OnPortChanged), NULL, this );
    m_datePicker1->Connect( wxEVT_DATE_CHANGED, wxDateEventHandler( frcurrentsUIDialogBase::OnDateSelChanged ), NULL, this );

	m_bpNow->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnNow ), NULL, this );

	m_bpPrev->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnPrev ), NULL, this );
	m_bpNext->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnNext ), NULL, this );
	m_button33->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::About ), NULL, this );
    m_bXML->Connect(wxEVT_COMMAND_BUTTON_CLICKED,  wxCommandEventHandler(frcurrentsUIDialogBase::OnXML), NULL, this);
}

frcurrentsUIDialogBase::~frcurrentsUIDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( frcurrentsUIDialogBase::OnClose ) );
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( frcurrentsUIDialogBase::OnSize ) );
	
    m_choice1->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( frcurrentsUIDialogBase::OnPortChanged), NULL, this );
    m_datePicker1->Disconnect( wxEVT_DATE_CHANGED, wxDateEventHandler( frcurrentsUIDialogBase::OnDateSelChanged ), NULL, this );

	m_bpNow->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnNow ), NULL, this );

	m_bpPrev->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnPrev ), NULL, this );
	m_bpNext->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::OnNext ), NULL, this );
	m_button33->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( frcurrentsUIDialogBase::About ), NULL, this );
    m_bXML->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(frcurrentsUIDialogBase::OnXML), NULL, this);
}

frcurrentsPreferencesDialogBase::frcurrentsPreferencesDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	 
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_cbUseRate = new wxCheckBox( this, wxID_ANY, _("Display tidal rates (knots)"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_cbUseRate, 0, wxALL, 5 );
	
	m_cbUseDirection = new wxCheckBox( this, wxID_ANY, _("Display tidal direction"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_cbUseDirection, 0, wxALL, 5 );
	
	m_cbFillColour = new wxCheckBox( this, wxID_ANY, _("Fill Colour"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_cbFillColour, 0, wxALL, 5 );

	fgSizer8->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	wxStdDialogButtonSizer* m_sdbSizer2;
	
	wxButton* m_sdbSizer2OK;
	wxButton* m_sdbSizer2Cancel;
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer2->AddButton( m_sdbSizer2Cancel );
	m_sdbSizer2->Realize();
	
	fgSizer8->Add( m_sdbSizer2, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( fgSizer8 );
	this->Layout();
	fgSizer8->Fit( this );
	
	this->Centre( wxBOTH );
}

frcurrentsPreferencesDialogBase::~frcurrentsPreferencesDialogBase()
{
	
}
