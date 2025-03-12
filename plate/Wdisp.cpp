#include "wdisp.h"
#include <math.h>

COLORREF BColor[16] =
{ 
	RGB(0,0,0),
	RGB(0,0,128),
	RGB(0,128,0),
	RGB(0,128,128),
	RGB(128,0,0),
	RGB(128,0,128),
	RGB(0,64,128),
	RGB(120,120,120),
	RGB(80,80,80),
	RGB(0,0,255),
	RGB(0,255,0),
	RGB(0,255,255),
	RGB(255,0,0),
	RGB(255,0,255),
	RGB(255,255,0),
	RGB(255,255,255)
};

CWnd *dosw = (CWnd *)NULL;
LOGBRUSH *nlb=NULL, flb[16];
CBrush *nb, fb[16];
CPen **cp;
CFont font, l_font;

int VLOCK = 0;

unsigned char bmp_file_head[14] = {
 66, 77, 54,  4,  2,  0,  0,  0,  0,  0, 54,  4,  0,  0};

unsigned char bmp_color_head[40] = {
 40,  0,  0,  0,184,  0,  0,  0,148,  0,
  0,  0,  1,  0, 24,  0,  0,  0,  0,  0,
  0,  0,  0,  0, 18, 11,  0,  0, 18, 11,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };

unsigned char bmp_gray_head[1064] = {
 40,  0,  0,  0,  0,  2,  0,  0,  0,  1,  
  0,  0,  1,  0,  8,  0,  0,  0,  0,  0,  
  0,  0,  2,  0, 35, 46,  0,  0, 35, 46,  
  0,  0,  0,  1,  0,  0,  0,  1,  0,  0};

CWnd *SelectWindow(CWnd *vr)
{
	CWnd *ow = dosw;
	dosw = vr;
	return ow;
}

void SetBmpHead()
{
	for(int i = 0; i < 256; i++) {
		bmp_gray_head[40+4*i] = i;
		bmp_gray_head[40+4*i+1] = i;
		bmp_gray_head[40+4*i+2] = i;
		bmp_gray_head[40+4*i+3] = 0;
	}
}

void GraphicMode()
{
	if(nlb) return;

    nlb = new LOGBRUSH;
	nlb->lbStyle = BS_NULL;

	nb = new CBrush;
	nb->CreateBrushIndirect(nlb);

	int i;
	for(i = 0; i < 16; i++) {
		flb[i].lbStyle = BS_SOLID;
		flb[i].lbColor = BColor[i];
	}

	for(i = 0; i < 16; i++)
		fb[i].CreateBrushIndirect(&flb[i]);
	
	cp = new CPen * [16];
	for(i = 0; i < 16; i++)
		cp[i] = new CPen(PS_SOLID, 1, BColor[i]);

	LOGFONT m_logFont;

	m_logFont.lfHeight			= 16;
	m_logFont.lfWidth			= 8;
	m_logFont.lfEscapement		= 0;
	m_logFont.lfOrientation		= 0;
	m_logFont.lfWeight			= FW_NORMAL;
	m_logFont.lfItalic			= FALSE;
	m_logFont.lfUnderline		= FALSE;
	m_logFont.lfStrikeOut		= FALSE;
	m_logFont.lfCharSet			= DEFAULT_CHARSET;
	m_logFont.lfOutPrecision	= OUT_CHARACTER_PRECIS;
	m_logFont.lfClipPrecision	= CLIP_CHARACTER_PRECIS;
	m_logFont.lfQuality			= DEFAULT_QUALITY;
	m_logFont.lfPitchAndFamily	= DEFAULT_PITCH|FF_DONTCARE;
	strcpy(m_logFont.lfFaceName, "굴림");

	font.CreateFontIndirect(&m_logFont);
	
	m_logFont.lfHeight			= 24;
	m_logFont.lfWidth			= 12;

	l_font.CreateFontIndirect(&m_logFont);

	SetBmpHead();
}

void GraphicMode(int nb)
{
	GraphicMode();
}

void TextMode()
{
}

void CharSize(int x, int y)
{
}

void SetCDC(CDC *tdc, int color, int fill)
{
	if(fill) tdc->SelectObject(&fb[color]);
	else tdc->SelectObject(nb);
	tdc->SelectObject(cp[color]);
}

