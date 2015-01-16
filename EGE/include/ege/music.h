#ifndef Inc_ege_music_h_
#define Inc_ege_music_h_

#include "ege/def.h"
#include <windows.h>

namespace ege
{

//音乐类宏
const auto MUSIC_ERROR(0xFFFFFFFF);

class EGEAPI MUSIC
{
public:
	MUSIC();
	virtual ~MUSIC();
	operator ::HWND()const
	{
		return (::HWND)m_dwCallBack;
	}
public:
	int IsOpen()
	{
		return (m_DID != MUSIC_ERROR) ? 1 : 0;
	}
	unsigned long OpenFile(const char*  filepath);
	unsigned long OpenFile(const wchar_t* filepath);
	unsigned long Play(unsigned long dwFrom = MUSIC_ERROR, unsigned long dwTo = MUSIC_ERROR);
	unsigned long Pause();
	unsigned long Seek(unsigned long dwTo); //播放位置定位，单位为ms
	unsigned long SetVolume(float value);
	unsigned long Close();
	unsigned long Stop();
	unsigned long GetPosition();
	unsigned long GetLength();
	// 以下函数GetPlayStatus的返回值为以下之一（意义看后缀）：
	// MUSIC_MODE_NOT_OPEN   //没有正确打开
	// MUSIC_MODE_NOT_READY  //设备没准备好（较少使用）
	// MUSIC_MODE_PAUSE  //暂停中
	// MUSIC_MODE_PLAY   //正在播放
	// MUSIC_MODE_STOP   //成功打开后，或者播放完是这个状态
	// MUSIC_MODE_OPEN   //打开中（较少使用）
	// MUSIC_MODE_SEEK   //定位中（较少使用）
	unsigned long GetPlayStatus();
private:
	unsigned long m_DID;
	void* m_dwCallBack;
};

}

#endif

