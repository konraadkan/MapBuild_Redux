#include "Walls.h"
#include "BaseLevel.h"

void Wall::BuildGeometry(const bool bClose)
{
	ID2D1PathGeometry* pG = nullptr;
	if (!SUCCEEDED(gfx->GetFactory()->CreatePathGeometry(&pG)))
	{
		pG = nullptr;
		return;
	}
	vGeometryPathHistory.push_back(pG);

	ID2D1GeometrySink* pSink = nullptr;
	if (SUCCEEDED(pG->Open(&pSink)))
	{
		pSink->BeginFigure(vPoints.front(), D2D1_FIGURE_BEGIN_FILLED);
		for (auto& p : vPoints)
		{
			if (&p == &vPoints.front()) continue;
			pSink->AddLine(p);
		}
		bClose ? pSink->EndFigure(D2D1_FIGURE_END_CLOSED) : pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		pSink->Close();
		SafeRelease(&pSink);
		return;
	}
	RemoveLastGeometry();
}

void Wall::RemoveLastGeometry()
{
	if (vGeometryPathHistory.empty()) return;

	SafeRelease(&vGeometryPathHistory.back());
	vGeometryPathHistory.pop_back();
}

void Wall::Draw()
{
	if (pSetGeometry)
	{
		if (bUseTexture && pBitmapBrush)
		{
			gfx->DrawGeometry(gfx->GetCompatibleTarget(), pSetGeometry, pBitmapBrush);
		}
		else gfx->DrawGeometry(gfx->GetCompatibleTarget(), pSetGeometry, mColor, fThickness);
		return;
	}

	if (vPoints.empty()) return;
	if (vPoints.size() == 1)
		gfx->FillCircle(gfx->GetCompatibleTarget(), vPoints.front(), fRadius, mColor);
	else
	{
		if (vGeometryPathHistory.size())
		{
			gfx->DrawGeometry(gfx->GetCompatibleTarget(), vGeometryPathHistory.back(), mColor, fThickness);
		}
	}
}

void Wall::DrawPreview()
{
	Draw();
	if (vPoints.size()) gfx->DrawLine(gfx->GetCompatibleTarget(), vPoints.back(), *pMouseCoordinates, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f), fThickness);
}

void Wall::DrawPreview(const D2D1_POINT_2F p)
{
	Draw();
	if (vPoints.size()) gfx->DrawLine(gfx->GetCompatibleTarget(), vPoints.back(), p, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f), fThickness);
}

void Wall::SetGeometry(const bool bClose)
{
	if (vGeometryPathHistory.empty()) return;
	if (bClose | bUseTexture) BuildGeometry(true);
	bIsClosed = bClose;

	pSetGeometry = vGeometryPathHistory.back();
	vGeometryPathHistory.pop_back();
	while (vGeometryPathHistory.size()) RemoveLastGeometry();
	//while (vPoints.size()) RemoveLastPoint(); need the points for the ability to erase...
}

void Wall::SetTexture(ID2D1Bitmap* const bitmap)
{
	SafeRelease(&pBitmapBrush);
	D2D1_BITMAP_BRUSH_PROPERTIES brushProperties = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
	if (FAILED(gfx->GetCompatibleTarget()->CreateBitmapBrush(bitmap, brushProperties, &pBitmapBrush)))
	{
		pBitmapBrush = nullptr;
	}
	bUseTexture = true;
}

void Wall::SetTexture(SpritePointer* const p)
{
	if (!p) return;
	SafeDelete(&pTexture);
	pTexture = new SpritePointer(gfx, p->GetPieces(), p->GetLocation(), pGridSquareSize, p->IsKeepAspectSprite(), p->IsKeepAspectPortrait());
	pTexture->SetCreatureSize(p->GetCreatureSize());
	if (pTexture->GetSprite())
	{
		pTexture->GetSprite()->CreateFrameBitmap();
		SetTexture(pTexture->GetSprite()->GetFrameBitmap());
	}
	else if (pTexture->GetPortrait())
	{
		pTexture->GetPortrait()->CreateFrameBitmap();
		SetTexture(pTexture->GetPortrait()->GetFrameBitmap());
	}
}

