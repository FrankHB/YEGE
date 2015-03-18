#include "ege.h"
#include <mmsystem.h>
#include <Digitalv.h>
#include <synchapi.h>

namespace ege
{

// Class MUSIC Construction
MUSIC::MUSIC()
{
	m_DID = MUSIC_ERROR;
	m_dwCallBack = {};
}

// Class MUSIC Destruction
MUSIC::~MUSIC()
{
	if(m_DID != MUSIC_ERROR)
		Close();
}

// mciOpenFileA(const char* _szStr)
// open a music file. szStr: Path of the file
unsigned long
MUSIC::OpenFile(const char* _szStr)
{
	::MCIERROR mciERR = 0;
	auto mci_p = ::MCI_OPEN_PARMSA();

	mci_p.lpstrElementName = _szStr;
	mci_p.lpstrDeviceType = {};
	mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);

	if(m_DID != MUSIC_ERROR)
		Close();

	mciERR = ::mciSendCommandA(
				 0,
				 MCI_OPEN,
				 MCI_NOTIFY | MCI_OPEN_ELEMENT,
				 ::DWORD_PTR(&mci_p));

	if(mciERR == 0)
	{
		m_DID = mci_p.wDeviceID;

		// Set time format with milliseconds
		{
			::MCI_SET_PARMS mci_p{::DWORD_PTR(), MCI_FORMAT_MILLISECONDS, 0UL};

			::mciSendCommandW(m_DID, MCI_SET, MCI_NOTIFY | MCI_SET_TIME_FORMAT,
				::DWORD_PTR(&mci_p));
		}
	}

	return mciERR;
}


// mciOpenFile(const wchar_t* _szStr)
// open a music file. szStr: Path of the file
unsigned long
MUSIC::OpenFile(const wchar_t* _szStr)
{
	::MCIERROR mciERR  = 0;
	auto mci_p = ::MCI_OPEN_PARMSW();

	mci_p.lpstrElementName = _szStr;
	mci_p.lpstrDeviceType = {};
	mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);

	if(m_DID != MUSIC_ERROR)
		Close();
	mciERR = ::mciSendCommandW(0, MCI_OPEN, MCI_NOTIFY | MCI_OPEN_ELEMENT,
		::DWORD_PTR(&mci_p));

	if(mciERR == 0)
	{
		m_DID = mci_p.wDeviceID;

		// Set time format with milliseconds
		auto mci_p = ::MCI_SET_PARMS();

		mci_p.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
		//unsigned long dw =
		::mciSendCommandW(m_DID, MCI_SET, MCI_NOTIFY | MCI_SET_TIME_FORMAT,
			::DWORD_PTR(&mci_p));
	}
	return mciERR;
}

// mciPlay(unsigned long dwFrom, unsigned long dwTo, unsigned long dwCallBack)
// play the music stream.
unsigned long
MUSIC::Play(unsigned long dwFrom, unsigned long dwTo)
{
	yconstraint(m_DID != MUSIC_ERROR);

	::MCIERROR mciERR = 0;
	auto mci_p = ::MCI_PLAY_PARMS();
	unsigned long dwFlag = MCI_NOTIFY;

	mci_p.dwFrom = dwFrom;
	mci_p.dwTo = dwTo;
	mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);

	if(dwFrom != MUSIC_ERROR)
		dwFlag |= MCI_FROM;
	if(dwTo != MUSIC_ERROR)
		dwFlag |= MCI_TO;
	mciERR = ::mciSendCommandW(m_DID, MCI_PLAY, dwFlag, ::DWORD_PTR(&mci_p));
	::Sleep(1);
	return mciERR;
}

// mciPause()
// pause the music stream.
unsigned long MUSIC::Pause()
{
	yconstraint(m_DID != MUSIC_ERROR);

	::MCIERROR mciERR = 0;
	auto mci_p = ::MCI_GENERIC_PARMS();

	mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);
	mciERR = ::mciSendCommandW(m_DID, MCI_PAUSE, MCI_NOTIFY, ::DWORD_PTR(&mci_p));
	return mciERR;
}


