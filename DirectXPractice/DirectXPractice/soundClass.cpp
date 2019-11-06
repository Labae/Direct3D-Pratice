#include "stdafx.h"
#include "soundClass.h"

SoundClass::SoundClass()
{
	m_pDirectSound = 0;
	m_pPrimaryBuffer = 0;
	m_pSecondBuffer1 = 0;
}

SoundClass::SoundClass(const SoundClass&)
{
}

SoundClass::~SoundClass()
{
}

bool SoundClass::Initialize(HWND hwnd)
{
	if (!InitializeDirectSound(hwnd))
	{
		return false;
	}

	if (!LoadWaveFile("Datas/Yamaha-V50-E-Piano-2B-C4.wav", "Datas/Yamaha-V50-Strike-1.wav", &m_pSecondBuffer1))
	{
		return false;
	}

	if (!PlayWaveFile())
	{
		return false;
	}

	return true;
}

void SoundClass::Shutdown()
{
	ShutdownWaveFile(&m_pSecondBuffer1);

	ShutdownDirectSound();
}

bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	if (FAILED(DirectSoundCreate8(NULL, &m_pDirectSound, NULL)))
	{
		return false;
	}

	if (FAILED(m_pDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))
	{
		return false;
	}

	DSBUFFERDESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	if (FAILED(m_pDirectSound->CreateSoundBuffer(&bufferDesc, &m_pPrimaryBuffer, NULL)))
	{
		return false;
	}

	WAVEFORMATEX waveFormat;
	ZeroMemory(&waveFormat, sizeof(waveFormat));

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	if (FAILED(m_pPrimaryBuffer->SetFormat(&waveFormat)))
	{
		return false;
	}

	return true;
}

void SoundClass::ShutdownDirectSound()
{
	SAFE_RELEASE(m_pPrimaryBuffer);
	SAFE_RELEASE(m_pDirectSound);
}

bool SoundClass::LoadWaveFile(char* waveFilename, char* waveFilename2, IDirectSoundBuffer8** secondaryBuffer)
{
	FILE* pFile = nullptr;
	int iError = fopen_s(&pFile, waveFilename, "rb");
	if (iError != 0)
	{
		return false;
	}

	WaveHeaderType waveFileHeader;
	ZeroMemory(&waveFileHeader, sizeof(waveFileHeader));

	UINT iCount = fread(&waveFileHeader, sizeof(waveFileHeader), 1, pFile);
	if (iCount != 1)
	{
		return false;
	}

	if (!CheckWaveFile(waveFileHeader))
	{
		return false;
	}

	WAVEFORMATEX waveFormat;
	ZeroMemory(&waveFormat, sizeof(waveFormat));

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	DSBUFFERDESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.dwSize = sizeof(bufferDesc);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	IDirectSoundBuffer* pTempBuffer = nullptr;
	if (FAILED(m_pDirectSound->CreateSoundBuffer(&bufferDesc, &pTempBuffer, NULL)))
	{
		return false;
	}

	if (FAILED(pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**) & *secondaryBuffer)))
	{
		return false;
	}

	SAFE_RELEASE(pTempBuffer);

	fseek(pFile, sizeof(WaveHeaderType), SEEK_SET);

	UCHAR* pWaveData = new UCHAR[waveFileHeader.dataSize];
	if (!pWaveData)
	{
		return false;
	}

	iCount = fread(pWaveData, 1, waveFileHeader.dataSize, pFile);
	if (iCount != waveFileHeader.dataSize)
	{
		return false;
	}

	UCHAR* pBuffer = nullptr;
	ULONG bufferSize = 0;

	if (FAILED((*secondaryBuffer)->Lock(0,  bufferDesc.dwBufferBytes, (void**)& pBuffer, (DWORD*)& bufferSize, NULL, 0, 0)))
	{
		return false;
	}

	memcpy(pBuffer, pWaveData, waveFileHeader.dataSize);

	if (FAILED((*secondaryBuffer)->Unlock((void*)pBuffer, bufferSize, NULL, 0)))
	{
		return false;
	}

	delete[] pWaveData;
	pWaveData = nullptr;

	return true;
}

bool SoundClass::CheckWaveFile(WaveHeaderType& waveFileHeader)
{
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	if (waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	return true;
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	if ((*secondaryBuffer))
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = nullptr;
	}
}

bool SoundClass::PlayWaveFile()
{
	if (FAILED(m_pSecondBuffer1->SetCurrentPosition(0)))
	{
		return false;
	}

	if (FAILED(m_pSecondBuffer1->SetVolume(DSBVOLUME_MAX)))
	{
		return false;
	}

	if (FAILED(m_pSecondBuffer1->Play(0, 0, 0)))
	{
		return false;
	}

	return true;
}
