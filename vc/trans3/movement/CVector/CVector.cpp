/*
 * All contents copyright 2005 Jonathan D. Hughes
 * All rights reserved. You may not remove this notice
 * Read license.txt for licensing details.
 */

/*
 * CVector.cpp - Vector collision implementation.
 */

#include "CVector.h"
#include <math.h>
#include "../movement.h"

/*
 * Default constructor.
 */
CVector::CVector(void):
m_closed(false),
m_curl(CURL_NDEF)
{
	// Create an void vector of no points.
	RECT bounds = {0, 0, 0, 0}; 
	m_bounds = bounds;
	m_p.clear();
}

/*
 * Point constructor. Note reserve need not be the exact number of points.
 */
CVector::CVector(const double x, const double y, const int reserve):
m_closed(false),
m_curl(CURL_NDEF)
{
	// Create a vector of one point.
	RECT bounds = {x, y, x, y}; 
	m_bounds = bounds;
	m_p.reserve(reserve);
	push_back(x, y);
}

/*
 * Point constructor. Note reserve need not be the exact number of points.
 */
CVector::CVector(const DB_POINT p, const int reserve):
m_closed(false),
m_curl(CURL_NDEF)
{
	// Create a vector of one point.
	RECT bounds = {p.x, p.y, p.x, p.y}; 
	m_bounds = bounds;
	m_p.reserve(reserve);
	push_back(p);
}

/*
 * Addition assignment operator - moves the entire vector.
 */
CVector &CVector::operator+= (const DB_POINT p)
{
	// Offset each point by the passed values.
	for (DB_ITR i = m_p.begin(); i != m_p.end(); ++i)	
	{
		i->x += p.x;
		i->y += p.y;
	}

	// Offset the bounding box.
	m_bounds.right += p.x;
	m_bounds.left += p.x;
	m_bounds.top += p.y;
	m_bounds.bottom += p.y;
	return *this;
}

/*
 * Push a point onto the end of the vector.
 */
void CVector::push_back(DB_POINT p)
{
	// If closed, don't allow more points to be added.
	if (m_closed) return;

	// Check we're not adding the same point twice.
	if (size() > 1)
	{
		if (p == m_p.back()) return;
	}

	// Push a point onto the vector.
	m_p.push_back(p);
}	

/*
 * Push a point onto the end of the vector.
 */
void CVector::push_back(const double x, const double y)
{
	DB_POINT p = {x, y};
	this->push_back(p);
}


/*
 * Push an array of points onto the end of the vector.
 */
void CVector::push_back(const DB_POINT pts[], const short size)
{
	for (int i = 0; i != size; ++i)
	{
		this->push_back(pts[i]);
	}
}

/*
 * Create the bounding box of the vector.
 */
void CVector::boundingBox(RECT &rect)
{
	// Create a box in which the vector is contained.
	rect.bottom = rect.left = rect.right = rect.top = 0;

	// Loop over subvectors and find biggest and smallest points.
	for (DB_ITR i = m_p.begin(); i != m_p.end(); ++i)
	{
		if ((i->x < rect.left) || (i == m_p.begin())) rect.left = i->x;
		if ((i->y < rect.top) || (i == m_p.begin())) rect.top = i->y;
		if (i->x > rect.right) rect.right = i->x;
		if (i->y > rect.bottom) rect.bottom = i->y;
	}
}

/*
 * Seal the vector: create a polygon and prevent 
 * further points from being added.
 */
bool CVector::close(const bool isClosed/*, const int curl*/)
{
	boundingBox(m_bounds);			// Make the bounding box.

	if ((size() > 1) && (isClosed))
	{
		// If this is closed, add the first points as the last (for curl estimate).
		push_back(m_p.front().x, m_p.front().y);
		m_closed = isClosed;

		// Assign the curl. Estimate if needed.
		// m_curl = (!curl ? estimateCurl() : curl);
		m_curl = estimateCurl();
	}
	return true;
}

/*
 * Determine if a polygon intersects or contains a CVector.
 * Return a point that represents the intersecting subvector
 * as a position vector for use in sliding tests, etc.
 */
