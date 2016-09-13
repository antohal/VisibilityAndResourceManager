#pragma once

class IGridIterator
{
public:
	virtual ~IGridIterator() {}

	virtual UINT_PTR			Get() const = 0;
	virtual bool				IsEnd() const = 0;
	virtual void				Next() = 0;
	virtual BYTE				GetFrustumFlag() const = 0;
	virtual IGridIterator&		operator++(int) = 0;
};
