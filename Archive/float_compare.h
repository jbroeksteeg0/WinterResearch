#ifndef FLOAT_COMPARE_H
#define FLOAT_COMPARE_H

#define EPS (1e-6)
#define isEQ(x, y) (std::abs((x)-(y)) <= (EPS))
#define isLT(x, y) ((x)-(y) < -(EPS))
#define isLE(x, y) ((x)-(y) <= (EPS))
#define isGT(x, y) ((x)-(y) > (EPS))
#define isGE(x, y) ((x)-(y) >= -(EPS))

#endif
