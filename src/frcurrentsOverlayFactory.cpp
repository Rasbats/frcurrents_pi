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

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif  // precompiled headers

#include <wx/glcanvas.h>
#include <wx/graphics.h>
#include <wx/progdlg.h>

#include "frcurrents_pi.h"
#include <vector>

#ifdef __WXMSW__
#define snprintf _snprintf
#endif  // __WXMSW__

using namespace std;

class Position;
class StandardPort;
class frcurrentsUIDialog;
class PlugIn_ViewPort;
class wxBoundingBox;

#define NUM_CURRENT_ARROW_POINTS 9

static int texture_format;
static bool glQueried = false;

#if defined(__WXMSW__)
#define systemGetProcAddress(ADDR) wglGetProcAddress(ADDR)
#elif defined(__WXOSX__)
#include <dlfcn.h>
#define systemGetProcAddress(ADDR) dlsym(RTLD_DEFAULT, ADDR)
#elif defined(__OCPN__ANDROID__)
#define systemGetProcAddress(ADDR) eglGetProcAddress(ADDR)
#else
#define systemGetProcAddress(ADDR) glXGetProcAddress((const GLubyte *)ADDR)
#endif

//----------------------------------------------------------------------------------------------------------
//    frcurrents Overlay Factory Implementation
//----------------------------------------------------------------------------------------------------------
frcurrentsOverlayFactory::frcurrentsOverlayFactory(frcurrentsUIDialog &dlg)
    : m_dlg(dlg) {
 
  pTCFont = new wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                       wxFONTWEIGHT_BOLD);
  m_last_vp_scale = 0.;
  m_bShowRate = m_dlg.m_bUseRate;
  m_bShowDirection = m_dlg.m_bUseDirection;
  m_bHighResolution = m_dlg.m_bUseHighRes;

  m_bShowFillColour = m_dlg.m_bUseFillColour;
  m_ShowArrowStyle = m_dlg.m_UseArrowStyle;
}

frcurrentsOverlayFactory::~frcurrentsOverlayFactory() {}

bool frcurrentsOverlayFactory::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp) {
#if wxUSE_GRAPHICS_CONTEXT
  wxMemoryDC *pmdc;
  pmdc = wxDynamicCast(&dc, wxMemoryDC);
  wxGraphicsContext *pgc = wxGraphicsContext::Create(*pmdc);
  m_gdc = pgc;
  m_pdc = &dc;
#else
  m_pdc = &dc;
#endif

  m_pdc = &dc;

  RenderMyArrows(vp, false, false, false, m_dtUseNew);
  return true;
}

bool frcurrentsOverlayFactory::RenderGLOverlay(wxGLContext *pcontext,
                                               PlugIn_ViewPort *vp) {
  m_pdc = NULL;  // inform lower layers that this is OpenGL render

  RenderMyArrows(vp, false, false, false, m_dtUseNew);
  return true;
}

void frcurrentsOverlayFactory::GetArrowStyle(int my_style) {
  switch (my_style) {
    case 0:
      myArrowArray[0] = wxPoint(0, 0);
      myArrowArray[1] = wxPoint(0, -10);
      myArrowArray[2] = wxPoint(55, -10);
      myArrowArray[3] = wxPoint(55, -25);
      myArrowArray[4] = wxPoint(100, 0);
      myArrowArray[5] = wxPoint(55, 25);
      myArrowArray[6] = wxPoint(55, 10);
      myArrowArray[7] = wxPoint(0, 10);
      myArrowArray[8] = wxPoint(0, 0);
      break;

    case 1:
      myArrowArray[0] = wxPoint(0, 0);
      myArrowArray[1] = wxPoint(0, -5);
      myArrowArray[2] = wxPoint(30, -5);
      myArrowArray[3] = wxPoint(30, -12);
      myArrowArray[4] = wxPoint(50, 0);
      myArrowArray[5] = wxPoint(30, 12);
      myArrowArray[6] = wxPoint(30, 5);
      myArrowArray[7] = wxPoint(0, 5);
      myArrowArray[8] = wxPoint(0, 0);
      break;

    case 2:
      myArrowArray[0] = wxPoint(0, 0);
      myArrowArray[1] = wxPoint(0, -5);
      myArrowArray[2] = wxPoint(75, -5);
      myArrowArray[3] = wxPoint(55, -15);
      myArrowArray[4] = wxPoint(100, 0);
      myArrowArray[5] = wxPoint(55, 15);
      myArrowArray[6] = wxPoint(75, 5);
      myArrowArray[7] = wxPoint(0, 5);
      myArrowArray[8] = wxPoint(0, 0);
      break;
  }
}

