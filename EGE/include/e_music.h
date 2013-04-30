#pragma once

#include <wingdi.h>
#include "e_base.h"
#include "e_mac.h"
#include "e_img.h"

namespace ege
{

//�������
const auto MUSIC_ERROR(0xFFFFFFFF);

class MUSIC
{
public:
	MUSIC();
	virtual ~MUSIC();
	operator HWND()const{return (HWND)m_dwCallBack;}
public:
	int   IsOpen() {return (m_DID != MUSIC_ERROR) ? 1 : 0;}
	DWORD OpenFile(LPCSTR  filepath);
	DWORD OpenFile(LPCWSTR filepath);
	DWORD Play(DWORD dwFrom=MUSIC_ERROR, DWORD dwTo=MUSIC_ERROR);
	DWORD Pause();
	DWORD Seek(DWORD dwTo); //����λ�ö�λ����λΪms
	DWORD SetVolume(float value);
	DWORD Close();
	DWORD Stop();
	DWORD GetPosition();
	DWORD GetLength();
	// ���º���GetPlayStatus�ķ���ֵΪ����֮һ�����忴��׺����
	// MUSIC_MODE_NOT_OPEN   //û����ȷ��
	// MUSIC_MODE_NOT_READY  //�豸û׼���� ������ʹ�ã�
	// MUSIC_MODE_PAUSE  //��ͣ��
	// MUSIC_MODE_PLAY   //���ڲ���
	// MUSIC_MODE_STOP   //�ɹ��򿪺󣬻��߲����������״̬
	// MUSIC_MODE_OPEN   //���� ������ʹ�ã�
	// MUSIC_MODE_SEEK   //��λ�� ������ʹ�ã�
	DWORD GetPlayStatus();
private:
	DWORD       m_DID;
	PVOID       m_dwCallBack;
};


}