const bool Wall::PointTouching(const D2D1_POINT_2F p)
{
	for (auto point : vPoints)
	{
		if (static_cast<int>(p.x) >= static_cast<int>(point.x - 3.0f) && static_cast<int>(p.x) <= static_cast<int>(point.x + 3.0f))
		{
			if (static_cast<int>(p.y) >= static_cast<int>(point.y - 3.0f) && static_cast<int>(p.y) <= static_cast<int>(point.y + 3.0f))
			{
				return true;
			}
		}
	}
	return false;
}

const uint32_t Wall::CalcSaveSize()
{
	uint32_t uSize = 0;

	uSize += sizeof(uSize);							//hold total size
	uSize += sizeof(fThickness);					//hold fThickness
	uSize += sizeof(fRadius);						//hold fRadius
	uSize += sizeof(float) * 4;						//hold mColor
	uSize += sizeof(uint32_t);					//holds number of points
	for (size_t i = 0; i < vPoints.size(); i++)
	{
		uSize += sizeof(float) * 2;					//holds point data
	}
	uSize += sizeof(unsigned char);					//holds booleen data
	if (pTexture)
	{
		uSize += pTexture->CalcSaveSize();				//holds SpritePointer data [spritepointer size is automatically stored as part of the spritepointer save data so no need to do that again]
	}
	
	return uSize;
}

const std::vector<char> Wall::CreateSaveInformationV()
{
	StoreData sd;

	sd.AddEntry(fThickness);
	sd.AddEntry(fRadius);
	sd.AddEntry(mColor.r);
	sd.AddEntry(mColor.g);
	sd.AddEntry(mColor.b);
	sd.AddEntry(mColor.a);
	uint32_t uLen = static_cast<uint32_t>(vPoints.size());
	sd.AddEntry(uLen);
	for (size_t i = 0; i < vPoints.size(); i++)
	{
		sd.AddEntry(vPoints.at(i).x);
		sd.AddEntry(vPoints.at(i).y);
	}
	unsigned char bools = 0;
	if (bUseTexture) bools |= 1;
	if (bIsClosed) bools |= 2;
	sd.AddEntry(bools);

	if (!pTexture)
	{
		sd.UpdateBufferSize();
		return sd.GetBuffer();
	}

	std::vector<char> Vtemp = pTexture->GetSaveInformationV();
	sd.CombineBuffer(Vtemp);

	sd.UpdateBufferSize();
	return sd.GetBuffer();
}

const char* Wall::CreateSaveInformation()
{
	uint32_t uSize = CalcSaveSize();
	char* Buffer = new char[uSize];
	size_t pos = 0;

	memcpy(Buffer, &uSize, sizeof(uSize));
	pos += sizeof(uSize);
	memcpy(Buffer + pos, &fThickness, sizeof(fThickness));
	pos += sizeof(fThickness);
	memcpy(Buffer + pos, &fRadius, sizeof(fRadius));
	pos += sizeof(fRadius);
	memcpy(Buffer + pos, &mColor.r, sizeof(mColor.r));
	pos += sizeof(mColor.r);
	memcpy(Buffer + pos, &mColor.g, sizeof(mColor.g));
	pos += sizeof(mColor.g);
	memcpy(Buffer + pos, &mColor.b, sizeof(mColor.b));
	pos += sizeof(mColor.b);
	memcpy(Buffer + pos, &mColor.a, sizeof(mColor.a));
	pos += sizeof(mColor.a);
	uint32_t uLen = static_cast<uint32_t>(vPoints.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);
	for (size_t i = 0; i < vPoints.size(); i++)
	{
		memcpy(Buffer + pos, &vPoints.at(i).x, sizeof(vPoints.at(i).x));
		pos += sizeof(vPoints.at(i).x);
		memcpy(Buffer + pos, &vPoints.at(i).y, sizeof(vPoints.at(i).y));
		pos += sizeof(vPoints.at(i).y);
	}
	unsigned char bools = 0;
	if (bUseTexture) bools |= 1;
	if (bIsClosed) bools |= 2;
	memcpy(Buffer + pos, &bools, sizeof(bools));
	pos += sizeof(bools);

	if (!pTexture) return Buffer;

	uLen = pTexture->CalcSaveSize();
	const char* t = pTexture->GetSaveInformation();
	memcpy(Buffer + pos, t, uLen);
	SafeDeleteArray(&t);

	return Buffer;
}

