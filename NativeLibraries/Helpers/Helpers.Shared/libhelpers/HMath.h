#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d2d1.h>

// Defines for accessing XMVECTOR components without XMVectorGet.. , XMVectorSet.. methods
#ifdef _M_ARM

#define XF n128_f32[0]
#define YF n128_f32[1]
#define ZF n128_f32[2]
#define WF n128_f32[3]

#define XU32 n128_u32[0]
#define YU32 n128_u32[1]
#define ZU32 n128_u32[2]
#define WU32 n128_u32[3]

#define XI32 n128_i32[0]
#define YI32 n128_i32[1]
#define ZI32 n128_i32[2]
#define WI32 n128_i32[3]

#define F32 n128_f32

#define I8 n128_i8
#define I16 n128_i16
#define I32 n128_i32
#define I64 n128_i64

#define U8 n128_u8
#define U16 n128_u16
#define U32 n128_u32
#define U64 n128_u64

#else

#define XF m128_f32[0]
#define YF m128_f32[1]
#define ZF m128_f32[2]
#define WF m128_f32[3]

#define XU32 m128_u32[0]
#define YU32 m128_u32[1]
#define ZU32 m128_u32[2]
#define WU32 m128_u32[3]

#define XI32 m128_i32[0]
#define YI32 m128_i32[1]
#define ZI32 m128_i32[2]
#define WI32 m128_i32[3]

#define F32 m128_f32

#define I8 m128_i8
#define I16 m128_i16
#define I32 m128_i32
#define I64 m128_i64

#define U8 m128_u8
#define U16 m128_u16
#define U32 m128_u32
#define U64 m128_u64

#endif

class HMath {
public:

	/// <summary>
	/// Inscribes source rectangle into destination rectangle.
	/// If source rectangle is bigger than destination one then
	/// inscribed will be rectangle with maximum size that can
	/// be in destination rectangle and same aspect ratio as source rectangle.
	/// </summary>
	/// <param name="sourRect">Rectangle that need to inscribed.</param>
	/// <param name="destRect">Rectangle in which sourRect will bee inscribed.</param>
	/// <returns>Returns inscribed rectangle.</returns>
	static DirectX::XMFLOAT2 InscribeRect(const DirectX::XMFLOAT2 &sourRect, const DirectX::XMFLOAT2 &destRect) {
		DirectX::XMFLOAT2 inscribed = sourRect;
		bool widthLess = sourRect.x < destRect.x;
		bool heightLess = sourRect.y < destRect.y;
		bool hardCase = !widthLess && !heightLess;

		if (hardCase) {
			if (sourRect.x > sourRect.y) {
				float invAr = sourRect.y / sourRect.x;

				inscribed.x = destRect.x;
				inscribed.y = destRect.x * invAr;

				if (inscribed.y > destRect.y) {
					float hscale = destRect.y / inscribed.y;

					inscribed.x *= hscale;
					inscribed.y *= hscale;
				}
			}
			else {
				float ar = sourRect.x / sourRect.y;

				inscribed.x = destRect.y * ar;
				inscribed.y = destRect.y;

				if (inscribed.x > destRect.x) {
					float wscale = destRect.x / inscribed.x;

					inscribed.x *= wscale;
					inscribed.y *= wscale;
				}
			}
		}
		else {
			if (widthLess && !heightLess) {
				float hscale = destRect.y / sourRect.y;

				inscribed.x *= hscale;
				inscribed.y *= hscale;
			}

			if (!widthLess && heightLess) {
				float wscale = destRect.x / sourRect.x;

				inscribed.x *= wscale;
				inscribed.y *= wscale;
			}
		}

		return inscribed;
	}

