#include "Sprite.h"
#include <comdef.h>
#include <mutex>

Sprite::Sprite(const wchar_t* sFilePath, Graphics* const graphics, HPTimer* const timer, D2D1_SIZE_F* const gridsize) : gfx(graphics), sFilePathW(sFilePath), pTimer(timer), pGridSize(gridsize)
{
	auto p = sFilePathW.find_last_of(L'.');
	std::wstring ext(sFilePathW.begin() + p + 1, sFilePathW.end());

	if (!_wcsicmp(ext.c_str(), L"spr3"))
	{
		SetIsSpr3();
		uint32_t ImageBufferLen = 0;
		char* ImageBuffer = DecodeSPR3(sFilePathW.c_str(), ImageBufferLen);
		bLoadSuccess = InitFromMemory(ImageBuffer, ImageBufferLen);

		SafeDeleteArray(&ImageBuffer);
	}
	else if (!_wcsicmp(ext.c_str(), L"spr"))
	{
		DecodeSPR(sFilePathW.c_str());
		size_t ImageBufferLen = 0;
		char* ImageBuffer = GetStreamBuffer(sFilePathW.c_str(), ImageBufferLen);
		bLoadSuccess = InitFromMemory(ImageBuffer, ImageBufferLen);

		SafeDeleteArray(&ImageBuffer);
	}
	else
	{
		size_t ImageBufferLen = 0;
		char* ImageBuffer = GetStreamBuffer(sFilePathW.c_str(), ImageBufferLen);
		bLoadSuccess = InitFromMemory(ImageBuffer, ImageBufferLen);
		vFrames.push_back(D2D1::RectF(0.0f, 0.0f, GetBitmapSize().width, GetBitmapSize().height));

		SafeDeleteArray(&ImageBuffer);
	}
}

Sprite::~Sprite()
{
	SafeRelease(&pBitmap);
	SafeRelease(&pFrameBitmap);
}

char* Sprite::DecodeSPR(const wchar_t* FilePath)
{
	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, FilePath, L"r"); int linenum = __LINE__;
	
	if (err)
	{
		SendErrorMessage(err, linenum);
		return nullptr;
	}

	fseek(file, 0, SEEK_END);
	size_t BufferLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* Buffer = new char[BufferLen];
	fread(Buffer, BufferLen, 1, file);
	fclose(file);

	char* line = Buffer;

	while (static_cast<size_t>(line - Buffer) < BufferLen)
	{
		std::string Line = GetLine(line, line);
		TranslateLine(Line);
	}

	SafeDeleteArray(&Buffer);
	return nullptr;
}

void Sprite::TranslateLine(std::string sLine)
{
	std::string front, back;
	SplitString(sLine, front, back, '=');

	if (!_stricmp(front.c_str(), "version"))
	{

	}
	else if (!_stricmp(front.c_str(), "BodyPath"))
	{
		sFilePathW = std::wstring(back.begin(), back.end());
	}
	else if (!_stricmp(front.c_str(), "FrameTime"))
	{
		fFrameSpeed = static_cast<float>(atof(back.c_str()));
		fFrameSpeed /= 1000.0f;
	}
	else if (!_stricmp(front.c_str(), "FrameArray"))
	{
		D2D1_RECT_F frame = D2D1::RectF();
		int w = 0;

		auto p = back.find_first_of(",\n\r");
		std::string temp = back;
		while (p != std::string::npos)
		{
			if (w < 3)
			{
				float* ftarget = nullptr;
				switch (w)
				{
				case 0:
					ftarget = &frame.left;
					break;
				case 1:
					ftarget = &frame.top;
					break;
				case 2:
					ftarget = &frame.right;
					break;
				default:
					ftarget = &frame.bottom;
				}
				if (p)
				{
					*ftarget = static_cast<float>(atof(std::string(temp.begin(), temp.begin() + p).c_str()));
					w++;
				}
				temp = std::string(temp.begin() + p + 1, temp.end());
			}
			else
			{
				if (p)
				{
					frame.bottom = static_cast<float>(atof(std::string(temp.begin(), temp.begin() + p).c_str()));					
					w = 0;
					vFrames.push_back(frame);
					frame = D2D1::RectF();
				}
				temp = std::string(temp.begin() + p + 1, temp.end());
			}
			p = temp.find_first_of(",\n\r");
		}
	}
}

