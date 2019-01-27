#include "BaseLevel.h"

BaseLevel::BaseLevel(Graphics* const graphics, D2D1_POINT_2F* const pMousePosition, int WindowX, int WindowY)
{
	this->gfx = graphics;
	this->pMouseCoordinate = pMousePosition;
	WindowSize = D2D1::SizeF(static_cast<float>(WindowX), static_cast<float>(WindowY));
	RotationCenter = D2D1::Point2F(WindowSize.width * 0.5f, WindowSize.height * 0.5f);
	pSideMenu = new SideMenu(D2D1::RectF(WindowSize.width * 0.85f, 0.0f, WindowSize.width, WindowSize.height), graphics);
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
	Center = D2D1::Point2F(WindowSize.width * 0.5f, 0.0f);
	
	/*** Example creation Custom Geometry
	std::queue<D2D1_POINT_2F> ps;
	ps.push(D2D1::Point2F(WindowSize.width - 22.0f, WindowSize.height * 0.5f));
	ps.push(D2D1::Point2F(WindowSize.width - 8.0f, WindowSize.height * 0.5f - 20.0f));
	ps.push(D2D1::Point2F(WindowSize.width - 8.0f, WindowSize.height * 0.5f + 20.0f));
	gfx->BuildCustomGeometry(ps, D2D1::ColorF(1.0f,1.0f,1.0f), true);*/
}

void BaseLevel::Unload()
{
	//cleanup as necessary
	SafeDelete(&pSideMenu);
	this->gfx = nullptr;
	this->pMouseCoordinate = nullptr;
}

void BaseLevel::Render()
{
	//build the frame
	gfx->BeginDraw(gfx->GetCompatibleTarget());
	gfx->ClearScreen(gfx->GetCompatibleTarget(), GridBackgroundColor);
	if (!bGridOnTop) gfx->DrawDefaultGrid(gfx->GetCompatibleTarget(), Transforms, D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height), GridSquareSize, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.9f), 3.0f);

	gfx->DrawCircle(gfx->GetCompatibleTarget(), Transforms, D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height), D2D1::Point2F(500, 500), 40, D2D1::ColorF(0, 1, 0), 5.0f);
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Test Small", D2D1::RectF(0.0f, 64.0f, 64.0f, 128.0f));
	gfx->OutputText(gfx->GetCompatibleTarget(), std::to_wstring(static_cast<long>(TranslatedCoordinates.x)).c_str(), D2D1::RectF(0, 0, 128, 128));
	gfx->FillCircle(gfx->GetCompatibleTarget(), D2D1::Point2F(800, 800), 100);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(1050, 100, 1900, 500), D2D1::ColorF(0,0,0), 25,25);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(1050, 505, 1900, 905));
	gfx->DrawRect(gfx->GetCompatibleTarget(), D2D1::RectF(500, 100, 1045, 500));

	if (bGridOnTop) gfx->DrawDefaultGrid(gfx->GetCompatibleTarget(), Transforms, D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height), GridSquareSize, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.9f), 3.0f);
	gfx->EndDraw(gfx->GetCompatibleTarget());

	//render the frame
	gfx->BeginDraw(gfx->GetRenderTarget());
	gfx->ClearScreen(gfx->GetRenderTarget());

	//apply Transforms as appropriate
	gfx->ApplyRotation(gfx->GetRenderTarget(), RotationAngle, RotationCenter);
	gfx->ApplyScale(gfx->GetRenderTarget(), Scale, Center);
	gfx->ApplyTranslation(gfx->GetRenderTarget(), Offset);
	gfx->GetRenderTarget()->GetTransform(&Transforms);
	
	//draw to RenderTarget
	gfx->SwapBuffer();
	//Get mouse coordinates
	TranslatedCoordinates = gfx->GetTransformedPoint(gfx->GetRenderTarget(), *pMouseCoordinate);
	//Center = gfx->GetTransformedPoint(gfx->GetRenderTarget(), D2D1::Point2F(WindowSize.width * 0.5f, WindowSize.height * .05f)); //currently commented out because it leads to wierd interactions
	gfx->RestoreTransform(gfx->GetRenderTarget());

	gfx->BeginDraw(gfx->GetCompatibleTarget());
	gfx->ClearScreen(gfx->GetCompatibleTarget(), D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	DrawSideMenu();
	gfx->EndDraw(gfx->GetCompatibleTarget());

	gfx->SwapBuffer();
	//build side bar menu / initiative
	gfx->EndDraw(gfx->GetRenderTarget());
}

