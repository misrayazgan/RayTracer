#include "Triangle.h"
#include "Scene.h"

Triangle::Triangle(const Scene* scene_, int vertexIndices_[], int textureIndices_[], int material_, Texture* texture_, Texture* normalTexture_, ShadingMode shadingMode_,
	const Matrix4f& matrix_, bool transform_, const Vec3f& motionVector_, bool motion_)
	: Object(scene_, material_, texture_, normalTexture_, matrix_, transform_, motionVector_, motion_), shadingMode(shadingMode_)
{
	v0 = vertexIndices_[0];
	v1 = vertexIndices_[1];
	v2 = vertexIndices_[2];

	t0 = textureIndices_[0];
	t1 = textureIndices_[1];
	t2 = textureIndices_[2];

	// Calculate surface normal
	Vec3f a = scene->vertexData[v0].position;
	Vec3f b = scene->vertexData[v1].position;
	Vec3f c = scene->vertexData[v2].position;

	normal = (b - a).crossProduct(c - a).unitVector();

	// Calculate triangle's bounding box
	float xmin = std::min(std::min(a.x, b.x), c.x);
	float ymin = std::min(std::min(a.y, b.y), c.y);
	float zmin = std::min(std::min(a.z, b.z), c.z);
	Vec3f minCorner = Vec3f(xmin, ymin, zmin);

	float xmax = std::max(std::max(a.x, b.x), c.x);
	float ymax = std::max(std::max(a.y, b.y), c.y);
	float zmax = std::max(std::max(a.z, b.z), c.z);
	Vec3f maxCorner = Vec3f(xmax, ymax, zmax);

	// For triangles belonging to a mesh, transform is set as false so that all triangles are not transformed.
	boundingBox = BoundingBox(minCorner, maxCorner);
	if (transform == true)
	{
		// Transform bounding box to world coords.
		boundingBox.applyTransformation(transformationMatrix);
	}

	// Compute TBN matrix of the triangle.
	tbnMatrix = computeTbnMatrix();
}

float Triangle::determinant(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2)
{
	return v0.x * (v1.y * v2.z - v2.y * v1.z)
		 + v0.y * (v2.x * v1.z - v1.x * v2.z)
		 + v0.z * (v1.x * v2.y - v1.y * v2.x);
}

bool Triangle::intersection(const Ray& ray, Hit& hit)
{
	bool result = false;

	Ray transformedRay = transformRay(ray);

	Vec3f o = transformedRay.origin;
	Vec3f d = transformedRay.direction;
	Vec3f a = scene->vertexData[v0].position;
	Vec3f b = scene->vertexData[v1].position;
	Vec3f c = scene->vertexData[v2].position;

	float detA = determinant(a - b, a - c, d);
	if (detA == 0.0f)
	{
		return result;
	}

	float t = (determinant(a - b, a - c, a - o)) / detA;
	if (t < 0.0f - epsilon)
	{
		return result;
	}

	float gamma = (determinant(a - b, a - o, d)) / detA;
	if (gamma < 0.0f - epsilon || gamma > 1.0f + epsilon)
	{
		return result;
	}

	float beta = (determinant(a - o, a - c, d)) / detA;
	if (beta < 0.0f - epsilon || beta > (1.0f + epsilon - gamma))
	{
		return result;
	}

	hit.t = t;
	hit.materialId = materialId;
	hit.intersectionPoint = transformedRay.pointAtParam(t);
	hit.texture = texture;
	hit.uvTexture = getTextureCoords(beta, gamma, texture);

	if (shadingMode == SHADINGMODE_FLAT)
	{
		hit.normal = normal;
	}
	else if (shadingMode == SHADINGMODE_SMOOTH)
	{
		hit.normal = (1 - beta - gamma) * scene->vertexData[v0].vertexNormal
								 + beta * scene->vertexData[v1].vertexNormal
								 + gamma * scene->vertexData[v2].vertexNormal;
	}

	if (normalTexture && normalTexture->isNormalMap)
	{
		// If normal mapping is used, replace the geometric normals with the normals computed from the texture image.
		Vec2f uvNormalTexture = getTextureCoords(beta, gamma, normalTexture);
		hit.normal = normalTexture->getNormalMapNormal(uvNormalTexture, tbnMatrix);
	}
	else if (normalTexture && normalTexture->isBumpMap)
	{
		// If bump mapping is used, replace the geometric normals with the bumped normals.
		Vec2f uvNormalTexture = getTextureCoords(beta, gamma, normalTexture);
		hit.normal = normalTexture->getBumpNormal(hit.intersectionPoint, hit.normal, tbnMatrix, uvNormalTexture);
	}

	// For triangles belonging to a mesh, transform is set as false, so hit will be returned as it is.
	hit = transformHit(hit, transformedRay.time);

	result = true;
	return result;
}

Vec2f Triangle::getTextureCoords(float beta, float gamma, Texture* tex) const
{
	Vec2f uv = Vec2f();

	if (!tex)
	{
		return uv;
	}

	Vec2f texCoords0 = scene->textureCoordData[t0];
	Vec2f texCoords1 = scene->textureCoordData[t1];
	Vec2f texCoords2 = scene->textureCoordData[t2];

	uv = texCoords0 + beta * (texCoords1 - texCoords0) + gamma * (texCoords2 - texCoords0);
	return uv;
}

Matrix3f Triangle::computeTbnMatrix()
{
	Matrix3f tbn = Matrix3f();
	if (!normalTexture)
	{
		return tbn;
	}

	Vec2f uv0 = scene->textureCoordData[t0];
	Vec2f uv1 = scene->textureCoordData[t1];
	Vec2f uv2 = scene->textureCoordData[t2];

	Vec3f posv0 = scene->vertexData[v0].position;
	Vec3f posv1 = scene->vertexData[v1].position;
	Vec3f posv2 = scene->vertexData[v2].position;

	Vec3f e1 = posv1 - posv0;
	Vec3f e2 = posv2 - posv0;

	// Find inverse of A
	float a = uv1.x - uv0.x;
	float b = uv1.y - uv0.y;
	float c = uv2.x - uv0.x;
	float d = uv2.y - uv0.y;

	float detA = d * a - b * c;

	float invA[2][2];
	invA[0][0] = d / detA;
	invA[0][1] = -1 * b / detA;
	invA[1][0] = -1 * c / detA;
	invA[1][1] = a / detA;

	// [T B]^T = invA * E;
	Vec3f T = e1 * invA[0][0] + e2 * invA[0][1];
	Vec3f B = e1 * invA[1][0] + e2 * invA[1][1];

	tbn = Matrix3f(T, B, normal);
	return tbn;
}

float Triangle::getArea(const Matrix4f& matrix) const
{
	// Apply the transformations and calculate the triangle's area.
	Vec3f a = scene->vertexData[v0].position;
	Vec3f b = scene->vertexData[v1].position;
	Vec3f c = scene->vertexData[v2].position;

	Vec3f aNew = matrix.multiplyWithPoint(a);
	Vec3f bNew = matrix.multiplyWithPoint(b);
	Vec3f cNew = matrix.multiplyWithPoint(c);

	float area = (bNew - aNew).crossProduct(cNew - aNew).length() / 2.0f;
	return area;
}