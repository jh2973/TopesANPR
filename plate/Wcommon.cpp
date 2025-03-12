#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <io.h>
#include <ctype.h>

#include "wcommon.h"
#include "Wdisp.h"

unsigned char **cmatrix(int w, int h)
{
	int     i;
	unsigned char   **m;

	m = new unsigned char * [h];
	if(!m) PutError("memory alloc failure in imatrix()"), exit(0);

	m[0] = new unsigned char [w*h];
	if(!m[0]) PutError("memory alloc failure in imatrix()"), exit(0);

	for(i = 1; i < h; i++) {
		m[i] = &m[0][i*w];
	}
	return  m;
}

short **smatrix(int w, int h)
{
	int     i;
	short   **m;

	m = new short * [h];
	if(!m) PutError("memory alloc failure in imatrix()"), exit(0);

	m[0] = new short [w*h];
	if(!m[0]) PutError("memory alloc failure in imatrix()"), exit(0);

	for(i = 1; i < h; i++) {
		m[i] = &m[0][i*w];
	}
	return  m;
}

int **imatrix(int w, int h)
{
	int     i;
	int   **m;

	m = new int * [h];
	if(!m) PutError("memory alloc failure in imatrix()"), exit(0);

	m[0] = new int [w*h];
	if(!m[0]) PutError("memory alloc failure in imatrix()"), exit(0);

	for(i = 1; i < h; i++) {
		m[i] = &m[0][i*w];
	}
	return  m;
}

float **fmatrix(int w, int h)
{
	int     i;
	float   **m;

	m = new float * [h];
	if(!m) PutError("memory alloc failure in fmatrix()"), exit(0);

	m[0] = new float [w*h];
	if(!m[0]) PutError("memory alloc failure in fmatrix()"), exit(0);

	for(i = 1; i < h; i++) {
		m[i] = &m[0][i*w];
	}

	return  m;
}

int *ivector(int nl, int nh)
{
	int *v;
	
	v=(int*)malloc((unsigned)(nh-nl+1)*sizeof(int));
	if (!v) PutError("allocation failure in ivector()");
	return v-nl;
}

double *dvector(int nl, int nh)
{
	double *v;
	
	v=(double*)malloc((unsigned)(nh-nl+1)*sizeof(double));
	if (!v) PutError("allocation failure in vector()");
	return v-nl;
}

int GetFileName(char *dir, char *ext, char **afn)
{
	struct _finddata_t  fileinfo;
	long                handle;
	int                 rc;

	char fn[80];
	sprintf(fn, "%s\\*.%s", dir, ext);

	handle = _findfirst(fn, &fileinfo);
	rc = handle;

	int i = 0;
	while( rc != -1 ) {
	  sprintf((char *)afn[i], "%s", fileinfo.name);
	  rc = _findnext( handle, &fileinfo );
	  i++;
	}

	_findclose( handle );
	return i;
}

int GetFileType(char *str)
{
	for(unsigned int i = 0; i < strlen(str); i++) str[i] = toupper(str[i]);

	char *ext = strchr(str, '.');
	if(!ext) return INVALID_FILE_TYPE;
	ext++;

	if(!strcmp(ext, "TGA")) return TGA_TYPE;
	else if(!strcmp(ext, "BMP")) return BMP_TYPE;
	else if(!strcmp(ext, "DAT")) return DAT_TYPE;
	else if(!strcmp(ext, "RAW")) return RAW_TYPE;
	else if(!strcmp(ext, "FLD")) return FLD_TYPE;
	else if(!strcmp(ext, "TIF")) return TIF_TYPE;
	else return INVALID_FILE_TYPE;
}

char *GetDataFromFile(char *fn)
{
	FILE *fp = fopen(fn, "rb");
	
	char str[80];
	if(!fp) { 
		sprintf(str, "%s file not exist", fn);
		PutError(str); 
		exit(0); 
	}

	fseek(fp, 0L, SEEK_END);
	long fl = ftell(fp);
	char *buf = new char [fl+1];

	fseek(fp, 0L, SEEK_SET);
	fread(buf, fl, 1, fp);
	fclose(fp);

	buf[fl] = 0;
	return buf;
}

char *GetCurrentDataFromBuf(char *prm, char *rp_str)
{
	char *start = strstr(prm, rp_str);
	if(!start) { PutError(rp_str); exit(0); }

	start = strchr(start, '{');
	if(!start) { PutError(rp_str); exit(0); }
	char *end = strchr(start, '}');
	if(!end) { PutError(rp_str); exit(0); }

	char *buf = new char [end-start];
	int i;
	for(i = 0; i < end-start-1; i++)
		buf[i] = start[i+1];
	buf[i] = 0;

	return buf;
}

int ExistStr(char *str, char *sub, int &pos)
{
	// 주어진 문자열 존재 여부 검사
	int flag = 0;
	int next = 0;
	char tstr[80];

	while(1) {
		if(sscanf(str+next, "%s", tstr) == EOF) break;
		if(!strcmp(sub, tstr)) {
			flag = 1;
			break;
		}
		next += strlen(tstr);
	}

	pos = next;

	if(!flag) return 0;
	else return 1;
}