wxColour frcurrentsOverlayFactory::GetSpeedColour(double my_speed) {
  wxColour c_blue = wxColour(m_dlg.myUseColour[0]);           // 127, 0, 255);
  wxColour c_green = wxColour(m_dlg.myUseColour[1]);          // 0, 166, 80);
  wxColour c_yellow_orange = wxColour(m_dlg.myUseColour[2]);  // 253, 184, 19);
  wxColour c_orange = wxColour(m_dlg.myUseColour[3]);         // 248, 128, 64);
  wxColour c_red = wxColour(m_dlg.myUseColour[4]);            // 248, 0, 0);

  if (my_speed < 0.5) {
    return c_blue;
  }
  if ((my_speed >= 0.5) && (my_speed < 1.5)) {
    return c_green;
  }
  if ((my_speed >= 1.5) && (my_speed < 2.5)) {
    return c_yellow_orange;
  }
  if ((my_speed >= 2.5) && (my_speed < 3.5)) {
    return c_orange;
  }
  if ((my_speed >= 3.5)) {
    return c_red;
  }

  return wxColour(0, 0, 0);
}

void frcurrentsOverlayFactory::RenderMyArrows(PlugIn_ViewPort *vp,
                                              bool bRebuildSelList,
                                              bool bforce_redraw_currents,
                                              bool bdraw_mono_for_mask,
                                              wxDateTime myTime) {
  wxPoint p;

  double myX, myY;
  myX = 50;
  myY = 0;
  vector<Position> m_new = m_dlg.my_positions;

  double value, decValue;
  double decValue1;

  wxString mLat;
  wxString mBitLat;
  wxString mDecLat;

  wxString mLon;
  wxString mFirstLon;
  wxString mBitLon;
  wxString mDecLon;

  double latF, lonF;
  int n = 0;
  int m_len;

  int m = m_dlg.m_choice1->GetSelection();
  wxString s = m_dlg.m_choice1->GetString(m);

  StandardPort myPortTides;
  myPortTides = m_dlg.PopulatePortTides(s);
  double myCurrent;

  for (std::vector<Position>::iterator it = m_new.begin(); it != m_new.end();
       it++) {
    mLat = (*it).lat;
    mBitLat = mLat.Mid(0, 2);
    mDecLat = mLat.Mid(2);
    mDecLat.ToDouble(&decValue);

    if (!mBitLat.ToDouble(&value)) { /* error! */
    }

    latF = value + (decValue / 60);

    mLon = (*it).lon;
    m_len = mLon.Len();

    if (mLon.Mid(0, 1) == "-") {
      if (mLon.length() == 6 || mLon.length() == 7) {
        mDecLon = "0";
        mBitLon = mLon.Mid(1);
      } else {
        mDecLon = mLon.Mid(1, 1);
        mBitLon = mLon.Mid(2);
      }
    } else {
      if (mLon.length() == 5 || mLon.length() == 6) {
        mDecLon = "0";
        mBitLon = mLon;
      } else {
        mDecLon = mLon.Mid(0, 1);
        mBitLon = mLon.Mid(1);
      }
    }
    mDecLon.ToDouble(&decValue1);
    mBitLon.ToDouble(&value);
    if (mLon.Mid(0, 1) == "-") {
      lonF = -1 * (decValue1 + value / 60);
    } else {
      lonF = (decValue1 + value / 60);
    }

    int pixxc, pixyc;
    wxPoint cpoint;
    GetCanvasPixLL(vp, &cpoint, latF, lonF);
    pixxc = cpoint.x;
    pixyc = cpoint.y;

    GetCanvasPixLL(vp, &p, latF, lonF);
    wxRect myRect = vp->rv_rect;

    wxString port_num = (*it).port_num;

    if (myRect.Contains(p.x, p.y)) {
      // drawing scaled current arrows
      m_fromHW = m_dlg.button_id;

      double dir = (*it).spDir[m_fromHW];

      double m_spdSpring = (*it).spRate[m_fromHW];

      double m_spdNeap = (*it).npRate[m_fromHW];

      int mmx, mmy;
      wxDisplaySizeMM(&mmx, &mmy);

      int sx, sy;
      wxDisplaySize(&sx, &sy);

      double m_pix_per_mm = ((double)sx) / ((double)mmx);

      int mm_per_knot = 10;
      float current_draw_scaler = mm_per_knot * m_pix_per_mm * 100 / 100.0;

      double coefficient = m_dlg.m_coeff;

      myCurrent = m_dlg.CalcCurrent(95, 45, m_spdSpring / 10, m_spdNeap / 10,
                                    coefficient);
      if (isnan(myCurrent)) myCurrent = 0.0;
      if (myCurrent == 0 || myCurrent < 0) myCurrent = 00.001;

      // myCurrent /= 10; //Files give speed in 1/10 knots i.e. 10 = 1 knot

      double a1 = myCurrent * 10;  // fabs( tcvalue ) * 10.;
      a1 = wxMax(1.0, a1);         // Current values less than 0.1 knot
                                   // will be displayed as 0
      double a2 = log10(a1);

      double scale = current_draw_scaler * a2;
      // dir = 45.00;
      // myCurrent = 0.5;

      bool rendered =
          drawCurrentArrow(pixxc, pixyc, dir - 90, scale / 100, myCurrent);

      int shift = 0;     

      if (!m_pdc) {
        if (rendered && m_bShowFillColour)
          drawGLPolygons(this, m_pdc, vp, DrawGLPolygon(), latF, lonF, shift);

        if (m_bShowRate) {
          DrawGLLabels(this, m_pdc, vp, DrawGLText(myCurrent, 1), latF,
                       lonF, 0);

          if (!m_bHighResolution) {
            shift = 13;
          } else {
            shift = 26;
          }
        }
        if (m_bShowDirection) {
          DrawGLLabels(this, m_pdc, vp, DrawGLTextDir(dir, 0), latF, lonF,
                       shift);
        }
      } else {
        char sbuf[20];
        if (m_bShowRate) {
          snprintf(sbuf, 19, "%3.1f", fabs(myCurrent));
          m_pdc->DrawText(wxString(sbuf, wxConvUTF8), pixxc, pixyc);
          if (!m_bHighResolution) {
            shift = 13;
          } else {
            shift = 26;
          }
        }

        if (m_bShowDirection) {
          snprintf(sbuf, 19, "%03.0f", dir);
          m_pdc->DrawText(wxString(sbuf, wxConvUTF8), pixxc, pixyc + shift);
        }
      }  // end if

    }  // end for
  }
}

