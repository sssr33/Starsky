
cbuffer buf0 : register(b0) {
	float4 color;
};

struct PsInput {
	float4 pos : SV_POSITION;
	float2 pt0 : TEXCOORD0;
	float2 pt1 : TEXCOORD1;
	float2 curPt : TEXCOORD2;
};

float4 main(PsInput input) : SV_TARGET {
	float2 vec;
float2 pt0 = input.pt0;
float2 pt1 = input.pt1;

//if (pt0.x > pt1.x) {
//	float2 tmp = pt1;
//	pt1 = pt0;
//	pt0 = tmp;
//}

	float2 vec2 = normalize(pt1 - pt0);

	vec.x = -vec2.y;
	vec.y = vec2.x;

	/*vec.x = vec2.y;
	vec.y = -vec2.x;*/

	float alpha;

	float2 curPtInt;
	//float2 curPt = abs(modf(input.curPt, curPtInt));
	float2 curPt = frac(input.curPt);

	/*if (vec.x <= 0.0f) {
		curPt = 1.0f - curPt;
	}*/

	//alpha = (curPt.x * abs(vec.x) + curPt.y * abs(vec.y));
	//alpha = abs(vec.x) * abs(vec.y);

	if (vec2.x >= vec2.y) {
		alpha = curPt.x;
	}
	else {
		alpha = curPt.y;
	}

	float4 finalColor = color * alpha;

	/*float2 curPt;
	curPt = 1.0f - frac(input.curPt);
	if (vec.x <= 0.0f) {
		curPt = 1.0f - frac(input.curPt);
	}

	float4 finalColor = color * (curPt.x * abs(vec.x) + curPt.y * abs(vec.y));*/

	//float4 finalColor = color * ((1.0f - frac(input.curPt.x)) * abs(vec.x) + (1.0f - frac(input.curPt.y)) * abs(vec.y));

	return finalColor;
}