#include "Sphere.h"
#include "Scene.h"

Sphere::Sphere(const Scene* scene_, const int center_, float radius_, int material_, Texture* texture_, Texture* normalTexture_,
	const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_)
	: Object(scene_, material_, texture_, normalTexture_, matrix_, transform_, motionVector_, motion_), centerVertexId(center_), radius(radius_)
{
	count = 0;
	Vec3f center = scene->vertexData[centerVertexId].position;
	Vec3f r = Vec3f(radius, radius, radius);
	Vec3f minCorner = center - r;
	Vec3f maxCorner = center + r;

	boundingBox = BoundingBox(minCorner, maxCorner);
	if (transform == true)
	{
		// Transform bounding box to world coords.
		boundingBox.applyTransformation(transformationMatrix);
	}

	if (motionBlur == true)
	{
		Vec3f minCornerLowerBound = boundingBox.minCorner - motionVector;
		Vec3f maxCornerUpperBound = boundingBox.maxCorner + motionVector;

		boundingBox.mergeBoundingBox(BoundingBox(minCornerLowerBound, maxCornerUpperBound));
	}
}

bool Sphere::intersection(const Ray& ray, Hit& hit)
{
	bool result = false;

	Ray transformedRay = transformRay(ray);
	Vec3f center = scene->vertexData[centerVertexId].position;

	float a = transformedRay.direction.dotProduct(transformedRay.direction);
	float b = 2 * transformedRay.direction.dotProduct(transformedRay.origin - center);
	float c = (transformedRay.origin - center).dotProduct(transformedRay.origin - center) - radius * radius;

	float discriminant = b*b - 4*a*c;

	if (discriminant < 0.0f)
	{
		// No intersection
		return result;
	}
	else
	{
		// Intersection at 1 or 2 points
		float t1 = (-1 * b + sqrt(discriminant)) / (2 * a);
		float t2 = (-1 * b - sqrt(discriminant)) / (2 * a);
		float t = std::min(t1, t2);

		// If ray's origin is on the surface of the sphere, t1 = 0.
		// If ray's origin is inside the sphere, t2 < 0.
		if (t1 < 0.0f)
		{
			t = t2;
		}
		else if (t2 < 0.0f)
		{
			t = t1;
		}

		Vec3f intersectionPoint = transformedRay.pointAtParam(t);
		Vec3f surfaceNormal = (intersectionPoint - center).unitVector();
		Vec3f translatedIntersectionPoint = intersectionPoint - center;

		hit.t = t;
		hit.materialId = materialId;
		hit.intersectionPoint = intersectionPoint;
		hit.texture = texture;
		hit.uvTexture = getTextureCoords(translatedIntersectionPoint, texture);
		hit.normal = surfaceNormal;

		if (normalTexture && normalTexture->isNormalMap)
		{
			// If normal mapping is used, replace the geometric normals with the normals computed from the texture image.
			Vec2f uvNormalTexture = getTextureCoords(translatedIntersectionPoint, normalTexture);
			Matrix3f tbnMatrix = computeTbnMatrix(translatedIntersectionPoint, surfaceNormal);
			hit.normal = normalTexture->getNormalMapNormal(uvNormalTexture, tbnMatrix);
		}
		else if (normalTexture && normalTexture->isBumpMap)
		{
			// If bump mapping is used, replace the geometric normals with the bumped normals.
			Vec2f uvNormalTexture = getTextureCoords(translatedIntersectionPoint, normalTexture);
			Matrix3f tbnMatrix = computeTbnMatrix(translatedIntersectionPoint, surfaceNormal);
			hit.normal = normalTexture->getBumpNormal(translatedIntersectionPoint, surfaceNormal, tbnMatrix, uvNormalTexture);
		}

		hit = transformHit(hit, transformedRay.time);

		result = true;
	}

	return result;
}

Vec2f Sphere::getTextureCoords(const Vec3f& point, Texture* tex) const
{
	Vec2f uv = Vec2f();

	if (!tex)
	{
		return uv;
	}

	float theta = acos(point.y / radius);
	float phi = atan2(point.z, point.x);
	uv.x = (PI - phi) / (2 * PI);
	uv.y = theta / PI;

	return uv;
}

Matrix3f Sphere::computeTbnMatrix(const Vec3f& point, const Vec3f& normal)
{
	Matrix3f tbn = Matrix3f();
	if (!normalTexture)
	{
		return tbn;
	}

	float theta = acos(point.y / radius);
	float phi = atan2(point.z, point.x);

	// T = p_u
	Vec3f T = Vec3f();
	T.x = 2 * PI * point.z;
	T.y = 0;
	T.z = -2 * PI * point.x;

	// B = p_v
	Vec3f B = Vec3f();
	B.x = point.y * cos(phi) * PI;
	B.y = -1 * radius * sin(theta) * PI;
	B.z = point.y * sin(phi) * PI;

	if (normalTexture->isNormalMap)
	{
		T = T.unitVector();
		B = B.unitVector();
	}

	tbn = Matrix3f(T, B, normal);
	return tbn;
}