void frcurrentsOverlayFactory::DrawGLLine(double x1, double y1, double x2,
                                          double y2, double width,
                                          wxColour myColour) {
  {
    wxColour isoLineColor = myColour;
    glColor4ub(isoLineColor.Red(), isoLineColor.Green(), isoLineColor.Blue(),
               255 /*isoLineColor.Alpha()*/);

    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_LINE_BIT | GL_ENABLE_BIT |
                 GL_POLYGON_BIT | GL_HINT_BIT);  // Save state
    {
      //      Enable anti-aliased lines, at best quality
      glEnable(GL_LINE_SMOOTH);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
      glLineWidth(width);

      glBegin(GL_LINES);
      glVertex2d(x1, y1);
      glVertex2d(x2, y2);
      glEnd();
    }

    glPopAttrib();
  }
}

bool frcurrentsOverlayFactory::drawCurrentArrow(int x, int y, double rot_angle,
                                                double scale, double rate) {
  double m_rate = fabs(rate);
  wxPoint p[9];

  GetArrowStyle(m_ShowArrowStyle);

  wxColour colour;
  colour = GetSpeedColour(m_rate);

  c_GLcolour = colour;  // for filling GL arrows
  if (scale <= 1e-2) return false;

  wxBrush brush(colour);

  if (m_pdc) {
    wxPen pen(colour, 2);

    m_pdc->SetPen(pen);
    m_pdc->SetBrush(brush);
  }

  float sin_rot = sin(rot_angle * PI / 180.);
  float cos_rot = cos(rot_angle * PI / 180.);

  // Move to the first point

  float xt = myArrowArray[0].x;
  float yt = myArrowArray[0].y;

  float xp = (xt * cos_rot) - (yt * sin_rot);
  float yp = (xt * sin_rot) + (yt * cos_rot);
  int x1 = (int)(xp * scale);
  int y1 = (int)(yp * scale);

  p[0].x = x;
  p[0].y = y;

  p_basic[0].x = 100;
  p_basic[0].y = 100;

  // Walk thru the point list
  for (int ip = 1; ip < NUM_CURRENT_ARROW_POINTS; ip++) {
    xt = myArrowArray[ip].x;
    yt = myArrowArray[ip].y;

    float xp = (xt * cos_rot) - (yt * sin_rot);
    float yp = (xt * sin_rot) + (yt * cos_rot);
    int x2 = (int)(xp * scale);
    int y2 = (int)(yp * scale);

    p_basic[ip].x = 100 + x2;
    p_basic[ip].y = 100 + y2;

    if (m_pdc) {
      m_pdc->DrawLine(x1 + x, y1 + y, x2 + x, y2 + y);
    } else {
      DrawGLLine(x1 + x, y1 + y, x2 + x, y2 + y, 2, colour);
    }
    p[ip].x = x1 + x;
    p[ip].y = y1 + y;

    x1 = x2;
    y1 = y2;
  }

  // p[9].x = x1;
  // p[9].y = y1;

  if (m_bShowFillColour && m_pdc) {
    m_pdc->SetBrush(brush);
    m_pdc->DrawPolygon(9, p, 0, 0);
  }
  return true;
}

