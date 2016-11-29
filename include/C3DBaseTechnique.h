#pragma once

// базовая техника
class C3DBaseTechnique : public C3DBaseResource
{
public:
	
	// тип ресурса здесь известен.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_TECHNIQUE; }

};
