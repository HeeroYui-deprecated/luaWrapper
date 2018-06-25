#ifndef VECTOR2D_H_
#define VECTOR2D_H_

struct Vector2D {
	Vector2D(float x_ = 0.f, float y_ = 0.f) :
	  x(x_),
	  y(y_) {
		
	}
	float x;
	float y;
};

#endif