void BaseLevel::DrawSideMenu()
{
	/***** replace with objects, this is just to get an idea of the visuals *****/
	if (!bShowSideMenu)
	{
		float halfheight = WindowSize.height * 0.5f;
		gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(WindowSize.width - 25.0f, 0.0f, WindowSize.width + 25.0f, WindowSize.height - 25), D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f), 25.0f, 25.0f);
		gfx->FillRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(WindowSize.width - 25.0f, 0.0f, WindowSize.width + 25.0f, WindowSize.height - 25), D2D1::ColorF(0.0f, 0.2f, 0.7f, 0.90f), 25.0f, 25.0f);		
	}
	pSideMenu->Draw();
}

void BaseLevel::Update(double dDelta)
{
	if (Movement.bMove)
	{
		for (auto& d : Movement.vDirection)
		{
			float* value = nullptr;
			float size = 100.0f;
			int direction = 1;
			switch (d)
			{
			case Move::Direction::Up:
			case Move::Direction::Down:
				value = &Offset.height;
				direction = (d == Move::Direction::Up ? -1 : 1);
				size = MovementSpeed;
				break;
			case Move::Direction::Left:
			case Move::Direction::Right:
				value = &Offset.width;
				direction = (d == Move::Direction::Left ? -1 : 1);
				size = MovementSpeed;
				break;
			case Move::Direction::RotateNegative:
			case Move::Direction::RotatePositive:
				value = &RotationAngle;
				direction = (d == Move::Direction::RotateNegative ? -1 : 1);
				size = RotationSpeed;
				break;
			default:
				value = nullptr;
			}
			if (value) *value = *value + size * static_cast<float>(dDelta) * direction;
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
				if (GetAsyncKeyState(VK_SHIFT) < 0)
				{
					//RotationAngle -= RotationSpeed * dDelta;
					Movement.bMove = true;
					Movement.vDirection.push_back(Move::Direction::RotateNegative);
				}
				else
				{
					Movement.bMove = true;
					Movement.vDirection.push_back(Move::Direction::Left);
				}
				break;
			case VK_RIGHT:
			case 'D':
				if (GetAsyncKeyState(VK_SHIFT) < 0)
				{
					//RotationAngle += RotationSpeed * dDelta;
					Movement.bMove = true;
					Movement.vDirection.push_back(Move::Direction::RotatePositive);
				}
				else
				{
					Movement.bMove = true;
					Movement.vDirection.push_back(Move::Direction::Right);
				}
				break;
			case VK_END:
				Scale = D2D1::SizeF(1.0f, 1.0f);
				Offset = D2D1::SizeF(0.0f, 0.0f);
				break;
			case VK_HOME:
				bShowSideMenu ^= true;
				break;
			}
		}
		if (keyEvents.IsRelease())
		{
			std::queue<Move::Direction> direction;
			switch (keyEvents.GetCode())
			{
			case VK_UP:
			case 'W':
				direction.push(Move::Direction::Up);
				break;
			case VK_DOWN:
			case 'S':
				direction.push(Move::Direction::Down);
				break;
			case VK_LEFT:
			case 'A':
				direction.push(Move::Direction::Left);
				direction.push(Move::Direction::RotateNegative);
				break;
			case VK_RIGHT:
			case 'D':
				direction.push(Move::Direction::Right);
				direction.push(Move::Direction::RotatePositive);
				break;
			}
			while (!direction.empty())
			{
				Movement.vDirection.erase(std::remove_if(Movement.vDirection.begin(), Movement.vDirection.end(), [direction](auto& o) { return o == direction.front(); }), Movement.vDirection.end());
				direction.pop();
			}
		}
	}
}