char* Sprite::DecodeSPR3(const wchar_t* FilePath, uint32_t& ImageBufferLen)
{
	if (!IsSpr3()) return nullptr;

	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, FilePath, L"rb"); int linenum = __LINE__;
	if (err)
	{
		SendErrorMessage(FilePath, err, linenum);
		return nullptr;
	}

	fseek(file, 0, SEEK_END);
	size_t BufferLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* Buffer = new char[BufferLen];
	fread(Buffer, BufferLen, 1, file);
	fclose(file);

	int frames = 0;
	float version = 0.0f;
	uint32_t position = sizeof(float) * 4;

	memcpy(&fFrameSpeed, Buffer + position, sizeof(fFrameSpeed));
	position += sizeof(float) + sizeof(double);
	fFrameSpeed /= 1000.0f;
	memcpy(&frames, Buffer + position, sizeof(int));
	position += sizeof(int);
	position += sizeof(bool);

	for (int i = 0; i < frames; i++)
	{
		D2D1_RECT_F frames = D2D1::RectF();
		memcpy(&frames.left, Buffer + position, sizeof(float));
		position += sizeof(float);
		memcpy(&frames.top, Buffer + position, sizeof(float));
		position += sizeof(float);
		memcpy(&frames.right, Buffer + position, sizeof(float));
		position += sizeof(float);
		memcpy(&frames.bottom, Buffer + position, sizeof(float));
		position += sizeof(float);
		AddFrame(frames);
	}

	int extlen = 0;
	memcpy(&extlen, Buffer + position, sizeof(int));
	position += sizeof(int);
	char* sImageExt = new char[extlen + 1];
	sImageExt[extlen] = '\0';
	memcpy(sImageExt, Buffer + position, extlen);
	position += extlen;

	ImageBufferLen = 0;
	memcpy(&ImageBufferLen, Buffer + position, sizeof(int));
	position += sizeof(int);

	char* ImageBuffer = new char[ImageBufferLen];
	memcpy(ImageBuffer, Buffer + position, ImageBufferLen);
	position += ImageBufferLen;

	SafeDeleteArray(&sImageExt);
	SafeDeleteArray(&Buffer);
	return ImageBuffer;
}

bool Sprite::InitFromMemory(char* const Buffer, size_t BufferLen)
{
	if (!Buffer) return false;
	if (!BufferLen) return false;

	int linenum = 0;
	if (pBitmap) SafeRelease(&pBitmap);
	HRESULT hr;

	IWICImagingFactory* pWicFactory = nullptr;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pWicFactory); linenum = __LINE__;
	if (!SUCCEEDED(hr))
	{
		SendErrorMessage(hr, linenum);
		return false;
	}

	std::mutex m;
	m.lock();
	uint32_t MaxBitmapSize = gfx->GetRenderTarget()->GetMaximumBitmapSize();
	m.unlock();
	//create stream
	IWICStream* pIWICStream = nullptr;
	hr = pWicFactory->CreateStream(&pIWICStream); linenum = __LINE__;
	if (!SUCCEEDED(hr))
	{
		SendErrorMessage(hr, linenum);
		SafeRelease(&pWicFactory);			
		return false;
	}

	//initialize stream
	hr = pIWICStream->InitializeFromMemory(reinterpret_cast<BYTE*>(Buffer), static_cast<DWORD>(BufferLen)); linenum = __LINE__;
	if (!SUCCEEDED(hr))
	{
		SendErrorMessage(hr, linenum);
		SafeRelease(&pIWICStream);
		SafeRelease(&pWicFactory);		
		return false;
	}

	//create decoder
	IWICBitmapDecoder* pIWICDecoder = nullptr;
	hr = pWicFactory->CreateDecoderFromStream(pIWICStream, nullptr, WICDecodeMetadataCacheOnLoad, &pIWICDecoder); linenum = __LINE__;
	if (!SUCCEEDED(hr))
	{
		SendErrorMessage(hr, linenum);
		SafeRelease(&pIWICStream);
		SafeRelease(&pWicFactory);		
		return false;
	}

	//read frame from the image
	IWICBitmapFrameDecode* pWICFrame = nullptr;
	hr = pIWICDecoder->GetFrame(0, &pWICFrame); linenum = __LINE__;
	if (!SUCCEEDED(hr))
	{
		SendErrorMessage(hr, linenum);
		SafeRelease(&pIWICDecoder);
		SafeRelease(&pIWICStream);
		SafeRelease(&pWicFactory);		
		return false;
	}

	D2D1_SIZE_U FrameSize = D2D1::SizeU();
	pWICFrame->GetSize(&FrameSize.width, &FrameSize.height);

	if (FrameSize.width > MaxBitmapSize || FrameSize.height > MaxBitmapSize)
	{
		std::wstring errmsg = L"Failed to load image. Bitmap size is " + std::to_wstring(FrameSize.width) + std::wstring(L"x") + std::to_wstring(FrameSize.height)
			+ std::wstring(L".\nThe Maximum allowed size is ") + std::to_wstring(MaxBitmapSize) + std::wstring(L".");
		MessageBoxW(nullptr, errmsg.c_str(), L"Error", MB_OK | MB_ICONERROR);

		SafeRelease(&pWICFrame);
		SafeRelease(&pIWICDecoder);
		SafeRelease(&pIWICStream);
		SafeRelease(&pWicFactory);
		return false;
	}

	//create converter
	IWICFormatConverter* pWicConverter = nullptr;
	hr = pWicFactory->CreateFormatConverter(&pWicConverter); linenum = __LINE__;
	if (!SUCCEEDED(hr))
	{
		SendErrorMessage(hr, linenum);

		SafeRelease(&pWICFrame);
		SafeRelease(&pIWICDecoder);
		SafeRelease(&pIWICStream);
		SafeRelease(&pWicFactory);
		return false;
	}

	hr = pWicConverter->Initialize(
		pWICFrame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom
	); linenum = __LINE__;
	if (!SUCCEEDED(hr))
	{
		SendErrorMessage(hr, linenum);
		SafeRelease(&pWicConverter);
		SafeRelease(&pWICFrame);
		SafeRelease(&pIWICDecoder);
		SafeRelease(&pIWICStream);
		SafeRelease(&pWicFactory);
		return false;
	}

	//use converter to create bitmap
	m.lock();
	hr = gfx->GetRenderTarget()->CreateBitmapFromWicBitmap(pWicConverter, nullptr, &pBitmap); linenum = __LINE__;
	m.unlock();
	if (!SUCCEEDED(hr))
	{
		pTimer->Update();
		double ttime = pTimer->GetTimeTotal();
		while (hr == E_OUTOFMEMORY)
		{
			m.lock();
			hr = gfx->GetRenderTarget()->CreateBitmapFromWicBitmap(pWicConverter, nullptr, &pBitmap); linenum = __LINE__;
			m.unlock();
			pTimer->Update();
			if (pTimer->GetTimeTotal() - ttime > dTimeout)
			{
				break;
			}
		}
		if (!SUCCEEDED(hr))
		{
			SendErrorMessage(hr, linenum);
			SafeRelease(&pWicConverter);
			SafeRelease(&pWICFrame);
			SafeRelease(&pIWICDecoder);
			SafeRelease(&pIWICStream);
			SafeRelease(&pWicFactory);
			return false;
		}
	}	
	fBitmapSize = pBitmap->GetSize();

	SafeRelease(&pWicConverter);
	SafeRelease(&pWICFrame);
	SafeRelease(&pIWICDecoder);
	SafeRelease(&pIWICStream);
	SafeRelease(&pWicFactory);
	return true;
}

