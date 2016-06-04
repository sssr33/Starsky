
cbuffer buf0 : register(b0) {
	matrix MVP;
};

struct VsInput {
	float2 pos : POSITION0;
};

struct PsInput {
	float4 pos : SV_POSITION;
};

PsInput main(VsInput input) {
	PsInput output;

	output.pos = float4(input.pos, 1.0f, 1.0f);
	output.pos = mul(output.pos, MVP);

	return output;
}