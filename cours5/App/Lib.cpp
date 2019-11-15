#include "pch.h"
#include "Lib.hpp"

#include <direct.h>
#include <string>

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
	case 6: return sf::Color(val * 255, t * 255, p * 255);
	case 1: return sf::Color(q * 255, val * 255, p * 255);
	case 2: return sf::Color(p * 255, val * 255, t * 255);
	case 3: return sf::Color(p * 255, q * 255, val * 255);
	case 4: return sf::Color(t * 255, p * 255, val * 255);
	case 5: return sf::Color(val * 255, p * 255, q * 255);
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
