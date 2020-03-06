#ifndef FILE_CTYPE_H
#define FILE_CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

inline static int isdigit(int ch) {
	return (ch >= '0') && (ch <= '9');
}
inline static int islower(int ch) {
	return (ch >= 'a') && (ch <= 'z');
}
inline static int isupper(int ch) {
	return (ch >= 'A') && (ch <= 'Z');
}
inline static int isalpha(int ch) {
	return islower(ch) || isupper(ch);
}
inline static int isalnum(int ch) {
	return isalpha(ch) || isdigit(ch);
}
inline static int isspace(int ch) {
	return (ch >= 0x09 && ch <=	0x0d) || (ch == 0x20);
}
inline static int isxdigit(int ch) {
	return isdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}
inline static int iscntrl(int ch) {
	return (ch >= 0x00 && ch <= 0x1f) || (ch == 0x7f);
}
inline static int isblank(int ch) {
	return ch == 0x09 || ch == 0x20;
}
inline static int ispunct(int ch) {
	return (ch >= 0x21 && ch <= 0x2f) || (ch >= 0x3a && ch <= 0x40) || (ch >= 0x5b && ch <= 0x60) || (ch >= 0x7b && ch <= 0x7e);
}
inline static int isgraph(int ch) {
	return (ch >= 0x21 && ch <= 0x7e);
}
inline static int isprint(int ch) {
	return (ch >= 0x20 && ch <= 0x7e);
}


inline static int tolower(int ch) {
	return isupper(ch) ? ch + 0x20 : ch;
}
inline static int toupper(int ch) {
	return islower(ch) ? ch - 0x20 : ch;
}

#ifdef __cplusplus
}
#endif

#endif /* FILE_CTYPE_H */