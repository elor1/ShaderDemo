//--------------------------------------------------------------------------------------
// Math convenience functions
//--------------------------------------------------------------------------------------

#ifndef _MATH_HELPERS_H_DEFINED_
#define _MATH_HELPERS_H_DEFINED_

#include <cmath>


// Surprisingly, pi is not *officially* defined anywhere in C++
const float PI = 3.14159265359f;



// Test if a float value is approximately 0
// Epsilon value is the range around zero that is considered equal to zero
const float EPSILON = 0.5e-6f; // For 32-bit floats, requires zero to 6 decimal places
inline bool IsZero(const float x)
{
    return std::abs(x) < EPSILON;
}


// 1 / Sqrt. Used often (e.g. normalising) and can be optimised, so it gets its own function
inline float InvSqrt(const float x)
{
    return 1.0f / std::sqrt(x);
}


// Pass an angle in degrees, returns the angle in radians
inline float ToRadians(float d)
{
    return  d * PI / 180.0f;
}

// Pass an angle in radians, returns the angle in degrees
inline float ToDegrees(float r)
{
    return  r * 180.0f / PI;
}

// Find the minimum of three numbers (helper function for exercise below)
inline float Min(float f1, float f2, float f3)
{
	float fMin = f1;
	if (f2 < fMin)
	{
		fMin = f2;
	}
	if (f3 < fMin)
	{
		fMin = f3;
	}
	return fMin;
}

// Find the maximum of three numbers (helper function for exercise below)
inline float Max(float f1, float f2, float f3)
{
	float fMax = f1;
	if (f2 > fMax)
	{
		fMax = f2;
	}
	if (f3 > fMax)
	{
		fMax = f3;
	}
	return fMax;
}

#endif  //_MATH_HELPERS_H_DEFINED_
