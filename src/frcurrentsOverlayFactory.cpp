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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers


#include <wx/graphics.h>
#include <wx/progdlg.h>

#include "frcurrents_pi.h"
#include <vector>


#ifdef __WXMSW__
#define snprintf _snprintf
#endif // __WXMSW__

using namespace std;

class Position;
class StandardPort;
class frcurrentsUIDialog;
class PlugIn_ViewPort;
class wxBoundingBox;
class piDC;

#define NUM_CURRENT_ARROW_POINTS 9
static wxPoint CurrentArrowArray1[NUM_CURRENT_ARROW_POINTS] = { wxPoint( 0, 0 ), wxPoint( 0, -10 ),
        wxPoint( 55, -10 ), wxPoint( 55, -25 ), wxPoint( 100, 0 ), wxPoint( 55, 25 ), wxPoint( 55,
                10 ), wxPoint( 0, 10 ), wxPoint( 0, 0 )};

static wxPoint CurrentArrowArray2[NUM_CURRENT_ARROW_POINTS] = { wxPoint( 0, 0 ), wxPoint( 0, -5 ),
        wxPoint( 30, -5 ), wxPoint( 30, -12 ), wxPoint( 50, 0 ), wxPoint( 30, 12 ), wxPoint( 30,
                5 ), wxPoint( 0, 5 ), wxPoint( 0, 0 )};

static wxPoint CurrentArrowArray3[NUM_CURRENT_ARROW_POINTS] = { wxPoint( 0, 0 ), wxPoint( 0, -5 ),
        wxPoint( 75, -5 ), wxPoint( 55, -15 ), wxPoint( 100, 0 ), wxPoint( 55, 15 ), wxPoint( 75,
                5 ), wxPoint( 0, 5 ), wxPoint( 0, 0 )};

static int texture_format;
static bool glQueried = false;

#if defined(__WXMSW__)
#define systemGetProcAddress(ADDR) wglGetProcAddress(ADDR)
#elif defined(__WXOSX__)
#include <dlfcn.h>
#define systemGetProcAddress(ADDR) dlsym( RTLD_DEFAULT, ADDR)
#elif defined(__OCPN__ANDROID__)
#define systemGetProcAddress(ADDR) eglGetProcAddress(ADDR)
#else
#define systemGetProcAddress(ADDR) glXGetProcAddress((const GLubyte*)ADDR)
#endif


//----------------------------------------------------------------------------------------------------------
//    frcurrents Overlay Factory Implementation
//----------------------------------------------------------------------------------------------------------
frcurrentsOverlayFactory::frcurrentsOverlayFactory( frcurrentsUIDialog &dlg )
    : m_dlg(dlg)
{

    m_last_vp_scale = 0.;
    m_bShowRate = m_dlg.m_bUseRate;
    m_bShowDirection = m_dlg.m_bUseDirection;
	m_bHighResolution = m_dlg.m_bUseHighRes;

    m_bShowFillColour = m_dlg.m_bUseFillColour;
	m_ShowArrowStyle = m_dlg.m_UseArrowStyle;	
	
}

frcurrentsOverlayFactory::~frcurrentsOverlayFactory()
{

}

void frcurrentsOverlayFactory::Reset()
{

}

void frcurrentsOverlayFactory::DrawGL(PlugIn_ViewPort &piVP) {
#ifdef ocpnUSE_GL
  // piDC dc;
  // dc.SetVP(&piVP);

  /* determine color and width */
  wxPenStyle style = wxPENSTYLE_SOLID;
  int width = 4;

  int j = 0;
  wxPoint r;

  g_pDC->SetPen(*wxThePenList->FindOrCreatePen("RED", width, style));
  g_pDC->SetBrush(
      *wxTheBrushList->FindOrCreateBrush("RED", wxBRUSHSTYLE_TRANSPARENT));
  g_pDC->SetGLStipple();

  RenderMyArrows(&g_VP);

#endif
}

