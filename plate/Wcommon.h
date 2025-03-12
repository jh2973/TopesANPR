#ifndef WCOMMONH
#define WCOMMONH

#define INVALID_NO	1000000

typedef unsigned char BYTE;
typedef unsigned char PIXEL;

extern unsigned char bmp_file_head[14];
extern unsigned char bmp_color_head[40];
extern unsigned char bmp_gray_head[1064];

unsigned char **cmatrix(int w, int h);
short **smatrix(int w, int h);
int **imatrix(int w, int h);
float **fmatrix(int w, int h);

int *ivector(int nl, int nh);
double *dvector(int nl, int nh);

int GetFileName(char *dir, char *ext, char **afn);

#define INVALID_FILE_TYPE       0
#define BMP_TYPE                1
#define TGA_TYPE                2
#define RAW_TYPE                3
#define FLD_TYPE                4
#define TIF_TYPE                5
#define DAT_TYPE                6

int  GetFileType(char *str);
char *GetDataFromFile(char *str);
char *GetCurrentDataFromBuf(char *f_data, char *rp_str);
int  GetVal(char *str, char *sub);
char *GetStr(char *str, char *sub);
float GetValF(char *str, char *sub);
void SetValue(char *fn, char *setprm, char *val);


#endif