// mciStop()
// stop the music stream.

unsigned long MUSIC::Stop()
{
	yconstraint(m_DID != MUSIC_ERROR);

	::MCIERROR mciERR = 0;
	auto mci_p = ::MCI_GENERIC_PARMS();

	mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);
	mciERR = ::mciSendCommandW(m_DID, MCI_STOP, MCI_NOTIFY, ::DWORD_PTR(&mci_p));
	return mciERR;
}

unsigned long MUSIC::SetVolume(float value)
{
	yconstraint(m_DID != MUSIC_ERROR);

	::MCIERROR mciERR = 0;
	auto mci_p = ::MCI_DGV_SETAUDIO_PARMSW();

	mci_p.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	//此处就是音量大小 (0--1000)
	mci_p.dwValue = static_cast<unsigned long>(value * 1000);
	mciERR = ::mciSendCommandW(m_DID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE
		| MCI_DGV_SETAUDIO_ITEM, ::DWORD_PTR(&mci_p));
	return mciERR;
}

// mciSeek(unsigned long dwTo)
// seek the music stream playposition to `dwTo`
unsigned long MUSIC::Seek(unsigned long dwTo)
{
	yconstraint(m_DID != MUSIC_ERROR);

	::MCIERROR mciERR = 0;
	auto mci_p = ::MCI_SEEK_PARMS();

	mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);
	mci_p.dwTo = dwTo;

	mciERR = ::mciSendCommandW(m_DID, MCI_SEEK, MCI_NOTIFY, ::DWORD_PTR(&mci_p));

	return mciERR;
}

// mciClose()
// close the music stream.
unsigned long MUSIC::Close()
{
	if(m_DID != MUSIC_ERROR)
	{
		::MCIERROR mciERR(0);
		auto mci_p = ::MCI_GENERIC_PARMS();

		mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);
		mciERR = ::mciSendCommandW(m_DID, MCI_CLOSE, MCI_NOTIFY,
			::DWORD_PTR(&mci_p));
		m_DID = MUSIC_ERROR;
		return mciERR;
	}
	else
		return 0;
}

// mciGetPosition()
// get the playing position. return by milliseconds
unsigned long
MUSIC::GetPosition()
{
	yconstraint(m_DID != MUSIC_ERROR);

	auto mci_p = ::MCI_STATUS_PARMS();

	mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);
	mci_p.dwItem = MCI_STATUS_POSITION;
	::mciSendCommandW(m_DID, MCI_STATUS, MCI_NOTIFY | MCI_STATUS_ITEM,
		::DWORD_PTR(&mci_p));
	return mci_p.dwReturn;
}

// mciGetLength()
// get the length of the music stream. return by milliseconds
unsigned long
MUSIC::GetLength()
{
	yconstraint(m_DID != MUSIC_ERROR);

	auto mci_p = ::MCI_STATUS_PARMS();

	mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);
	mci_p.dwItem = MCI_STATUS_LENGTH;
	::mciSendCommandW(m_DID, MCI_STATUS, MCI_NOTIFY | MCI_STATUS_ITEM,
		::DWORD_PTR(&mci_p));
	return mci_p.dwReturn;
}

unsigned long
MUSIC::GetPlayStatus()
{
	yconstraint(m_DID != MUSIC_ERROR);

	auto mci_p = ::MCI_STATUS_PARMS();

	mci_p.dwCallback = ::DWORD_PTR(m_dwCallBack);
	mci_p.dwItem = MCI_STATUS_MODE;
	::mciSendCommandW(m_DID, MCI_STATUS, MCI_NOTIFY | MCI_STATUS_ITEM,
		::DWORD_PTR(&mci_p));
	return mci_p.dwReturn;
}

} // namespace ege;