void frcurrentsOverlayFactory::DrawGLLabels(frcurrentsOverlayFactory *pof,
                                            wxDC *dc, PlugIn_ViewPort *vp,
                                            wxImage &imageLabel, double myLat,
                                            double myLon, int offset) {
  //---------------------------------------------------------
  // Ecrit les labels
  //---------------------------------------------------------

  wxPoint ab;
  GetCanvasPixLL(vp, &ab, myLat, myLon);

  wxPoint cd;
  GetCanvasPixLL(vp, &cd, myLat, myLon);

  int w = imageLabel.GetWidth();
  int h = imageLabel.GetHeight();

  int label_offset = 0;
  int xd = (ab.x + cd.x - (w + label_offset * 2)) / 2;
  int yd = (ab.y + cd.y - h) / 2 + offset;

  if (dc) {
    /* don't use alpha for isobars, for some reason draw bitmap ignores
       the 4th argument (true or false has same result) */
    wxImage img(w, h, imageLabel.GetData(), true);
    dc->DrawBitmap(img, xd, yd, false);
  } else { /* opengl */

    int w = imageLabel.GetWidth(), h = imageLabel.GetHeight();

    unsigned char *d = imageLabel.GetData();
    unsigned char *a = imageLabel.GetAlpha();

    unsigned char mr, mg, mb;
    if (!a && !imageLabel.GetOrFindMaskColour(&mr, &mg, &mb))
      wxMessageBox(
          _T("trying to use mask to draw a bitmap without alpha or mask\n" ));

    unsigned char *e = new unsigned char[4 * w * h];
    {
      for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
          unsigned char r, g, b;
          int off = (y * w + x);
          r = d[off * 3 + 0];
          g = d[off * 3 + 1];
          b = d[off * 3 + 2];

          e[off * 4 + 0] = r;
          e[off * 4 + 1] = g;
          e[off * 4 + 2] = b;

          e[off * 4 + 3] =
              a ? a[off] : ((r == mr) && (g == mg) && (b == mb) ? 0 : 255);
        }
    }

    glColor4f(1, 1, 1, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glRasterPos2i(xd, yd);
    glPixelZoom(1, -1);
    glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, e);
    glPixelZoom(1, 1);
    glDisable(GL_BLEND);

    delete[] (e);
  }
}