void SetTextCDC(CDC *tdc, int color, int large)
{
	if(large) tdc->SelectObject(l_font);
	else tdc->SelectObject(font);
	tdc->SetBkMode(TRANSPARENT);
	tdc->SetTextColor(BColor[color]);
}

void OutText(CDC *dc, int x, int y, char *t, int large)
{
	SetTextCDC(dc, YELLOW, large);
	dc->TextOut(x, y, t, strlen(t));
}

void OutText(int x, int y, char *t, int large)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	OutText(dosdc, x, y, t, large);
	dosw->ReleaseDC(dosdc);
}

void OutText(CDC *dc, int x, int y, int color, char *t, int large)
{
	SetTextCDC(dc, color, large);
	dc->TextOut(x, y, t, strlen(t));
}

void OutText(int x, int y, int color, char *t, int large)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	OutText(dosdc, x, y, color, t, large);
	dosw->ReleaseDC(dosdc);
}

void OutText(short x, short y, char *t)
{
	if(VLOCK) return;
	if(!dosw) return;

	OutText(x, y, t, 0);
}

void OutColorText(short x, short y,short color, char *t)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	SetTextCDC(dosdc, color);
	dosdc->TextOut(x, y, t, strlen(t));
	dosw->ReleaseDC(dosdc);
}

void PutError(char *err)
{
	AfxMessageBox(err, MB_ICONERROR);
}

void PutErrorExit(char *err)
{
	AfxMessageBox(err, MB_ICONERROR);
	exit(0);
}

void gputs(short x, short y, char *s, int color)
{
	if(VLOCK) return;
	if(!dosw) return;

	OutText(x, y, color, s);
}

void gputs(CDC *pdc, short x, short y, char *s, int color)
{
	OutText(pdc, x, y, color, s);
}

void DispLine(CDC *tdc, int sx, int sy, int ex, int ey, int color )
{
	if(VLOCK) return;

	SetCDC(tdc, color, 1);
	tdc->MoveTo(sx, sy);
	tdc->LineTo(ex, ey);
}

void DispLine( int sx, int sy, int ex, int ey, int color )
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	DispLine(dosdc, sx, sy, ex, ey, color);
	dosw->ReleaseDC(dosdc);
}

void Ellipse(CDC *tdc, int sx, int sy, int ex, int ey, int color )
{
	if(VLOCK) return;

	SetCDC(tdc, color, 0);
	tdc->Ellipse(sx, sy, ex, ey);
}

void Ellipse( int sx, int sy, int ex, int ey, int color )
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	Ellipse(dosdc, sx, sy, ex, ey, color);
	dosw->ReleaseDC(dosdc);
}

void line(short sx, short sy, short ex, short ey, short color )
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	DispLine(dosdc, sx, sy, ex, ey, color);
	dosw->ReleaseDC(dosdc);
}

void Polygon(POINT *pp, int n, int color)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	SetCDC(dosdc, color, 0);
	dosdc->Polygon(pp, n);
	dosw->ReleaseDC(dosdc);
}

void Polygon(CDC *tdc, POINT *pp, int n, int color)
{
	SetCDC(tdc, color, 0);
	tdc->Polygon(pp, n);
}

void Rect(CDC *tdc, int sx, int sy, int ex, int ey, int color)
{
	SetCDC(tdc, color, 0);
	tdc->Rectangle(sx, sy, ex, ey);
}

void RectFill(CDC *tdc, int sx, int sy, int ex, int ey, int color)
{
	SetCDC(tdc, color, 1);
	tdc->Rectangle(sx, sy, ex, ey);
}

void Rect(int sx, int sy, int ex, int ey, int color)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	Rect(dosdc, sx, sy, ex, ey, color);
	dosw->ReleaseDC(dosdc);
}

void rectangle(short sx, short sy, short ex, short ey, short color)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	Rect(dosdc, sx, sy, ex, ey, color);
	dosw->ReleaseDC(dosdc);
}

void Rect(int sx, int sy, RECT r, int color, float scale)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	Rect(dosdc, sx+scale*r.left, sy+scale*r.top, sx+scale*r.right, sy+scale*r.bottom, color);
	dosw->ReleaseDC(dosdc);
}

void Rect(int sx, int sy, RECT r, int color)
{
	if(VLOCK) return;
	if(!dosw) return;

	float scale = 1;

	CDC *dosdc = dosw->GetDC();
	Rect(dosdc, sx+scale*r.left, sy+scale*r.top, sx+scale*r.right, sy+scale*r.bottom, color);
	dosw->ReleaseDC(dosdc);
}

