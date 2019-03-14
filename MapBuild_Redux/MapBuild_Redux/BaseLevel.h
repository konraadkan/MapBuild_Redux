#pragma once
#include "Level.h"
#include "SideMenu.h"
#include "SizeMenu.h"
#include "Pieces.h"
#include "HPTimer.h"
#include "Walls.h"
#include "ruler.h"
#include <vector>
#include <atomic>
#include "TurnCounter.h"
#include "Logging.h"

class BaseLevel : public Level, Logging
{
private:
	template<typename T> void SafeDelete(T** ppT)
	{
		if (ppT)
		{
			delete (*ppT);
			(*ppT) = nullptr;
		}
	}
	template<typename T> void SafeDeleteArray(T** ppT)
	{
		if (ppT)
		{
			delete[](*ppT);
			(*ppT) = nullptr;
		}
	}
private:
	const float fVERSION_NUMBER = 0.26f;
	std::vector<InteractObjects*> IObjects;
	D2D1_POINT_2F Center = D2D1::Point2F();
	D2D1_SIZE_F GridSquareSize = D2D1::SizeF(64.0f, 64.0f);
	D2D1_SIZE_F Scale = D2D1::SizeF(1.0f, 1.0f);
	D2D1_SIZE_F ScaleSpeed = D2D1::SizeF(0.05f, 0.05f);
	D2D1_SIZE_F Offset = D2D1::SizeF();
	D2D1_POINT_2F* pMouseCoordinate = nullptr;
	D2D1_POINT_2F TranslatedCoordinates = D2D1::Point2F();
	D2D1_POINT_2F MenuCoordinates = D2D1::Point2F();
	D2D1_SIZE_F WindowSize = D2D1::SizeF();
	D2D1_SIZE_F ScreenSize = D2D1::SizeF();
	D2D1_RECT_F m_ClientWindow = D2D1::RectF();
	D2D1_RECT_F mPreviewDest = D2D1::RectF();
	D2D1_POINT_2F mPreviewPoint = D2D1::Point2F();
	float RotationAngle = 0.0f;
	float RotationSpeed = 30.0f;
	float MovementSpeed = 100.0f;
	D2D1_POINT_2F RotationCenter = D2D1::Point2F();	
	bool bShowSideMenu = true;
	bool bShowCounter = false;
	uint32_t uCounterValue = 0;
	D2D1_COLOR_F GridBackgroundColor = D2D1::ColorF(0.75f, 0.75f, 0.75f);
	D2D1::Matrix3x2F Transforms = D2D1::Matrix3x2F::Identity();
	std::vector<bool> vVisibleRooms;
	std::vector< std::vector<bool>> vVisibleLayers;
	HPTimer* const pTimer;
	D2D1_POINT_2F PushMouseCoordinate = D2D1::Point2F();
	const D2D1_RECT_F GetPreviewRect(SpritePointer* const pSpritePointer, const D2D1_POINT_2F p);
	MeasurementMenu::SizeMenuType mSizeMenuType = MeasurementMenu::SizeMenuType::CreatureSize;
	Ruler* pRuler = nullptr;
	D2D1_RECT_F mRulerDest = D2D1::RectF();	
	AoeSpritePointer* pLastAttachedSpritePointer = nullptr;
public:
	BaseLevel(const HWND hwnd, Graphics* const graphics, D2D1_POINT_2F* const pMousePosition, int WindowX, int WindowY, HPTimer* const timer);
	BaseLevel(const BaseLevel&) = delete;
	BaseLevel& operator=(const BaseLevel&) = delete;
	~BaseLevel();
	HWND hWnd;
	void Load(Keyboard* const keyboard, Mouse* const mouse) override;
	void Unload() override;
	void Render() override;
	void ProcessEvents(double dDelta) override;
	void Update(double dDelta) override;
	void SetLockToGrid() { bLockToGrid = true; }
	void UnsetLockToGrid() { bLockToGrid = false; }
	void ToggleLockToGrid() { bLockToGrid ^= true; }
	void ToggleKeepAspect() { bKeepAspect ^= true; }
	void SetKeepAspect() { bKeepAspect = true; }
	void UnsetKeepAspect() { bKeepAspect = false; }
	void ToggleGridOnTop() { bGridOnTop ^= true; }
	void ToggleUseTexture() override { bUseTexture ^= true; if (wptest) bUseTexture ? wptest->SetUseTexture() : wptest->UnsetUseTexture(); }
	const D2D1_POINT_2F GetNearestCorner();	
	void OutputImageLoadingStatusM(std::atomic<uint32_t>& numloaded, uint32_t total, const std::wstring imagetype);	
	const bool CreateNew() override;
	const char* GetMapSpriteListSaveBuffer() { return BuildMapSpriteListSaveBuffer(); }
	const char* BuildMapSpriteListSaveBuffer();
	const char* GetMapWallListSaveBuffer() { return BuildMapWallListSaveBuffer(); }
	const char* BuildMapWallListSaveBuffer();
	const uint32_t CalcMapSpriteListBufferSize();
	const uint32_t CalcMapWallListBufferSize();
	const bool LoadMapSpriteList(const char* Buffer);
	const bool LoadMapWallList(const char* Buffer);
	PiecesW* const FindPiece(const char* Buffer);
	D2D1_SIZE_F* const GetGridSquareSizePtr() { return &GridSquareSize; }
	const bool Save(const std::wstring wFilePath) override;
	const bool Open(const std::wstring sFilePath) override;
	const char* GetSaveInformation() { return CreateSaveInformation(); }
	const bool LoadSaveBuffer(const char* Buffer);
	const uint32_t CalcInitativeSaveBufferSize();
	const char* BuildInitiativeListSaveBuffer();
	const char* GetInitiativeListSaveBuffer() { return BuildInitiativeListSaveBuffer(); }
	const bool LoadInitiativeSaveBuffer(const char* Buffer);
private:	
	void ResetVectors();
	void DrawSideMenu();
	void DrawSizeMenu();
	void ProcessKeyboardEvents(double dDelta);
	void ProcessMouseEvents(double dDelta);
	void CreateRoom();
	void CreateLayer(size_t uRoomNumber);
	int afps = 0;
	bool bShowFPS = false;
	const bool BuildObjects(const wchar_t* sFilePath);
	void LoadSprites();
	void LoadPortraits();
	void LoadImages();
	void OutputImageLoadingStatus(uint32_t numloaded, uint32_t total, const std::wstring imagetype);
	const std::wstring GetLineW(wchar_t* const buffer, wchar_t*& pos);
	const std::wstring GetLineW(char* const buffer, char*& pos);
	template<typename T> void RemoveEmptyPieces(T& pieces);
	void SendErrorMessage(HRESULT hr, int iLineNumber);
	void SendErrorMessage(std::wstring filname, HRESULT hr, int iLineNumber);
	const uint32_t CalcSaveBufferSize();
	const char* CreateSaveInformation();
	const uint32_t GetRoomsStates();
	const uint32_t GetLayersStates(const uint32_t uRoom);	
public:
	SideMenu* pSideMenu = nullptr;
	SizeMenu* pSizeMenu = nullptr;
	ThicknessMenu* pThicknessMenu = nullptr;
	AoeSizeMenu* pAoeSizeMenu = nullptr;
	std::vector<PiecesW> vPiecesW;
	std::vector<Pieces> vPieces;
	std::vector< std::vector< std::vector<SpritePointer*>>> vSprites;
	SpritePointer* sptest = nullptr;
	std::vector<SpritePointer*>* pSelectedLayer = nullptr;
	std::vector< std::vector<SpritePointer*>>* pSelectedRoom = nullptr;
	std::vector< std::vector< std::vector<SpritePointer*>>>* ppvSprites;
public:
	std::vector< std::vector< std::vector<std::unique_ptr<Wall>>>> vWalls;
	std::vector<std::unique_ptr<Wall>>* pSelectedLayerWall = nullptr;
	std::vector< std::vector<std::unique_ptr<Wall>>>* pSelectedRoomWall = nullptr;
	std::vector< std::vector< std::vector<std::unique_ptr<Wall>>>>* pvWalls = nullptr;
	std::unique_ptr<Wall> wptest = nullptr;
	SpritePointer* pSelectedObject = nullptr;
	TurnCounter* pTurnCounter = nullptr;

	FILE* pLogFile = nullptr;
};