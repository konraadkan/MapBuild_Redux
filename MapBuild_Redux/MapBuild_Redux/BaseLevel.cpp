#include "BaseLevel.h"
#include <thread>
#include <mutex>


const bool AlphaSortSubType(PiecesW& p1, PiecesW& p2)
{
	return (toupper(p1.GetSubMenu().front()) < toupper(p2.GetSubMenu().front()));
}

const bool AlphaSortType(PiecesW& p1, PiecesW& p2)
{
	return (toupper(p1.GetType().front()) < toupper(p2.GetType().front()));
}

const bool AlphaSortName(PiecesW& p1, PiecesW& p2)
{
	return (toupper(p1.GetName().front()) < toupper(p2.GetName().front()));
}

BaseLevel::BaseLevel(Graphics* const graphics, D2D1_POINT_2F* const pMousePosition, int WindowX, int WindowY, HPTimer* const timer) : pTimer(timer)
{
	this->gfx = graphics;
	this->pMouseCoordinate = pMousePosition;
	WindowSize = D2D1::SizeF(static_cast<float>(WindowX), static_cast<float>(WindowY));
	m_ClientWindow = D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height);
	RotationCenter = D2D1::Point2F(WindowSize.width * 0.5f, WindowSize.height * 0.5f);

	pSideMenu = new SideMenu(D2D1::RectF(WindowSize.width * 0.75f, 0.0f, WindowSize.width, WindowSize.height), graphics, &Transforms, &m_ClientWindow, &MenuCoordinates, &pSelectedRoom, &pSelectedLayer, &ppvSprites, &vVisibleRooms, &vVisibleLayers, &sptest, &pvWalls, &pSelectedRoomWall, &pSelectedLayerWall);
	pSideMenu->pBaseLevel = this;
	pSideMenu->SetMousePointer(&MenuCoordinates);
	IObjects.push_back(pSideMenu);
	CreateRoom();
	
	//BuildObjects(L"mainpcs-unicode.ini");	
	//BuildObjects(L"mainpcs.ini");
	BuildObjects(L"init.ini");
	while (vPieces.size())
	{
		PiecesW t(gfx, pTimer, &GridSquareSize);
		t.Convert(vPieces.back());
		vPiecesW.push_back(t);
		vPieces.pop_back();
	}
	//std::sort(vPiecesW.begin(), vPiecesW.end(), AlphaSortSubType);
	
	this->LoadImages();
	std::sort(vPiecesW.begin(), vPiecesW.end(), AlphaSortType);
	pSideMenu->BuildCategories(&vPiecesW);
	std::sort(vPiecesW.begin(), vPiecesW.end(), AlphaSortSubType);
	pSideMenu->BuildWallMenu(&vPiecesW);	
	pSideMenu->BuildSubcategories(&vPiecesW);	

	pSizeMenu = new SizeMenu(gfx, D2D1::RectF(WindowSize.width * 0.75f, WindowSize.height - 133.0f, WindowSize.width, WindowSize.height), pMouseCoordinate);
	pSizeMenu->BuildMenuCreatureSize();
	pSizeMenu->SetSelectedCreatureSize(vPiecesW.front().GetSize());

	pThicknessMenu = new ThicknessMenu(gfx, D2D1::RectF(WindowSize.width * 0.75f, WindowSize.height - 98.0f, WindowSize.width, WindowSize.height), pMouseCoordinate);
	
	sptest = new SpritePointer(&vPiecesW.front(), Location());
	sptest->SetDestSprite(D2D1::RectF(-sptest->GetSpriteFrameSize().width, -sptest->GetSpriteFrameSize().height, 0.0f, 0.0f));
	sptest->SetCreatureSize(vPiecesW.front().GetSize());
	
	ppvSprites = &vSprites;
	pSelectedRoom = &vSprites.front();
	pSelectedLayer = &pSelectedRoom->front();
	pSideMenu->vSelectRoomsandLayers = &ppvSprites;
	pSideMenu->SetSelectedRoomPointer(&pSelectedRoom);
	pSideMenu->SetSelectedLayerPointer(&pSelectedLayer);

	wptest = std::make_unique<Wall>(gfx, &TranslatedCoordinates, bUseTexture);
	wptest->SetColor(D2D1::ColorF(1, 0, 0));
	wptest->SetThickness(pThicknessMenu->GetSelectedThickness());

	pvWalls = &vWalls;
	pSelectedRoomWall = &vWalls.front();
	pSelectedLayerWall = &pSelectedRoomWall->front();

	pSideMenu->pSelectWallRoomsandLayers = &pvWalls;
	pSideMenu->ppSelectedWallLayer = &pSelectedLayerWall;
	pSideMenu->ppSelectedWallRoom = &pSelectedRoomWall;
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
	SafeDelete(&pSizeMenu);

	while (vWalls.size())
	{
		while (vWalls.back().size())
		{
			while (vWalls.back().back().size())
			{
				vWalls.back().back().back().reset();
				vWalls.back().back().pop_back();
			}
			vWalls.back().pop_back();
		}
		vWalls.pop_back();
	}

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
	
	//gfx->DrawCircle(gfx->GetCompatibleTarget(), Transforms, D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height), D2D1::Point2F(500, 500), 40, D2D1::ColorF(0, 1, 0), 5.0f);
	//gfx->OutputTextSmall(gfx->GetCompatibleTarget(), Transforms, D2D1::RectF(0.0f,0.0f,WindowSize.width, WindowSize.height), L"Test Small", D2D1::RectF(0.0f, 64.0f, 64.0f, 128.0f));
	//gfx->OutputText(gfx->GetCompatibleTarget(), std::wstring(std::to_wstring(static_cast<long>(TranslatedCoordinates.x)) + L"," + std::to_wstring(static_cast<long>(TranslatedCoordinates.y))).c_str(), D2D1::RectF(0, 0, 128, 128));
	//gfx->FillCircle(gfx->GetCompatibleTarget(), D2D1::Point2F(800, 800), 100);
	//gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(1050, 100, 1900, 500), D2D1::ColorF(0,0,0), 25,25);
	//gfx->FillRoundedRect(gfx->GetCompatibleTarget(), D2D1::RectF(1050, 505, 1900, 905));
	//gfx->DrawRect(gfx->GetCompatibleTarget(), D2D1::RectF(500, 100, 1045, 500));

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
					for (auto& wall : vWalls[i][w])
						wall->Draw();
				}
			}
		}
	}
	
	if (sptest && !pSideMenu->WallSelected())
	{
		gfx->DrawRect(gfx->GetCompatibleTarget(), mPreviewDest, D2D1::ColorF(1.0f, 0.1f, 0.05f, 1.0f), 5.0f);
	}

	if (wptest && pSideMenu->WallSelected())
	{
		if (bLockToGrid) wptest->DrawPreview(mPreviewPoint);
		else wptest->DrawPreview();
		if (bLockToGrid) gfx->FillCircle(gfx->GetCompatibleTarget(), mPreviewPoint, 3.0f, D2D1::ColorF(1.0f, 0.0f, 0.0f));
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
	gfx->RestoreTransform(gfx->GetRenderTarget());

	gfx->BeginDraw(gfx->GetCompatibleTarget());
	gfx->ClearScreen(gfx->GetCompatibleTarget(), D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	DrawSideMenu();
	DrawSizeMenu();
	gfx->EndDraw(gfx->GetCompatibleTarget());

	gfx->SwapBuffer();
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

void BaseLevel::DrawSizeMenu()
{
	switch (mSizeMenuType)
	{
	case MeasurementMenu::SizeMenuType::CreatureSize:
		if (pSizeMenu) pSizeMenu->Draw();
		break;
	case MeasurementMenu::SizeMenuType::ThicknessSize:
		if (pThicknessMenu) pThicknessMenu->Draw();
		break;
	}
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
	//if (pMouse->LeftPressed())
	//{
	//	if (pSelectedLayer && !pSideMenu->IsInRealRect())
	//	{
	//		bool bAdd = true;
	//		for (auto& sl : *pSelectedLayer)
	//		{
	//			if (sl->GetSprite() == sptest->GetPieces()->GetSprite())
	//			{
	//				if (bLockToGrid)
	//				{
	//					if (static_cast<int>(mPreviewDest.left) == static_cast<int>(sptest->GetDestSprite().left) && static_cast<int>(mPreviewDest.top) == static_cast<int>(sptest->GetDestSprite().top))
	//					{
	//						bAdd = false;
	//						break;
	//					}
	//				}
	//				else
	//				{
	//					if (static_cast<int>(sl->GetDestSprite().left) == static_cast<int>(sptest->GetDestSprite().left) && static_cast<int>(sl->GetDestSprite().top) == static_cast<int>(sptest->GetDestSprite().top))
	//					{
	//						bAdd = false;
	//						break;
	//					}
	//				}
	//			}
	//		}
	//		if (bAdd)
	//		{
	//			pSelectedLayer->push_back(new SpritePointer(sptest->GetPieces(), Location()));
	//			pSelectedLayer->back()->SetCreatureSize(sptest->GetCreatureSize());
	//			//pSelectedLayer->back()->SetDestSprite(D2D1::RectF(TranslatedCoordinates.x, TranslatedCoordinates.y, TranslatedCoordinates.x + pSelectedLayer->back()->GetSpriteFrameSize().width, TranslatedCoordinates.y + pSelectedLayer->back()->GetSpriteFrameSize().height));				
	//			if (bLockToGrid)
	//				pSelectedLayer->back()->SetDestSprite(mPreviewDest, false);
	//			else
	//				pSelectedLayer->back()->SetDestSprite(D2D1::RectF(TranslatedCoordinates.x, TranslatedCoordinates.y, TranslatedCoordinates.x + GridSquareSize.width, TranslatedCoordinates.y + GridSquareSize.height));
	//		}
	//	}
	//}
	if (pMouse->RightPressed() && wptest->PointsEmpty())
	{
		for (size_t i =0; i < pSelectedLayer->size(); i++)
		{
			if (pSelectedLayer->at(i)->PointInSprite(TranslatedCoordinates))
			{
				pSelectedLayer->erase(pSelectedLayer->begin() + i);
				pSelectedLayer->shrink_to_fit();
				break;
			}
		}
		for (size_t i = 0; i < pSelectedLayerWall->size(); i++)
		{
			if (pSelectedLayerWall->at(i)->PointTouching(TranslatedCoordinates))
			{
				pSelectedLayerWall->erase(pSelectedLayerWall->begin() + i);
				pSelectedLayerWall->shrink_to_fit();
				break;
			}
		}
	}
	if (pMouse->MiddlePressed())
	{		
		if (PushMouseCoordinate.x || PushMouseCoordinate.y)
		{
			float dY = PushMouseCoordinate.y - pMouse->GetY();
			float dX = PushMouseCoordinate.x - pMouse->GetX();
			float d = static_cast<float>(dDelta);
			if (dY)
				Offset.height -= (dY < 0 ? d * dY : d * dY);
			if (dX)
				Offset.width -= (dX < 0 ? d * dX : d * dX);
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
			if (sptest)
			{
				sptest->SetDestSpritePosition(TranslatedCoordinates);
				sptest->SetDestResizedSpritePosition(TranslatedCoordinates);

				mPreviewDest = GetPreviewRect();
			}
			if (wptest)
			{
				if (bLockToGrid) mPreviewPoint = GetNearestCorner();
			}
			if (pThicknessMenu->IsSelected())
			{
				pThicknessMenu->UpdateSlider();
				wptest->SetThickness(pThicknessMenu->GetSelectedThickness());								
			}
			break;
		}
		case Mouse::Event::Type::LPress:
			if (pSelectedLayer && !pSideMenu->IsInRealRect() && !pSizeMenu->IsInRealRect())
			{
				if (sptest && !pSideMenu->WallSelected())
				{
					pSelectedLayer->push_back(new SpritePointer(sptest->GetPieces(), Location()));
					pSelectedLayer->back()->SetCreatureSize(sptest->GetCreatureSize());				
					bKeepAspect ? pSelectedLayer->back()->SetKeepAspectSprite() : pSelectedLayer->back()->UnsetKeepAspectSprite();
					
					if (bLockToGrid)
						pSelectedLayer->back()->SetDestSprite(mPreviewDest);
					else
						pSelectedLayer->back()->SetDestSprite(mPreviewDest);
						//pSelectedLayer->back()->SetDestSprite(D2D1::RectF(TranslatedCoordinates.x, TranslatedCoordinates.y, TranslatedCoordinates.x + sptest->GetDestResizedSpriteSize().width, TranslatedCoordinates.y + sptest->GetDestResizedSpriteSize().height));
				}
				else if (wptest)
				{
					sptest ? wptest->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f)) : wptest->SetColor(pSideMenu->GetSelectedWallColor());
					wptest->AddPoint(bLockToGrid ? mPreviewPoint : TranslatedCoordinates);
				}
			}
			else if (pSideMenu->IsInRealRect())
			{
				if (mSizeMenuType == MeasurementMenu::SizeMenuType::ThicknessSize)
				{
					if (!pThicknessMenu->IsHidden())
					{
						if (pThicknessMenu->PointInSlider())
						{
							pThicknessMenu->SetSelected();
						}
						else if (pThicknessMenu->PointOnSlideLine())
						{
							pThicknessMenu->JumpPosition();
							pThicknessMenu->SetPreviewRadius(pThicknessMenu->CalcRadius());
							wptest->SetThickness(pThicknessMenu->GetSelectedThickness());
						}
					}
				}
			}
			break;
		case Mouse::Event::Type::LRelease:
		{
			if (mSizeMenuType == MeasurementMenu::SizeMenuType::CreatureSize && pSizeMenu->Interact())
			{
				if (sptest) sptest->SetCreatureSize(pSizeMenu->GetSelectedCreatureSize());
			}
			else if (mSizeMenuType == MeasurementMenu::SizeMenuType::ThicknessSize && pThicknessMenu->Interact())
			{
				if (pThicknessMenu->IsSelected())
				{
					pThicknessMenu->UnsetSelected();					
				}
			}
			else 
			{
				for (auto& io : IObjects)
				{
					if (io->PointInRect())
					{
						io->Interact();						
						break;
					}
				}
				if (sptest) pSizeMenu->SetSelectedCreatureSize(sptest->GetCreatureSize());
				if (pSideMenu) mSizeMenuType = pSideMenu->GetSizeMenuType();
				if (wptest) bUseTexture ? wptest->SetUseTexture() : wptest->UnsetUseTexture();
			}
		}
		break;
		case Mouse::Event::Type::RPress:
			if (pSideMenu->WallSelected()) if (wptest) wptest->RemoveLastPoint();
			break;
		case Mouse::Event::Type::RRelease:
			break;
		case Mouse::Event::Type::MPress:
			if(!pSideMenu->PointInRect(*pMouseCoordinate) || pSideMenu->IsHidden())
				PushMouseCoordinate = e.GetPos();
			break;
		case Mouse::Event::Type::MRelease:
			PushMouseCoordinate = D2D1::Point2F();
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
			if (pSideMenu->IsInRealRect())
			{
				for (auto& categories : pSideMenu->CategoryMenu->vSubsections)
				{
					if (!categories->IsHidden())
					{
						for (auto& sub : categories->vSubsections)
						{
							if (!sub->IsHidden()) //if (sub->PointInRect())
								sub->WheelUp();
						}
					}
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
			if (pSideMenu->IsInRealRect())
			{
				for (auto& categories : pSideMenu->CategoryMenu->vSubsections)
				{
					if (!categories->IsHidden())
					{
						for (auto& sub : categories->vSubsections)
						{
							if (!sub->IsHidden())
								sub->WheelDown();
						}
					}
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
			case VK_RETURN:
				if (pSideMenu->WallSelected())
				{
					bool close = pKeyboard->KeyIsPressed(VK_SHIFT) | pSideMenu->UseTexture();
					if (pSideMenu->UseTexture())
					{
						//need to define which texture to use somehow
						ID2D1Bitmap* t = sptest->GetSprite()->GetFrameBitmap();
						if (!t)
						{
							sptest->GetSprite()->CreateFrameBitmap();
							t = sptest->GetSprite()->GetFrameBitmap();
						}
						if (t) wptest->SetTexture(t);
					}
					wptest->SetGeometry(close);
					if (pSelectedLayerWall)
					{
						pSelectedLayerWall->push_back(std::unique_ptr<Wall>(wptest.get()));
					}
					wptest.release();
					wptest = std::make_unique<Wall>(gfx, &TranslatedCoordinates, pSideMenu->UseTexture());
					wptest->SetColor(D2D1::ColorF(1, 0, 0));
					wptest->SetThickness(pThicknessMenu->GetSelectedThickness());
				}
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

void BaseLevel::OutputImageLoadingStatus(unsigned int numloaded, unsigned int total, const std::wstring imagetype)
{
	std::wstring msg = L"Loading " + imagetype + L" " + std::to_wstring(numloaded + 1) + L" of " + std::to_wstring(total);
	gfx->BeginDraw(gfx->GetRenderTarget());
	gfx->ClearScreen(gfx->GetRenderTarget(), D2D1::ColorF(0.75f, 0.75f, 0.75f));
	gfx->OutputText(gfx->GetRenderTarget(), msg.c_str(), m_ClientWindow, D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	gfx->EndDraw(gfx->GetRenderTarget());
}

void BaseLevel::OutputImageLoadingStatusM(std::atomic<unsigned int>& numloaded, unsigned int total, const std::wstring imagetype)
{
	std::mutex m;
	double dtTimeout = 120.0;
	pTimer->Update();
	double ttime = pTimer->GetTimeTotal();
	while (numloaded < total)
	{		
		m.lock();
		std::wstring msg = L"Loading " + imagetype + L" " + std::to_wstring(numloaded + 1) + L" of " + std::to_wstring(total);
		m.unlock();
		gfx->BeginDraw(gfx->GetRenderTarget());
		gfx->ClearScreen(gfx->GetRenderTarget(), D2D1::ColorF(0.75f, 0.75f, 0.75f));
		gfx->OutputText(gfx->GetRenderTarget(), msg.c_str(), m_ClientWindow, D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		gfx->EndDraw(gfx->GetRenderTarget());
		if (pTimer->GetTimeTotal() - ttime > dtTimeout)
			break;
	}
}

void BaseLevel::LoadSprites()
{
	unsigned int totalnumbersprites = static_cast<unsigned int>(vPieces.size() + vPiecesW.size());
	std::atomic<unsigned int> numberfinished = 0;
	std::vector<std::thread> tthreads;

	std::thread temp([this, &numberfinished, &totalnumbersprites]() { this->OutputImageLoadingStatusM(numberfinished, totalnumbersprites, L"Sprites"); });
	for (auto& p : vPieces)
	{
		tthreads.push_back(std::thread([&p, &numberfinished]() { p.LoadSpriteM(numberfinished); }));
	}
	for (auto& p : vPiecesW)
	{
		tthreads.push_back(std::thread([&p, &numberfinished]() { p.LoadSpriteM(numberfinished); }));
	}

	for (auto& t : tthreads)
		t.join();
	temp.join();
}

void BaseLevel::LoadImages()
{
	LoadSprites();
	LoadPortraits();
}

void BaseLevel::LoadPortraits()
{
	unsigned int totalnumbersprites = static_cast<unsigned int>(vPieces.size() + vPiecesW.size());
	unsigned int spritesloaded = 0;
	std::atomic<unsigned int> numberfinished = 0;
	std::vector<std::thread> tthreads;
	
	std::thread temp([this, &numberfinished, &totalnumbersprites]() { this->OutputImageLoadingStatusM(numberfinished, totalnumbersprites - 1, L"Portraits"); });
	for (auto& p : vPieces)
	{
		tthreads.push_back(std::thread([&p, &numberfinished]() { p.LoadPortraitM(numberfinished); }));
	}
	for (auto& p : vPiecesW)
	{
		tthreads.push_back(std::thread([&p, &numberfinished]() { p.LoadPortraitM(numberfinished); }));
	}

	for (auto& t : tthreads)
		t.join();
	temp.join();
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
		while (static_cast<size_t>(pos - buffer) <= BufferSize)
		{
			vPiecesW.push_back(PiecesW(gfx, pTimer, &GridSquareSize));
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
			vPieces.push_back(Pieces(gfx, pTimer, &GridSquareSize));
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
	vWalls.push_back(std::vector< std::vector<std::unique_ptr<Wall>>>());
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

void BaseLevel::CreateLayer(size_t uRoomNumber)
{
	vSprites[uRoomNumber].push_back(std::vector<SpritePointer*>());
	vWalls[uRoomNumber].push_back(std::vector<std::unique_ptr<Wall>>());
	vVisibleLayers[uRoomNumber].push_back(true);
	if (pSideMenu)
	{
		pSideMenu->CreateLayer(uRoomNumber);
		pSideMenu->CreateLayerButton(&Transforms, &m_ClientWindow, uRoomNumber);
	}
}

const D2D1_RECT_F BaseLevel::GetPreviewRect()
{
	D2D1_SIZE_F size;
	float sizemod = 1.0f;
	switch (sptest->GetCreatureSize())
	{
	case CreatureSize::Diminutive:
	case CreatureSize::Fine:
	case CreatureSize::Tiny:
	case CreatureSize::Small:
	{
		sizemod = static_cast<float>(1.0f / pow(2, static_cast<unsigned long>(CreatureSize::Medium) - static_cast<unsigned long>(sptest->GetCreatureSize())));
		size = D2D1::SizeF(GridSquareSize.width * sizemod, GridSquareSize.height * sizemod);
		break;
	}
	case CreatureSize::Medium:
		size = GridSquareSize;
		break;
	case CreatureSize::Large:
	case CreatureSize::Huge:
	case CreatureSize::Gargantuan:
	case CreatureSize::Colossal:	
		sizemod = static_cast<float>(1 + static_cast<unsigned int>(sptest->GetCreatureSize()) - static_cast<unsigned int>(CreatureSize::Medium));
		size = D2D1::SizeF(GridSquareSize.width * sizemod, GridSquareSize.height * sizemod);
		break;
	default:
		sizemod = static_cast<float>(sptest->GetCreatureSize()) - static_cast<float>(CreatureSize::Colossal);
		size = D2D1::SizeF(sptest->GetSpriteFrameSize().width * sizemod, sptest->GetSpriteFrameSize().height * sizemod);
	}
	if (bLockToGrid)
	{
		D2D1_POINT_2F lockedPoint;
		if (static_cast<int>(sptest->GetCreatureSize()) < static_cast<int>(CreatureSize::Medium))
		{
			lockedPoint.x = static_cast<float>(static_cast<unsigned int>(sptest->GetDestSprite().left) / static_cast<unsigned int>(GridSquareSize.width * sizemod));			
			lockedPoint.y = static_cast<float>(static_cast<unsigned int>(sptest->GetDestSprite().top) / static_cast<unsigned int>(GridSquareSize.height * sizemod));
			lockedPoint.x *= GridSquareSize.width * sizemod;
			lockedPoint.y *= GridSquareSize.height * sizemod;
		}
		else
		{
			lockedPoint.x = static_cast<float>(static_cast<unsigned int>(sptest->GetDestSprite().left) / static_cast<unsigned int>(GridSquareSize.width));
			lockedPoint.y = static_cast<float>(static_cast<unsigned int>(sptest->GetDestSprite().top) / static_cast<unsigned int>(GridSquareSize.height));
			lockedPoint.x *= GridSquareSize.width;
			lockedPoint.y *= GridSquareSize.height;
		}
		
		return D2D1::RectF(lockedPoint.x, lockedPoint.y, lockedPoint.x + size.width, lockedPoint.y + size.height);
	}
	return D2D1::RectF(sptest->GetDestSprite().left, sptest->GetDestSprite().top, sptest->GetDestSprite().left + size.width, sptest->GetDestSprite().top + size.height);
}

const D2D1_POINT_2F BaseLevel::GetNearestCorner()
{
	if (bLockToGrid)
	{
		D2D1_POINT_2F closecorner;
		closecorner.x = static_cast<float>(static_cast<int>(TranslatedCoordinates.x) / static_cast<int>(GridSquareSize.width));
		closecorner.y = static_cast<float>(static_cast<int>(TranslatedCoordinates.y) / static_cast<int>(GridSquareSize.height));
		closecorner.x *= GridSquareSize.width;
		closecorner.y *= GridSquareSize.height;

		float val = TranslatedCoordinates.x - closecorner.x;
		if (val > GridSquareSize.width * 0.5f)
			closecorner.x += GridSquareSize.width;
		val = TranslatedCoordinates.y - closecorner.y;
		if (val > GridSquareSize.height * 0.5f)
			closecorner.y += GridSquareSize.height;
		return closecorner;
	}
	return TranslatedCoordinates;
}