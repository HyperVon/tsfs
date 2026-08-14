#ifndef _HLS_H_
#define _HLS_H_

#define UNDEFINED -1

void RGB_to_HLS(float r, float g, float b, float *h, float *l, float *s);
float value(float n1, float n2, float hue);
void HLS_to_RGB(float h, float l, float s, float *r, float *g, float *b);

#endif