const bool Wall::LoadSaveBuffer(ReceiveData& rd)
{
	vPoints = std::vector<D2D1_POINT_2F>();
	uint32_t uLen = 0;
	uint32_t uConfirmPosition = 0;
	uConfirmPosition += rd.GetData(uLen);
	uConfirmPosition += rd.GetData(fThickness);
	uConfirmPosition += rd.GetData(fRadius);
	uConfirmPosition += rd.GetData(mColor.r);
	uConfirmPosition += rd.GetData(mColor.g);
	uConfirmPosition += rd.GetData(mColor.b);
	uConfirmPosition += rd.GetData(mColor.a);

	uint32_t uNumberPoints = 0;
	uConfirmPosition += rd.GetData(uNumberPoints);
	for (uint32_t i = 0; i < uNumberPoints; i++)
	{
		D2D1_POINT_2F p = D2D1::Point2F();
		uConfirmPosition += rd.GetData(p.x);
		uConfirmPosition += rd.GetData(p.y);
		vPoints.push_back(p);
	}
	unsigned char bools = 0;
	uConfirmPosition += rd.GetData(bools);
	bUseTexture = (bools & 1);
	bIsClosed = (bools & 2);
	
	if (uConfirmPosition < uLen)
	{
		SafeDelete(&pTexture);
		SpritePointer* tempP = new SpritePointer(gfx, nullptr, Location(), pGridSquareSize);
		tempP->LoadSaveBuffer(rd);
		tempP->SetPiecePointer(static_cast<BaseLevel*>(pBaseLevel)->FindPiece(tempP->GetPieceBufferV()));
		SetTexture(tempP);
		SafeDelete(&tempP);
	}

	BuildGeometry(bIsClosed);
	SetGeometry(bIsClosed);

	return true;
}

const bool Wall::LoadSaveBuffer(const char* Buffer)
{
	if (!Buffer) return false;
	vPoints = std::vector<D2D1_POINT_2F>();

	uint32_t uLen = 0;
	size_t pos = 0;
	memcpy(&uLen, Buffer, sizeof(uLen));
	pos += sizeof(uLen);
	memcpy(&fThickness, Buffer + pos, sizeof(fThickness));
	pos += sizeof(fThickness);
	memcpy(&fRadius, Buffer + pos, sizeof(fRadius));
	pos += sizeof(fRadius);
	memcpy(&mColor.r, Buffer + pos, sizeof(mColor.r));
	pos += sizeof(mColor.r);
	memcpy(&mColor.g, Buffer + pos, sizeof(mColor.g));
	pos += sizeof(mColor.g);
	memcpy(&mColor.b, Buffer + pos, sizeof(mColor.b));
	pos += sizeof(mColor.b);
	memcpy(&mColor.a, Buffer + pos, sizeof(mColor.a));
	pos += sizeof(mColor.a);

	uint32_t uNumberPoints = 0;
	memcpy(&uNumberPoints, Buffer + pos, sizeof(uNumberPoints));
	pos += sizeof(uNumberPoints);
	for (uint32_t i = 0; i < uNumberPoints; i++)
	{
		D2D1_POINT_2F p = D2D1::Point2F();
		memcpy(&p.x, Buffer + pos, sizeof(p.x));
		pos += sizeof(p.x);
		memcpy(&p.y, Buffer + pos, sizeof(p.y));
		pos += sizeof(p.y);
		vPoints.push_back(p);
	}
	unsigned char bools = 0;
	memcpy(&bools, Buffer + pos, sizeof(bools));
	pos += sizeof(bools);
	bUseTexture = (bools & 1);
	bIsClosed = (bools & 2);
	if (pos < uLen)
	{
		SafeDelete(&pTexture);
		SpritePointer* tempP = new SpritePointer(gfx, nullptr, Location(), pGridSquareSize);
		tempP->LoadSaveBuffer(Buffer + pos);
		tempP->SetPiecePointer(static_cast<BaseLevel*>(pBaseLevel)->FindPiece(tempP->GetPieceBuffer()));
		SetTexture(tempP);
		SafeDelete(&tempP);
	}

	BuildGeometry(bIsClosed);
	SetGeometry(bIsClosed);

	return true;
}