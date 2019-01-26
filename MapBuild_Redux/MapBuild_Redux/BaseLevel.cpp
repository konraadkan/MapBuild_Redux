#include "BaseLevel.h"

BaseLevel::BaseLevel(Graphics* const graphics, D2D1_POINT_2F* const pMousePosition)
{
	this->gfx = graphics;
	this->pMouseCoordinate = pMousePosition;
}

BaseLevel::~BaseLevel()
{
	Unload();
}

void BaseLevel::Load(Keyboard* const keyboard)
{
	if (!gfx) return;
	//set default values
	pKeyboard = keyboard;
	Center = D2D1::Point2F(gfx->GetCompatibleTargetSize().width * 0.5f, 0.0f);
}

void BaseLevel::Unload()
{
	//cleanup as necessary
	this->gfx = nullptr;
	this->pMouseCoordinate = nullptr;
}

void BaseLevel::Render()
{
	//build the frame
	gfx->BeginDraw(gfx->GetCompatibleTarget());
	gfx->ClearScreen(gfx->GetCompatibleTarget(), D2D1::ColorF(0.75f, 0.75f, 0.75f));	
	gfx->DrawCircle(gfx->GetCompatibleTarget(), D2D1::Point2F(500, 500), 40, D2D1::ColorF(0,0,0));
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Test Small", D2D1::RectF(0, 200, 500, 400));
	gfx->OutputText(gfx->GetCompatibleTarget(), std::to_wstring(static_cast<long>(TranslatedCoordinates.x)).c_str(), D2D1::RectF(0, 0, 500, 500));
	gfx->FillCircle(gfx->GetCompatibleTarget(), D2D1::Point2F(800, 800), 100);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(1050, 100, 1900, 500), D2D1::ColorF(0,0,0), 25,25);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(1050, 505, 1900, 905));
	gfx->DrawRect(gfx->GetCompatibleTarget(), D2D1::RectF(500, 100, 1045, 500));
	//if grid-on-top apply last, otherwise apply first
	gfx->DrawDefaultGrid(gfx->GetCompatibleTarget(), GridSquareSize, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.9f), 3.0f);
	gfx->EndDraw(gfx->GetCompatibleTarget());

	//render the frame
	gfx->BeginDraw(gfx->GetRenderTarget());
	gfx->ClearScreen(gfx->GetRenderTarget());
	gfx->ApplyScale(gfx->GetRenderTarget(), Scale, Center);
	gfx->ApplyTranslation(gfx->GetRenderTarget(), Offset);
	gfx->SwapBuffer();
	TranslatedCoordinates = gfx->GetTransformedPoint(gfx->GetRenderTarget(), *pMouseCoordinate);
	gfx->RestoreTransform(gfx->GetRenderTarget());
	gfx->EndDraw(gfx->GetRenderTarget());
}

void BaseLevel::Update(double dDelta)
{
	if (Movement.bMove)
	{
		for (auto& d : Movement.vDirection)
		{
			float* value = nullptr;
			int direction = 1;
			switch (d)
			{
			case Move::Direction::Up:
			case Move::Direction::Down:
				value = &Offset.height;
				direction = (d == Move::Direction::Up ? -1 : 1);
				break;
			case Move::Direction::Left:
			case Move::Direction::Right:
				value = &Offset.width;
				direction = (d == Move::Direction::Left ? -1 : 1);
				break;
			}
			*value = *value + 100.0f * static_cast<float>(dDelta) * direction;
		}
	}
}

void BaseLevel::ProcessEvents(double dDelta)
{
	if (!pKeyboard) return;

	//update the scene
	Keyboard::Event keyEvents = {};
	if (!pKeyboard->CharIsEmpty())
	{
		//this is for character entry; at first will use for zoom probably; most likely make it a setting in the keys.ini once thats implemented
		switch (pKeyboard->ReadChar())
		{
		case L'*':
			Scale.height += ScaleSpeed.height;
			Scale.width += ScaleSpeed.width;
			break;
		case L'/':
			Scale.height -= ScaleSpeed.height;
			Scale.width -= ScaleSpeed.width;
			if (Scale.height <= 0) Scale.height = ScaleSpeed.height;
			if (Scale.width <= 0) Scale.width = ScaleSpeed.width;
			break;
		case L'+':
		{
			D2D1_SIZE_F temp = gfx->GetCompatibleTargetSize();
			temp.height += GridSquareSize.height;
			temp.width += GridSquareSize.width;
			gfx->ResizeCompatibleRenderTarget(temp);
			break;
		}
		case L'-':
		{
			D2D1_SIZE_F temp = gfx->GetCompatibleTargetSize();
			temp.height -= GridSquareSize.height;
			temp.width -= GridSquareSize.width;
			gfx->ResizeCompatibleRenderTarget(temp);
			break;
		}
		}
	}
	if (!pKeyboard->KeyIsEmpty())
	{
		keyEvents = pKeyboard->ReadKey();
		if (keyEvents.IsPress())
		{
			switch (keyEvents.GetCode())
			{
			case VK_UP:
			case 'W':
				Movement.bMove = true;
				Movement.vDirection.push_back(Move::Direction::Up);
				break;
			case VK_DOWN:
			case 'S':
				Movement.bMove = true;
				Movement.vDirection.push_back(Move::Direction::Down);
				break;
			case VK_LEFT:
			case 'A':
				Movement.bMove = true;
				Movement.vDirection.push_back(Move::Direction::Left);
				break;
			case VK_RIGHT:
			case 'D':
				Movement.bMove = true;
				Movement.vDirection.push_back(Move::Direction::Right);
				break;
			case VK_END:
				Scale = D2D1::SizeF(1.0f, 1.0f);
				Offset = D2D1::SizeF(0.0f, 0.0f);
				break;
			}
		}
		if (keyEvents.IsRelease())
		{
			Move::Direction direction = Move::Direction::Error;
			switch (keyEvents.GetCode())
			{
			case VK_UP:
			case 'W':
				direction = Move::Direction::Up;
				break;
			case VK_DOWN:
			case 'S':
				direction = Move::Direction::Down;
				break;
			case VK_LEFT:
			case 'A':
				direction = Move::Direction::Left;
				break;
			case VK_RIGHT:
			case 'D':
				direction = Move::Direction::Right;
				break;
			}
			Movement.vDirection.erase(std::remove_if(Movement.vDirection.begin(), Movement.vDirection.end(), [direction](auto& o) { return o == direction; }), Movement.vDirection.end());
		}
	}
}