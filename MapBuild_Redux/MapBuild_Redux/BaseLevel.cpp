#include "BaseLevel.h"

BaseLevel::BaseLevel(Graphics* const graphics, D2D1_POINT_2F* const pMousePosition, int WindowX, int WindowY, HPTimer* const timer) : pTimer(timer)
{
	this->gfx = graphics;
	this->pMouseCoordinate = pMousePosition;
	WindowSize = D2D1::SizeF(static_cast<float>(WindowX), static_cast<float>(WindowY));
	m_ClientWindow = D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height);
	RotationCenter = D2D1::Point2F(WindowSize.width * 0.5f, WindowSize.height * 0.5f);

	pSideMenu = new SideMenu(D2D1::RectF(WindowSize.width * 0.75f, 0.0f, WindowSize.width, WindowSize.height), graphics, &Transforms, &m_ClientWindow, &MenuCoordinates, &pSelectedRoom, &pSelectedLayer, &ppvSprites, &vVisibleRooms, &vVisibleLayers);
	pSideMenu->SetMousePointer(&MenuCoordinates);
	IObjects.push_back(pSideMenu);
	CreateRoom();
	
	BuildObjects(L"mainpcs-unicode.ini");
	//BuildObjects(L"mainpcs.ini");
	while (vPieces.size())
	{
		PiecesW t(gfx, pTimer);
		t.Convert(vPieces.back());
		vPiecesW.push_back(t);
		vPieces.pop_back();
	}
	pSideMenu->BuildCategories(&vPiecesW);
	pSideMenu->BuildSubcategories(&vPiecesW);
	
	sptest = new SpritePointer(&vPiecesW.front(), Location());
//	sptest->SetCreatureSize(CreatureSize::Large); //example of how to set creature size for spritepointer objects
	
	ppvSprites = &vSprites;
	pSelectedRoom = &vSprites.front();
	pSelectedLayer = &pSelectedRoom->front();
}

BaseLevel::~BaseLevel()
{
	Unload();
}

void BaseLevel::Load(Keyboard* const keyboard, Mouse* const mouse)
{
	if (!gfx) return;
	//set default values
	pKeyboard = keyboard;
	pMouse = mouse;
	Center = D2D1::Point2F();
}

void BaseLevel::Unload()
{
	//cleanup as necessary
	SafeDelete(&sptest);
	SafeDelete(&pSideMenu);

	while (vSprites.size())
	{
		while (vSprites.back().size())
		{
			while (vSprites.back().back().size())
			{
				SafeDelete(&vSprites.back().back().back());
				vSprites.back().back().pop_back();
			}
			vSprites.back().pop_back();
		}
		vSprites.pop_back();
	}

	while (IObjects.size())
	{
		IObjects.back() = nullptr;
		IObjects.pop_back();
	}
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
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), Transforms, D2D1::RectF(0.0f,0.0f,WindowSize.width, WindowSize.height), L"Test Small", D2D1::RectF(0.0f, 64.0f, 64.0f, 128.0f));
	gfx->OutputText(gfx->GetCompatibleTarget(), std::wstring(std::to_wstring(static_cast<long>(TranslatedCoordinates.x)) + L"," + std::to_wstring(static_cast<long>(TranslatedCoordinates.y))).c_str(), D2D1::RectF(0, 0, 128, 128));
	gfx->FillCircle(gfx->GetCompatibleTarget(), D2D1::Point2F(800, 800), 100);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(1050, 100, 1900, 500), D2D1::ColorF(0,0,0), 25,25);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(1050, 505, 1900, 905));
	gfx->DrawRect(gfx->GetCompatibleTarget(), D2D1::RectF(500, 100, 1045, 500));

	for (size_t i = 0; i < vVisibleRooms.size(); i++)
	{
		if (vVisibleRooms[i])
		{
			for (size_t w = 0; w < vVisibleLayers[i].size(); w++)
			{
				if (vVisibleLayers[i][w])
				{
					for (auto& sprite : vSprites[i][w])
						sprite->DrawSprite(gfx);
				}
			}
		}
	}
	
	if (sptest)
	{
		sptest->DrawSprite(gfx);
	}

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
	gfx->OutputText(gfx->GetRenderTarget(), std::to_wstring(afps).c_str(), D2D1::RectF(0, 500, 500, 564));
	gfx->EndDraw(gfx->GetRenderTarget());
}

