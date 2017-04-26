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

	void				SetCoordinates(const Coordinates& in_Coordinates);
	const Coordinates&	GetCoordinates() const;

	void				CreateDebugTextBlock();
	void				ShowDebugTextBlock(bool in_bShow = true);
	void				HideDebugTextBlock();
	void				DeleteDebugTextBlock();

	//@{ CD3DCameraController
	virtual void		Update(CD3DCamera* in_pCamera, float deltaTime) override;
	//@}

private:

	Coordinates			_coordinates;

	CDirect2DTextBlock*	_pTextBlock = nullptr;

	UINT				_uiLatLongHeightParam = -1;
	UINT				_uiAzElParam = -1;
};
