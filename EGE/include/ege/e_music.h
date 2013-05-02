#ifndef Inc_ege_e_music_h_
#define Inc_ege_e_music_h_

#include <wingdi.h>
#include "e_base.h"
#include "e_mac.h"
#include "e_img.h"

namespace ege
{

//音乐类宏
const auto MUSIC_ERROR(0xFFFFFFFF);

class MUSIC
{
public:
	MUSIC();
	virtual ~MUSIC();
	operator ::HWND()const
	{
		return (::HWND)m_dwCallBack;
	}
public:
	int   IsOpen()
	{
		return (m_DID != MUSIC_ERROR) ? 1 : 0;
	}
	::DWORD OpenFile(const char*  filepath);
	::DWORD OpenFile(const wchar_t* filepath);
	::DWORD Play(::DWORD dwFrom = MUSIC_ERROR, ::DWORD dwTo = MUSIC_ERROR);
	::DWORD Pause();
	::DWORD Seek(::DWORD dwTo); //播放位置定位，单位为ms
	::DWORD SetVolume(float value);
	::DWORD Close();
	::DWORD Stop();
	::DWORD GetPosition();
	::DWORD GetLength();
	// 以下函数GetPlayStatus的返回值为以下之一（意义看后缀）：
	// MUSIC_MODE_NOT_OPEN   //没有正确打开
	// MUSIC_MODE_NOT_READY  //设备没准备好（较少使用）
	// MUSIC_MODE_PAUSE  //暂停中
	// MUSIC_MODE_PLAY   //正在播放
	// MUSIC_MODE_STOP   //成功打开后，或者播放完是这个状态
	// MUSIC_MODE_OPEN   //打开中（较少使用）
	// MUSIC_MODE_SEEK   //定位中（较少使用）
	::DWORD GetPlayStatus();
private:
	::DWORD       m_DID;
	PVOID       m_dwCallBack;
};

}

#endif

