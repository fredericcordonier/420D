/**
 * \file float.h
 * \brief Header for float.c
 */
#ifndef FLOAT_H_
#define FLOAT_H_

#ifdef __cplusplus
extern "C" {
#endif

float float_abs(float x);
float float_exp(float x);
float float_log(float x);
float float_pow(float x, float c);

float float_log2(float x);
float float_pow2(float x);

#ifdef __cplusplus
}
#endif

#endif /* FLOAT_H_ */