void BaseLevel::DrawSideMenu()
{
	D2D1::Matrix3x2F oTransform;
	gfx->GetCompatibleTarget()->GetTransform(&oTransform);	
	if (pSideMenu->IsHidden())
	{
		gfx->GetCompatibleTarget()->SetTransform(D2D1::Matrix3x2F::Translation(D2D1::SizeF(pSideMenu->GetSize().width, 0.0f)));
	}
	pSideMenu->Draw();
	MenuCoordinates = gfx->GetTransformedPoint(gfx->GetCompatibleTarget(), *pMouseCoordinate);
	if (pSideMenu->IsHidden()) gfx->GetCompatibleTarget()->SetTransform(oTransform);
}

void BaseLevel::Update(double dDelta)
{
	afps = static_cast<int>(1.0 / dDelta);
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
	if (pMouse->LeftPressed())
	{
		if (pSelectedLayer && !pSideMenu->IsInRealRect())
		{
			bool bAdd = true;
			for (auto& sl : *pSelectedLayer)
			{
				if (sl->GetSprite() == sptest->GetPieces()->GetSprite())
				{
					if (static_cast<int>(sl->GetDestSprite().left) == static_cast<int>(sptest->GetDestSprite().left) && static_cast<int>(sl->GetDestSprite().top) == static_cast<int>(sptest->GetDestSprite().top))
					{
						bAdd = false;
						break;
					}
				}
			}
			if (bAdd)
			{
				pSelectedLayer->push_back(new SpritePointer(sptest->GetPieces(), Location()));
				pSelectedLayer->back()->SetCreatureSize(sptest->GetCreatureSize());
				pSelectedLayer->back()->SetDestSprite(D2D1::RectF(TranslatedCoordinates.x, TranslatedCoordinates.y, TranslatedCoordinates.x + pSelectedLayer->back()->GetSpriteFrameSize().width, TranslatedCoordinates.y + pSelectedLayer->back()->GetSpriteFrameSize().height));				
			}
		}
	}
}

void BaseLevel::ProcessEvents(double dDelta)
{
	ProcessKeyboardEvents(dDelta);
	ProcessMouseEvents(dDelta);
}

void BaseLevel::ProcessMouseEvents(double dDelta)
{
	if (!pMouse) return;

	if (!pMouse->IsEmpty())
	{
		Mouse::Event e = pMouse->Read();
		switch (e.GetType())
		{
		case Mouse::Event::Type::Move:
		{
			sptest->SetDestSprite(D2D1::RectF(TranslatedCoordinates.x, TranslatedCoordinates.y, TranslatedCoordinates.x + sptest->GetSpriteFrameSize().width, TranslatedCoordinates.y + sptest->GetSpriteFrameSize().height));
			break;
		}
		case Mouse::Event::Type::LPress:
			if (pSelectedLayer && !pSideMenu->IsInRealRect())
			{
				pSelectedLayer->push_back(new SpritePointer(sptest->GetPieces(), Location()));
				pSelectedLayer->back()->SetCreatureSize(sptest->GetCreatureSize());
				pSelectedLayer->back()->SetDestSprite(D2D1::RectF(TranslatedCoordinates.x, TranslatedCoordinates.y, TranslatedCoordinates.x + pSelectedLayer->back()->GetSpriteFrameSize().width, TranslatedCoordinates.y + pSelectedLayer->back()->GetSpriteFrameSize().height));				
			}
			break;
		case Mouse::Event::Type::LRelease:
		{
			for (auto& io : IObjects)
			{
				if (io->PointInRect())
				{
					io->Interact();
					break;
				}
			}
		}
		break;
		case Mouse::Event::Type::RPress:
			break;
		case Mouse::Event::Type::RRelease:
			break;
		case Mouse::Event::Type::MPress:
			break;
		case Mouse::Event::Type::MRelease:
			break;
		case Mouse::Event::Type::WheelUp:
		{
			for (auto& io : IObjects)
			{
				if (io->PointInRect())
				{
					io->WheelUp();
					break;
				}
			}
		}
			break;
		case Mouse::Event::Type::WheelDown:
		{
			for (auto& io : IObjects)
			{
				if (io->PointInRect())
				{
					io->WheelDown();
					break;
				}
			}
		}
			break;
		}
	}
}

void BaseLevel::ProcessKeyboardEvents(double dDelta)
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

template<typename T> void BaseLevel::RemoveEmptyPieces(T& pieces)
{
	pieces.erase(std::remove_if(pieces.begin(), pieces.end(), [](auto& o) { return o.GetType().empty(); }), pieces.end());
}