bool CVector::contains(CVector &rhs, DB_POINT &ref)
{
	/*
	 * The rhs CVector intersects the polygon if any of the rhs's
	 * points lie within it, or if any of the CVector's lines
	 * intersect any of the polygon's lines.
	 */

	// Reset ref, but not to zero (div 0 error).
	ref.x = ref.y = 1;

	// Check the pointer (for selected player checking).
	if (&rhs == this) return false;

	// Do a bounding box test for the entire rhs vector.
	if ((rhs.m_bounds.right < m_bounds.left) || (rhs.m_bounds.left > m_bounds.right) ||
		(rhs.m_bounds.bottom < m_bounds.top) || (rhs.m_bounds.top > m_bounds.bottom)) 
		return false;

	// Check for boundary collisions first.
	// CVECTOR_TYPE tt = intersect(rhs, ref);
//	if (intersect(rhs, ref) != TT_NORMAL) return m_type;

	/* BOARD VECTOR */
	// Loop over the subvectors in this vector (to size() - 1).
	for (DB_ITR i = m_p.begin(); i != m_p.end() - 1; ++i)	
	{
		if (intersect(i, rhs))
		{
			ref.x = (i + 1)->x - i->x;
			ref.y = (i + 1)->y - i->y;

/*			if (m_type & TT_UNIDIRECTIONAL)
			{
				// Continue one point for the case of corners.
				if (intersect(++i, rhs))
				{
					ref.x = (i + 1)->x - i->x;
					ref.y = (i + 1)->y - i->y;
				}
			}
*/
			return true;
		}
	}

	// We may be completely inside the vector.
	// Check we have a closed object.
	if (m_closed && m_curl)
	{
		/* SPRITE VECTOR */
		// Loop over the points of the rhs vector.
		for (i = rhs.m_p.begin(); i != rhs.m_p.end(); ++i)
		{
			// Determine if this point is contained in the polygon.
			if (containsPoint(*i) % 2 == 1) return true;
		}
	}
	return false;
}

/*
 * Determine if a polygon intersects or contains a CVector.
 * Return z-order flags describing if a collision occurred and
 * whether the source is above or below the target (based on the
 * number of times the target's boundaries are crossed.
 */
ZO_ENUM CVector::contains(CVector &rhs/*, DB_POINT &ref*/)
{
	/*
	 * The rhs CVector intersects the polygon if any of the rhs's
	 * points lie within it, or if any of the CVector's lines
	 * intersect any of the polygon's lines.
	 */

	// Check the pointer (for selected player checking).
	if (&rhs == this) return ZO_NONE;

	// Do a bounding box test for the entire rhs vector.
	if ((rhs.m_bounds.right < m_bounds.left) ||
		(rhs.m_bounds.left > m_bounds.right) ||
		(rhs.m_bounds.bottom < m_bounds.top) ||
		(rhs.m_bounds.top > m_bounds.bottom))
	{
		// No overlap - no strict z-order.
		return ZO_NONE;
	}

	ZO_ENUM zo = ZO_NONE;

	// Loop over the subvectors in this vector (to size() - 1).
	for (DB_ITR i = m_p.begin(); i != m_p.end() - 1; ++i)	
	{
		if (intersect(i, rhs))
		{
			zo = ZO_COLLIDE;

//			ref.x = (i + 1)->x - i->x;
//			ref.y = (i + 1)->y - i->y;
		}
	}

	// We may be completely inside the vector.
	// Check we have a closed object.

	if (m_closed && m_curl)
	{
		// Loop over the points of the rhs vector.
		for (i = rhs.m_p.begin(); i != rhs.m_p.end(); ++i)
		{
			// Determine if this point is contained in the polygon.
			// Returns the number of times the target vector's borders
			// were crossed.
			int count = containsPoint(*i);
			if (count % 2 == 1) 
			{
				// A point is contained.
				zo = ZO_ENUM (zo | ZO_COLLIDE);
			}
			else if (count)
			{
				// Even count > 0.
				zo = ZO_ENUM (zo | ZO_ABOVE);
			}
		}
	}
	return zo;
}

/*
 * Determine if a point lies on a subvector.
 */		