void frcurrentsOverlayFactory::GetArrowStyle(int my_style) {
	switch (my_style){		
		case 0:
		myArrowArray[0] = wxPoint( 0, 0 );
		myArrowArray[1] = wxPoint( 0, -10 );
		myArrowArray[2] = wxPoint( 55, -10 );
		myArrowArray[3] = wxPoint( 55, -25 );
		myArrowArray[4] = wxPoint( 100, 0 );
		myArrowArray[5] = wxPoint( 55, 25 );
		myArrowArray[6] = wxPoint( 55, 10 );
		myArrowArray[7] = wxPoint( 0, 10 );
		myArrowArray[8] = wxPoint( 0, 0 );
		break;

		case 1:
		myArrowArray[0] = wxPoint( 0, 0 );
		myArrowArray[1] = wxPoint(0, -5);
		myArrowArray[2] = wxPoint( 30, -5 );
		myArrowArray[3] = wxPoint( 30, -12 );
		myArrowArray[4] = wxPoint( 50, 0 );
		myArrowArray[5] = wxPoint( 30, 12 );
		myArrowArray[6] = wxPoint( 30, 5 );
		myArrowArray[7] = wxPoint( 0, 5 );
		myArrowArray[8] = wxPoint( 0, 0 );
		break;

		case 2:
		myArrowArray[0] = wxPoint( 0, 0 );
		myArrowArray[1] = wxPoint( 0, -5 );
		myArrowArray[2] = wxPoint( 75, -5 );
		myArrowArray[3] = wxPoint( 55, -15 );
		myArrowArray[4] = wxPoint( 100, 0 );
		myArrowArray[5] = wxPoint( 55, 15 );
		myArrowArray[6] = wxPoint( 75, 5 );
		myArrowArray[7] = wxPoint( 0, 5 );
		myArrowArray[8] = wxPoint( 0, 0 );
		break;
	} 
}

wxColour frcurrentsOverlayFactory::GetSpeedColour(double my_speed){

	wxColour c_blue = wxColour(m_dlg.myUseColour[0]);						//127, 0, 255);
	wxColour c_green = wxColour(m_dlg.myUseColour[1]);						//0, 166, 80);
	wxColour c_yellow_orange = wxColour(m_dlg.myUseColour[2]);				//253, 184, 19);
	wxColour c_orange = wxColour(m_dlg.myUseColour[3]); 					//248, 128, 64);
	wxColour c_red = wxColour(m_dlg.myUseColour[4]);						//248, 0, 0);

	if (my_speed < 0.5){ return c_blue;}
	if ((my_speed >= 0.5) && (my_speed < 1.5)){ return c_green;}
	if ((my_speed >= 1.5) && (my_speed < 2.5)){ return c_yellow_orange;}
	if ((my_speed >= 2.5) && (my_speed < 3.5)){ return c_orange;}
	if ((my_speed >= 3.5) ){ return c_red;}

	return wxColour(0, 0, 0);

}

bool frcurrentsOverlayFactory::drawCurrentArrow(int x, int y, double rot_angle, double scale, double rate )
{
    double m_rate = fabs(rate);

	GetArrowStyle(m_ShowArrowStyle);

    wxColour colour;

    colour = GetSpeedColour( m_rate );

    wxString c = colour.GetAsString();
    //wxMessageBox(c);

    c_GLcolour = colour;  // for filling GL arrows
    if( scale <= 1e-2 )
        return false;

    wxBrush brush(colour);

    if( m_dc ) {
        wxPen pen( colour, 4 );

        m_dc->SetPen( pen );
        m_dc->SetBrush( brush);
    }
    float sin_rot = sin( rot_angle * PI / 180. );
    float cos_rot = cos( rot_angle * PI / 180. );

    // Move to the first point

    float xt = myArrowArray[0].x;
    float yt = myArrowArray[0].y;

    float xp = ( xt * cos_rot ) - ( yt * sin_rot );
    float yp = ( xt * sin_rot ) + ( yt * cos_rot );
    int x1 = (int) ( xp * scale );
    int y1 = (int) ( yp * scale );

    p[0].x = x1 + x;
    p[0].y = y1 + y;

    p_basic[0].x = 100;
    p_basic[0].y = 100;

    // Walk thru the point list
    for( int ip = 1; ip < NUM_CURRENT_ARROW_POINTS; ip++ ) {
        xt = myArrowArray[ip].x;
        yt = myArrowArray[ip].y;

        float xp = ( xt * cos_rot ) - ( yt * sin_rot );
        float yp = ( xt * sin_rot ) + ( yt * cos_rot );
        int x2 = (int) ( xp * scale );
        int y2 = (int) ( yp * scale );

        p_basic[ip].x = 100 + x2;
        p_basic[ip].y = 100 + y2;

        if (m_dc){
            m_dc->DrawLine( x1 + x, y1 + y, x2 + x, y2 + y );
        }

        p[ip].x = x2 + x;
        p[ip].y = y2 + y;

        x1 = x2;
        y1 = y2;
     }



    if( m_bShowFillColour && m_dc){


    /*
   *           4
   *          /\
   *         /  \
   *        /    \
   *     3 /      \ 5
   *      /_ 2   6_\
   *        |    |
   *        |    |
   *        |    |
   *        |____|
   *       1   0  7
   */


        polyPoints[0] = p[3];
        polyPoints[1] = p[4];
        polyPoints[2] = p[5];

        rectPoints[0] = p[1];
        rectPoints[1] = p[2];
        rectPoints[2] = p[6];
        rectPoints[3] = p[7];

        //polyPoints[4] = p[8];

        brush.SetStyle(wxBRUSHSTYLE_SOLID);
        m_dc->SetBrush(brush);

        m_dc->DrawPolygonTessellated(3, polyPoints);
        m_dc->DrawPolygonTessellated(4, rectPoints);
    }
    return true;
}

