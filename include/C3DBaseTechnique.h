#pragma once

// ������� �������
class C3DBaseTechnique : public C3DBaseResource
{
public:
	
	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_TECHNIQUE; }

};