wxPoint frcurrentsOverlayFactory::ScaleCurrentArrow(int index, wxPoint myPoint,
                                                    int scale) {
  wxPoint dummy = myPoint;
  switch (index) {
    case 1: {
      dummy.y -= scale / 3;
      return dummy;
    }
    case 2: {
      dummy.x += scale;
      dummy.y -= scale / 3;
      return dummy;
    }
    case 3: {
      dummy.x += scale;
      dummy.y -= scale / 3;
      return dummy;
    }
    case 4: {
      dummy.x += scale * 2;
      return dummy;
    }
    case 5: {
      dummy.x += scale;
      dummy.y += scale / 3;
      return dummy;
    }
    case 6: {
      dummy.x += scale;
      dummy.y += scale / 3;
      return dummy;
    }
    case 7: {
      dummy.y += scale / 3;
      return dummy;
    }
    case 8: {
      return dummy;
    }
  }
}

void frcurrentsOverlayFactory::drawGLPolygons(frcurrentsOverlayFactory *pof,
                                              wxDC *dc, PlugIn_ViewPort *vp,
                                              wxImage &imageLabel, double myLat,
                                              double myLon, int offset) {
  //---------------------------------------------------------
  // Ecrit les labels
  //---------------------------------------------------------

  wxPoint ab;
  GetCanvasPixLL(vp, &ab, myLat, myLon);

  wxPoint cd;
  GetCanvasPixLL(vp, &cd, myLat, myLon);

  int w = imageLabel.GetWidth();
  int h = imageLabel.GetHeight();

  int label_offset = 0;
  int xd = (ab.x + cd.x - (w + label_offset * 2)) / 2;
  int yd = (ab.y + cd.y - h) / 2 + offset;

  if (dc) {
    /* don't use alpha for isobars, for some reason draw bitmap ignores
       the 4th argument (true or false has same result) */
    wxImage img(w, h, imageLabel.GetData(), true);
    dc->DrawBitmap(img, xd, yd, false);
  } else { /* opengl */

    unsigned char *d = imageLabel.GetData();
    unsigned char *a = imageLabel.GetAlpha();

    unsigned char mr, mg, mb;
    if (!a && !imageLabel.GetOrFindMaskColour(&mr, &mg, &mb))
      wxMessageBox(
          _T("trying to use mask to draw a bitmap without alpha or mask\n" ));

    unsigned char *e = new unsigned char[4 * w * h];
    {
      for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
          unsigned char r, g, b;
          int off = (y * w + x);
          r = d[off * 3 + 0];
          g = d[off * 3 + 1];
          b = d[off * 3 + 2];

          e[off * 4 + 0] = r;
          e[off * 4 + 1] = g;
          e[off * 4 + 2] = b;

          e[off * 4 + 3] =
              a ? a[off] : ((r == mr) && (g == mg) && (b == mb) ? 0 : 255);
        }
    }

    glColor4f(1, 1, 1, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glRasterPos2i(xd, yd);
    glPixelZoom(1, -1);
    glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, e);
    glPixelZoom(1, 1);
    glDisable(GL_BLEND);

    delete[] (e);
  }
}