void frcurrentsOverlayFactory::RenderMyArrows(PlugIn_ViewPort *vp ){

		   wxPoint p;			
	
		   double myX, myY;
		   myX = 50;
     	   myY = 0;
		   vector<Position>m_new = m_dlg.my_positions;
			
		   double value,  decValue; 
		   double decValue1;
		   
		   wxString mLat;
		   wxString mBitLat;
		   wxString mDecLat;

		   wxString mLon;
		   wxString mFirstLon;
		   wxString mBitLon;
		   wxString mDecLon;

		   double latF, lonF;
		   int n=0;
		   int m_len;		   
		   
		   int m = m_dlg.m_choice1->GetSelection();
		   wxString s = m_dlg.m_choice1->GetString(m);

	
		   StandardPort myPortTides;
		   myPortTides = m_dlg.PopulatePortTides(s);
		   double myCurrent;

		   for(std::vector<Position>::iterator it = m_new.begin();  it != m_new.end(); it++){
           
			  mLat = (*it).lat;
			  mBitLat = mLat.Mid(0,2);
			  mDecLat = mLat.Mid(2);			  			  
			  mDecLat.ToDouble(&decValue);

		    if(!mBitLat.ToDouble(&value)){ /* error! */ }
				
			  latF = value + (decValue/60);

				wxString showlat = wxString::Format("%f", latF);
        //wxMessageBox(showlat);


				mLon = (*it).lon;
				m_len = mLon.Len();
				
				if (mLon.Mid(0, 1) == "-") {
					if (mLon.length() == 6 || mLon.length() == 7) {
						mDecLon = "0";
						mBitLon = mLon.Mid(1);
					}
					else {
						mDecLon = mLon.Mid(1, 1);
						mBitLon = mLon.Mid(2);
					}
				}else {
					if (mLon.length() == 5 || mLon.length() == 6) {
						mDecLon = "0";
						mBitLon = mLon;
					}
					else {
						mDecLon = mLon.Mid(0, 1);
						mBitLon = mLon.Mid(1);
					}
				}
				mDecLon.ToDouble(&decValue1);
				mBitLon.ToDouble(&value);
				if (mLon.Mid(0, 1) == "-") {
					lonF = -1 * (decValue1 + value / 60);
				}else {
					lonF = (decValue1 + value / 60);
				}

				wxString showlon = wxString::Format("%f", lonF);
        //wxMessageBox(showlon);

			 

      GetCanvasPixLL( vp, &p,latF, lonF );
      wxRect myRect = vp->rv_rect;

			wxString port_num = (*it).port_num;		

			if (myRect.Contains(p.x, p.y)){ 
										        
					// drawing scaled current arrows												
				  m_fromHW = m_dlg.button_id;

					
					double dir = (*it).spDir[m_fromHW];

					wxString showDir = wxString::Format("%f", dir);
         // wxMessageBox(showDir);

					

					double m_spdSpring = (*it).spRate[m_fromHW];
					

					wxString showspdsp = wxString::Format("%f", m_spdSpring);
        // wxMessageBox(showspdsp);


					double m_spdNeap = (*it).npRate[m_fromHW];
					

					wxString showspdnp = wxString::Format("%f", m_spdNeap);
         // wxMessageBox(showspdnp);

					double range = m_dlg.myRange;
					wxString showrg = wxString::Format("%f", range);
          //wxMessageBox(showrg);

					int mmx, mmy;
					wxDisplaySizeMM( &mmx, &mmy );

					int sx, sy;
					wxDisplaySize( &sx, &sy );

					double m_pix_per_mm = ( (double) sx ) / ( (double) mmx );

					int mm_per_knot = 10;
					float current_draw_scaler = mm_per_knot * m_pix_per_mm * 100 / 100.0;

					double coefficient = m_dlg.m_coeff;

					myCurrent = m_dlg.CalcCurrent(95,45,m_spdSpring/10,m_spdNeap/10,coefficient);
					if (isnan(myCurrent)) myCurrent = 0.0;
					if (myCurrent == 0 || myCurrent < 0)
						myCurrent = 00.001;

					//myCurrent /= 10; //Files give speed in 1/10 knots i.e. 10 = 1 knot

					double a1 = myCurrent*10;    //fabs( tcvalue ) * 10.;
					a1 = wxMax(1.0, a1);		// Current values less than 0.1 knot
												// will be displayed as 0
					double a2 = log10( a1 );

					double scale = current_draw_scaler * a2;			        
					//dir = 45.00;
					//myCurrent = 0.5;

					bool d = drawCurrentArrow( p.x, p.y,
												dir - 90 , scale / 100, myCurrent );
					//if (d) wxMessageBox("Drawing");
					//else { wxMessageBox("Not Drawing"); }

					int shift = 0;

          char sbuf[20];
          if( m_bShowRate ) {
						snprintf( sbuf, 19, "%3.1f", myCurrent);
            m_dc->DrawText( wxString( sbuf, wxConvUTF8 ), p.x, p.y );
            if (!m_bHighResolution){
                shift = 13;
            }
            else {
                shift = 26;
            }
          }

          if ( m_bShowDirection ) {
            snprintf( sbuf, 19, "%03.0f", dir );
            m_dc->DrawText( wxString( sbuf, wxConvUTF8 ), p.x, p.y + shift );
          }					// end scaled current			
			}   // end if         

	    }// end for			   		   											   		
}


