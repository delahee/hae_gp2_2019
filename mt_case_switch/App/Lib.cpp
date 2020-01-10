
#include "Lib.hpp"

#include <direct.h>
#include <string>

double Lib::PI = 3.14159265359;
bool Lib::loadFile(const std::string & path, MemFile & out) {
	MemFile res;
	res.data = nullptr;
	res.size = 0;

	FILE * f = nullptr;
	fopen_s(&f, path.c_str() , "r");
	if (f == nullptr) return false;
	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char * mem = (char*)malloc(fsize);

	fread(mem, 1, fsize, f);
	fclose(f);

	res.data = mem;
	res.size = fsize;

	out = res;
	return true;
}

//// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f 

sf::Color Lib::hsv(int hue, float sat, float val)
{
	hue %= 360;
	while (hue < 0) hue += 360;

	if (sat < 0.f) sat = 0.f;
	if (sat > 1.f) sat = 1.f;

	if (val < 0.f) val = 0.f;
	if (val > 1.f) val = 1.f;

	int h = hue / 60;
	float f = float(hue) / 60 - h;
	float p = val * (1.f - sat);
	float q = val * (1.f - sat * f);
	float t = val * (1.f - sat * (1 - f));

	switch (h)
	{
	default:
	case 0:
	case 6: return sf::Color((sf::Uint8)val * 255, (sf::Uint8)t * 255, (sf::Uint8)p * 255);
	case 1: return sf::Color((sf::Uint8)q * 255, (sf::Uint8)val * 255, (sf::Uint8)p * 255);
	case 2: return sf::Color((sf::Uint8)p * 255, (sf::Uint8) val * 255, (sf::Uint8)t * 255);
	case 3: return sf::Color((sf::Uint8)p * 255, (sf::Uint8) q * 255, (sf::Uint8)val * 255);
	case 4: return sf::Color((sf::Uint8)t * 255, (sf::Uint8)p * 255, (sf::Uint8)val * 255);
	case 5: return sf::Color((sf::Uint8)val * 255, (sf::Uint8)p * 255, (sf::Uint8)q * 255);
	}
}

void Lib::m_gaussian_kernel(float * dest, int size, float radius)
{
	float *k;
	float rs, s2, sum;
	float sigma = 1.6f;
	float tetha = 2.25f;
	int r, hsize = size / 2;

	s2 = 1.0f / expf(sigma * sigma * tetha);
	rs = sigma / radius;

	k = dest;
	sum = 0.0f;

	/* compute gaussian kernel */
	for (r = -hsize; r <= hsize; r++) {
		float x = r * rs;
		float v = (1.0f / expf(x * x)) - s2;
		v = std::max(v, 0.0f);
		*k = v;
		sum += v;
		k++;
	}

	/* normalize */
	if (sum > 0.0f) {
		float isum = 1.0f / sum;
		for (r = 0; r < size; r++)
			dest[r] *= isum;
	}
}

double Lib::v2Dist(const Vector2f & v0, const Vector2f & v1)
{
	return v2Len(v1-v0);
}
void Lib::v2Norm(Vector2f & v0)
{
	double len = v2Len(v0);
	if (len == 0.0) {
		//do nothing
	}
	else {
		v0.x /= len;
		v0.y /= len;
	}
}



double Lib::v2iLen(const Vector2i & v0)
{
	return sqrt(v0.x*v0.x + v0.y*v0.y);
}

double Lib::v2iDist(const Vector2i & v0, const Vector2i & v1)
{
	Vector2i d = v1 - v0;
	return v2iLen(d);
}

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

bool Lib::willCollide(Vector2f pos, Vector2f speed, Shape * other, b2Vec2 & inter, b2Vec2 & normal ) {

	sf::FloatRect oBounds = other->getGlobalBounds();
	b2Transform transform;
	transform.SetIdentity(); 
	
	b2RayCastInput input; 
	input.p1.Set(pos.x, pos.y);
	input.p2.Set(pos.x + speed.x , pos.y + speed.y);
		
	input.maxFraction = 1.0f; 
	int32 childIndex = 0; 
	b2RayCastOutput output; 

	b2PolygonShape polyB;

	b2Vec2 center;
	center.x = oBounds.left + oBounds.width * 0.5f;
	center.y = oBounds.top + oBounds.height * 0.5f;

	b2Vec2 size;
	size.x = oBounds.width;
	size.y = oBounds.height;
	polyB.SetAsBox(size.x*0.5, size.y*0.5, center, 0.0f);

	bool hit = polyB.RayCast(&output, input, transform, childIndex);
	if (hit) { 
		inter = input.p1 + output.fraction * (input.p2 - input.p1);
		normal = output.normal;
		return true;
	}
	return false;
}

double Lib::dot(b2Vec2 v0, b2Vec2 v1)
{
	return v0.x * v1.x + v0.y * v1.y;
}

double Lib::v2Len(const Vector2f & v0)
{
	return sqrt(v0.x*v0.x + v0.y*v0.y);
}
