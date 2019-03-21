#include "BaseLevel.h"
#include <thread>
#include <mutex>
#include <cmath>
#include <comdef.h>

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

const bool BaseLevel::CreateNew()
{
	if (pSideMenu) return pSideMenu->CreateNew();
	return true;
}

BaseLevel::BaseLevel(const HWND hwnd, Graphics* const graphics, D2D1_POINT_2F* const pMousePosition, int WindowX, int WindowY, HPTimer* const timer) : pTimer(timer), hWnd(hwnd)
{
	this->gfx = graphics;
	this->pMouseCoordinate = pMousePosition;
	WindowSize = D2D1::SizeF(static_cast<float>(WindowX), static_cast<float>(WindowY));
	m_ClientWindow = D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height);
	RotationCenter = D2D1::Point2F(WindowSize.width * 0.5f, WindowSize.height * 0.5f);

	pSideMenu = new SideMenu(&bShowCounter, &pTurnCounter, hWnd, &bExit, &bLockToGrid, &bGridOnTop, &bKeepAspect, D2D1::RectF(WindowSize.width * 0.75f, 0.0f, WindowSize.width, WindowSize.height), graphics, &Transforms, &m_ClientWindow, &MenuCoordinates, &pSelectedRoom, &pSelectedLayer, &ppvSprites, &vVisibleRooms, &vVisibleLayers, &sptest, &pvWalls, &pSelectedRoomWall, &pSelectedLayerWall);
	pSideMenu->pBaseLevel = this;
	pSideMenu->SetMousePointer(&MenuCoordinates);
	IObjects.push_back(pSideMenu);
	CreateRoom();
	
	if (!BuildObjects(L"init.ini"))
	{
		bExit = true;
		return;
	}
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
	pSideMenu->BuildAoeMenu(&vPiecesW);
	pSideMenu->BuildSubcategories(&vPiecesW);	

	pSizeMenu = new SizeMenu(gfx, D2D1::RectF(WindowSize.width * 0.75f, WindowSize.height - 133.0f, WindowSize.width, WindowSize.height), pMouseCoordinate);
	pSizeMenu->BuildMenuCreatureSize();
	pSizeMenu->SetSelectedCreatureSize(vPiecesW.front().GetSize());

	pThicknessMenu = new ThicknessMenu(gfx, D2D1::RectF(WindowSize.width * 0.75f, WindowSize.height - 98.0f, WindowSize.width, WindowSize.height), pMouseCoordinate);
	pAoeSizeMenu = new AoeSizeMenu(&pSideMenu->SelectedAoeType, gfx, D2D1::RectF(WindowSize.width * 0.75f, WindowSize.height * 0.5f, WindowSize.width, WindowSize.height), pMouseCoordinate);
	
	sptest = new SpritePointer(gfx, &vPiecesW.front(), Location(), &GridSquareSize);
	sptest->SetDestSprite(D2D1::RectF(-sptest->GetSpriteFrameSize().width, -sptest->GetSpriteFrameSize().height, 0.0f, 0.0f));
	sptest->SetCreatureSize(vPiecesW.front().GetSize());
	
	ppvSprites = &vSprites;
	pSelectedRoom = &vSprites.front();
	pSelectedLayer = &pSelectedRoom->front();
	pSideMenu->vSelectRoomsandLayers = &ppvSprites;
	pSideMenu->SetSelectedRoomPointer(&pSelectedRoom);
	pSideMenu->SetSelectedLayerPointer(&pSelectedLayer);

	wptest = std::make_unique<Wall>(this, &GridSquareSize, gfx, &TranslatedCoordinates, bUseTexture);
	wptest->SetColor(D2D1::ColorF(1, 0, 0));
	wptest->SetThickness(pThicknessMenu->GetSelectedThickness());

	pvWalls = &vWalls;
	pSelectedRoomWall = &vWalls.front();
	pSelectedLayerWall = &pSelectedRoomWall->front();

	pSideMenu->pSelectWallRoomsandLayers = &pvWalls;
	pSideMenu->ppSelectedWallLayer = &pSelectedLayerWall;
	pSideMenu->ppSelectedWallRoom = &pSelectedRoomWall;

	for (auto& p : vPiecesW)
	{
		if (p.IsDefault())
		{
			pSideMenu->vInitativeList.push_back(&p);
		}
	}
	pSideMenu->BuildInitativeList();

	mRulerDest = D2D1::RectF(5.0f, (WindowSize.height * 0.5f) - 16.0f, 105.0f, (WindowSize.height * 0.5f) + 16.0f);
	pTurnCounter = new TurnCounter(gfx, D2D1::Ellipse(D2D1::Point2F(0.0f, 0.0f), GridSquareSize.width * 3.0f, GridSquareSize.height * 3.0f), &uCounterValue, D2D1::ColorF(0.69f, 0.89f, 0.91f, 0.6f));
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
	SafeDelete(&sptest);	UpdateLog(L"SafeDelete(sptest)", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	SafeDelete(&pSideMenu);	UpdateLog(L"SafeDelete(sidemenu)", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	SafeDelete(&pSizeMenu);	UpdateLog(L"SafeDelete(sizemenu)", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	SafeDelete(&pAoeSizeMenu);	UpdateLog(L"SafeDelete(AoesizeMenu)", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	SafeDelete(&pRuler);	UpdateLog(L"SafeDelete(pRuler)", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	SafeDelete(&pTurnCounter);	UpdateLog(L"SafeDelete(pTurnCounter)", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	wptest.reset();	UpdateLog(L"wptest.reset()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));

	while (vPieces.size())
	{
		vPieces.back().UnloadSprite();
		vPieces.back().UnloadPortrait();
		vPieces.pop_back();
	}

	while (vPiecesW.size())
	{
		vPiecesW.back().UnloadSprite();
		vPiecesW.back().UnloadPortrait();
		vPiecesW.pop_back();
	}

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
	ForceThreadJoin();
}

void BaseLevel::Render()
{
	//build the frame
	gfx->BeginDraw(gfx->GetCompatibleTarget());
	gfx->ClearScreen(gfx->GetCompatibleTarget(), GridBackgroundColor);
	if (!bGridOnTop) gfx->DrawDefaultGrid(gfx->GetCompatibleTarget(), Transforms, D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height), GridSquareSize, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.9f), 3.0f);
	
	for (size_t i = 0; i < vVisibleRooms.size(); i++)
	{
		if (vVisibleRooms[i])
		{
			for (size_t w = 0; w < vVisibleLayers[i].size(); w++)
			{
				if (vVisibleLayers[i][w])
				{
					for (auto& sprite : vSprites[i][w])
					{
						if (pSideMenu->ShowPieceColors()) gfx->FillRect(gfx->GetCompatibleTarget(), sprite->GetDestSprite(), sprite->GetPieces()->GetBackgroundColor());
						sprite->DrawSprite(gfx);
					}
					for (auto& wall : vWalls[i][w])
						wall->Draw();
				}
			}
		}
	}
		
	if (pSelectedObject)
	{
		gfx->DrawRect(gfx->GetCompatibleTarget(), mPreviewDest, D2D1::ColorF(1.0f, 0.1f, 0.05f, 1.0f), 5.0f);
	}
	else if (sptest && !pSideMenu->WallSelected() && !pSideMenu->AoeSelected())
	{
		if (pSideMenu->IsBuildMode() || pSelectedObject) gfx->DrawRect(gfx->GetCompatibleTarget(), mPreviewDest, D2D1::ColorF(1.0f, 0.1f, 0.05f, 1.0f), 5.0f);
	}
	else if (wptest && pSideMenu->WallSelected() && !pSideMenu->AoeSelected())
	{
		if (bLockToGrid) wptest->DrawPreview(mPreviewPoint);
		else wptest->DrawPreview();
		if (bLockToGrid) gfx->FillCircle(gfx->GetCompatibleTarget(), mPreviewPoint, 3.0f, D2D1::ColorF(1.0f, 0.0f, 0.0f));
	}
	else if (pSideMenu->AoeSelected() && pSideMenu->IsBuildMode())
	{
		if (bLockToGrid) gfx->FillCircle(gfx->GetCompatibleTarget(), mPreviewPoint, 3.0f, D2D1::ColorF(1.0f, 0.0f, 0.0f, 0.85f));
		else gfx->FillCircle(gfx->GetCompatibleTarget(), TranslatedCoordinates, 3.0f, D2D1::ColorF(1.0f, 0.0f, 0.0f, 0.85f));
	}

	if (bGridOnTop) gfx->DrawDefaultGrid(gfx->GetCompatibleTarget(), Transforms, D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height), GridSquareSize, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.9f), 3.0f);
	if (pRuler)	pRuler->DrawLine();

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
	pTurnCounter->Draw();
	gfx->EndDraw(gfx->GetCompatibleTarget());

	gfx->SwapBuffer();
	if (bShowFPS) gfx->OutputText(gfx->GetRenderTarget(), std::to_wstring(afps).c_str(), D2D1::RectF(0, 500, 500, 564));
	if (pRuler) pRuler->DrawDistance();
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
	case MeasurementMenu::SizeMenuType::AoeSize:
		if (pAoeSizeMenu) pAoeSizeMenu->Draw();
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

	//----- Eraser Logic-----
	if (pMouse->RightPressed() && wptest->PointsEmpty())
	{
		if (pSideMenu->IsAttachObject())
		{
			for (auto& sprite : *pSelectedLayer)
			{
				if (sprite->PointInSprite(TranslatedCoordinates))
				{
					sprite->RemoveChildren();
				}
			}
		}
		else
		{
			for (size_t i = 0; i < pSelectedLayer->size(); i++)
			{
				if (pSelectedLayer->at(i)->PointInSprite(TranslatedCoordinates))
				{
					if (pSelectedLayer->at(i) == pSelectedObject) pSelectedObject = nullptr;
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
	}

	//----- Alternate method to move the screen ------
	if (pMouse->X1Pressed())
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
	
	if (bUpdateRotation)
	{
		if (pSideMenu->IsAttachObject())
		{
			if (pLastAttachedSpritePointer)
			{
				D2D1::Matrix3x2F transforms = pLastAttachedSpritePointer->GetTransformsMatrix();
				D2D1_POINT_2F p1 = transforms.TransformPoint(pLastAttachedSpritePointer->GetCenter());
				float deltax = TranslatedCoordinates.x - p1.x;
				float deltay = TranslatedCoordinates.y - p1.y;
				float theta = atan2(deltay, deltax);
				pLastAttachedSpritePointer->SetRotation(theta);
				pLastAttachedSpritePointer->UpdateRotationMatrix();
			}
		}
		else
		{
			D2D1::Matrix3x2F transforms = static_cast<AoeSpritePointer*>(pSelectedLayer->back())->GetTransformsMatrix();
			D2D1_POINT_2F p1 = transforms.TransformPoint(static_cast<AoeSpritePointer*>(pSelectedLayer->back())->GetCenter());
			float deltax = TranslatedCoordinates.x - p1.x;
			float deltay = TranslatedCoordinates.y - p1.y;
			float theta = atan2(deltay, deltax);
			static_cast<AoeSpritePointer*>(pSelectedLayer->back())->SetRotation(theta);
			static_cast<AoeSpritePointer*>(pSelectedLayer->back())->UpdateRotationMatrix();
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

				mPreviewDest = GetPreviewRect(pSelectedObject ? pSelectedObject : sptest, TranslatedCoordinates);
			}
			else if (pSelectedObject)
			{
				mPreviewDest = GetPreviewRect(pSelectedObject, TranslatedCoordinates);
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
			if (pAoeSizeMenu->IsSelected())
			{
				pAoeSizeMenu->UpdateSlider();
			}
			if (pRuler)
			{
				pRuler->SetEndPoint(TranslatedCoordinates);
				pRuler->CalcDistance();
			}
			break;
		}
		case Mouse::Event::Type::LPress:
			UpdateLog(L"Mouse::Event::LPress", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			if (pSelectedLayer && !pSideMenu->IsInRealRect() && !pSizeMenu->IsInRealRect() && pSideMenu->IsBuildMode())
			{
				if (!pSideMenu->WallSelected())
				{
					if (pSideMenu->IsAttachObject())
					{
						for (auto& sprite : *pSelectedLayer)
						{
							if (sprite->PointInSprite(TranslatedCoordinates))
							{
								UpdateLog(L"Attach Selected to Sprite", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
								if (pSideMenu->AoeSelected())
								{
									//for aoe sprites
									D2D1_COLOR_F color_ = pSideMenu->GetSelectedAoeColor();
									color_.a = pAoeSizeMenu->GetOpacity();
									AoeSpritePointer* p = new AoeSpritePointer(gfx, pSideMenu->GetSelectedAoeType(), D2D1::ColorF(0, 0, 0), color_, nullptr, Location(), &GridSquareSize);
									
									p->SetLength(static_cast<int>(pAoeSizeMenu->GetLength()));
									p->SetRadius(static_cast<int>(pAoeSizeMenu->GetRadius()));
									p->SetThickness(3.0f);
									p->SetWidth(static_cast<int>(pAoeSizeMenu->GetWidth()));
									p->BuildShape();
									p->SetStartPoint(D2D1::Point2F((sprite->GetDestSprite().right + sprite->GetDestSprite().left) * 0.5f, (sprite->GetDestSprite().bottom + sprite->GetDestSprite().top) * 0.5f));
									sprite->AddAoeSpriteChild(p);
									pLastAttachedSpritePointer = p;
									bUpdateRotation = true;
								}
								else if (sptest) sprite->AddSpriteChild(sptest->GetPieces());
								break;
							}
						}
					}
					else if (sptest)
					{
						UpdateLog(L"Update pSelectedLayer with sptest", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
						pSelectedLayer->push_back(new SpritePointer(gfx, sptest->GetPieces(), Location(), &GridSquareSize));
						pSelectedLayer->back()->SetCreatureSize(sptest->GetCreatureSize());
						bKeepAspect ? pSelectedLayer->back()->SetKeepAspectSprite() : pSelectedLayer->back()->UnsetKeepAspectSprite();

						if (bLockToGrid)
							pSelectedLayer->back()->SetDestSprite(mPreviewDest);
						else
							pSelectedLayer->back()->SetDestSprite(mPreviewDest);
						//pSelectedLayer->back()->SetDestSprite(D2D1::RectF(TranslatedCoordinates.x, TranslatedCoordinates.y, TranslatedCoordinates.x + sptest->GetDestResizedSpriteSize().width, TranslatedCoordinates.y + sptest->GetDestResizedSpriteSize().height));
					}
				}
				else if (wptest && !pSideMenu->AoeSelected())
				{
					sptest ? wptest->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f)) : wptest->SetColor(pSideMenu->GetSelectedWallColor());
					wptest->AddPoint(bLockToGrid ? mPreviewPoint : TranslatedCoordinates);
				}
				if (pSideMenu->AoeSelected() && !pSideMenu->IsAttachObject())
				{
					UpdateLog(L"pSideMenu->AoeSelected() !IsAttachObject()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
					D2D1_COLOR_F fillcolor = pSideMenu->GetSelectedAoeColor();
					fillcolor.a = pAoeSizeMenu->GetOpacity();
					AoeSpritePointer* tSprite = new AoeSpritePointer(gfx, pSideMenu->GetSelectedAoeType(), D2D1::ColorF(0.0f, 0.0f, 0.0f), fillcolor, nullptr, Location(), &GridSquareSize);
					tSprite->SetLength(static_cast<int>(pAoeSizeMenu->GetLength()));
					tSprite->SetRadius(static_cast<int>(pAoeSizeMenu->GetRadius()));
					tSprite->SetThickness(3.0f);
					tSprite->SetWidth(static_cast<int>(pAoeSizeMenu->GetWidth()));
					tSprite->BuildShape();
					tSprite->SetStartPoint(bLockToGrid ? mPreviewPoint : TranslatedCoordinates);
					pSelectedLayer->push_back(tSprite);
					bUpdateRotation = true;
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
							UpdateLog(L"MeasuremenuMenu::SizeMenuType::ThicknessSize", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
							pThicknessMenu->SetSelected();
						}
						else if (pThicknessMenu->PointOnSlideLine())
						{
							UpdateLog(L"MeasuremenuMenu::SizeMenuType::ThicknessSize", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
							pThicknessMenu->JumpPosition();
							pThicknessMenu->SetPreviewRadius(pThicknessMenu->CalcRadius());
							wptest->SetThickness(pThicknessMenu->GetSelectedThickness());
						}
					}
				}
				else if (mSizeMenuType == MeasurementMenu::SizeMenuType::AoeSize)
				{
					if (!pAoeSizeMenu->IsHidden())
					{
						UpdateLog(L"MeasuremenuMenu::SizeMenuType::AoeSize", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
						if (pAoeSizeMenu->PointInOpacitySlider())
						{
							pAoeSizeMenu->SetOpacitySelected();
						}
						else if (pAoeSizeMenu->PointOnOpacitySlideLine())
						{
							//jumpposition needs to be tweaked to support multiples before it can do this
						}
						if (pAoeSizeMenu->PointInLengthSlider())
						{
							pAoeSizeMenu->SetLengthSelected();
						}
						else if (pAoeSizeMenu->PointOnLengthSlideLine())
						{

						}
						if (pAoeSizeMenu->PointInWidthSlider())
						{
							pAoeSizeMenu->SetWidthSelected();
						}
						else if (pAoeSizeMenu->PointOnWidthSlideLine())
						{

						}
						 if (pAoeSizeMenu->PointInRadiusSlider())
						{
							pAoeSizeMenu->SetRadiusSelected();
						}
						else if (pAoeSizeMenu->PointOnRadiusSlideLine())
						{

						}
					}
				}
			}
			else if (!pSideMenu->IsBuildMode())
			{
				bool move = true;
				if (pSelectedLayer)
				{
					for (auto& c : *pSelectedLayer)
					{
						if (c->PointInSprite(TranslatedCoordinates))
						{
							if (!pSelectedObject)
							{
								UpdateLog(L"pSelectedObject = c", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
								pSelectedObject = c;
								if (!pSelectedObject->IsAoe())
								if (!pRuler) pRuler = new Ruler(gfx, GridSquareSize, D2D1::Point2F(pSelectedObject->GetDestSprite().left + (pSelectedObject->GetDestSprite().right - pSelectedObject->GetDestSprite().left) * 0.5f,
									pSelectedObject->GetDestSprite().top + (pSelectedObject->GetDestSprite().bottom - pSelectedObject->GetDestSprite().top) * 0.5f), mRulerDest);
								move = false;
								mPreviewDest = GetPreviewRect(pSelectedObject, TranslatedCoordinates);
							}							
						}
					}
					if (pSelectedObject && move)
					{
						UpdateLog(L"pSelectedObject && move", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
						pSelectedObject->SetDestSprite(GetPreviewRect(pSelectedObject, TranslatedCoordinates));
						pSelectedObject->UpdateRotationMatrix();
						for (auto& child : pSelectedObject->vSpriteChild)
						{
							if (child)
							{
								child->SetDestSprite(pSelectedObject->GetDestSprite());								
							}
						}
						pSelectedObject = nullptr;
						SafeDelete(&pRuler);
					}
				}
			}
			break;
		case Mouse::Event::Type::LRelease:
		{
			UpdateLog(L"Mouse::Event::LRelease", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			if (bUpdateRotation)
			{
				bUpdateRotation = false;
				pLastAttachedSpritePointer = nullptr;
			}
			if (pTurnCounter) if (pTurnCounter->PointInEllipse(*pMouseCoordinate)) pTurnCounter->Interact();
			if (mSizeMenuType == MeasurementMenu::SizeMenuType::CreatureSize && pSizeMenu->Interact())
			{
				UpdateLog(L"pSideMenu->Interact() [true]", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
				if (sptest) sptest->SetCreatureSize(pSizeMenu->GetSelectedCreatureSize());
			}
			else if (mSizeMenuType == MeasurementMenu::SizeMenuType::ThicknessSize && pThicknessMenu->Interact())
			{
				UpdateLog(L"pThicknessMenu->Interact() [true]", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
				if (pThicknessMenu->IsSelected())
				{
					pThicknessMenu->UnsetSelected();					
				}
			}
			else if (mSizeMenuType == MeasurementMenu::SizeMenuType::AoeSize && pAoeSizeMenu->Interact())
			{
				UpdateLog(L"pAoeSizeMenu->Interact() [true]", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
				if (pAoeSizeMenu->OpacitySelected())
				{
					pAoeSizeMenu->UnsetOpacitySelected();
				}
				if (pAoeSizeMenu->LengthSelected())
				{
					pAoeSizeMenu->UnsetLengthSelected();
				}
				if (pAoeSizeMenu->WidthSelected())
				{
					pAoeSizeMenu->UnsetWidthSelected();
				}
				if (pAoeSizeMenu->RadiusSelected())
				{
					pAoeSizeMenu->UnsetRadiusSelected();
				}
			}
			else 
			{
				for (auto& io : IObjects)
				{
					if (io->PointInRect())
					{
						io->Interact();
						UpdateLog(std::wstring(io->GetLabel()) + L"->Interact()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
					}
				}
				if (!pSideMenu->IsBuildMode())
				{
					if (pSideMenu->InitiativeList)
					{
						pSideMenu->InitiativeList->Interact();
						int selectedpos = -1;
						for (size_t i = 0; i < pSideMenu->InitiativeList->pChild.size(); i++)
						{
							if (pSideMenu->InitiativeList->pChild.at(i)->IsSelected())
							{
								if (selectedpos < 0)
								{
									selectedpos = static_cast<int>(i);
								}
								else
								{
									std::iter_swap(pSideMenu->vInitativeList.begin() + i, pSideMenu->vInitativeList.begin() + selectedpos);
									pSideMenu->BuildInitativeList();
									break;
								}
							}
						}
					}
				}
				if (sptest) pSizeMenu->SetSelectedCreatureSize(sptest->GetCreatureSize());
				if (pSideMenu) mSizeMenuType = pSideMenu->GetSizeMenuType();
				if (wptest) bUseTexture ? wptest->SetUseTexture() : wptest->UnsetUseTexture();
			}
		}
		break;
		case Mouse::Event::Type::RPress:
			UpdateLog(L"Mouse::Event::RPress", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			if (pSelectedObject) { pSelectedObject = nullptr; SafeDelete(&pRuler); }
			else if (pSideMenu->WallSelected()) if (wptest) wptest->RemoveLastPoint();
			break;
		case Mouse::Event::Type::RRelease:
		{
			UpdateLog(L"Mouse::Event::RRelease", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			if (pSideMenu->IsInRealRect() && pSideMenu->IsBuildMode())
			{
				uint32_t uInitativeListSize = pSideMenu->GetInitativeListSize();
				for (auto& io : IObjects)
				{
					if (io->PointInRect())
					{
						io->AlternateInteract();
						break;
					}
				}
				if (uInitativeListSize != pSideMenu->GetInitativeListSize())
				{
					pSideMenu->BuildInitativeList();
				}
			}
			if (pSideMenu->IsInRealRect() && !pSideMenu->IsBuildMode())
			{
				if (pSideMenu->InitiativeList)
				{
					for (size_t i = 0; i < pSideMenu->InitiativeList->pChild.size(); i++)
					{
						if (pSideMenu->InitiativeList->pChild.at(i)->PointInRect())
						{
							pSideMenu->vInitativeList.erase(pSideMenu->vInitativeList.begin() + i);
							pSideMenu->vInitativeList.shrink_to_fit();
							pSideMenu->BuildInitativeList();
							break;
						}
					}
				}
			}
			break;
		}
		case Mouse::Event::Type::X1Press:
			UpdateLog(L"Mouse::Event::X1Press", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			if (!pSideMenu->PointInRect(*pMouseCoordinate) || pSideMenu->IsHidden())
				PushMouseCoordinate = e.GetPos();
			break;
		case Mouse::Event::Type::X1Release:
			UpdateLog(L"Mouse::Event::X1Release", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			PushMouseCoordinate = D2D1::Point2F();
			break;
		case Mouse::Event::Type::MPress:
			UpdateLog(L"Mouse::Event::MPress", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			if (!pSideMenu->IsInRealRect())
			{
				if (pSelectedLayer)
				{
					for (auto& sprite : *pSelectedLayer)
					{
						if (sprite->PointInSprite(TranslatedCoordinates))
						{
							sprite->IncreaseCreatureSize();
							break;
						}
					}
				}
			}
			break;
		case Mouse::Event::Type::MRelease:
			UpdateLog(L"Mouse::Event::MRelease", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			if (pSideMenu->IsInRealRect() && !pSideMenu->IsBuildMode())
			{
				int selectedid = -1;
				PiecesW* tempP = nullptr;
				if (pSideMenu->InitiativeList)
				{
					for (size_t i = 0; i < pSideMenu->InitiativeList->pChild.size(); i++)
					{
						if (pSideMenu->InitiativeList->pChild.at(i)->IsSelected())
						{
							selectedid = static_cast<int>(i);
							tempP = pSideMenu->vInitativeList.at(i);
							pSideMenu->vInitativeList.erase(pSideMenu->vInitativeList.begin() + i);
							break;
						}
					}
				}
				if (selectedid >= 0)
				{
					for (size_t i = 0; i < pSideMenu->InitiativeList->pChild.size(); i++)
					{
						if (i == selectedid) continue;
						if (pSideMenu->InitiativeList->pChild.at(i)->PointInRect())
						{
							if (tempP)
							{
								pSideMenu->vInitativeList.insert(pSideMenu->vInitativeList.begin() + i, tempP);
								pSideMenu->BuildInitativeList();
							}
						}
					}
				}
			}
			break;
		case Mouse::Event::Type::WheelUp:
		{
			UpdateLog(L"Mouse::Event::WheelUp", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
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
				if (pSideMenu->IsBuildMode())
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
				else
				{
					pSideMenu->InitiativeList->WheelUp();
				}
			}
			else
			{
				if (pMouse->X1Pressed())
				{
					D2D1_SIZE_F temp = gfx->GetCompatibleTargetSize();
					temp.height += GridSquareSize.height;
					temp.width += GridSquareSize.width;
					gfx->ResizeCompatibleRenderTarget(temp);
				}
				else
				{
					Scale.height += ScaleSpeed.height;
					Scale.width += ScaleSpeed.width;
				}
			}
		}
			break;
		case Mouse::Event::Type::WheelDown:
		{
			UpdateLog(L"Mouse::Event::WheelDown", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
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
				if (pSideMenu->IsBuildMode())
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
				else
				{
					pSideMenu->InitiativeList->WheelDown();
				}
			}
			else
			{
				if (pMouse->X1Pressed())
				{
					D2D1_SIZE_F temp = gfx->GetCompatibleTargetSize();
					temp.height -= GridSquareSize.height;
					temp.width -= GridSquareSize.width;
					gfx->ResizeCompatibleRenderTarget(temp);
				}
				else
				{
					Scale.height -= ScaleSpeed.height;
					Scale.width -= ScaleSpeed.width;
					if (Scale.height <= 0) Scale.height = ScaleSpeed.height;
					if (Scale.width <= 0) Scale.width = ScaleSpeed.width;
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
			UpdateLog(L"gfx->ResizeCompatibleRenderTarget()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			D2D1_SIZE_F temp = gfx->GetCompatibleTargetSize();
			temp.height += GridSquareSize.height;
			temp.width += GridSquareSize.width;
			gfx->ResizeCompatibleRenderTarget(temp);
			break;
		}
		case L'-':
		{
			UpdateLog(L"gfx->ResizeCompatibleRenderTarget()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			D2D1_SIZE_F temp = gfx->GetCompatibleTargetSize();
			temp.height -= GridSquareSize.height;
			temp.width -= GridSquareSize.width;
			gfx->ResizeCompatibleRenderTarget(temp);
			break;
		}
		case L' ':
		{
			if (!pSideMenu->IsBuildMode())
			{
				UpdateLog(L"NextTurn()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
				pSideMenu->NextTurn();
			}
			break;
		}		
		case VK_BACK:
		{
			if (!pSideMenu->IsBuildMode())
			{
				UpdateLog(L"PreviousTurn()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
				pSideMenu->PreviousTurn();
			}
			break;
		}
		}
	}
	if (!pKeyboard->KeyIsEmpty())
	{
		keyEvents = pKeyboard->ReadKey();
		if (keyEvents.IsPress())
		{
			UpdateLog(L"keyEvents.IsPress() [" + std::to_wstring(keyEvents.GetCode()) + L"]", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			switch (keyEvents.GetCode())
			{			
			case VK_UP:
				Movement.bMove = true;
				Movement.vDirection.push_back(Move::Direction::Up);
				break;
			case VK_DOWN:
				Movement.bMove = true;
				Movement.vDirection.push_back(Move::Direction::Down);
				break;
			case VK_LEFT:
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
						wptest->SetTexture(sptest);
					}
					wptest->SetGeometry(close);
					if (pSelectedLayerWall)
					{
						pSelectedLayerWall->push_back(std::unique_ptr<Wall>(wptest.get()));
					}
					wptest.release();
					wptest = std::make_unique<Wall>(this, &GridSquareSize, gfx, &TranslatedCoordinates, pSideMenu->UseTexture());
					wptest->SetColor(D2D1::ColorF(1, 0, 0));
					wptest->SetThickness(pThicknessMenu->GetSelectedThickness());
				}
				break;
			case VK_SHIFT:
				SafeDelete(&pRuler);
				pRuler = new Ruler(gfx, GridSquareSize, TranslatedCoordinates, mRulerDest);
				break;
			case VK_NEXT:
				for (auto& child : *pSelectedLayer)
				{
					if (child->PointInSprite(TranslatedCoordinates))
					{
						child->IncreaseRotation(45.0f);
						child->UpdateRotationMatrix();
						break;
					}
				}
				break;
			case VK_PRIOR:
				for (auto& child : *pSelectedLayer)
				{
					if (child->PointInSprite(TranslatedCoordinates))
					{
						child->IncreaseRotation(-45.0f);
						child->UpdateRotationMatrix();
						break;
					}
				}
				break;
			case VK_F4:
				GridSquareSize.width--; if (GridSquareSize.width < 5.0f) GridSquareSize.width = 5.0f;
				GridSquareSize.height--; if (GridSquareSize.height < 5.0f) GridSquareSize.height = 5.0f;
				{
					for (auto& room : vSprites)
						for (auto& layer : room)
							for (auto& sprite : layer)
								if (sprite->IsAoe()) static_cast<AoeSpritePointer*>(sprite)->BuildShape();
				}
				break;
			case VK_F5:
				GridSquareSize.width++;
				GridSquareSize.height++;
				{
					for (auto& room : vSprites)
						for (auto& layer : room)
							for (auto& sprite : layer)
								if (sprite->IsAoe()) static_cast<AoeSpritePointer*>(sprite)->BuildShape();
				}
				break;
			case 'F':
				if (pKeyboard->KeyIsPressed(VK_CONTROL)) bShowFPS ^= true;
				break;
			default:
			{
				if ((keyEvents.GetCode() >= L'0' && keyEvents.GetCode() <= L'9') || (keyEvents.GetCode() >= 'A' && keyEvents.GetCode() <= 'Z'))
				{
					for (auto& sprite : *pSelectedLayer)
					{
						if (sprite->PointInSprite(TranslatedCoordinates)) sprite->AddTag(static_cast<wchar_t>(keyEvents.GetCode()));
					}
				}
			}
			}
		}
		if (keyEvents.IsRelease())
		{
			UpdateLog(L"keyEvents.IsRelease() [" + std::to_wstring(keyEvents.GetCode()) + L"]", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
			std::queue<Move::Direction> direction;
			switch (keyEvents.GetCode())
			{
			case VK_UP:
				direction.push(Move::Direction::Up);
				break;
			case VK_DOWN:
				direction.push(Move::Direction::Down);
				break;
			case VK_LEFT:
				direction.push(Move::Direction::Left);
				direction.push(Move::Direction::RotateNegative);
				break;
			case VK_RIGHT:
				direction.push(Move::Direction::Right);
				direction.push(Move::Direction::RotatePositive);
				break;
			case VK_SHIFT:
				UpdateLog(L"SafeDelete(pRuler)", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
				SafeDelete(&pRuler);
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

void BaseLevel::OutputImageLoadingStatus(uint32_t numloaded, uint32_t total, const std::wstring imagetype)
{
	std::wstring msg = L"Loading " + imagetype + L" " + std::to_wstring(numloaded + 1) + L" of " + std::to_wstring(total);
	gfx->BeginDraw(gfx->GetRenderTarget());
	gfx->ClearScreen(gfx->GetRenderTarget(), D2D1::ColorF(0.75f, 0.75f, 0.75f));
	gfx->OutputText(gfx->GetRenderTarget(), msg.c_str(), m_ClientWindow, D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	gfx->EndDraw(gfx->GetRenderTarget());
}

void BaseLevel::OutputImageLoadingStatusM(std::atomic<uint32_t>& numloaded, uint32_t total, const std::wstring imagetype)
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
	UpdateLog(L"LoadSprites()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	uint32_t totalnumbersprites = static_cast<uint32_t>(vPieces.size() + vPiecesW.size());
	std::atomic<uint32_t> numberfinished = 0;
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
	UpdateLog(L"LoadPortraits()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	uint32_t totalnumbersprites = static_cast<uint32_t>(vPieces.size() + vPiecesW.size());
	uint32_t spritesloaded = 0;
	std::atomic<uint32_t> numberfinished = 0;
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

const bool BaseLevel::BuildObjects(const wchar_t* sFilePath)
{
	UpdateLog(L"BuildObjects(" + std::wstring(sFilePath) + L")", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	size_t BufferSize = 0;
	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, sFilePath, L"rb");
	if (err)
	{
		std::wstring errmsg = L"Failed to open ";
		errmsg.append(sFilePath);
		std::wstring errtopic = L"Error #" + std::to_wstring(static_cast<int>(err));
		MessageBoxW(nullptr, errmsg.c_str(), errtopic.c_str(), MB_OK | MB_ICONERROR);
		return false;
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
	return true;
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
	UpdateLog(L"CreateRoom()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
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
	UpdateLog(L"CreateLayer(" + std::to_wstring(uRoomNumber) + L")", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	vSprites[uRoomNumber].push_back(std::vector<SpritePointer*>());
	vWalls[uRoomNumber].push_back(std::vector<std::unique_ptr<Wall>>());
	vVisibleLayers[uRoomNumber].push_back(true);
	if (pSideMenu)
	{
		pSideMenu->CreateLayer(uRoomNumber);
		pSideMenu->CreateLayerButton(&Transforms, &m_ClientWindow, uRoomNumber);
	}
}

const D2D1_RECT_F BaseLevel::GetPreviewRect(SpritePointer* const pSpritePointer, const D2D1_POINT_2F p)
{
	D2D1_SIZE_F size;
	float sizemod = 1.0f;
	switch (pSpritePointer->GetCreatureSize())
	{
	case CreatureSize::Diminutive:
	case CreatureSize::Fine:
	case CreatureSize::Tiny:
	case CreatureSize::Small:
	{
		sizemod = static_cast<float>(1.0f / pow(2, static_cast<unsigned long>(CreatureSize::Medium) - static_cast<unsigned long>(pSpritePointer->GetCreatureSize())));
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
		sizemod = static_cast<float>(1 + static_cast<uint32_t>(pSpritePointer->GetCreatureSize()) - static_cast<uint32_t>(CreatureSize::Medium));
		size = D2D1::SizeF(GridSquareSize.width * sizemod, GridSquareSize.height * sizemod);
		break;
	case CreatureSize::Invalid:
		size = D2D1::SizeF(pSpritePointer->GetDestSprite().right - pSpritePointer->GetDestSprite().left, pSpritePointer->GetDestSprite().bottom - pSpritePointer->GetDestSprite().top);
		break;
	default:
		sizemod = static_cast<float>(pSpritePointer->GetCreatureSize()) - static_cast<float>(CreatureSize::Colossal);
		size = D2D1::SizeF(pSpritePointer->GetSpriteFrameSize().width * sizemod, pSpritePointer->GetSpriteFrameSize().height * sizemod);
	}
	if (bLockToGrid)
	{
		D2D1_POINT_2F lockedPoint;
		if (static_cast<int>(pSpritePointer->GetCreatureSize()) < static_cast<int>(CreatureSize::Medium))
		{
			lockedPoint.x = static_cast<float>(static_cast<uint32_t>(p.x) / static_cast<uint32_t>(GridSquareSize.width * sizemod));
			lockedPoint.y = static_cast<float>(static_cast<uint32_t>(p.y) / static_cast<uint32_t>(GridSquareSize.height * sizemod));
			lockedPoint.x *= GridSquareSize.width * sizemod;
			lockedPoint.y *= GridSquareSize.height * sizemod;
		}
		else
		{
			lockedPoint.x = static_cast<float>(static_cast<uint32_t>(p.x) / static_cast<uint32_t>(GridSquareSize.width));
			lockedPoint.y = static_cast<float>(static_cast<uint32_t>(p.y) / static_cast<uint32_t>(GridSquareSize.height));
			lockedPoint.x *= GridSquareSize.width;
			lockedPoint.y *= GridSquareSize.height;
		}

		return D2D1::RectF(lockedPoint.x, lockedPoint.y, lockedPoint.x + size.width, lockedPoint.y + size.height);
	}
	return D2D1::RectF(p.x - size.width * 0.5f, p.y - size.height * 0.5f, p.x + size.width * 0.5f, p.y + size.height * 0.5f);
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

/*
const bool SaveMapSpriteList();
	void LoadMapSpriteList(const char* Buffer);*/

const bool BaseLevel::LoadMapSpriteList(ReceiveData& rd)
{
	UpdateLog(L"LoadMapSpriteList", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	vSprites = std::vector< std::vector< std::vector<SpritePointer*>>>();
	vWalls = std::vector< std::vector< std::vector<std::unique_ptr<Wall>>>>();
	vVisibleRooms = std::vector<bool>();
	vVisibleLayers = std::vector< std::vector< bool>>();

	uint32_t uBufferSize = 0u;
	rd.GetData(uBufferSize);
	uint32_t uLen = 0u;
	rd.GetData(uLen);
	for (uint32_t i = 0u; i < uLen; i++)
	{
		CreateRoom();
		uint32_t uLayers = 0u;
		rd.GetData(uLayers);
		for (uint32_t j = 0u; j < uLayers; j++)
		{
			if (j) CreateLayer(i);
			uint32_t uSpritePointers = 0u;
			rd.GetData(uSpritePointers);
			for (uint32_t w = 0u; w < uSpritePointers; w++)
			{
				uint32_t uSpriteSize = 0u;
				uint32_t uSpriteType = 0u;
				rd.GetData(uSpriteSize);
				rd.GetData(uSpriteType);
				rd.MoveCurrentPosition(-1 * static_cast<int32_t>((sizeof(uSpriteSize) + sizeof(uSpriteType))));
				if (uSpriteType == 0)
				{
					SpritePointer* p = new SpritePointer(gfx, nullptr, Location(), &GridSquareSize);
					p->LoadSaveBuffer(rd);
					p->SetPiecePointer(FindPiece(p->GetPieceBufferV()));
					for (auto c : p->vSpriteChild)
					{
						c->SetPiecePointer(FindPiece(c->GetPieceBufferV()));
					}
					for (auto c : p->vPortraitChild)
					{
						c->SetPiecePointer(FindPiece(c->GetPieceBufferV()));
					}
					vSprites.back().back().push_back(p);
				}
				else if (uSpriteType == 1)
				{
					SpritePointer* p = new AoeSpritePointer(gfx, AoeSpritePointer::AoeTypes::Invalid, D2D1::ColorF(0, 0, 0), D2D1::ColorF(0, 0, 0), nullptr, Location(), &GridSquareSize);
					p->LoadSaveBuffer(rd);
					for (auto c : p->vSpriteChild)
					{
						c->SetPiecePointer(FindPiece(c->GetPieceBufferV()));
					}
					for (auto c : p->vPortraitChild)
					{
						c->SetPiecePointer(FindPiece(c->GetPieceBufferV()));
					}
					vSprites.back().back().push_back(p);
				}
			}
		}
	}
	return true;
}

const bool BaseLevel::LoadMapSpriteList(const char* Buffer)
{
	UpdateLog(L"LoadMapSpriteList", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	if (!Buffer) return false;
	vSprites = std::vector< std::vector< std::vector<SpritePointer*>>>();
	vWalls = std::vector< std::vector< std::vector<std::unique_ptr<Wall>>>>();
	vVisibleRooms = std::vector<bool>();
	vVisibleLayers = std::vector< std::vector< bool>>();
	
	uint32_t uBufferSize = 0;
	size_t pos = 0;
	memcpy(&uBufferSize, Buffer, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);

	uint32_t uLen = 0;
	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	pos += sizeof(uLen);
	for (uint32_t i = 0; i < uLen; i++)
	{
		//vSprites.push_back(std::vector< std::vector<SpritePointer*>>());
		CreateRoom();
		uint32_t uLayers = 0;
		memcpy(&uLayers, Buffer + pos, sizeof(uLayers));
		pos += sizeof(uLayers);
		for (uint32_t j = 0; j < uLayers; j++)
		{
			//vSprites.at(i).push_back(std::vector<SpritePointer*>());
			if (j) CreateLayer(i);
			uint32_t uSpritePointers = 0;
			memcpy(&uSpritePointers, Buffer + pos, sizeof(uSpritePointers));
			pos += sizeof(uSpritePointers);
			for (uint32_t w = 0; w < uSpritePointers; w++)
			{
				uint32_t uSpriteSize = 0;
				memcpy(&uSpriteSize, Buffer + pos, sizeof(uSpriteSize));
				uint32_t spritetype = 0;
				memcpy(&spritetype, Buffer + pos + sizeof(uSpriteSize), sizeof(spritetype));
				if (spritetype == 0)
				{
					SpritePointer* p = new SpritePointer(gfx, nullptr, Location(), &GridSquareSize);
					p->LoadSaveBuffer(Buffer + pos);
					p->SetPiecePointer(FindPiece(p->GetPieceBuffer()));
					for (auto c : p->vSpriteChild)
					{
						c->SetPiecePointer(FindPiece(c->GetPieceBuffer()));
					}
					for (auto c : p->vPortraitChild)
					{
						c->SetPiecePointer(FindPiece(c->GetPieceBuffer()));
					}
					pos += uSpriteSize;
					vSprites.back().back().push_back(p);
				}
				else if (spritetype == 1)
				{
					SpritePointer* p = new AoeSpritePointer(gfx, AoeSpritePointer::AoeTypes::Invalid, D2D1::ColorF(0,0,0), D2D1::ColorF(0,0,0), nullptr, Location(), &GridSquareSize);
					p->LoadSaveBuffer(Buffer + pos);
					for (auto c : p->vSpriteChild)
					{
						c->SetPiecePointer(FindPiece(c->GetPieceBuffer()));
					}
					for (auto c : p->vPortraitChild)
					{
						c->SetPiecePointer(FindPiece(c->GetPieceBuffer()));
					}
					pos += uSpriteSize;
					vSprites.back().back().push_back(p);
				}
			}
		}
	}
	return true;
}

PiecesW* const BaseLevel::FindPiece(ReceiveData& rd)
{
	PiecesW temp(gfx, pTimer, &GridSquareSize);
	temp.LoadSaveBuffer(rd);

	for (auto& t : vPiecesW)
	{
		if (!_wcsicmp(t.GetName().c_str(), temp.GetName().c_str()))
		{
			if (!_wcsicmp(t.GetType().c_str(), temp.GetType().c_str()))
			{
				if (!_wcsicmp(t.GetSubMenu().c_str(), temp.GetSubMenu().c_str()))
				{
					return &t;
				}
			}
		}
	}
	vPiecesW.push_back(temp);
	return &vPiecesW.back();
}

PiecesW* const BaseLevel::FindPiece(const std::vector<char>& vBuffer)
{
	if (vBuffer.empty()) return nullptr;
	PiecesW temp(gfx, pTimer, &GridSquareSize);
	ReceiveData rd;
	rd.SetVBuffer(vBuffer);
	temp.LoadSaveBuffer(rd);

	for (auto& t : vPiecesW)
	{
		if (!_wcsicmp(t.GetName().c_str(), temp.GetName().c_str()))
		{
			if (!_wcsicmp(t.GetType().c_str(), temp.GetType().c_str()))
			{
				if (!_wcsicmp(t.GetSubMenu().c_str(), temp.GetSubMenu().c_str()))
				{
					return &t;
				}
			}
		}
	}

	vPiecesW.push_back(temp);
	return &vPiecesW.back();
}

PiecesW* const BaseLevel::FindPiece(const char* Buffer)
{
	if (!Buffer) return nullptr;
	PiecesW temp(gfx, pTimer, &GridSquareSize);
	temp.LoadSaveBuffer(Buffer);

	for (auto& t : vPiecesW)
	{
		if (!_wcsicmp(t.GetName().c_str(), temp.GetName().c_str()))
		{
			if (!_wcsicmp(t.GetType().c_str(), temp.GetType().c_str()))
			{
				if (!_wcsicmp(t.GetSubMenu().c_str(), temp.GetSubMenu().c_str()))
				{
					return &t;
				}
			}
		}
	}
	//if it was not found, the PiecesW does not exists, therefore: create it
	vPiecesW.push_back(temp);
	return &vPiecesW.back();
}

const std::vector<char> BaseLevel::BuildMapSpriteListSaveBufferV()
{
	UpdateLog(L"BuildMapSpriteListSaveBufferV()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	StoreData sd;
	uint32_t uLen = static_cast<uint32_t>(vSprites.size());
	sd.AddEntry(uLen);
	for (auto& room : vSprites)
	{
		uLen = static_cast<uint32_t>(room.size());
		sd.AddEntry(uLen);
		for (auto& layer : room)
		{
			uLen = static_cast<uint32_t>(layer.size());
			sd.AddEntry(uLen);
			for (auto& sp : layer)
			{
				std::vector<char> Vtemp = sp->GetSaveInformationV();
				sd.CombineBuffer(Vtemp);
			}
		}
	}

	sd.UpdateBufferSize();
	return sd.GetBuffer();
}

const char* BaseLevel::BuildMapSpriteListSaveBuffer()
{
	UpdateLog(L"BuildMapSpriteListSaveBuffer()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	uint32_t uBufferSize = CalcMapSpriteListBufferSize();
	char* Buffer = new char[uBufferSize];
	size_t pos = 0;

	memcpy(Buffer, &uBufferSize, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	uint32_t uLen = static_cast<uint32_t>(vSprites.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);
	for (auto& room : vSprites)
	{
		uLen = static_cast<uint32_t>(room.size());
		memcpy(Buffer + pos, &uLen, sizeof(uLen));
		pos += sizeof(uLen);
		for (auto& layer : room)
		{
			uLen = static_cast<uint32_t>(layer.size());
			memcpy(Buffer + pos, &uLen, sizeof(uLen));
			pos += sizeof(uLen);
			for (auto& sp : layer)
			{
				uLen = sp->CalcSaveSize();
				const char* tbuff = sp->GetSaveInformation();
				memcpy(Buffer + pos, tbuff, uLen);
				pos += uLen;
				SafeDeleteArray(&tbuff);
			}
		}
	}
	return Buffer;
}

const uint32_t BaseLevel::CalcMapSpriteListBufferSize()
{
	uint32_t uBufferSize = 0;
	uBufferSize += sizeof(uint32_t);							//store buffer size
	uBufferSize += sizeof(uint32_t);							//store number rooms
	for (auto& room : vSprites)
	{
		uBufferSize += sizeof(uint32_t);						//store number layer for each room
		for (auto& layer : room)
		{
			uBufferSize += sizeof(uint32_t);					//store number spritepointers for each layer
			for (auto& sp : layer)
			{
				uBufferSize += sp->CalcSaveSize();					//store size of spritepointer
			}
		}
	}
	return uBufferSize;
}

const uint32_t BaseLevel::CalcMapWallListBufferSize()
{
	uint32_t uBufferSize = 0;
	uBufferSize += sizeof(uint32_t);							//store buffer size
	uBufferSize += sizeof(uint32_t);							//store number rooms
	for (auto& room : vWalls)
	{
		uBufferSize += sizeof(uint32_t);						//store number layer for each room
		for (auto& layer : room)
		{
			uBufferSize += sizeof(uint32_t);					//store number walls for each layer
			for (auto& wall : layer)
			{
				uBufferSize += wall->CalcSaveSize();				//store size of wall
			}
		}
	}
	return uBufferSize;
}

const std::vector<char> BaseLevel::BuildMapWallListSaveBufferV()
{
	UpdateLog(L"BuildMapWallListSaveBufferV()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	StoreData sd;
	uint32_t uNumRooms = static_cast<uint32_t>(vWalls.size());
	sd.AddEntry(uNumRooms);

	for (auto& room : vWalls)
	{
		uint32_t uNumLayers = static_cast<uint32_t>(room.size());
		sd.AddEntry(uNumLayers);
		for (auto& layer : room)
		{
			uint32_t uNumWalls = static_cast<uint32_t>(layer.size());
			sd.AddEntry(uNumWalls);
			for (auto& wall : layer)
			{
				std::vector<char> Vtemp = wall->GetSaveInformationV();
				sd.CombineBuffer(Vtemp);
			}
		}
	}
	sd.UpdateBufferSize();
	return sd.GetBuffer();
}

const char* BaseLevel::BuildMapWallListSaveBuffer()
{
	UpdateLog(L"BuildMapWallListSaveBuffer()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	uint32_t uBufferSize = CalcMapWallListBufferSize();
	char* Buffer = new char[uBufferSize];
	size_t pos = 0;
	memcpy(Buffer + pos, &uBufferSize, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	uint32_t uNumRooms = static_cast<uint32_t>(vWalls.size());
	memcpy(Buffer + pos, &uNumRooms, sizeof(uNumRooms));
	pos += sizeof(uNumRooms);

	for (auto& room : vWalls)
	{
		uint32_t uNumLayers = static_cast<uint32_t>(room.size());
		memcpy(Buffer + pos, &uNumLayers, sizeof(uNumLayers));
		pos += sizeof(uNumLayers);
		for (auto& layer : room)
		{
			uint32_t uNumWalls = static_cast<uint32_t>(layer.size());
			memcpy(Buffer + pos, &uNumWalls, sizeof(uNumWalls));
			pos += sizeof(uNumWalls);
			for (auto& wall : layer)
			{
				uint32_t wallsize = wall->CalcSaveSize();
				const char* tbuff = wall->GetSaveInformation();
				memcpy(Buffer + pos, tbuff, wallsize);
				pos += wallsize;
				SafeDeleteArray(&tbuff);
			}
		}
	}
	return Buffer;
}

const bool BaseLevel::LoadMapWallList(ReceiveData& rd)
{
	UpdateLog(L"LoadMapWallList()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	uint32_t uBufferSize = 0;
	rd.GetData(uBufferSize);
	uint32_t uNumRooms = 0;
	rd.GetData(uNumRooms);

	for (uint32_t i = 0; i < uNumRooms; i++)
	{
		uint32_t uNumLayers = 0;
		rd.GetData(uNumLayers);
		for (uint32_t j = 0; j < uNumLayers; j++)
		{
			uint32_t uNumWalls = 0;
			rd.GetData(uNumWalls);
			for (uint32_t w = 0; w < uNumWalls; w++)
			{
				std::unique_ptr<Wall> twall = std::make_unique<Wall>(this, &GridSquareSize, gfx, pMouseCoordinate, false);
				twall->LoadSaveBuffer(rd);
				vWalls[i][j].push_back(std::unique_ptr<Wall>(twall.get()));
				twall.release();
			}
		}
	}
	return true;
}

const bool BaseLevel::LoadMapWallList(const char* Buffer)
{
	UpdateLog(L"LoadMapWallList()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	if (!Buffer) return false;
	//this function assumes LoadMapSpriteList was already called. If not called inthe correct order there will be issues
	uint32_t uBufferSize = 0;
	size_t pos = 0;
	memcpy(&uBufferSize, Buffer, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	uint32_t uNumRooms = 0;
	memcpy(&uNumRooms, Buffer + pos, sizeof(uNumRooms));
	pos += sizeof(uNumRooms);

	for (uint32_t i = 0; i < uNumRooms; i++)
	{
		uint32_t uNumLayers = 0;
		memcpy(&uNumLayers, Buffer + pos, sizeof(uNumLayers));
		pos += sizeof(uNumLayers);
		for (uint32_t j = 0; j < uNumLayers; j++)
		{
			uint32_t uNumWalls = 0;
			memcpy(&uNumWalls, Buffer + pos, sizeof(uNumWalls));
			pos += sizeof(uNumWalls);
			for (uint32_t w = 0; w < uNumWalls; w++)
			{
				uint32_t wallsize = 0;
				memcpy(&wallsize, Buffer + pos, sizeof(wallsize));
				std::unique_ptr<Wall> twall = std::make_unique<Wall>(this, &GridSquareSize, gfx, pMouseCoordinate, false);
				twall->LoadSaveBuffer(Buffer + pos);
				pos += wallsize;
				vWalls[i][j].push_back(std::unique_ptr<Wall>(twall.get()));
				twall.release();
			}
		}
	}
	return true;
}

/*
const uint32_t CalcInitativeSaveBufferSize();
	const char* BuildInitativeListSaveBuffer();
	const char* GetInitativeListSaveBuffer();
*/

const uint32_t BaseLevel::CalcInitativeSaveBufferSize()
{
	uint32_t uBufferSize = 0;
	uBufferSize += sizeof(uint32_t);		//store size
	uBufferSize += sizeof(uint32_t);		//store number of entries
	for (auto& entry : pSideMenu->vInitativeList)
	{
		uBufferSize += entry->CalcSaveSize();	//store entry data
	}
	return uBufferSize;
}

const std::vector<char> BaseLevel::BuildInitiativeListSaveBufferV()
{
	UpdateLog(L"BuildInitiativeListSaveBufferV", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	StoreData sd;

	uint32_t uNumEntries = static_cast<uint32_t>(pSideMenu->vInitativeList.size());
	sd.AddEntry(uNumEntries);
	for (auto& entry : pSideMenu->vInitativeList)
	{
		std::vector<char> Vtemp = entry->GetSaveBufferV();
		sd.CombineBuffer(Vtemp);
	}

	sd.UpdateBufferSize();
	return sd.GetBuffer();
}

const char* BaseLevel::BuildInitiativeListSaveBuffer()
{
	UpdateLog(L"BuildInitiativeListSaveBuffer", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	uint32_t uBufferSize = CalcInitativeSaveBufferSize();
	size_t pos = 0;
	char* Buffer = new char[uBufferSize];
	memcpy(Buffer, &uBufferSize, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	uint32_t uNumEntries = static_cast<uint32_t>(pSideMenu->vInitativeList.size());
	memcpy(Buffer + pos, &uNumEntries, sizeof(uNumEntries));
	pos += sizeof(uNumEntries);
	for (auto& entry : pSideMenu->vInitativeList)
	{
		uint32_t uSize = entry->CalcSaveSize();
		const char* tbuff = entry->GetSaveBuffer();
		memcpy(Buffer + pos, tbuff, uSize);
		pos += uSize;
		SafeDeleteArray(&tbuff);
	}
	return Buffer;
}

const bool BaseLevel::LoadInitiativeSaveBuffer(ReceiveData& rd)
{
	UpdateLog(L"LoadInitiativeSaveBuffer()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	pSideMenu->vInitativeList = std::vector<PiecesW*>();

	uint32_t uBufferSize = 0;
	rd.GetData(uBufferSize);
	uint32_t uNumEntries = 0;
	rd.GetData(uNumEntries);
	for (uint32_t i = 0; i < uNumEntries; i++)
	{
		uint32_t uSize = 0;
		rd.GetData(uSize, false);
		std::vector<char> vTemp;
		rd.GetDataBuffer(vTemp, uSize);
		PiecesW* p = FindPiece(vTemp);
		if (p) pSideMenu->vInitativeList.push_back(p);
	}
	pSideMenu->BuildInitativeList();
	return true;
}

const bool BaseLevel::LoadInitiativeSaveBuffer(const char* Buffer)
{
	UpdateLog(L"LoadInitiativeSaveBuffer()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	if (!Buffer) return false;
	pSideMenu->vInitativeList = std::vector<PiecesW*>();

	uint32_t uBufferSize = 0;
	size_t pos = 0;
	memcpy(&uBufferSize, Buffer, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	uint32_t uNumEntries = 0;
	memcpy(&uNumEntries, Buffer + pos, sizeof(uNumEntries));
	pos += sizeof(uNumEntries);
	for (uint32_t i = 0; i < uNumEntries; i++)
	{
		uint32_t uSize = 0;
		memcpy(&uSize, Buffer + pos, sizeof(uSize));
		PiecesW* p = FindPiece(Buffer + pos);
		if (p) pSideMenu->vInitativeList.push_back(p);
		pos += uSize;
	}
	pSideMenu->BuildInitativeList();
	return true;
}

const bool BaseLevel::Save(const std::wstring wFilePath)
{
	UpdateLog(L"Save(" + wFilePath + L")", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	if (wFilePath.empty()) return false;

	StoreData Buffer = CreateSaveInformationV();

	Buffer.WriteBuffer(wFilePath, L"wb");
	
	/*if (wFilePath.empty()) return false;

	uint32_t buffersize = CalcSaveBufferSize();
	const char* Buffer = GetSaveInformation();

	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, wFilePath.c_str(), L"wb"); uint32_t iline = __LINE__;
	if (err)
	{
		SendErrorMessage(wFilePath, err, iline);
		return false;
	}
	fwrite(Buffer, buffersize, 1, file);
	fclose(file);
	SafeDeleteArray(&Buffer);*/
	
	return true;
}

void BaseLevel::ResetVectors()
{
	UpdateLog(L"ResetVectors()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	for (size_t i = 0; i < pSideMenu->pRoomsMenu->pChild.size(); i++)
	{
		SafeDelete(&pSideMenu->pRoomsMenu->pChild.at(i));
	}
	while (pSideMenu->pRoomsMenu->pChild.size())
		pSideMenu->pRoomsMenu->pChild.pop_back();
	for (auto& layer : pSideMenu->pLayersMenu)
	{
		for (size_t i = 0; i < layer->pChild.size(); i++)
		{
			SafeDelete(&layer->pChild.at(i));
		}
		while (layer->pChild.size())
			layer->pChild.pop_back();
	}
	for (auto& l : pSideMenu->pLayersMenu)
	{
		pSideMenu->RemoveFromMenuSection(l);
		SafeDelete(&l);
	}
	while (pSideMenu->pLayersMenu.size()) pSideMenu->pLayersMenu.pop_back();
}

const bool BaseLevel::OpenTest(const std::wstring wFilePath)
{
	UpdateLog(L"OpenTest(" + wFilePath + L")", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	if (wFilePath.empty()) return false;

	ReceiveData rd;
	if (!rd.FillBufferFromFile(wFilePath)) return false;

	this->ResetVectors();
	LoadSaveBuffer(rd);
	wptest.reset();
	wptest = std::make_unique<Wall>(this, &GridSquareSize, gfx, &TranslatedCoordinates, bUseTexture);
	wptest->SetThickness(pThicknessMenu->GetSelectedThickness());
	pSideMenu->RealignAddLayerButton();

	return true;
}

const bool BaseLevel::Open(const std::wstring wFilePath)
{
	UpdateLog(L"Open(" + wFilePath + L")", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	if (wFilePath.empty()) return false;	

	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, wFilePath.c_str(), L"rb"); uint32_t iline = __LINE__;
	if (err)
	{
		SendErrorMessage(wFilePath, err, iline);
		return false;
	}
	fseek(file, 0, SEEK_END);
	uint32_t uBufferSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* Buffer = new char[uBufferSize];
	fread(Buffer, uBufferSize, 1, file);
	fclose(file);
	this->ResetVectors();

	LoadSaveBuffer(Buffer);
	SafeDeleteArray(&Buffer);
	SafeDelete(&sptest);
	wptest.reset();
	wptest = std::make_unique<Wall>(this, &GridSquareSize, gfx, &TranslatedCoordinates, bUseTexture);
	wptest->SetThickness(pThicknessMenu->GetSelectedThickness());
	pSideMenu->RealignAddLayerButton();

	return true;
}

void BaseLevel::SendErrorMessage(HRESULT hr, int iLineNumber)
{
	_com_error err(hr);
	std::string sErrMsg = err.ErrorMessage();
	std::wstring sErrMsgW(sErrMsg.begin(), sErrMsg.end());
	sErrMsgW = L"Error on Line " + std::to_wstring(iLineNumber) + std::wstring(L".\n") + sErrMsgW;
	MessageBoxW(nullptr, sErrMsgW.c_str(), L"Error", MB_OK | MB_ICONERROR);
}

void BaseLevel::SendErrorMessage(std::wstring filename, HRESULT hr, int iLineNumber)
{
	_com_error err(hr);
	std::string sErrMsg = err.ErrorMessage();
	std::wstring sErrMsgW = L"Unable to open " + filename;
	sErrMsgW.append(L".\n");
	sErrMsgW.append(std::wstring(sErrMsg.begin(), sErrMsg.end()));
	sErrMsgW = L"Error on Line " + std::to_wstring(iLineNumber) + std::wstring(L".\n") + sErrMsgW;
	MessageBoxW(nullptr, sErrMsgW.c_str(), L"Error", MB_OK | MB_ICONERROR);
}

const uint32_t BaseLevel::CalcSaveBufferSize()
{
	uint32_t uBufferSize = 0;
	uBufferSize += sizeof(uBufferSize);					//store total size
	uBufferSize += sizeof(fVERSION_NUMBER);				//store the version number of the program when the map was created
	uBufferSize += sizeof(float) * 2;					//store GrdiSquareSize
	uBufferSize += sizeof(float) * 2;					//store scaling
	uBufferSize += sizeof(float) * 2;					//store scaling speed
	uBufferSize += sizeof(float) * 2;					//store offset
	uBufferSize += sizeof(float);						//store rotation
	uBufferSize += sizeof(float);						//store rotation speed
	uBufferSize += sizeof(float);						//store movement speed
	uBufferSize += sizeof(uint32_t);					//store boolean values
	uBufferSize += sizeof(float) * 4;					//store grid backbround color
	uBufferSize += sizeof(float) * 2;					//gfx->GetCompatibleTargetSize()
	uBufferSize += sizeof(uint32_t);					//store turn counter value
	uBufferSize += sizeof(uint32_t);					//store position value for who is top of the round
	uBufferSize += sizeof(uint32_t);					//store currently selected room number
	uBufferSize += sizeof(uint32_t);					//store currently selected layer number
	uBufferSize += sizeof(uint32_t);					//store on/off values for each room as boolean values
	uBufferSize += sizeof(uint32_t);					//store number layers
	for (size_t i = 0; i < vVisibleRooms.size(); i++)
	{
		uBufferSize += sizeof(uint32_t);				//store on/off values for each layer as boolean values
	}
	uBufferSize += CalcInitativeSaveBufferSize();		//store initiative
	uBufferSize += CalcMapSpriteListBufferSize();		//store sprites
	uBufferSize += CalcMapWallListBufferSize();			//store walls	
	return uBufferSize;
}

const uint32_t BaseLevel::GetRoomsStates()
{
	uint32_t bools = 0;
	for (size_t i = 0; i < vVisibleRooms.size(); i++)
	{
		bools |= (vVisibleRooms.at(i) << i);
	}
	return bools;
}

const uint32_t BaseLevel::GetLayersStates(const uint32_t uRoom)
{
	uint32_t bools = 0;
	for (size_t i = 0; i < vVisibleLayers.at(uRoom).size(); i++)
	{
		bools |= (vVisibleLayers.at(uRoom).at(i) << i);
	}
	return bools;
}

const StoreData BaseLevel::CreateSaveInformationV()
{
	UpdateLog(L"CreateSaveInformationV()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	uint32_t uBufferSize = 0;	//this is no longer needed, StoreData automatically does this portion before saving
	StoreData sd;
	sd.AddEntry(fVERSION_NUMBER);
	sd.AddEntry(GridSquareSize.width);
	sd.AddEntry(GridSquareSize.height);
	sd.AddEntry(Scale.width);
	sd.AddEntry(Scale.height);
	sd.AddEntry(ScaleSpeed.width);
	sd.AddEntry(ScaleSpeed.height);
	sd.AddEntry(Offset.width);
	sd.AddEntry(Offset.height);
	sd.AddEntry(RotationAngle);
	sd.AddEntry(RotationSpeed);
	sd.AddEntry(MovementSpeed);
	uint32_t bools = 0u;
	if (bGridOnTop) bools |= 1;
	if (bLockToGrid) bools |= 2;
	if (bKeepAspect) bools |= 4;
	if (bUseTexture) bools |= 8;
	if (bShowSideMenu) bools |= 16;
	if (bShowCounter) bools |= 32;
	if (pSideMenu->ShowPieceColors()) bools |= 64;
	if (pSideMenu->IsAttachObject()) bools |= 128;
	if (pSideMenu->IsBuildMode()) bools |= 256;
	sd.AddEntry(bools);
	sd.AddEntry(GridBackgroundColor.r);
	sd.AddEntry(GridBackgroundColor.g);
	sd.AddEntry(GridBackgroundColor.b);
	sd.AddEntry(GridBackgroundColor.a);
	
	D2D1_SIZE_F size = gfx->GetCompatibleTargetSize();
	sd.AddEntry(size.width);
	sd.AddEntry(size.height);
	sd.AddEntry(uCounterValue);
	uint32_t uTopOfTheRoundValue = pSideMenu->GetTopOfTheRoundPosition();
	sd.AddEntry(uTopOfTheRoundValue);
	uint32_t uSelectedRoomNumber = pSideMenu->GetSelectedRoomNumber();
	uint32_t uSelectedLayerNumber = pSideMenu->GetSelectedLayerNumber();
	sd.AddEntry(uSelectedRoomNumber);
	sd.AddEntry(uSelectedLayerNumber);
	uint32_t uRoomsStates = GetRoomsStates();
	sd.AddEntry(uRoomsStates);
	uint32_t uNumberRooms = static_cast<uint32_t>(vVisibleRooms.size());
	sd.AddEntry(uNumberRooms);
	for (size_t i = 0; i < vVisibleRooms.size(); i++)
	{
		uint32_t uLayerStates = GetLayersStates(static_cast<uint32_t>(i));
		sd.AddEntry(uLayerStates);
	}
	std::vector<char> Vtemp = GetMapSpriteListSaveBufferV();
	sd.CombineBuffer(Vtemp);
	Vtemp = GetMapWallListSaveBufferV();
	sd.CombineBuffer(Vtemp);
	Vtemp = GetInitiativeListSaveBufferV();
	sd.CombineBuffer(Vtemp);
	
	return sd;
}

const char* BaseLevel::CreateSaveInformation()
{
	UpdateLog(L"CreateSaveInformation()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	uint32_t uBufferSize = CalcSaveBufferSize();
	char* Buffer = new char[uBufferSize];
	size_t pos = 0;
	memcpy(Buffer, &uBufferSize, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	memcpy(Buffer + pos, &fVERSION_NUMBER, sizeof(fVERSION_NUMBER));
	pos += sizeof(fVERSION_NUMBER);
	memcpy(Buffer + pos, &GridSquareSize.width, sizeof(GridSquareSize.width));
	pos += sizeof(GridSquareSize.width);
	memcpy(Buffer + pos, &GridSquareSize.height, sizeof(GridSquareSize.height));
	pos += sizeof(GridSquareSize.height);
	memcpy(Buffer + pos, &Scale.width, sizeof(Scale.width));
	pos += sizeof(Scale.width);
	memcpy(Buffer + pos, &Scale.height, sizeof(Scale.height));
	pos += sizeof(Scale.height);
	memcpy(Buffer + pos, &ScaleSpeed.width, sizeof(ScaleSpeed.width));
	pos += sizeof(ScaleSpeed.width);
	memcpy(Buffer + pos, &ScaleSpeed.height, sizeof(ScaleSpeed.height));
	pos += sizeof(ScaleSpeed.height);
	memcpy(Buffer + pos, &Offset.width, sizeof(Offset.width));
	pos += sizeof(Offset.width);
	memcpy(Buffer + pos, &Offset.height, sizeof(Offset.height));
	pos += sizeof(Offset.height);
	memcpy(Buffer + pos, &RotationAngle, sizeof(RotationAngle));
	pos += sizeof(RotationAngle);
	memcpy(Buffer + pos, &RotationSpeed, sizeof(RotationSpeed));
	pos += sizeof(RotationSpeed);
	memcpy(Buffer + pos, &MovementSpeed, sizeof(MovementSpeed));
	pos += sizeof(MovementSpeed);
	uint32_t bools = 0;
	if (bGridOnTop) bools |= 1;
	if (bLockToGrid) bools |= 2;
	if (bKeepAspect) bools |= 4;
	if (bUseTexture) bools |= 8;
	if (bShowSideMenu) bools |= 16;
	if (bShowCounter) bools |= 32;
	if (pSideMenu->ShowPieceColors()) bools |= 64;
	if (pSideMenu->IsAttachObject()) bools |= 128;
	if (pSideMenu->IsBuildMode()) bools |= 256;
	memcpy(Buffer + pos, &bools, sizeof(bools));
	pos += sizeof(bools);
	memcpy(Buffer + pos, &GridBackgroundColor.r, sizeof(GridBackgroundColor.r));
	pos += sizeof(GridBackgroundColor.r);
	memcpy(Buffer + pos, &GridBackgroundColor.g, sizeof(GridBackgroundColor.g));
	pos += sizeof(GridBackgroundColor.g);
	memcpy(Buffer + pos, &GridBackgroundColor.b, sizeof(GridBackgroundColor.b));
	pos += sizeof(GridBackgroundColor.b);
	memcpy(Buffer + pos, &GridBackgroundColor.a, sizeof(GridBackgroundColor.a));
	pos += sizeof(GridBackgroundColor.a);

	D2D1_SIZE_F size = gfx->GetCompatibleTargetSize();
	memcpy(Buffer + pos, &size.width, sizeof(size.width));
	pos += sizeof(size.width);
	memcpy(Buffer + pos, &size.height, sizeof(size.height));
	pos += sizeof(size.height);

	memcpy(Buffer + pos, &uCounterValue, sizeof(uCounterValue));
	pos += sizeof(uCounterValue);

	uint32_t uTopOfTheRoundValue = pSideMenu->GetTopOfTheRoundPosition();
	memcpy(Buffer + pos, &uTopOfTheRoundValue, sizeof(uTopOfTheRoundValue));
	pos += sizeof(uTopOfTheRoundValue);

	uint32_t uSelectedRoomNumber = pSideMenu->GetSelectedRoomNumber();
	uint32_t uSelectedLayerNumber = pSideMenu->GetSelectedLayerNumber();
	memcpy(Buffer + pos, &uSelectedRoomNumber, sizeof(uSelectedRoomNumber));
	pos += sizeof(uSelectedRoomNumber);
	memcpy(Buffer + pos, &uSelectedLayerNumber, sizeof(uSelectedLayerNumber));
	pos += sizeof(uSelectedLayerNumber);
	uint32_t uRoomsStates = GetRoomsStates();
	memcpy(Buffer + pos, &uRoomsStates, sizeof(uRoomsStates));
	pos += sizeof(uRoomsStates);
	uint32_t uNumberRooms = static_cast<uint32_t>(vVisibleRooms.size());
	memcpy(Buffer + pos, &uNumberRooms, sizeof(uNumberRooms));
	pos += sizeof(uNumberRooms);
	for (size_t i = 0; i < vVisibleRooms.size(); i++)
	{
		uint32_t uLayerStates = GetLayersStates(static_cast<uint32_t>(i));
		memcpy(Buffer + pos, &uLayerStates, sizeof(uLayerStates));
		pos += sizeof(uLayerStates);
	}

	uint32_t uLen = CalcMapSpriteListBufferSize();
	const char* t = GetMapSpriteListSaveBuffer();
	memcpy(Buffer + pos, t, uLen);
	SafeDeleteArray(&t);
	pos += uLen;
	uLen = CalcMapWallListBufferSize();
	t = GetMapWallListSaveBuffer();
	memcpy(Buffer + pos, t, uLen);
	SafeDelete(&t);
	pos += uLen;
	uLen = CalcInitativeSaveBufferSize();
	t = GetInitiativeListSaveBuffer();
	memcpy(Buffer + pos, t, uLen);
	SafeDeleteArray(&t);

	return Buffer;
}

const bool BaseLevel::LoadSaveBuffer(ReceiveData& rd)
{
	UpdateLog(L"LoadSaveBuffer()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));

	uint32_t uBufferSize = 0u;
	rd.GetData(uBufferSize);
	float fVersion = 0.0f;
	rd.GetData(fVersion);
	rd.GetData(GridSquareSize.width);
	rd.GetData(GridSquareSize.height);
	rd.GetData(Scale.width);
	rd.GetData(Scale.height);
	rd.GetData(ScaleSpeed.width);
	rd.GetData(ScaleSpeed.height);
	rd.GetData(Offset.width);
	rd.GetData(Offset.height);
	rd.GetData(RotationAngle);
	rd.GetData(RotationSpeed);
	rd.GetData(MovementSpeed);

	uint32_t bools = 0u;
	rd.GetData(bools);
	bGridOnTop = (bools & 1);
	bLockToGrid = (bools & 2);
	bKeepAspect = (bools & 4);
	bUseTexture = (bools & 8);
	bShowSideMenu = (bools & 16);
	bShowCounter = (bools & 32);
	(bools & 64) ? pSideMenu->SetUseColors() : pSideMenu->UnsetUseColors();
	(bools & 128) ? pSideMenu->SetAttachObject() : pSideMenu->UnsetAttachObject();
	(bools & 256) ? pSideMenu->SetIsBuildMode() : pSideMenu->UnsetIsBuildMode();
	if (bShowCounter) pTurnCounter->UnsetHidden();
	
	rd.GetData(GridBackgroundColor.r);
	rd.GetData(GridBackgroundColor.g);
	rd.GetData(GridBackgroundColor.b);
	rd.GetData(GridBackgroundColor.a);

	D2D1_SIZE_F size = D2D1::SizeF();
	rd.GetData(size.width);
	rd.GetData(size.height);
	gfx->ResizeCompatibleRenderTarget(size);

	rd.GetData(uCounterValue);

	uint32_t uTopOfTheroundValue = 0;
	rd.GetData(uTopOfTheroundValue);

	uint32_t uSelectedRoomNumber = 0u;
	uint32_t uSelectedLayerNumber = 0u;
	uint32_t uRoomsStates = 0u;
	uint32_t uNumberRooms = 0u;
	rd.GetData(uSelectedRoomNumber);
	rd.GetData(uSelectedLayerNumber);
	rd.GetData(uRoomsStates);
	rd.GetData(uNumberRooms);
	std::vector<uint32_t> vLayersStates;
	for (size_t i = 0; i < uNumberRooms; i++)
	{
		uint32_t uLayerStates = 0;
		rd.GetData(uLayerStates);
		vLayersStates.push_back(uLayerStates);
	}

	uint32_t uLen = 0;
	LoadMapSpriteList(rd);
	LoadMapWallList(rd);
	LoadInitiativeSaveBuffer(rd);

	for (size_t i = 0; i < vVisibleRooms.size(); i++)
	{
		vVisibleRooms.at(i) = (uRoomsStates & (1 << i));
		for (size_t j = 0; j < vVisibleLayers.at(i).size(); j++)
		{
			vVisibleLayers.at(i).at(j) = (vLayersStates.at(i) & (1 << j));
		}
	}

	for (auto& button : pSideMenu->pOptionsMenu->pChild)
	{
		bool setselected = false;
		if (!_wcsicmp(button->GetLabel(), L"Grid On Top"))
		{
			setselected = bGridOnTop;
		}
		else if (!_wcsicmp(button->GetLabel(), L"Lock To Grid"))
		{
			setselected = bLockToGrid;
		}
		else if (!_wcsicmp(button->GetLabel(), L"Toggle Keep Aspect"))
		{
			setselected = bKeepAspect;
		}
		else if (!_wcsicmp(button->GetLabel(), L"Show Counter"))
		{
			setselected = bShowCounter;
		}
		else if (!_wcsicmp(button->GetLabel(), L"Toggle PC Colors"))
		{
			setselected = pSideMenu->ShowPieceColors();
		}
		else if (!_wcsicmp(button->GetLabel(), L"Attach Object"))
		{
			setselected = pSideMenu->IsAttachObject();
		}
		else if (!_wcsicmp(button->GetLabel(), L"Turn Counter"))
		{
			setselected = bShowCounter;
		}
		setselected ? button->SetIsSelected() : button->UnsetIsSelected();
	}

	for (size_t i = 0; i < pSideMenu->pRoomsMenu->pChild.size(); i++)
	{
		vVisibleRooms.at(i) ? pSideMenu->pRoomsMenu->pChild.at(i)->SetIsSelected() : pSideMenu->pRoomsMenu->pChild.at(i)->UnsetIsSelected();
		pSideMenu->pRoomsMenu->pChild.at(i)->pChild.front()->UnsetIsSelected();
		for (size_t j = 0; j < pSideMenu->pLayersMenu.at(i)->pChild.size(); j++)
		{			
			vVisibleLayers.at(i).at(j) ? pSideMenu->pLayersMenu.at(i)->pChild.at(j)->SetIsSelected() : pSideMenu->pLayersMenu.at(i)->pChild.at(j)->UnsetIsSelected();
			pSideMenu->pLayersMenu.at(i)->SetHidden();
			if (i == uSelectedRoomNumber)
			{
				pSideMenu->pLayersMenu.at(i)->pChild.at(j)->pChild.front()->UnsetIsSelected();
				pSideMenu->pLayersMenu.at(i)->SetUnhidden();
			}
		}
	}
	pSideMenu->pRoomsMenu->pChild.at(uSelectedRoomNumber)->pChild.front()->SetIsSelected();
	pSideMenu->pLayersMenu.at(uSelectedRoomNumber)->pChild.at(uSelectedLayerNumber)->pChild.front()->SetIsSelected();

	pSelectedRoom = &vSprites.at(uSelectedRoomNumber);
	pSelectedLayer = &vSprites.at(uSelectedRoomNumber).at(uSelectedLayerNumber);
	pSelectedRoomWall = &vWalls.at(uSelectedRoomNumber);
	pSelectedLayerWall = &vWalls.at(uSelectedRoomNumber).at(uSelectedLayerNumber);
	pSideMenu->pSelectWallRoomsandLayers = &pvWalls;

	pSideMenu->RealignAddLayerButton();
	pTurnCounter->Update();

	pSideMenu->pFirstPieceW = pSideMenu->vInitativeList.at(uTopOfTheroundValue);

	return true;
}

const bool BaseLevel::LoadSaveBuffer(const char* Buffer)
{
	UpdateLog(L"LoadSaveBuffer()", L"BaseLevel.cpp", static_cast<uint32_t>(__LINE__));
	if (!Buffer) return false;

	uint32_t uBufferSize = 0;
	size_t pos = 0;
	memcpy(&uBufferSize, Buffer, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	float fVersion = 0;
	memcpy(&fVersion, Buffer + pos, sizeof(fVersion));
	pos += sizeof(fVersion);
	memcpy(&GridSquareSize.width, Buffer + pos, sizeof(GridSquareSize.width));
	pos += sizeof(GridSquareSize.width);
	memcpy(&GridSquareSize.height, Buffer + pos, sizeof(GridSquareSize.height));
	pos += sizeof(GridSquareSize.height);
	memcpy(&Scale.width, Buffer + pos, sizeof(Scale.width));
	pos += sizeof(Scale.width);
	memcpy(&Scale.height, Buffer + pos, sizeof(Scale.height));
	pos += sizeof(Scale.height);
	memcpy(&ScaleSpeed.width, Buffer + pos, sizeof(ScaleSpeed.width));
	pos += sizeof(ScaleSpeed.width);
	memcpy(&ScaleSpeed.height, Buffer + pos, sizeof(ScaleSpeed.height));
	pos += sizeof(ScaleSpeed.height);
	memcpy(&Offset.width, Buffer + pos, sizeof(Offset.width));
	pos += sizeof(Offset.width);
	memcpy(&Offset.height, Buffer + pos, sizeof(Offset.height));
	pos += sizeof(Offset.height);
	memcpy(&RotationAngle, Buffer + pos, sizeof(RotationAngle));
	pos += sizeof(RotationAngle);
	memcpy(&RotationSpeed, Buffer + pos, sizeof(RotationSpeed));
	pos += sizeof(RotationSpeed);
	memcpy(&MovementSpeed, Buffer + pos, sizeof(MovementSpeed));
	pos += sizeof(MovementSpeed);
	uint32_t bools = 0;
	memcpy(&bools, Buffer + pos, sizeof(bools));
	pos += sizeof(bools);
	bGridOnTop = (bools & 1);
	bLockToGrid = (bools & 2);
	bKeepAspect = (bools & 4);
	bUseTexture = (bools & 8);
	bShowSideMenu = (bools & 16);
	bShowCounter = (bools & 32);
	(bools & 64) ? pSideMenu->SetUseColors() : pSideMenu->UnsetUseColors();
	(bools & 128) ? pSideMenu->SetAttachObject() : pSideMenu->UnsetAttachObject();
	(bools & 256) ? pSideMenu->SetIsBuildMode() : pSideMenu->UnsetIsBuildMode();

	if (bShowCounter) pTurnCounter->UnsetHidden();
	
	memcpy(&GridBackgroundColor.r, Buffer + pos, sizeof(GridBackgroundColor.r));
	pos += sizeof(GridBackgroundColor.r);
	memcpy(&GridBackgroundColor.g, Buffer + pos, sizeof(GridBackgroundColor.g));
	pos += sizeof(GridBackgroundColor.g);
	memcpy(&GridBackgroundColor.b, Buffer + pos, sizeof(GridBackgroundColor.b));
	pos += sizeof(GridBackgroundColor.b);
	memcpy(&GridBackgroundColor.a, Buffer + pos, sizeof(GridBackgroundColor.a));
	pos += sizeof(GridBackgroundColor.a);

	D2D1_SIZE_F size = D2D1::SizeF();
	memcpy(&size.width, Buffer + pos, sizeof(size.width));
	pos += sizeof(size.width);
	memcpy(&size.height, Buffer + pos, sizeof(size.height));
	pos += sizeof(size.height);
	gfx->ResizeCompatibleRenderTarget(size);

	memcpy(&uCounterValue, Buffer + pos, sizeof(uCounterValue));
	pos += sizeof(uCounterValue);
	
	uint32_t uTopOfTheroundValue = 0;
	memcpy(&uTopOfTheroundValue, Buffer + pos, sizeof(uTopOfTheroundValue));
	pos += sizeof(uTopOfTheroundValue);

	uint32_t uSelectedRoomNumber = 0;
	uint32_t uSelectedLayerNumber = 0;
	memcpy(&uSelectedRoomNumber, Buffer + pos, sizeof(uSelectedRoomNumber));
	pos += sizeof(uSelectedRoomNumber);
	memcpy(&uSelectedLayerNumber, Buffer + pos, sizeof(uSelectedLayerNumber));
	pos += sizeof(uSelectedLayerNumber);
	uint32_t uRoomsStates = 0;
	memcpy(&uRoomsStates, Buffer + pos, sizeof(uRoomsStates));
	pos += sizeof(uRoomsStates);
	uint32_t uNumberRooms = 0;
	memcpy(&uNumberRooms, Buffer + pos, sizeof(uNumberRooms));
	pos += sizeof(uNumberRooms);
	std::vector<uint32_t> vLayersStates;
	for (size_t i = 0; i < uNumberRooms; i++)
	{
		uint32_t uLayerStates = 0;
		memcpy(&uLayerStates, Buffer + pos, sizeof(uLayerStates));
		pos += sizeof(uLayerStates);
		vLayersStates.push_back(uLayerStates);
	}

	uint32_t uLen = 0;
	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	LoadMapSpriteList(Buffer + pos);
	pos += uLen;
	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	LoadMapWallList(Buffer + pos);
	pos += uLen;
	memcpy(&uLen, Buffer + pos, sizeof(uLen));	
	LoadInitiativeSaveBuffer(Buffer + pos);
	pos += uLen;
	
	for (size_t i = 0; i < vVisibleRooms.size(); i++)
	{
		vVisibleRooms.at(i) = (uRoomsStates & (1 << i));
		for (size_t j = 0; j < vVisibleLayers.at(i).size(); j++)
		{
			vVisibleLayers.at(i).at(j) = (vLayersStates.at(i) & (1 << j));
		}
	}

	for (auto& button : pSideMenu->pOptionsMenu->pChild)
	{
		bool setselected = false;
		if (!_wcsicmp(button->GetLabel(), L"Grid On Top"))
		{
			setselected = bGridOnTop;
		}
		else if (!_wcsicmp(button->GetLabel(), L"Lock To Grid"))
		{
			setselected = bLockToGrid;
		}
		else if (!_wcsicmp(button->GetLabel(), L"Toggle Keep Aspect"))
		{
			setselected = bKeepAspect;
		}
		else if (!_wcsicmp(button->GetLabel(), L"Show Counter"))
		{
			setselected = bShowCounter;
		}
		else if (!_wcsicmp(button->GetLabel(), L"Toggle PC Colors"))
		{
			setselected = pSideMenu->ShowPieceColors();
		}
		else if (!_wcsicmp(button->GetLabel(), L"Attach Object"))
		{
			setselected = pSideMenu->IsAttachObject();
		}
		else if (!_wcsicmp(button->GetLabel(), L"Turn Counter"))
		{
			setselected = bShowCounter;
		}
		setselected ? button->SetIsSelected() : button->UnsetIsSelected();
	}

	for (size_t i = 0; i < pSideMenu->pRoomsMenu->pChild.size(); i++)
	{
		vVisibleRooms.at(i) ? pSideMenu->pRoomsMenu->pChild.at(i)->SetIsSelected() : pSideMenu->pRoomsMenu->pChild.at(i)->UnsetIsSelected();
		pSideMenu->pRoomsMenu->pChild.at(i)->pChild.front()->UnsetIsSelected();
		for (size_t j = 0; j < pSideMenu->pLayersMenu.at(i)->pChild.size(); j++)
		{			
			vVisibleLayers.at(i).at(j) ? pSideMenu->pLayersMenu.at(i)->pChild.at(j)->SetIsSelected() : pSideMenu->pLayersMenu.at(i)->pChild.at(j)->UnsetIsSelected();
			pSideMenu->pLayersMenu.at(i)->SetHidden();
			if (i == uSelectedRoomNumber)
			{
				pSideMenu->pLayersMenu.at(i)->pChild.at(j)->pChild.front()->UnsetIsSelected();
				pSideMenu->pLayersMenu.at(i)->SetUnhidden();
			}
		}
	}
	pSideMenu->pRoomsMenu->pChild.at(uSelectedRoomNumber)->pChild.front()->SetIsSelected();
	pSideMenu->pLayersMenu.at(uSelectedRoomNumber)->pChild.at(uSelectedLayerNumber)->pChild.front()->SetIsSelected();

	pSelectedRoom = &vSprites.at(uSelectedRoomNumber);
	pSelectedLayer = &vSprites.at(uSelectedRoomNumber).at(uSelectedLayerNumber);
	pSelectedRoomWall = &vWalls.at(uSelectedRoomNumber);
	pSelectedLayerWall = &vWalls.at(uSelectedRoomNumber).at(uSelectedLayerNumber);
	pSideMenu->pSelectWallRoomsandLayers = &pvWalls;

	pSideMenu->RealignAddLayerButton();
	pTurnCounter->Update();

	pSideMenu->pFirstPieceW = pSideMenu->vInitativeList.at(uTopOfTheroundValue);

	return true;
}