int GetVal(char *str, char *sub)
{
	int pos = 0;
	if(!ExistStr(str, sub, pos)) return INVALID_NO;

	char *fch = strchr(str+pos, '=');
	if(!fch) return INVALID_NO;

	char val[80];
	sscanf(fch+1, "%s", val);

	return atoi(val);
}

char *GetStr(char *str, char *sub)
{
	int pos = 0;
	if(!ExistStr(str, sub, pos)) return 0;

	char *fch = strchr(str+pos, '=');
	if(!fch) return 0;

	char *set = new char [80];
	sscanf(fch+1, "%s", set);

	return set;
}

float GetValF(char *str, char *sub)
{
	int pos = 0;
	if(!ExistStr(str, sub, pos)) return INVALID_NO;

	char *fch = strchr(str+pos, '=');
	if(!fch) return INVALID_NO;

	char val[80];
	sscanf(fch+1, "%s", val);

	return (float)atof(val);
}

RECT GetValRect(char *str, char *sub)
{
	RECT r;
	char *pos = strstr(str, sub);
	if(!pos) { r.left = -1; return r; }

	pos = strchr(pos, '=');
	if(!pos) { r.left = -1; return r; }

	char val[4][50];
	sscanf(pos+1, "%s%s%s%s", val[0], val[1], val[2], val[3]);
	r.left = atoi(val[0]);
	r.top  = atoi(val[1]);
	r.right = atoi(val[2]);
	r.bottom = atoi(val[3]);

	return r;
}

RECT GetValFRect(char *str, int bwp, int bhp, char *sub)
{
	RECT r;
	char *pos = strstr(str, sub);
	if(!pos) { r.left = -1; return r; }

	pos = strchr(pos, '=');
	if(!pos) { r.left = -1; return r; }

	char val[4][50];
	sscanf(pos+1, "%s%s%s%s", val[0], val[1], val[2], val[3]);

	float fval;
	fval = atof(val[0]);    r.left = bwp*fval + 0.5;
	fval = atof(val[1]);    r.top = bhp*fval + 0.5;
	fval = atof(val[2]);    r.right = bwp*fval + 0.5;
	fval = atof(val[3]);    r.bottom = bhp*fval + 0.5;

	return r;
}

#define IS_INT		3
#define IS_FLOAT    2
#define IS_STRING   1
#define IS_NONE     0

int isint(char *str)
{
	for(int i = 0; i < strlen(str); i++)
		if(!isdigit(str[i])) return 0;

	return 1;
}

int isfloat(char *str)
{
	int point = 0;
	for(int i = 0; i < strlen(str); i++)
		if(!isdigit(str[i]) && str[i] != '.') return 0;
		else if(str[i] == '.') point++;

	if(point == 1) return 1;
	else return 0;
}

int isstring(char *str)
{
	for(int i = 0; i < strlen(str); i++)
		if(!isdigit(str[i]) && !isalpha(str[i])) return 0;

	return 1;
}

int istype(char *str)
{
	if(isint(str)) return IS_INT;
	else if(isfloat(str)) return IS_FLOAT;
	else if(isstring(str)) return IS_STRING;
	else return IS_NONE;
}

void SetValue(char *fn, char *setprm, char *val)
{
	/*if(istype(val) == IS_NONE) {
		PutError("Not Surported Paramter Type!");
		return;
	}*/

	FILE *fp = fopen(fn, "r");

	int nline = 0;
	int ch;

	while((ch = getc(fp)) != EOF)
		if(ch == '\n') nline ++;

	char **prm;
	prm = new char * [nline];
	int i;
	for(i = 0; i < nline; i++) {
		prm[i] = new char [100];
		memset(prm[i], 0, 100);
	}

	fseek(fp, 0L, SEEK_SET);
	nline = 0;
	int t = 0;
	while((ch = getc(fp)) != EOF) {
		prm[nline][t++] = ch;
		if(ch == '\n') {
			nline ++;
			t = 0;
		}
	}

	fclose(fp);

	char old_prm[80], dummy[10], old_val[80];
	int flag = 0;
	for(i = 0; i < nline; i++) {
		char *ptr = strstr(prm[i], setprm);
		if(ptr) {
			sscanf(prm[i], "%s%s%s", old_prm, dummy, old_val);

			//if(istype(old_val) != istype(val)) {
			//	printf("Parameter Value Type is Not Matched!\n");
			//	exit(0);
			//}

			sprintf(prm[i], "%s = %s\n", setprm, val);
			flag = 1;
		}
	}

	//if(!flag) printf("%s paramter not exist\n", setprm);
	//else {
		fp = fopen(fn, "w");
		for(i = 0; i < nline; i++) {
			t = 0;
			while(prm[i][t]) putc(prm[i][t++], fp);
		}
		fclose(fp);
	//}

}