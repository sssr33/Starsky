
cbuffer buf0 : register(b0) {
	float4 color;
};

struct PsInput {
	float4 pos : SV_POSITION;
};

float4 main(PsInput input) : SV_TARGET {
	return color;
}