bool CVector::pointOnLine(const DB_ITR &i, const DB_POINT &p) const
{
	/*
	 * In some cases, due to the finite capacity of variables, some
	 * points will not evaluate as being on the line even if algebraically
	 * they are. Hence compare to a precision of ~1 pixel.
	 */
	if ((p.y < i->y - CV_PRECISION) && (p.y < (i + 1)->y - CV_PRECISION) ||
		(p.y > i->y + CV_PRECISION) && (p.y > (i + 1)->y + CV_PRECISION) ||
		(p.x < i->x - CV_PRECISION) && (p.x < (i + 1)->x - CV_PRECISION) ||
		(p.x > i->x + CV_PRECISION) && (p.x > (i + 1)->x + CV_PRECISION)) 
	{
		// Bounding box check.
		return false;
	} 
	else 
	{
		// Evaluate the straight-line equation.
		if (isVertical(i)) 
		{
			if (p.x == i->x) return true;
		}
		else
		{
			if (abs(p.y - (gradient(i) * p.x + intercept(i))) < CV_PRECISION) return true;
		}
	}
	return false;
}

/*
 * Determine if a polygon contains a point (a CVector point, or any other).
 * Return the number of times the boundary was crossed (for z-ordering).
 */
int CVector::containsPoint(const DB_POINT p)
{
	/*
	 * Determine if a point lies within a polygon by counting the number
	 * of times a test vector from the point to the top of the board crosses 
	 * the polygon's boundaries. The vector must cross an odd number of 
	 * boundaries if the point is inside the polygon.
	 * We can also use this method to z-order sprites by their bases:
	 * If the sprite is "above" the vector on the screen, the boundaries
	 * will not be crossed.
	 * If the sprite is below the vector, the boundaries will be crossed
	 * (an even number of times).
	 */

	// Do a bounding box test for *this* point only.
	// (As opposed to the whole vector check in contains().)
	// Return 0 boundaries crossed for left, right or above.
	// Return 2 boundaries crossed for below.
	if ((p.x < m_bounds.left) || 
		(p.x > m_bounds.right) ||
		(p.y < m_bounds.top)) return 0;
	if	(p.y > m_bounds.bottom) return 2;

	/*
	 * We need to consider the situation where we pass through a
	 * point in the polygon - this method would count two boundary
	 * crosses, so we need to compare successive vectors to determine
	 * if we should count them once or twice. We record the last boundary
	 * point and the last vector direction (right->left or left->right).
	 */
	int count = 0; 

	/*
	 * Record the last vector's values to compare to the first vector,
	 * since they are successive in the shape but are not in the loop.
	 */
	DB_ITR i = m_p.end() - 2;				// Start of the last vector.
	bool lastDir = ((i + 1)->x > i->x);		// true = right, false = left.
	double lastY = gradient(i) * p.x + intercept(i);

	for (i = m_p.begin(); i != m_p.end() - 1; ++i)	
	{
		// Skip this subvector if line is parallel to test vector.
		if (isVertical(i)) continue;

		// Check if the point is in the column created by the vector.
		if (((i->x >= p.x) && ((i + 1)->x <= p.x)) ||
			((i->x <= p.x) && ((i + 1)->x >= p.x)))
		{
			// Calculate intersection point of the subvector and a
			// vertical vector from the point to the top of the board.
			const double y = gradient(i) * p.x + intercept(i);
			const bool dir = ((i + 1)->x > i->x);

			// Check the point is on the test vector.
			if ((y < p.y) &&
			   (((y == lastY) && (dir != lastDir)) || (y != lastY)))
			{
				// Check that if we are at a corner, we make the correct
				// number of counts (depending on direction of the two 
				// vectors making corner).
				++count;
			}
			lastY = y;
			lastDir = ((i + 1)->x > i->x);
		}
	} // for (i)

	// If the vertical vector has crossed the polygon boundary an odd
	// number of times, the point is inside the polygon.
	return count;
}

/*
 * Create a mask by drawing a closed area and flooding.
 * (Currently using the GDI).
 */