	/*
	DirectXMath have bug in BoundingOrientedBox::Transform method:
	http://xboxforums.create.msdn.com/forums/p/113061/680807.aspx
	This implementation have fix for bug.
	*/
	static void BoundingOrientedBoxTransform(const DirectX::BoundingOrientedBox &_this, DirectX::BoundingOrientedBox &Out, DirectX::CXMMATRIX M) {
		// Load the box.
		DirectX::XMVECTOR vCenter = DirectX::XMLoadFloat3(&_this.Center);
		DirectX::XMVECTOR vExtents = DirectX::XMLoadFloat3(&_this.Extents);
		DirectX::XMVECTOR vOrientation = DirectX::XMLoadFloat4(&_this.Orientation);

		assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

		// Composite the box rotation and the transform rotation.
		DirectX::XMMATRIX nM;
		nM.r[0] = DirectX::XMVector3Normalize(M.r[0]);
		nM.r[1] = DirectX::XMVector3Normalize(M.r[1]);
		nM.r[2] = DirectX::XMVector3Normalize(M.r[2]);
		nM.r[3] = DirectX::g_XMIdentityR3;
		DirectX::XMVECTOR Rotation = DirectX::XMQuaternionRotationMatrix(nM);
		vOrientation = DirectX::XMQuaternionMultiply(vOrientation, Rotation);

		// Transform the center.
		vCenter = DirectX::XMVector3Transform(vCenter, M);

		// Scale the box extents.
		DirectX::XMVECTOR dX = DirectX::XMVector3Length(M.r[0]);
		DirectX::XMVECTOR dY = DirectX::XMVector3Length(M.r[1]);
		DirectX::XMVECTOR dZ = DirectX::XMVector3Length(M.r[2]);

		// bug:
		/*XMVECTOR VectorScale = XMVectorSelect(dX, dY, g_XMSelect1000);
		VectorScale = XMVectorSelect(VectorScale, dZ, g_XMSelect1100);*/
		// fix:
		DirectX::XMVECTOR VectorScale = DirectX::XMVectorSelect(dY, dX, DirectX::g_XMSelect1000);
		VectorScale = DirectX::XMVectorSelect(dZ, VectorScale, DirectX::g_XMSelect1100);
		vExtents = DirectX::XMVectorMultiply(vExtents, VectorScale);
		// Store the box.
		DirectX::XMStoreFloat3(&Out.Center, vCenter);
		DirectX::XMStoreFloat3(&Out.Extents, vExtents);
		DirectX::XMStoreFloat4(&Out.Orientation, vOrientation);
	}

