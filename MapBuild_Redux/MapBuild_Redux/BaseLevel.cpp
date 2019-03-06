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

const bool BaseLevel::CreateNew()
{
	if (pSideMenu) return pSideMenu->CreateNew();
	return true;
}

BaseLevel::BaseLevel(Graphics* const graphics, D2D1_POINT_2F* const pMousePosition, int WindowX, int WindowY, HPTimer* const timer) : pTimer(timer)
{
	this->gfx = graphics;
	this->pMouseCoordinate = pMousePosition;
	WindowSize = D2D1::SizeF(static_cast<float>(WindowX), static_cast<float>(WindowY));
	m_ClientWindow = D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height);
	RotationCenter = D2D1::Point2F(WindowSize.width * 0.5f, WindowSize.height * 0.5f);

	pSideMenu = new SideMenu(&bExit, D2D1::RectF(WindowSize.width * 0.75f, 0.0f, WindowSize.width, WindowSize.height), graphics, &Transforms, &m_ClientWindow, &MenuCoordinates, &pSelectedRoom, &pSelectedLayer, &ppvSprites, &vVisibleRooms, &vVisibleLayers, &sptest, &pvWalls, &pSelectedRoomWall, &pSelectedLayerWall);
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
	
	sptest = new SpritePointer(&vPiecesW.front(), Location(), &GridSquareSize);
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

	for (auto& p : vPiecesW)
	{
		if (p.IsDefault())
		{
			pSideMenu->vInitativeList.push_back(&p);
		}
	}
	pSideMenu->BuildInitativeList();

	mRulerDest = D2D1::RectF(5.0f, (WindowSize.height * 0.5f) - 16.0f, 105.0f, (WindowSize.height * 0.5f) + 16.0f);
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
	SafeDelete(&pRuler);
	wptest.reset();

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
		gfx->DrawRect(gfx->GetCompatibleTarget(), pSelectedObject->GetDestSprite(), D2D1::ColorF(1.0f, 0.0f, 1.0f), 3.0f);
	
	if (sptest && !pSideMenu->WallSelected())
	{
		if (pSideMenu->IsBuildMode() || pSelectedObject) gfx->DrawRect(gfx->GetCompatibleTarget(), mPreviewDest, D2D1::ColorF(1.0f, 0.1f, 0.05f, 1.0f), 5.0f);
	}

	if (wptest && pSideMenu->WallSelected())
	{
		if (bLockToGrid) wptest->DrawPreview(mPreviewPoint);
		else wptest->DrawPreview();
		if (bLockToGrid) gfx->FillCircle(gfx->GetCompatibleTarget(), mPreviewPoint, 3.0f, D2D1::ColorF(1.0f, 0.0f, 0.0f));
	}

	if (bGridOnTop) gfx->DrawDefaultGrid(gfx->GetCompatibleTarget(), Transforms, D2D1::RectF(0.0f, 0.0f, WindowSize.width, WindowSize.height), GridSquareSize, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.9f), 3.0f);
	if (pRuler) pRuler->DrawLine();
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
//	gfx->OutputText(gfx->GetRenderTarget(), std::to_wstring(afps).c_str(), D2D1::RectF(0, 500, 500, 564));			//remove comment to display avg. FPS
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
	//if (pMouse->MiddlePressed())
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
			if (wptest)
			{
				if (bLockToGrid) mPreviewPoint = GetNearestCorner();
			}
			if (pThicknessMenu->IsSelected())
			{
				pThicknessMenu->UpdateSlider();
				wptest->SetThickness(pThicknessMenu->GetSelectedThickness());								
			}
			if (pRuler)
			{
				pRuler->SetEndPoint(TranslatedCoordinates);
				pRuler->CalcDistance();
			}
			break;
		}
		case Mouse::Event::Type::LPress:
			if (pSelectedLayer && !pSideMenu->IsInRealRect() && !pSizeMenu->IsInRealRect() && pSideMenu->IsBuildMode())
			{
				if (sptest && !pSideMenu->WallSelected())
				{
					if (pSideMenu->IsAttachObject())
					{
						for (auto& sprite : *pSelectedLayer)
						{
							if (sprite->PointInSprite(TranslatedCoordinates))
							{
								sprite->AddSpriteChild(sptest->GetPieces());
								break;
							}
						}
					}
					else
					{
						pSelectedLayer->push_back(new SpritePointer(sptest->GetPieces(), Location(), &GridSquareSize));
						pSelectedLayer->back()->SetCreatureSize(sptest->GetCreatureSize());
						bKeepAspect ? pSelectedLayer->back()->SetKeepAspectSprite() : pSelectedLayer->back()->UnsetKeepAspectSprite();

						if (bLockToGrid)
							pSelectedLayer->back()->SetDestSprite(mPreviewDest);
						else
							pSelectedLayer->back()->SetDestSprite(mPreviewDest);
						//pSelectedLayer->back()->SetDestSprite(D2D1::RectF(TranslatedCoordinates.x, TranslatedCoordinates.y, TranslatedCoordinates.x + sptest->GetDestResizedSpriteSize().width, TranslatedCoordinates.y + sptest->GetDestResizedSpriteSize().height));
					}
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
								pSelectedObject = c;
								if (!pRuler) pRuler = new Ruler(gfx, GridSquareSize, D2D1::Point2F(pSelectedObject->GetDestSprite().left + (pSelectedObject->GetDestSprite().right - pSelectedObject->GetDestSprite().left) * 0.5f,
									pSelectedObject->GetDestSprite().top + (pSelectedObject->GetDestSprite().bottom - pSelectedObject->GetDestSprite().top) * 0.5f), mRulerDest);
							}
							/*else
							{
								if (c == pSelectedObject) pSelectedObject = nullptr;
								else
								{
									D2D1_RECT_F temp = c->GetDestSprite();
									c->SetDestSprite(pSelectedObject->GetDestSprite());
									pSelectedObject->SetDestSprite(temp);
									pSelectedObject = nullptr;
								}
							}*/
							move = false;
						}
					}
					if (pSelectedObject && move)
					{
						pSelectedObject->SetDestSprite(GetPreviewRect(pSelectedObject, TranslatedCoordinates));
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
			if (pSideMenu->WallSelected()) if (wptest) wptest->RemoveLastPoint();
			break;
		case Mouse::Event::Type::RRelease:
		{
			if (pSideMenu->IsInRealRect() && pSideMenu->IsBuildMode())
			{
				unsigned int uInitativeListSize = pSideMenu->GetInitativeListSize();
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
			if (!pSideMenu->PointInRect(*pMouseCoordinate) || pSideMenu->IsHidden())
				PushMouseCoordinate = e.GetPos();
			break;
		case Mouse::Event::Type::X1Release:
			PushMouseCoordinate = D2D1::Point2F();
			break;
		case Mouse::Event::Type::MPress:			
			break;
		case Mouse::Event::Type::MRelease:			
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
		case L' ':
		{
			if (!pSideMenu->IsBuildMode())
			{
				pSideMenu->NextTurn();
			}
			break;
		}
		case VK_BACK:
		{
			if (!pSideMenu->IsBuildMode())
			{
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
			case VK_SHIFT:
				SafeDelete(&pRuler);
				pRuler = new Ruler(gfx, GridSquareSize, TranslatedCoordinates, mRulerDest);
				break;
			case 'V':
			{
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
				FILE* file = nullptr;
				fopen_s(&file, "delme.dat", "rb");
				fseek(file, 0, SEEK_END);
				unsigned int ulen = ftell(file);
				fseek(file, 0, SEEK_SET);
				char* t = new char[ulen];
				fread(t, ulen, 1, file);
				fclose(file);
				this->LoadMapSpriteList(t);
				SafeDeleteArray(&t);
				pSelectedRoom = &vSprites.front();
				pSelectedLayer = &vSprites.front().front();
				pSideMenu->pSelectWallRoomsandLayers = &pvWalls;
				sptest = nullptr;				
				pSideMenu->RealignAddLayerButton();
			}
				break;
			case 'B':
			{//test button
				unsigned int tlen = this->CalcMapSpriteListBufferSize();
				const char* t = this->GetMapSpriteListSaveBuffer();
				FILE* file = nullptr;
				fopen_s(&file, "delme.dat", "wb");
				fwrite(t, tlen, 1, file);
				fclose(file);
				SafeDeleteArray(&t);
				break;
			}
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
			case VK_SHIFT:
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
		sizemod = static_cast<float>(1 + static_cast<unsigned int>(pSpritePointer->GetCreatureSize()) - static_cast<unsigned int>(CreatureSize::Medium));
		size = D2D1::SizeF(GridSquareSize.width * sizemod, GridSquareSize.height * sizemod);
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
			lockedPoint.x = static_cast<float>(static_cast<unsigned int>(p.x) / static_cast<unsigned int>(GridSquareSize.width * sizemod));
			lockedPoint.y = static_cast<float>(static_cast<unsigned int>(p.y) / static_cast<unsigned int>(GridSquareSize.height * sizemod));
			lockedPoint.x *= GridSquareSize.width * sizemod;
			lockedPoint.y *= GridSquareSize.height * sizemod;
		}
		else
		{
			lockedPoint.x = static_cast<float>(static_cast<unsigned int>(p.x) / static_cast<unsigned int>(GridSquareSize.width));
			lockedPoint.y = static_cast<float>(static_cast<unsigned int>(p.y) / static_cast<unsigned int>(GridSquareSize.height));
			lockedPoint.x *= GridSquareSize.width;
			lockedPoint.y *= GridSquareSize.height;
		}

		return D2D1::RectF(lockedPoint.x, lockedPoint.y, lockedPoint.x + size.width, lockedPoint.y + size.height);
	}
	return D2D1::RectF(p.x, p.y, p.x + size.width, p.y + size.height);
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

const bool BaseLevel::LoadMapSpriteList(const char* Buffer)
{
	if (!Buffer) return false;
	vSprites = std::vector< std::vector< std::vector<SpritePointer*>>>();
	vWalls = std::vector< std::vector< std::vector<std::unique_ptr<Wall>>>>();
	vVisibleRooms = std::vector<bool>();
	vVisibleLayers = std::vector< std::vector< bool>>();

	/****** Need to delete these and rebuild them, probably just delete thier children and resize their m_Dest
	pSideMenu->pRoomsCheckMenu;
	pSideMenu->pRoomsMenu;
	pSideMenu->pLayersCheckMenu;
	pSideMenu->pLayersMenu;
	*/	

	unsigned int uBufferSize = 0;
	size_t pos = 0;
	memcpy(&uBufferSize, Buffer, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);

	unsigned int uLen = 0;
	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	pos += sizeof(uLen);
	for (unsigned int i = 0; i < uLen; i++)
	{
		//vSprites.push_back(std::vector< std::vector<SpritePointer*>>());
		CreateRoom();
		unsigned int uLayers = 0;
		memcpy(&uLayers, Buffer + pos, sizeof(uLayers));
		pos += sizeof(uLayers);
		for (unsigned int j = 0; j < uLayers; j++)
		{
			//vSprites.at(i).push_back(std::vector<SpritePointer*>());
			if (j) CreateLayer(i);
			unsigned int uSpritePointers = 0;
			memcpy(&uSpritePointers, Buffer + pos, sizeof(uSpritePointers));
			pos += sizeof(uSpritePointers);
			for (unsigned int w = 0; w < uSpritePointers; w++)
			{
				unsigned int uSpriteSize = 0;
				memcpy(&uSpriteSize, Buffer + pos, sizeof(uSpriteSize));
				SpritePointer* p = new SpritePointer(nullptr, Location(), &GridSquareSize);
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
		}
	}
	return true;
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

const char* BaseLevel::BuildMapSpriteListSaveBuffer()
{
	unsigned int uBufferSize = CalcMapSpriteListBufferSize();
	char* Buffer = new char[uBufferSize];
	size_t pos = 0;

	memcpy(Buffer, &uBufferSize, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	unsigned int uLen = static_cast<unsigned int>(vSprites.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);
	for (auto& room : vSprites)
	{
		uLen = static_cast<unsigned int>(room.size());
		memcpy(Buffer + pos, &uLen, sizeof(uLen));
		pos += sizeof(uLen);
		for (auto& layer : room)
		{
			uLen = static_cast<unsigned int>(layer.size());
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

const unsigned int BaseLevel::CalcMapSpriteListBufferSize()
{
	unsigned int uBufferSize = 0;
	uBufferSize += sizeof(unsigned int);							//store buffer size
	uBufferSize += sizeof(unsigned int);							//store number rooms
	for (auto& room : vSprites)
	{
		uBufferSize += sizeof(unsigned int);						//store number layer for each room
		for (auto& layer : room)
		{
			uBufferSize += sizeof(unsigned int);					//store number spritepointers for each layer
			for (auto& sp : layer)
			{
				uBufferSize += sp->CalcSaveSize();					//store size of spritepointer
			}
		}
	}
	return uBufferSize;
}