bool CVector::createMask(CGDICanvas *cnv, const int x, const int y, CONST LONG color)
{
	// Can't create a mask from an open vector (or a null canvas).
	if (!m_closed || !cnv) return false;

	// Could end up drawing lines off the canvas.
	if (x > m_bounds.left || y > m_bounds.top) return false;

	// Set up to draw using GDI.
	CONST HDC hdc = cnv->OpenDC();
	HPEN pen = CreatePen(0, 1, color);
	HGDIOBJ m = SelectObject(hdc, pen);

	// Draw the bounding box.
	for (DB_ITR i = m_p.begin(); i != m_p.end(); ++i)
	{
		if (i != m_p.end() - 1)
		{
			MoveToEx(hdc, i->x - x, i->y - y, NULL);
			LineTo(hdc, (i + 1)->x - x, (i + 1)->y - y);
		}
	}
	SelectObject(hdc, m);
	DeleteObject(pen);

	// Flood the created area.
    HBRUSH brush = CreateSolidBrush(color);
    m = SelectObject(hdc, brush);

	// Find a point that is contained in the vector.
	// Work down a vertical line along the centre of the vector
	// and check if the point is both contained in the vector and
	// is not on the outline (if GetPixel returns the same colour as
	// we're flooding with, the flood won't work.

	DB_POINT p = {(m_bounds.left + m_bounds.right) / 2, m_bounds.top};
	for (; p.y != m_bounds.bottom; ++p.y)
	{
		if (containsPoint(p) && GetPixel(hdc, p.x - x, p.y - y) != color)
		{
			ExtFloodFill(hdc, p.x - x, p.y - y, color, FLOODFILLBORDER);
			break;
		}
	}

	SelectObject(hdc, m);
	DeleteObject(brush);

	cnv->CloseDC(hdc);

	return true;
}

/*
 * Draw the vector / polygon onto a canvas, offset by x,y. 
 */
void CVector::draw(CONST LONG color, const bool drawText, const int x, const int y, CGDICanvas *const cnv)
{
	for (DB_ITR i = m_p.begin(); i != m_p.end(); ++i)
	{
		if (i != m_p.end() - 1)
		{
			cnv->DrawLine(i->x - x, i->y - y, (i + 1)->x - x, (i + 1)->y - y, color);
		}

		// Draw the co-ordinates for each corner.
		if (drawText)
		{
			std::string text; 
			char c [5]; 
			text = gcvt (i->x, 5, c);
			cnv->DrawText(i->x - x, i->y - y, text, "Arial", 10, color);
			text = gcvt (i->y, 5, c);
			cnv->DrawText(i->x - x, i->y - y + 8, text, "Arial", 10, color);
		}
	}
}

/*
 * Estimate which area is "inside" the polygon created 
 * by the vector (if a polygon exists).
 */
int CVector::estimateCurl(void)
{
	if (!m_closed) return CURL_NDEF;

	/*
	 * Curl is estimated by summing the internal angles
	 * of the polygon. The obtuse and acute angles are summed
	 * for each point - the smaller total indicates the inner edge.
	 *
	 * "Right" curl is defined as anti-clockwise movement of subvectors.
	 * "Left" curl is defined as clockwise movement of subvectors.
	 */

	// NOTE! This is for use in the board editor, and users 
	// can "flip" the curl if it is mis-calculated.
	// NOTE! Currently misses last angle (last vector -> first vector).

	double lAngle = 0, rAngle = 0;

	for (DB_ITR i = m_p.begin(); i != m_p.end() - 2; ++i)	
	{
		/*
		 * We use the vector and scalar products to determine the angle between
		 * consecutive vectors. The scalar product contains the angle, but we
		 * need the vector product to indicate which angle we've measured (as
		 * arccos will always return the lesser of the two).
		 */

		// Skip parallel lines as conributions to sums are equal.
		if (gradient(i) == gradient(i + 1)) continue;

		// Dimensions of vectors.
		const double dx1 = (i + 1)->x - i->x;
		const double dy1 = (i + 1)->y - i->y;

		const double dx2 = (i + 2)->x - (i + 1)->x;
		const double dy2 = (i + 2)->y - (i + 1)->y;

		/*
		 * Using the scalar (dot) product: a.b = |a||b|cos(c).
		 * For our two subvectors end-on-end, the angle we want is (180 - c).
		 * Conveniently, cos(180 - c) = -cos(c).
		 * c = arccos(a.b / |a||b|)
		 */
		double angle = RADIAN * acos(-(dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2)));

		/*
		 * Vector (cross) product returns a vector in the z-plane.
		 * [k] represents the z unit vector (the normal).
		 * a x b = |a||b|sin(c)[k] = (ax*by - ay*bx)[k] (for a,b in 2 dimensions)
		 * Note that we cannot use the vector product to find the angle since arcsin only 
		 * returns in the range -90 to +90 degrees - so we use the scalar product instead.
		 *
		 * double normal = (dx1 * dy2 - dy1 * dx2);
		 *
		 * If the normal is -ve, this angle is the right-curl inner angle.
		 * If the normal is +ve, this is the left-curl inner angle.
		 * NB: this would be the other way around normally, but we've flipped the 
		 * y-axis on the board so the normal points in the opposite direction.
		 */
		if ((dx1 * dy2 - dy1 * dx2) > 0) angle = 360 - angle;
		rAngle += angle;
		lAngle += 360 - angle;
	} // for (i)

	// The inside will be the edge with the smallest sum of angles.
	return (rAngle < lAngle ? CURL_RIGHT : CURL_LEFT);
}