void Sprite::SendErrorMessage(HRESULT hr, int iLineNumber)
{
	_com_error err(hr);
	std::string sErrMsg = err.ErrorMessage();
	std::wstring sErrMsgW(sErrMsg.begin(), sErrMsg.end());
	//sErrMsgW = L"Error on Line " + std::to_wstring(iLineNumber) + std::wstring(L".\n") + sErrMsgW;
	//MessageBoxW(nullptr, sErrMsgW.c_str(), L"Error", MB_OK | MB_ICONERROR);
	UpdateLog(sErrMsgW, L"Sprite.cpp", static_cast<uint32_t>(iLineNumber));
}

void Sprite::SendErrorMessage(std::wstring filename, HRESULT hr, int iLineNumber)
{
	_com_error err(hr);
	std::string sErrMsg = err.ErrorMessage();
	std::wstring sErrMsgW = L"Unable to open " + filename;
	sErrMsgW.append(L".\n");
	sErrMsgW.append(std::wstring(sErrMsg.begin(), sErrMsg.end()));
	//sErrMsgW = L"Error on Line " + std::to_wstring(iLineNumber) + std::wstring(L".\n") + sErrMsgW;
	//MessageBoxW(nullptr, sErrMsgW.c_str(), L"Error", MB_OK | MB_ICONERROR);
	UpdateLog(sErrMsgW, L"Sprite.cpp", static_cast<uint32_t>(iLineNumber));
}

char* Sprite::GetStreamBuffer(const wchar_t* FilePath, size_t& BufferLen)
{
	if (!FilePath) return nullptr;
	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, FilePath, L"rb"); int linenum = __LINE__;
	
	if (err)
	{
		SendErrorMessage(FilePath, err, linenum);
		return nullptr;
	}

	fseek(file, 0, SEEK_END);
	BufferLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* Buffer = new char[BufferLen];
	fread(Buffer, BufferLen, 1, file);
	fclose(file);

	return Buffer;
}

void Sprite::NextFrame() 
{ 
	if (cTime - pTime > fFrameSpeed)
	{
		pTime = cTime;
		iCurrentFrame = iCurrentFrame == vFrames.size() - 1 ? 0 : iCurrentFrame + 1;
		CreateFrameBitmap();
	}
	cTime = static_cast<float>(pTimer->GetTimeTotal());
}

void Sprite::CreateFrameBitmap()
{
	SafeRelease(&pFrameBitmap);
	ID2D1BitmapRenderTarget* tcompatibletarget;
	if (SUCCEEDED(gfx->GetRenderTarget()->CreateCompatibleRenderTarget(*pGridSize, &tcompatibletarget)))
	{
		tcompatibletarget->BeginDraw();
		tcompatibletarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
		tcompatibletarget->DrawBitmap(pBitmap, D2D1::RectF(0.0f, 0.0f, pGridSize->width, pGridSize->height), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, vFrames.at(iCurrentFrame));
		tcompatibletarget->EndDraw();
		if (FAILED(tcompatibletarget->GetBitmap(&pFrameBitmap)))
		{
			pFrameBitmap = nullptr;
		}
		SafeRelease(&tcompatibletarget);
	}
}