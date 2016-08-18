
#include "StdAfx.h"
#include "BSSegment.h"

CBSCSegment::CBSCSegment():
	m_linearLength(0),
	MP_VECTOR_INIT(m_linearPoints)
{
}

void CBSCSegment::Create(CBSCPoint *p1, CBSCPoint *p2)
{
	m_linearPoints.clear();
	m_start = p1;
	m_end = p2;
	CreateLinearPoints();
}

CVector3D CBSCSegment::GetPoint(float t)
{
	float var1, var2, var3;
    CVector3D vPoint(0.0f, 0.0f, 0.0f), p1, p2, p3, p4;

	p1 = m_start->GetPosition();
	p2 = m_start->GetCtrlp2();
	p3 = m_end->GetCtrlp1();
	p4 = m_end->GetPosition();

	// Here is the juice of our tutorial.  Below is the equation for a 4 control point
	// bezier curve:
	// B(t) = P1 * ( 1 - t )^3 + P2 * 3 * t * ( 1 - t )^2 + P3 * 3 * t^2 * ( 1 - t ) + P4 * t^3 
	// Yes I agree, this isn't the most intuitive equation, but it is pretty straight forward.
	// If you got up to Trig, you will notice that this is a polynomial.  That is what a curve is.
	// "t" is the time from 0 to 1.  You could also think of it as the distance along the
	// curve, because that is really what it is.  P1 - P4 are the 4 control points.
	// They each have an (x, y, z) associated with them.  You notice that there is a lot of
	// (1 - t) 's?  Well, to clean up our code we will try to contain some of these
	// repetitions into variables.  This helps our repeated computations as well.

	// Store the (1 - t) in a variable because it is used frequently
    var1 = 1 - t;
	
	// Store the (1 - t)^3 into a variable to cut down computation and create clean code
    var2 = var1 * var1 * var1;

	// Store the t^3 in a variable to cut down computation and create clean code
    var3 = t * t * t;

	// Now that we have some computation cut down, we just follow the equation above.
	// If you multiply and simplify the equation, you come up with what we have below.
	// If you don't see how we came to here from the equation, multiply the equation
	// out and it will become more clear.  I don't intend to go into any more detail
	// on the math of a bezier curve, because there are far better places out there
	// with graphical displays and tons of examples.  Look in our * Quick Notes *
	// for an EXCELLENT web site that does just this.  It derives everything and has
	// excellent visuals.  It's the best I have seen so far.
    vPoint.x = var2 * p1.x + 3 * t * var1 * var1 * p2.x + 3 * t * t * var1 * p3.x + var3 * p4.x;
    vPoint.y = var2 * p1.y + 3 * t * var1 * var1 * p2.y + 3 * t * t * var1 * p3.y + var3 * p4.y;
    vPoint.z = var2 * p1.z + 3 * t * var1 * var1 * p2.z + 3 * t * t * var1 * p3.z + var3 * p4.z;

	// Now we should have the point on the curve, so let's return it.
	return(vPoint);
}

void	CBSCSegment::CreateLinearPoints( )
{
	if (m_linearPoints.size() == 0)
	{
		m_linearLength = 0;
		m_linearPoints.resize(LINEAR_APPROXIMATION_COUNT);
		CVector3D prevV = GetPoint(0.0f);
		for (int i = 0; i < LINEAR_APPROXIMATION_COUNT; i++)
		{
			CVector3D v = GetPoint((float)i / (LINEAR_APPROXIMATION_COUNT - 1));
			m_linearPoints[i] = (v - prevV).GetLength();
			m_linearLength += m_linearPoints[i];
			prevV = v;
		}
	}
}

CVector3D CBSCSegment::GetLinearPoint(float t)
{
	CreateLinearPoints();

	double needLength = m_linearLength * t;
	float prevLength = 0;

	for( int i = 0; i < LINEAR_APPROXIMATION_COUNT; i++)
	{
		float currentLength = prevLength + (float)m_linearPoints[i];

		if ((prevLength <= needLength) && (currentLength >= needLength))
		{
			double insegmentFloat = 0.0;
			if (currentLength - prevLength != 0)	
			{
				insegmentFloat = (needLength - prevLength) / (currentLength - prevLength);
			}
			if (i + insegmentFloat != 0)
			{
				t = (float)(i + insegmentFloat) / (float)(LINEAR_APPROXIMATION_COUNT - 1);
				return GetPoint(t);
			}
			break;
		}

		prevLength = currentLength;
	}

	return GetPoint(t);
}

CVector3D CBSCSegment::GetTangent(float t)
{
	if (t > 0.99f)	
	{
		t = 0.99f;
	}

	float d, e, a, b, c, var1, var2, f;
    CVector3D vPoint(0.0f, 0.0f, 0.0f), p1, p2, p3, p4;

	p1 = m_start->GetPosition();
	p2 = m_start->GetCtrlp2();
	p3 = m_end->GetCtrlp1();
	p4 = m_end->GetPosition();

	var1 = 1 - t;
    var2 = var1 * var1;

	d = t * t;
	a = -3 * var2;
	b = 3 * var2;
	c = 6 * var1 * t;
	e = 6 * t * var1;
	f = 3 * d;

	vPoint.x = a * p1.x + b * p2.x - c * p2.x + e * p3.x - f * p3.x + f * p4.x;
	vPoint.y = a * p1.y + b * p2.y - c * p2.y + e * p3.y - f * p3.y + f * p4.y;
	vPoint.z = a * p1.z + b * p2.z - c * p2.z + e * p3.z - f * p3.z + f * p4.z;

	return vPoint;
}

CVector3D CBSCSegment::GetLinearTangent(float t)
{
	CreateLinearPoints();

	double needLength = m_linearLength * t;
	float prevLength = 0;

	for (int i = 0; i < LINEAR_APPROXIMATION_COUNT; i++)
	{
		float currentLength = prevLength + (float)m_linearPoints[i];

		if ((prevLength <= needLength) && (currentLength >= needLength))
		{
			double insegmentFloat = 0.0;
			if (currentLength - prevLength != 0)	
			{
				insegmentFloat = (needLength - prevLength) / (currentLength - prevLength);
			}
			if (i + insegmentFloat != 0)
			{
				t = (float)(i + insegmentFloat) / (float)(LINEAR_APPROXIMATION_COUNT - 1);
				return GetTangent(t);
			}
			break;
		}

		prevLength = currentLength;
	}

	return GetTangent(t);
}

double CBSCSegment::GetLength()
{	
	return m_linearLength;
}

CBSCSegment::~CBSCSegment()
{
}