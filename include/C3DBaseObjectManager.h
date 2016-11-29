#pragma once

// базовый класс менеджера объектов
class C3DBaseObjectManager : public C3DBaseManager
{
public:

	// получить список объектов по заданному баунд-боксу
	virtual void GetObjectList(const D3DXVECTOR3& bboxMin, const D3DXVECTOR3& bboxMax, std::vector<C3DBaseObject*>& out_vecObjects) = 0;
};