	static DirectX::ContainmentType BoundingBoxContains(const DirectX::BoundingBox &_this, const DirectX::BoundingOrientedBox& box) {
		if (!box.Intersects(_this))
			return DirectX::ContainmentType::DISJOINT;

		DirectX::XMVECTOR vCenter = DirectX::XMLoadFloat3(&_this.Center);
		DirectX::XMVECTOR vExtents = DirectX::XMLoadFloat3(&_this.Extents);

		// Subtract off the AABB center to remove a subtract below
		DirectX::XMVECTOR oCenter = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&box.Center), vCenter);

		DirectX::XMVECTOR oExtents = DirectX::XMLoadFloat3(&box.Extents);
		DirectX::XMVECTOR oOrientation = DirectX::XMLoadFloat4(&box.Orientation);

		assert(DirectX::Internal::XMQuaternionIsUnit(oOrientation));

		DirectX::XMVECTOR Inside = DirectX::XMVectorTrueInt();

		for (size_t i = 0; i < DirectX::BoundingOrientedBox::CORNER_COUNT; ++i)
		{
			DirectX::XMVECTOR C = DirectX::XMVectorAdd(DirectX::XMVector3Rotate(DirectX::XMVectorMultiply(oExtents, DirectX::g_BoxOffset[i]), oOrientation), oCenter);
			DirectX::XMVECTOR d = DirectX::XMVectorAbs(C);
			Inside = DirectX::XMVectorAndInt(Inside, DirectX::XMVectorLessOrEqual(d, vExtents));
		}

		return (DirectX::XMVector3EqualInt(Inside, DirectX::XMVectorTrueInt())) ? DirectX::ContainmentType::CONTAINS : DirectX::ContainmentType::INTERSECTS;
	}

	static DirectX::XMMATRIX XMMatrixFrom(const D2D1_MATRIX_3X2_F &v) {
		DirectX::XMMATRIX m = DirectX::XMMatrixIdentity();

		m.r[0].XF = v._11;
		m.r[0].YF = v._12;

		m.r[1].XF = v._21;
		m.r[1].YF = v._22;

		m.r[3].XF = v._31;
		m.r[3].YF = v._32;

		return m;
	}

	static D2D1::Matrix3x2F D2D1Matrix3X2From(DirectX::CXMMATRIX v) {
		D2D1::Matrix3x2F m;

		m._11 = v.r[0].XF;
		m._12 = v.r[0].YF;

		m._21 = v.r[1].XF;
		m._22 = v.r[1].YF;

		m._31 = v.r[3].XF;
		m._32 = v.r[3].YF;

		return m;
	}

	static DirectX::XMVECTOR GetAABBSizeFrom(DirectX::CXMMATRIX matrix) {
		DirectX::XMVECTOR axisX = DirectX::XMVectorAbs(matrix.r[0]);
		DirectX::XMVECTOR axisY = DirectX::XMVectorAbs(matrix.r[1]);
		DirectX::XMVECTOR axisZ = DirectX::XMVectorAbs(matrix.r[2]);

		DirectX::XMVECTOR addXY = DirectX::XMVectorAdd(axisX, axisY);
		DirectX::XMVECTOR addXYZ = DirectX::XMVectorAdd(addXY, axisZ);

		return addXYZ;
	}

	static float GetRadsToDegsScale() {
		const float scale = (180.0f / DirectX::XM_PI);
		return scale;
	}

	static float GetDegsToRadsScale() {
		const float scale = (DirectX::XM_PI / 180.0f);
		return scale;
	}

	static float GetAngleBetweenLines(DirectX::CXMVECTOR p00, DirectX::CXMVECTOR p01, DirectX::CXMVECTOR p10, DirectX::CXMVECTOR p11) {
		auto p0 = DirectX::XMVectorSubtract(p01, p00);
		auto p1 = DirectX::XMVectorSubtract(p11, p10);

		return HMath::GetAngleBetweenVectors(p0, p1);
	}

	static float GetAngleBetweenVectors(DirectX::CXMVECTOR p0, DirectX::CXMVECTOR p1, float *angle0 = nullptr, float *angle1 = nullptr) {
		auto x = DirectX::XMVectorPermute<0, 4, 0, 0>(p0, p1);
		auto y = DirectX::XMVectorPermute<1, 5, 0, 0>(p0, p1);

		auto tmp = DirectX::XMVectorATan2(y, x);

		if (angle0) {
			*angle0 = tmp.F32[0];
		}

		if (angle1) {
			*angle1 = tmp.F32[1];
		}

		float angle = tmp.F32[1] - tmp.F32[0];
		return angle;
	}

	// cmyk in range 0.0 1.0
	// rgb in range 0.0 1.0
	// rgb = xyz
	// cmyk = xyzw
	// based on http://www.rapidtables.com/convert/color/cmyk-to-rgb.htm
	static DirectX::XMFLOAT3 CMYKtoRGB(const DirectX::XMFLOAT4 cmyk) {
		DirectX::XMFLOAT3 res;

		res.x = (1.0f - cmyk.x) * (1.0f - cmyk.w);
		res.y = (1.0f - cmyk.y) * (1.0f - cmyk.w);
		res.z = (1.0f - cmyk.z) * (1.0f - cmyk.w);

		return res;
	}

	static void Concat(const D2D1_RECT_F &a, const D2D1_RECT_F &b, D2D1_RECT_F &result) {
		if (a.right >= a.left && a.bottom >= a.top &&
			b.right >= b.left && b.bottom >= b.top)
		{
			D2D1_RECT_F resultTmp;

			resultTmp.left = (std::min)(a.left, b.left);
			resultTmp.top = (std::min)(a.top, b.top);
			resultTmp.right = (std::max)(a.right, b.right);
			resultTmp.bottom = (std::max)(a.bottom, b.bottom);

			result = resultTmp;
		}
	}

	static DirectX::XMVECTOR ToCameraSpace(
		DirectX::CXMMATRIX projection,
		DirectX::CXMMATRIX camera,
		const DirectX::XMFLOAT2 &screenPos,
		const DirectX::XMFLOAT2 &screenSize,
		float cameraZ,
		float resultZ = 1.0f)
	{
		DirectX::XMVECTOR res;
		auto tmpPt = DirectX::XMVectorSet(0, 0, resultZ - cameraZ, 1);

		tmpPt = DirectX::XMVector3TransformCoord(tmpPt, projection);

		DirectX::XMVECTOR pt = DirectX::XMVectorSet(screenPos.x, screenPos.y, tmpPt.ZF, 1.0f);

		res = DirectX::XMVector3Unproject(
			pt,
			0.0f, 0.0f, screenSize.x, screenSize.y,
			0.0f, 1.0f,
			projection,
			camera,
			DirectX::XMMatrixIdentity());


		return res;
	}

	

	template <typename T> 
	static int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}
	
	static DirectX::XMVECTOR Signum(DirectX::CXMVECTOR v) {
		auto cmp1 = DirectX::XMVectorLess(DirectX::g_XMZero, v);
		auto cmp2 = DirectX::XMVectorLess(v, DirectX::g_XMZero);

		auto v1 = DirectX::XMVectorSelect(DirectX::g_XMZero, DirectX::g_XMOne, cmp1);
		auto v2 = DirectX::XMVectorSelect(DirectX::g_XMZero, DirectX::g_XMOne, cmp2);

		auto signum = DirectX::XMVectorSubtract(v1, v2);
		return signum;
	}

	// v < 0 = -1
	// v >= 0 = +1;
	static DirectX::XMVECTOR Sign(DirectX::CXMVECTOR v) {
		auto mask = DirectX::XMVectorAndInt(v, DirectX::XMVectorSplatSignMask());
		auto sign = DirectX::XMVectorOrInt(DirectX::g_XMOne, mask);

		return sign;
	}

	static int GetIntersectingFactor(const D2D1_RECT_F& annotRect, const DirectX::XMFLOAT2& pageDimensions)
	{
		int factor = 0;

		factor = -1 * (annotRect.bottom <= 0 ||
			annotRect.top <= 0 ||
			annotRect.right >= pageDimensions.x ||
			annotRect.left >= pageDimensions.x ||
			annotRect.bottom >= pageDimensions.y ||
			annotRect.top >= pageDimensions.y);

		factor += annotRect.right <= 0 ||
			annotRect.left <= 0;

		return factor;
	}

	static void InscribePointInRect(D2D1_POINT_2F &point, const D2D1_RECT_F& rect)
	{
		if (point.x < rect.left)
			point.x = rect.left;
		if (point.x > rect.right)
			point.x = rect.right;

		if (point.y < rect.top)
			point.y = rect.top;
		if (point.y > rect.bottom)
			point.y = rect.bottom;
	}

	static void InscribeRectInRect(D2D1_RECT_F &rect, const D2D1_RECT_F& targetRect)
	{
		if (rect.left < targetRect.left)
			rect.left = targetRect.left;

		if (rect.right < targetRect.left)
			rect.right = targetRect.left;

		if (rect.left > targetRect.right)
			rect.left = targetRect.right;

		if (rect.right > targetRect.right)
			rect.right = targetRect.right;

		if (rect.top < targetRect.top)
			rect.top = targetRect.top;

		if (rect.bottom < targetRect.top)
			rect.bottom = targetRect.top;

		if (rect.top > targetRect.bottom)
			rect.top = targetRect.bottom;

		if (rect.bottom > targetRect.bottom)
			rect.bottom = targetRect.bottom;
	}

	static DirectX::XMFLOAT2 GetScale(const D2D1_MATRIX_3X2_F &v) {
		DirectX::XMFLOAT2 scale;
		DirectX::XMVECTOR xVec = DirectX::XMVectorSet(v._11, v._12, 0.0f, 0.0f);
		DirectX::XMVECTOR yVec = DirectX::XMVectorSet(v._21, v._22, 0.0f, 0.0f);

		scale.x = DirectX::XMVector2Length(xVec).XF;
		scale.y = DirectX::XMVector2Length(yVec).XF;

		return scale;
	}

	static void GetComponents(
		const D2D1_MATRIX_3X2_F &v, 
		DirectX::XMFLOAT2 &scale, 
		DirectX::XMFLOAT2 &xVec, 
		DirectX::XMFLOAT2 &yVec) 
	{
		DirectX::XMVECTOR xVecTmp = DirectX::XMVectorSet(v._11, v._12, 0.0f, 0.0f);
		DirectX::XMVECTOR yVecTmp = DirectX::XMVectorSet(v._21, v._22, 0.0f, 0.0f);

		scale.x = DirectX::XMVector2Length(xVecTmp).XF;
		scale.y = DirectX::XMVector2Length(yVecTmp).XF;

		// normalize
		xVecTmp = DirectX::XMVectorScale(xVecTmp, 1.0f / scale.x);
		yVecTmp = DirectX::XMVectorScale(yVecTmp, 1.0f / scale.y);

		DirectX::XMStoreFloat2(&xVec, xVecTmp);
		DirectX::XMStoreFloat2(&yVec, yVecTmp);
	}

	static void SetComponents(
		D2D1_MATRIX_3X2_F &v,
		const DirectX::XMFLOAT2 &scale,
		const DirectX::XMFLOAT2 &xVec,
		const DirectX::XMFLOAT2 &yVec) 
	{
		auto xVecTmp = DirectX::XMLoadFloat2(&xVec);
		auto yVecTmp = DirectX::XMLoadFloat2(&yVec);

		xVecTmp = DirectX::XMVectorScale(xVecTmp, scale.x);
		yVecTmp = DirectX::XMVectorScale(yVecTmp, scale.y);

		v._11 = xVecTmp.XF;
		v._12 = xVecTmp.YF;

		v._21 = yVecTmp.XF;
		v._22 = yVecTmp.YF;
	}

	static uint32_t NextPowerOf2(uint32_t v) {
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	template<class T>
	static T Clamp(T value, T min, T max) {
		T res = (std::min)((std::max)(min, value), max);
		return res;
	}

	template<class T>
	static T Lerp(T start, T end, T t) {
		T res = start + (end - start) * t;
		return res;
	}
};

inline bool operator==(const D2D1::ColorF &a, const D2D1::ColorF &b) {
	bool res = a.a == b.a && a.r == b.r && a.g == b.g && a.b == b.b;
	return res;
}

inline DirectX::XMFLOAT3 operator/ (const DirectX::XMFLOAT3& a,
	const DirectX::XMFLOAT3& b) {
	return DirectX::XMFLOAT3(a.x / b.x,
		a.y / b.y,
		a.z / b.z);
}