void RectFill(int sx, int sy, int ex, int ey, int color)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	RectFill(dosdc, sx, sy, ex, ey, color);
	dosw->ReleaseDC(dosdc);
}

void bar(short sx, short sy, short ex, short ey, short color)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	RectFill(dosdc, sx, sy, ex, ey, color);
	dosw->ReleaseDC(dosdc);
}

void ClearScreen(void)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	RectFill(dosdc, 0, 0, 1600, 1200, 0);
	dosw->ReleaseDC(dosdc);
}

BYTE DIBHdr[14000000];
BYTE *tmp_img_p;
void FastPutImage(CDC *tdc, int dx, int dy, BYTE *buf, int w, int h)
{
	memcpy(DIBHdr, bmp_gray_head, 1064);

	LPBITMAPINFOHEADER bmp = (LPBITMAPINFOHEADER)DIBHdr;
	bmp->biWidth      = w;
	bmp->biHeight     = h;
	bmp->biSizeImage  = w*h;

	StretchDIBits(
				   tdc->m_hDC,   // hDC
				   dx,              // DestX
				   dy,              // DestY
				   w, 
				   h, 
				   0,                                  // SrcX
				   0,     // SrcY
				   w,                                 // SrcW
				   h,                                  // SrcH
				   buf,      // lpBits
				   (LPBITMAPINFO)DIBHdr,                // lpBitsInfo
				   DIB_RGB_COLORS,
				   SRCCOPY);      // wUsage
}

void PutImage(CDC *tdc, int dx, int dy, BYTE *buf, int w, int h, float scale)
{
	memcpy(DIBHdr, bmp_gray_head, 1064);
	tmp_img_p = &DIBHdr[1064];

	int dib_w;
	if(w%4 == 0) dib_w = w;
	else dib_w = w + 4 - w%4;

	for(int i = 0; i < h; i++)
		memcpy(tmp_img_p+i*dib_w, buf+(h-1-i)*w, w);

	LPBITMAPINFOHEADER bmp = (LPBITMAPINFOHEADER)DIBHdr;
	bmp->biWidth      = w;
	bmp->biHeight     = h;
	bmp->biSizeImage  = dib_w*h;

	tdc->SetStretchBltMode(COLORONCOLOR);
	StretchDIBits(
				   tdc->m_hDC,   // hDC
				   dx,              // DestX
				   dy,              // DestY
					(int)(scale*w), 
					(int)(scale*h), 
				   0,                                  // SrcX
				   0,     // SrcY
				   w,                                 // SrcW
				   h,                                  // SrcH
				   tmp_img_p,      // lpBits
				   (LPBITMAPINFO)DIBHdr,                // lpBitsInfo
				   DIB_RGB_COLORS,
				   SRCCOPY);      // wUsage
/*
	DrawDibDraw(vdd1, 
		        tdc->m_hDC, 
				dx, 
				dy, 
				(int)(scale*w), 
				(int)(scale*h), 
				(LPBITMAPINFOHEADER)DIBHdr, 
				tmp_img_p, 0, 0, w, h, 0);
				*/
}

void PutImage(CDC *tdc, int dx, int dy, BYTE **buf, int w, int h, float scale)
{
	memcpy(DIBHdr, bmp_gray_head, 1064);
	tmp_img_p = &DIBHdr[1064];

	int dib_w;
	if(w%4 == 0) dib_w = w;
	else dib_w = w + 4 - w%4;

	for(int i = 0; i < h; i++)
		memcpy(tmp_img_p+i*dib_w, buf[h-1-i], dib_w);

	LPBITMAPINFOHEADER bmp = (LPBITMAPINFOHEADER)DIBHdr;
	bmp->biWidth      = w;
	bmp->biHeight     = h;
	bmp->biSizeImage  = dib_w*h;

	tdc->SetStretchBltMode(COLORONCOLOR);
	StretchDIBits(
				   tdc->m_hDC,   // hDC
				   dx,              // DestX
				   dy,              // DestY
					(int)(scale*w), 
					(int)(scale*h), 
				   0,                                  // SrcX
				   0,     // SrcY
				   w,                                 // SrcW
				   h,                                  // SrcH
				   tmp_img_p,      // lpBits
				   (LPBITMAPINFO)DIBHdr,                // lpBitsInfo
				   DIB_RGB_COLORS,
				   SRCCOPY);      // wUsage
}

