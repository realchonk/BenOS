#ifndef FILE_MATH_H
#define FILE_MATH_H

#ifdef __cplusplus
extern "C" {
#endif
	
#define signbit(x) __builtin_signbit(x)
#define isnan(x) __builtin_isnan(x)
#define isinf(x) __builtin_isinf(x)

double rad2deg(double rad);
double deg2rad(double deg);

double sqrt(double x);
float sqrtf(float f);

double sin(double x);
float sinf(float x);

double cos(double x);
float cosf(float x);

double fabs(double x);
float fabsf(float x);

double tan(double x);
float tanf(float x);

double exp(double x);
float expf(float x);

double pow(double x, double y);
float powf(float x, float y);

double round(double x);
float roundf(float x);

double fmin(double a, double b);
float fminf(float a, float b);

double fmax(double a, double b);
float fmaxf(float a, float b);

#ifdef __cplusplus
}
#endif

#endif /* FILE_MATH_H */