void BaseLevel::BuildObjects(const wchar_t* sFilePath)
{
	size_t BufferSize = 0;
	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, sFilePath, L"rb");
	if (err)
	{
		std::wstring errmsg = L"Failed to open ";
		errmsg.append(sFilePath);
		std::wstring errtopic = L"Error #" + std::to_wstring(static_cast<int>(err));
		MessageBoxW(nullptr, errmsg.c_str(), errtopic.c_str(), MB_OK | MB_ICONERROR);
		return;
	}

	fseek(file, 0, SEEK_END);
	BufferSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	//handles if the file is in unicode
	if (fgetwc(file) == 0xFEFF)
	{
		wchar_t* buffer = new wchar_t[BufferSize];
		buffer[BufferSize - 1] = L'\0';
		fread(buffer, BufferSize - 1, 1, file);
		fclose(file);
		file = nullptr;
		wchar_t* pos = buffer;
		while (static_cast<size_t>(pos - buffer ) <= BufferSize)
		{
			vPiecesW.push_back(PiecesW(gfx, pTimer));
			std::queue<std::wstring> temp = vPiecesW.back().FillPiece(pos, pos);
			while (temp.size())
			{
				BuildObjects(temp.front().c_str());
				temp.pop();
			}
		}
		pos = nullptr;
		RemoveEmptyPieces(vPiecesW);
		SafeDeleteArray(&buffer);
	}
	//handles if the file is in ansi
	else
	{
		fseek(file, 0, SEEK_SET);
		char* buffer = new char[BufferSize + 1];
		buffer[BufferSize] = '\0';
		fread(buffer, BufferSize, 1, file);
		fclose(file);
		file = nullptr;
		char* pos = buffer;
		while (static_cast<size_t>(pos - buffer + 1) <= BufferSize)
		{
			vPieces.push_back(Pieces(gfx, pTimer));
			std::queue<std::wstring> temp = vPieces.back().FillPiece(pos, pos);
			while (temp.size())
			{
				BuildObjects(temp.front().c_str());
				temp.pop();
			}
		}
		pos = nullptr;
		RemoveEmptyPieces(vPieces);
		SafeDeleteArray(&buffer);
	}
	if (file) fclose(file);
}

const std::wstring BaseLevel::GetLineW(wchar_t* const buffer, wchar_t*& pos)
{
	std::wstring wbuffer = (buffer[0] == 0xFEFF) ? buffer + 1 : buffer;

	wchar_t endline[] = L";þ\n\r";
	std::string::size_type p = wbuffer.find_first_of(endline);
	if (p == std::string::npos)
	{
		pos = pos + wbuffer.size() + 1;
		return wbuffer;
	}
	pos = buffer + p + 1;
	std::wstring tw(wbuffer.begin(), wbuffer.begin() + p);
	return tw;
}

const std::wstring BaseLevel::GetLineW(char* const buffer, char*& pos)
{
	std::wstring wbuffer(strlen(buffer), L'#');
	size_t conv = 0;
	mbstowcs_s(&conv, &wbuffer[0], strlen(buffer) + 1, buffer, strlen(buffer));

	wchar_t endline[] = L";þ\n\r";
	std::string::size_type p = wbuffer.find_first_of(endline);
	if (p == std::string::npos)
	{
		pos = pos + wbuffer.size() + 1;
		return wbuffer;
	}

	pos = buffer + p + 1;

	std::wstring tw(wbuffer.begin(), wbuffer.begin() + p);
	return tw;
}

void BaseLevel::CreateRoom()
{
	vSprites.push_back(std::vector< std::vector<SpritePointer*>>());
	vVisibleRooms.push_back(true);
	vVisibleLayers.push_back(std::vector<bool>());	
	if (pSideMenu)
	{
		pSideMenu->CreateLayerMenuSection();
		CreateLayer(vSprites.size() - 1);
		pSideMenu->CreateRoomButton(&Transforms, &m_ClientWindow);
		pSideMenu->RealignAddLayerButton();
	}
}

void BaseLevel::CreateLayer(std::vector< std::vector<SpritePointer*>>* const Room)
{
	Room->push_back(std::vector<SpritePointer*>());
}

void BaseLevel::CreateLayer(size_t uRoomNumber)
{
	vSprites[uRoomNumber].push_back(std::vector<SpritePointer*>());
	vVisibleLayers[uRoomNumber].push_back(true);
	if (pSideMenu)
	{
		pSideMenu->CreateLayer(uRoomNumber);
		pSideMenu->CreateLayerButton(&Transforms, &m_ClientWindow, uRoomNumber);
	}
}