wxImage &frcurrentsOverlayFactory::DrawGLPolygon() {
  wxImage img;

  wxColour c_orange = c_GLcolour;

  wxPen penText(c_orange);
  wxBrush backBrush(c_orange);

  wxMemoryDC mdc(wxNullBitmap);

  int w, h;

  w = 200;
  h = 200;

  wxBitmap bm(w, h);
  mdc.SelectObject(bm);
  mdc.Clear();

  mdc.SetPen(penText);
  mdc.SetBrush(backBrush);
  mdc.SetTextForeground(c_orange);
  mdc.SetTextBackground(c_orange);

  // int xd = 0, yd = 0;
  // mdc.DrawRoundedRectangle(xd, yd, w+(label_offset * 2), h+2, -.25);
  /*

                          wxPoint p2;
                                          p2.x = 100;
                                          p2.y = 100 ;

                          wxPoint z[9];
                                          z[0].x = p2.x;
                      z[0].y = p2.y;
                      z[1].x = p2.x;
                      z[1].y = p2.y -10 ;
                      z[2].x = p2.x + 55;
                      z[2].y = p2.y - 10;
                      z[3].x = p2.x + 55;
                      z[3].y = p2.y -25;
                                          z[4].x = p2.x + 100;
                      z[4].y = p2.y;
                      z[5].x = p2.x +55;
                      z[5].y = p2.y + 25 ;
                      z[6].x = p2.x + 55;
                      z[6].y = p2.y + 10;
                      z[7].x = p2.x;
                      z[7].y = p2.y +10;
                                          z[8].x = p2.x;
                      z[8].y = p2.y;

          */

  mdc.DrawPolygon(9, p_basic, 0);

  mdc.SelectObject(wxNullBitmap);

  m_fillImg = bm.ConvertToImage();

  wxImage &image = m_fillImg;
  image.InitAlpha();

  unsigned char *d = image.GetData();
  unsigned char *a = image.GetAlpha();

  w = image.GetWidth(), h = image.GetHeight();
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++) {
      int r, g, b;
      int ioff = (y * w + x);
      r = d[ioff * 3 + 0];
      g = d[ioff * 3 + 1];
      b = d[ioff * 3 + 2];

      a[ioff] = 255 - (r + g + b) / 3;
    }

  return image;
  /*
                  wxPoint p;
                                  p.x = 200;
                                  p.y = 200;

                  wxPoint z[9];
                                  z[0].x = p.x;
              z[0].y = p.y;
              z[1].x = p.x;
              z[1].y = p.y -10 ;
              z[2].x = p.x + 55;
              z[2].y = p.y - 10;
              z[3].x = p.x + 55;
              z[3].y = p.y -25;
                                  z[4].x = p.x + 100;
              z[4].y = p.y;
              z[5].x = p.x +55;
              z[5].y = p.y + 25 ;
              z[6].x = p.x + 55;
              z[6].y = p.y + 10;
              z[7].x = p.x;
              z[7].y = p.y +10;
                                  z[8].x = p.x;
              z[8].y = p.y;



  */
}

wxImage &frcurrentsOverlayFactory::DrawGLText(double value, int precision) {
  wxString labels;

  labels.Printf(_T("%.*f"), precision, value);
  //
  return DrawGLTextString(labels);
}

wxImage &frcurrentsOverlayFactory::DrawGLTextDir(double value, int precision) {
  wxString labels;

  labels.Printf(_T("%03.*f"), precision, value);
  return DrawGLTextString(labels);
}

wxImage &frcurrentsOverlayFactory::DrawGLTextString(wxString myText) {
  wxString labels;
  labels = myText;
  // wxMessageBox(labels);
  std::map<wxString, wxImage>::iterator it;

  it = m_labelCacheText.find(labels);
  if (it != m_labelCacheText.end()) return it->second;

  wxMemoryDC mdc(wxNullBitmap);

  mdc.SetFont(*pTCFont);

  int w, h;
  mdc.GetTextExtent(labels, &w, &h);

  int label_offset = 10;  // 5

  wxBitmap bm(w + label_offset * 2, h + 1);
  mdc.SelectObject(bm);
  mdc.Clear();

  m_text_color = wxColour("BLACK");
  wxPen penText(m_text_color);
  mdc.SetPen(penText);

  mdc.SetBrush(*wxTRANSPARENT_BRUSH);
  mdc.SetTextForeground(m_text_color);
  mdc.SetTextBackground(wxTRANSPARENT);

  int xd = 0;
  int yd = 0;

  mdc.DrawText(labels, label_offset + xd, yd + 1);
  mdc.SelectObject(wxNullBitmap);

  m_labelCacheText[myText] = bm.ConvertToImage();

  m_labelCacheText[myText].InitAlpha();

  wxImage &image = m_labelCacheText[myText];

  unsigned char *d = image.GetData();
  unsigned char *a = image.GetAlpha();

  w = image.GetWidth(), h = image.GetHeight();
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int r, g, b;
      int ioff = (y * w + x);
      r = d[ioff * 3 + 0];
      g = d[ioff * 3 + 1];
      b = d[ioff * 3 + 2];

      a[ioff] = 255 - (r + g + b) / 3;
    }
  }
  return image;
}
