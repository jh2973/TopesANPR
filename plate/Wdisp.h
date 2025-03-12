#ifndef _WDISP
#define _WDISP

#include <afxwin.h>        

#define    BLACK			0
#define	   BLUE				1
#define    GREEN			2
#define    CYAN				3
#define    RED				4
#define    MAGENTA			5
#define    BROWN			6
#define    LIGHTGRAY		7
#define    DARKGRAY			8
#define    LIGHTBLUE		9
#define    LIGHTGREEN		10
#define    LIGHTCYAN		11
#define    LIGHTRED			12
#define    LIGHTMAGENTA		13
#define    YELLOW			14
#define    WHITE			15

typedef unsigned char PIXEL;
extern COLORREF BColor[16];

void FastPutImage(int dx, int dy, BYTE *buf, int w, int h);
void PutImage(int dx, int dy, BYTE *img, int w, int h, float scale=1.0);
void PutImage(int dx, int dy, BYTE **img, int w, int h, float scale = 1);
void PutImageOnly(int dx, int dy, BYTE *img, int w, int h, float scale = 1);
void PutImage(int dx, int dy, int **img, int w, int h, int sclae=1);
void PutImageMat(int dx, int dy, int **img, int w, int h, int scale);

CWnd *SelectWindow(CWnd *vr);
void SetBmpHead();
void GraphicMode(int nb);
void TextMode();
void CharSize(int, int);
void ClearScreen(void);
void Polygon(POINT *pp, int n, int color);

void SetTextCDC(CDC *tdc, int color, int large);

void OutText(short x, short y, char *t);
void OutText(int x, int y, char *t, int large = 0);
void OutText(int x, int y, int color, char *t, int large = 0);
void DispLine(int sx, int sy, int ex, int ey, int color );
void line(short sx, short sy, short ex, short ey, short color );

void Rect(int sx, int sy, int ex, int ey, int color);
void Rect(int sx, int sy, RECT r, int color);
void RectFill(int sx, int sy, int ex, int ey, int color);
void rectangle(short sx, short sy, short ex, short ey, short color);
void bar(short sx, short sy, short ex, short ey, short color);

void Ellipse( int sx, int sy, int ex, int ey, int color );
void REllipse(int dx, int dy, int dw, int dh, double m_Angle, int color);

void PutError(char *err);
void PutErrorExit(char *err);

//--- with DC pointer -----------------------------------------
void SetTextCDC(CDC *tdc, int color, int large = 0);

void OutText(CDC *tdc, int x, int y, char *t, int large = 0);
void OutText(CDC *tdc, int x, int y, int color, char *t, int large = 0);
void gputs(CDC *pdc, short x, short y, char *s, int color);

void DispLine(CDC *tdc, int sx, int sy, int ex, int ey, int color );

void Rect(CDC *tdc, int sx, int sy, int ex, int ey, int color);
void RectFill(CDC *tdc, int sx, int sy, int ex, int ey, int color);

void PutImage(CDC *tdc, int dx, int dy, BYTE *buf, int w, int h, float scale);
void PutImage(CDC *tdc, int dx, int dy, BYTE **buf, int w, int h, float scale);

void Polygon(CDC *tdc, POINT *pp, int n, int color);

void FastPutImage(CDC *tdc, int dx, int dy, BYTE *buf, int w, int h);
void PutColorImage(int dx, int dy, int dw, int dh,
	              BYTE *bmp_buf, int sx, int sy, int sw, int sh) ;
void PutColorImage(int dx, int dy, BYTE *bmp_buf, int w, int h, float scale) ;
#endif