/*
 * Calculate gradient. Return GRAD_INF for vertical lines
 * (need something other than zero for gradient comparisons).
 */				
double CVector::gradient(const DB_ITR &i) const
{
	// Avoid divide by zero.
	if (isVertical(i)) return GRAD_INF;

	return (((i + 1)->y - i->y) / ((i + 1)->x - i->x));
}

/*
 * Calculate Y-axis intercept, c = y - mx.
 */
double CVector::intercept(const DB_ITR &i) const
{
	// Avoid vertical lines - return (unless other?).
	if (isVertical(i)) return 0;

	return (i->y - (gradient(i) * i->x));
}

/*
 * Determine if a CVector intersects this CVector.
 * Return a point that represents the intersecting subvector
 * as a position vector for use in sliding tests, etc.
 */
bool CVector::intersect(CVector &rhs, DB_POINT &ref)
{
	// Check the rhs vector.
	if (&rhs == this) return false;

	/* BOARD VECTOR */
	// Loop over the subvectors in this vector (to size() - 1).
	for (DB_ITR i = m_p.begin(); i != m_p.end() - 1; ++i)	
	{
		const double m1 = gradient(i), c1 = intercept(i);

		/* SPRITE VECTOR */
		// Loop over the subvectors in the target vector.
		for (DB_ITR j = rhs.m_p.begin(); j != rhs.m_p.end() - 1; ++j)	
		{
			// Calculate gradient, intercept.
			const double m2 = rhs.gradient(j), c2 = rhs.intercept(j);

			// Skip this subvector if lines are parallel.
			if (m1 == m2) continue;

			// Deal with vertical lines.
			if (isVertical(i)) 
			{
				ref.x = i->x;
				ref.y = m2 * ref.x + c2;
			} 
			else if (rhs.isVertical(j)) 
			{
				ref.x = j->x;
				ref.y = m1 * ref.x + c1;
			} 
			else 
			{
				// Solve the equations.
				ref.x = (c2 - c1) / (m1 - m2);
				// Unless m2 is 0, use m1 as it is more likely to be more simply shaped.
				ref.y = (!m2 ? c2 : m1 * ref.x + c1);
			}

			// Determine if this point lies on either line.
			if ((pointOnLine(i, ref)) && (rhs.pointOnLine(j, ref)))
			{
				/*
				 * Return a point that represents this board's subvector
				 * as a position vector (i.e. with origin at 0, 0), so
				 * that we can test for one-way movement and collision
				 * angle with the velocity vector.
				 */
				ref.x = (i + 1)->x - i->x;
				ref.y = (i + 1)->y - i->y;

				// Return the tile type.
				return true;
			}
		} // for (j)
	} // for (i)

	return false;
}

/* Internal function to be contained in a loop over i */

/*
 * Determine if a CVector intersects this CVector.
 */
