#pragma once

namespace BLUE
{
struct CColor
{
	operator float *()
	{
		return &r;

	}
	float r, g, b, a;	
};

} // end namespace BLUE
