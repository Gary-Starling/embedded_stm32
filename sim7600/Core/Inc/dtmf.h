#ifndef _DTMF_H_
#define _DTMF_H_

#include <stdbool.h>
#include "sim76xx.h"

/* ������� �������� ����� ������ */
void dtmfPass(const sExtraMsg * pDtmf, const char * pass, bool * pass_fl, bool flush);

/* ���� ������� ������ � dtmf ��������� */
void dtmfLed(const sExtraMsg * pExtra, bool flush);

#endif