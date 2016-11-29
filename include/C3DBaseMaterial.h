#pragma once

class C3DBaseTechnique;
class C3DBaseTexture;

// ������� ��������
class C3DBaseMaterial : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MATERIAL; }

	// ������� ������������ � �������� ���������� [��� �������������� �������� ������� � Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*) = 0;

	// �������� ����� ������
	virtual void	GetTechniques(std::vector<C3DBaseTechnique*>& out_vecTechniques) const = 0;

	// �������� ������ �������
	virtual void	GetTextures(std::vector<C3DBaseTexture*>& out_vecTextures) const = 0;
};
