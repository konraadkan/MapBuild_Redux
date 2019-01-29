#pragma once
#include <vector>
#include "InteractObjects.h"
#include "ShapeList.h"

class ClassShapes : public InteractObjects
{
private:
	ShapeTypes ShapeType;
	D2D1_POINT_2F Center;
	std::vector<D2D1_POINT_2F> Points;
	std::vector<ID2D1PathGeometry*> pPathGeometry;
	bool bFill = false;
	bool bBack = false;
	bool bUseTransform = false;
private:
	void CalcRect();
public:
	ClassShapes(ShapeTypes Type, Graphics* const graphics, bool UseTransform, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, bool Back = false) : InteractObjects(graphics, Transform, area, p), ShapeType(Type), bBack(Back), bUseTransform(UseTransform)
	{
		switch (ShapeType)
		{
		case ShapeTypes::Ellipses:		
			break;
		}
	}
	~ClassShapes()
	{
		Unload();
		while (pPathGeometry.size())
		{
			SafeRelease(&pPathGeometry.back());
			pPathGeometry.pop_back();
		}
	}
	void Draw() override;
	void SetCenter(const D2D1_POINT_2F p) override;
	void Interact(const D2D1_POINT_2F p) override;
	void Interact() override;
	void SetFill() override;
	void UnsetFill() override;
	bool BuildCustomShape(std::queue<D2D1_POINT_2F> p, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f)) override;
	bool IsBack() override { return bBack; }
	void Unload() override;
};