void PutColorImage(CDC *tdc, int dx, int dy, BYTE **buf, int w, int h, float scale)
{
	memcpy(DIBHdr, bmp_color_head, 40);
	tmp_img_p = &DIBHdr[40];

	int dib_w;
	if(w%4 == 0) dib_w = w;
	else dib_w = w + 4 - w%4;

	for(int i = 0; i < h; i++)
		memcpy(tmp_img_p+i*dib_w, buf[h-1-i], 3*dib_w);

	LPBITMAPINFOHEADER bmp = (LPBITMAPINFOHEADER)DIBHdr;
	bmp->biWidth      = w;
	bmp->biHeight     = h;
	bmp->biSizeImage  = dib_w*h;

	tdc->SetStretchBltMode(COLORONCOLOR);
	StretchDIBits(
				   tdc->m_hDC,   // hDC
				   dx,              // DestX
				   dy,              // DestY
					(int)(scale*w), 
					(int)(scale*h), 
				   0,                                  // SrcX
				   0,     // SrcY
				   w,                                 // SrcW
				   h,                                  // SrcH
				   tmp_img_p,      // lpBits
				   (LPBITMAPINFO)DIBHdr,                // lpBitsInfo
				   DIB_RGB_COLORS,
				   SRCCOPY);      // wUsage
}

void PutImage(CDC *tdc, int dx, int dy, int **buf, int w, int h, float scale)
{
	memcpy(DIBHdr, bmp_gray_head, 1064);
	tmp_img_p = &DIBHdr[1064];

	int dib_w;
	if(w%4 == 0) dib_w = w;
	else dib_w = w + 4 - w%4;

	for(int i = 0; i < h; i++)
		for(int j = 0; j < dib_w; j++)
			tmp_img_p[i*dib_w + j] = buf[h-1-i][j];

	LPBITMAPINFOHEADER bmp = (LPBITMAPINFOHEADER)DIBHdr;
	bmp->biWidth      = w;
	bmp->biHeight     = h;
	bmp->biSizeImage  = dib_w*h;

	tdc->SetStretchBltMode(COLORONCOLOR);
	StretchDIBits(
				   tdc->m_hDC,   // hDC
				   dx,              // DestX
				   dy,              // DestY
					(int)(scale*w), 
					(int)(scale*h), 
				   0,                                  // SrcX
				   0,     // SrcY
				   w,                                 // SrcW
				   h,                                  // SrcH
				   tmp_img_p,      // lpBits
				   (LPBITMAPINFO)DIBHdr,                // lpBitsInfo
				   DIB_RGB_COLORS,
				   SRCCOPY);      // wUsage

}

void PutImage(int dx, int dy, BYTE *img, int w, int h, float scale)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	PutImage(dosdc, dx, dy, img, w, h, scale);
	dosw->ReleaseDC(dosdc);
}

void FastPutImage(int dx, int dy, BYTE *buf, int w, int h)
{
	if(!dosw) return;
	CDC *dosdc = dosw->GetDC();
	FastPutImage(dosdc, dx, dy, buf, w, h);
	dosw->ReleaseDC(dosdc);
}

void PutImage(int dx, int dy, BYTE **img, int w, int h, float scale)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	PutImage(dosdc, dx, dy, img, w, h, scale);
	dosw->ReleaseDC(dosdc);
}

void PutImageOnly(int dx, int dy, BYTE *img, int w, int h, float scale)
{
	CDC *dosdc = dosw->GetDC();
	PutImage(dosdc, dx, dy, img, w, h, scale);
	dosw->ReleaseDC(dosdc);
}

void PutImage(int dx, int dy, int **img, int w, int h, int scale)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	PutImage(dosdc, dx, dy, img, w, h, (float)scale);
	dosw->ReleaseDC(dosdc);
}

void PutImageMat(int dx, int dy, int **img, int w, int h, int scale)
{
	if(VLOCK) return;
	if(!dosw) return;

	CDC *dosdc = dosw->GetDC();
	PutImage(dosdc, dx, dy, img, w, h, (float)scale);
	dosw->ReleaseDC(dosdc);
}