bool CVector::intersect(DB_ITR &i, CVector &rhs)
{
	const double m1 = gradient(i), c1 = intercept(i);
	DB_POINT p = {0, 0};

	/* SPRITE VECTOR */
	// Loop over the subvectors in the target vector.
	for (DB_ITR j = rhs.m_p.begin(); j != rhs.m_p.end() - 1; ++j)	
	{
		// Calculate gradient, intercept.
		const double m2 = rhs.gradient(j), c2 = rhs.intercept(j);

		// Skip this subvector if lines are parallel.
		if (m1 == m2) continue;

		// Deal with vertical lines.
		if (isVertical(i)) 
		{
			p.x = i->x;
			p.y = m2 * p.x + c2;
		} 
		else if (rhs.isVertical(j)) 
		{
			p.x = j->x;
			p.y = m1 * p.x + c1;
		} 
		else 
		{
			// Solve the equations.
			p.x = (c2 - c1) / (m1 - m2);
			// Unless m2 is 0, use m1 as it is more likely to be more simply shaped.
			p.y = (!m2 ? c2 : m1 * p.x + c1);
		}

		// Determine if this point lies on either line.
		if ((pointOnLine(i, p)) && (rhs.pointOnLine(j, p)))
		{
			return true;
		}
	} // for (j)

	return false;
}

/*
 * Path-find ::contains() equivalent.
 */
bool CVector::pfContains(CVector &rhs)
{
	/*
	 * pf-specific: the nodes of the path are corners of the
	 * board vectors, so standard collision detection will always
	 * block movement at these points.
	 * But the pf vectors have been grown and we want to allow
	 * movement between consecutive points on the vector.
	 */

	// Do a bounding box test for the entire rhs vector.
	if ((rhs.m_bounds.right < m_bounds.left) ||
		(rhs.m_bounds.left > m_bounds.right) ||
		(rhs.m_bounds.bottom < m_bounds.top) ||
		(rhs.m_bounds.top > m_bounds.bottom))
	{
		return false;
	}

	// rhs will only be comprise two points.
	DB_POINT start = rhs.m_p.front(), end = rhs.m_p.back();

	DB_ITR first = NULL, last = NULL;

	// Loop over the subvectors in this vector (to size() - 1).
	for (DB_ITR i = m_p.begin(); i != m_p.end() - 1; ++i)	
	{
		// Check if *both* of the points of the rhs vector are in this vector.
		if (*i == start || *i == end)
		{
			if (!first) first = i;
			else
			{
				last = i;
				// We have two points on the vector.
				// Are they consecutive? If so, we can move between them.
				// If one of the points is concave, we can also move to
				// the next point.
				// TO BE DONE.
				if (abs(first - last) == 1)
					// Consecutive.
					return false;
				else
					return true;
			}
		}
		else
		{
			if (*(i + 1) != start && *(i + 1) != end)
			{
				// Check the other subvectors of this that don't
				// connect to these.
				if (intersect(i, rhs)) return true;
			}
		}

	} // for (i)

	// No internal point requirements (except for goal, perhaps).

}

/*
 * Expand the vector around its origin.
 */
void CVector::grow(const int width, const int height)
{
	if (!m_closed) return;

	DB_POINT centre = { (m_bounds.left + m_bounds.right) * 0.5,
						(m_bounds.top + m_bounds.bottom) * 0.5};

	for (DB_ITR i = m_p.begin(); i != m_p.end(); ++i)
	{
		// The nodes need to be offset from the vector points so that
		// they do not interfere with collision checks and sprites
		// can reach to them.

		// Temporary solution: create two offset nodes (useful for
		// open CVectors, concave points).
		i->x += width * sgn(i->x - centre.x);
		i->y += height * sgn(i->y - centre.y);
	}
}

/*
 * Construct CPathFind nodes from a CVector and add to the nodes vector.
 */
void CVector::createNodes(std::vector<DB_POINT> &points, const DB_POINT max)
{
	for (DB_ITR i = m_p.begin(); i != m_p.end(); ++i)
	{
		if (i->x > 0 && i->y > 0 && i->x < max.x && i->y < max.y) 
		{
			points.push_back(*i);
		}
	}
}
