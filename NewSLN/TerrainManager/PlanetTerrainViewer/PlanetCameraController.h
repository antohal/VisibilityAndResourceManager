#pragma once

#include "Camera.h"

class CDirect2DTextBlock;

class CPlanetCameraController : public CD3DCameraController
{
public:

	struct Coordinates
	{
		Coordinates() {}

		Coordinates(const double& longitude, const double& lattitude, const double& height, const double& azimuth, const double& elevation)
			: _longitude(longitude), _lattitude(lattitude), _height(height), _azimuth(azimuth), _elevation(elevation)
		{}

		double			_longitude = 0;
		double			_lattitude = 0;
		double			_height = 0;
		double			_azimuth = 0;
		double			_elevation = 0;
	};

	void				SetScrollCoeff(float in_fScrollCoeff) { _fScrollCoeff = in_fScrollCoeff; }
	void				SetCoordinates(const Coordinates& in_Coordinates);
	const Coordinates&	GetCoordinates() const;

	void				CreateDebugTextBlock();
	void				ShowDebugTextBlock(bool in_bShow = true);
	void				HideDebugTextBlock();
	void				DeleteDebugTextBlock();

	void				SetWorldScale(float in_fScale) { _fWorldScale = in_fScale; }
	void				SetMaxHeight(double in_dfMaxHeight) { _dfMaxHeight = in_dfMaxHeight; }

	//@{ CD3DCameraController
	virtual void		Update(CD3DCamera* in_pCamera, float deltaTime) override;
	//@}

private:

	void				MoveHeight(float deltaTime, int wheelDelta);

	Coordinates			_coordinates;

	CDirect2DTextBlock*	_pTextBlock = nullptr;

	UINT				_uiLatLongHeightParam = -1;
	UINT				_uiAzElParam = -1;

	float				_fScrollCoeff = 5;
	float				_fHeightCommand = 0;
	float				_fWorldScale = 1;
	double				_dfMaxHeight = 50000000.0;
};