void PutColorImage(int dx, int dy, int dw, int dh,
	              BYTE *bmp_buf, int sx, int sy, int sw, int sh) 
{
	CDC *dosdc = dosw->GetDC();
    BITMAPINFOHEADER *bmp = (BITMAPINFOHEADER *)bmp_buf;
    StretchDIBits(
       dosdc->m_hDC,		// hDC
       dx,              // DestX
       dy,              // DestY
       dw,				// DestW
       dh,				//  DestH
       sx,					// SrcX
       bmp->biHeight-sh-sy, // SrcY
       sw,                  // SrcW
       sh,                  // SrcH
       bmp_buf+40+4*bmp->biClrUsed,      // lpBits
       (BITMAPINFO *)bmp_buf,            // lpBitsInfo
       DIB_RGB_COLORS,
       SRCCOPY
	);      // wUsage

	dosw->ReleaseDC(dosdc);
}

void REllipse(int dx, int dy, int dw, int dh, double m_Angle, int color)
{
	POINT centerPt;
	centerPt.x = dx;
	centerPt.y = dy;

	//--------------------------------
	// 좌표 회전
	//--------------------------------
	CDC *dc = dosw->GetDC();
	HDC hDc = dc->GetSafeHdc();
	int nGraphicsMode = SetGraphicsMode(hDc, GM_ADVANCED);
	XFORM xform;

	if ( m_Angle != 0 )
	{
		double angle = (double)m_Angle;

		xform.eM11 = (float) cos(angle);
		xform.eM12 = (float) sin(angle);
		xform.eM21 = (float)-sin(angle);
		xform.eM22 = (float) cos(angle);
		xform.eDx  = (float)(centerPt.x - cos(angle)*centerPt.x + sin(angle)*centerPt.y);
		xform.eDy  = (float)(centerPt.y - cos(angle)*centerPt.y - sin(angle)*centerPt.x);

		SetWorldTransform(hDc, &xform);
	}

	//--------------------------------
	// 회전된 상태에서 처리 
	//-------------------------------- 
	SetCDC(dc, color, 0);

	Ellipse(hDc, centerPt.x - dw, centerPt.y - dh, 
				 centerPt.x + dw, centerPt.y + dh );
	Ellipse(hDc, centerPt.x - dw - 1, centerPt.y - dh - 1, 
				 centerPt.x + dw + 1, centerPt.y + dh + 1);
	
	//--------------------------------
	// 좌표 원위치 
	//-------------------------------- 

	if ( m_Angle != 0 )
	{
		xform.eM11 = (float)1.0; 
		xform.eM12 = (float)0;
		xform.eM21 = (float)0;
		xform.eM22 = (float)1.0;
		xform.eDx  = (float)0;
		xform.eDy  = (float)0;
    
		SetWorldTransform(hDc, &xform);
	}
	SetGraphicsMode(hDc, nGraphicsMode);

	dosw->ReleaseDC(dc);
}

unsigned char tbmp_color_head[40] = {
 40,  0,  0,  0,184,  0,  0,  0,148,  0,
  0,  0,  1,  0, 24,  0,  0,  0,  0,  0,
  0,  0,  0,  0, 18, 11,  0,  0, 18, 11,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 };

void PutColorImage(int dx, int dy, BYTE *bmp_buf, int w, int h, float scale) 
{
	CDC *dosdc = dosw->GetDC();

    BITMAPINFOHEADER *bmp = (BITMAPINFOHEADER *)tbmp_color_head;
	
	int dib_w;
	if(w%4 == 0) dib_w = w;
	else dib_w = w + 4 - w%4;

	for(int i = 1; i <= h; i++)
		memcpy(DIBHdr+(h-i)*3*dib_w, bmp_buf + i*3*w, 3*w);

	bmp->biWidth      = dib_w;
	bmp->biHeight     = h;
	bmp->biSizeImage  = 3*dib_w*h;

    StretchDIBits(
       dosdc->m_hDC,		// hDC
       dx,              // DestX
       dy,              // DestY
		(int)(scale*w), 
		(int)(scale*h), 
       0,					// SrcX
       0, // SrcY
       w,                  // SrcW
       h,                  // SrcH
       DIBHdr,      // lpBits
       (BITMAPINFO *)bmp,            // lpBitsInfo
       DIB_RGB_COLORS,
       SRCCOPY
	);      // wUsage

}