void frcurrentsOverlayFactory::DrawAllCurrentsInViewPort(PlugIn_ViewPort *BBox, bool bRebuildSelList,
        bool bforce_redraw_currents, bool bdraw_mono_for_mask, wxDateTime myTime)
{

    if (BBox->chart_scale > 1000000){
        return;
    }
    wxColour text_color;

    GetGlobalColor( _T ("UINFD" ), &text_color );
    if (text_color != m_text_color) {
       // color changed, invalid cache
       m_text_color = text_color;
       m_labelCacheText.clear();
    }

    double rot_vp = BBox->rotation*180/M_PI;

    // Set up the scaler
    double mmx = PlugInGetDisplaySizeMM();

    int sx, sy;
    wxDisplaySize( &sx, &sy );

    double m_pix_per_mm = ( (double) sx ) / ( mmx );

    int mm_per_knot = 10;
    float current_draw_scaler = mm_per_knot * m_pix_per_mm * 100 / 100.0;

    // End setting up scaler

    float tcvalue, dir;
    bool bnew_val = true;
    double lon_last = 0.;

    double lat_last = 0.;
    TCMgr *ctcmgr = m_dlg.ptcmgr;


    wxDateTime yn = wxDateTime::Now().ToGMT();
    time_t myTimeNow = yn.GetTicks();

    wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	#ifdef __WXMSW__  // for Windows hi-def screens
		double factor = (double)(GetOCPNCanvasWindow()->ToDIP(100)) / 100.;
		font.Scale(1. / factor);
	#endif

    m_dc->SetFont(font);
    wxRect myRect = BBox->rv_rect;


        for( int i = 1; i <  ctcmgr->Get_max_IDX() + 1; i++ ) {
            const IDX_entry *pIDX = ctcmgr->GetIDX_entry( i );
            double lon = pIDX->IDX_lon;
            double lat = pIDX->IDX_lat;
            bool b_dup = false;

            char type = pIDX->IDX_type;             // Entry "TCtcIUu" identifier

            if( ( ( type == 'c' ) || ( type == 'C' ) ) && ( 1/*pIDX->IDX_Useable*/) ) {

                if ((lat == lat_last) && (lon == lon_last)) {
                b_dup = true;
                lon_last = lon;
                lat_last = lat;
                continue;
            }
                int pixxc, pixyc;
                wxPoint cpoint;
                GetCanvasPixLL(BBox,&cpoint, lat, lon);
                pixxc = cpoint.x;
                pixyc = cpoint.y;

                if( !b_dup && myRect.Contains(cpoint.x, cpoint.y)) {


                    if( ctcmgr->GetTideOrCurrent15( myTimeNow, i, tcvalue, dir, bnew_val) ) {

                      if( type == 'c' || type == 'C' ) {

                        //    Adjust drawing size using logarithmic scale
                        double a1 = fabs( tcvalue ) * 10;
                        a1 = wxMax(1.0, a1);      // Current values less than 0.1 knot
                                                // will be displayed as 0
                        double a2 = log10( a1 );
                        double scale = current_draw_scaler * a2;

                        bool rendered = drawCurrentArrow( pixxc, pixyc, dir -90 + rot_vp , scale/100, tcvalue );

                        int shift = 0;

                          char sbuf[20];
                          if( m_bShowRate ) {
                            snprintf( sbuf, 19, "%3.1f", fabs(tcvalue) );
                            m_dc->DrawText( wxString( sbuf, wxConvUTF8 ), pixxc, pixyc );
                            if (!m_bHighResolution){
                                shift = 13;
                            }
                            else {
                                shift = 26;
                            }
                          }

                          if ( m_bShowDirection ) {
                            snprintf( sbuf, 19, "%03.0f", dir );
                            m_dc->DrawText( wxString( sbuf, wxConvUTF8 ), pixxc, pixyc + shift );
                          }
                      }
                    }


                lon_last = lon;
                lat_last = lat;

                }
            }

        }
}
