#pragma once

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <mmsystem.h>
#include <dsound.h>

class SoundClass
{
private:
	struct WaveHeaderType
	{
		char chunkId[4];
		ULONG chunkSize;
		char format[4];
		char subChunkId[4];
		ULONG subChunkSize;
		USHORT audioFormat;
		USHORT numChannels;
		ULONG sampleRate;
		ULONG bytesPerSecond;
		USHORT blockAlign;
		USHORT bitsPerSample;
		char dataChunkId[4];
		ULONG dataSize;
	};

public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	bool Initialize(HWND);
	void Shutdown();

private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadWaveFile(char*, char*, IDirectSoundBuffer8**);
	bool CheckWaveFile(WaveHeaderType&);
	void ShutdownWaveFile(IDirectSoundBuffer8**);

	bool PlayWaveFile();

private:
	IDirectSound8* m_pDirectSound;
	IDirectSoundBuffer* m_pPrimaryBuffer;
	IDirectSoundBuffer8* m_pSecondBuffer1;
};

