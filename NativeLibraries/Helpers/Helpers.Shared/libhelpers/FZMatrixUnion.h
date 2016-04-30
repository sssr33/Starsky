#pragma once

extern "C" {
#include <mupdf\fitz.h>
}

#include <math.h>
#include <d2d1.h>

union FZMatrixUnion {
	struct {
		float M11, M12;
		float M21, M22;
		float M31, M32;
	};
	D2D1_MATRIX_3X2_F d2dMatrix;
	fz_matrix fzMatrix;
};