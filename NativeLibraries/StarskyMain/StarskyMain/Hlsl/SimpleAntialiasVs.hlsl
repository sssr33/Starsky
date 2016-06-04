
cbuffer buf0 : register(b0) {
	matrix MVP;
};

struct VsInput {
	float2 pos : POSITION0;
	float2 pt0 : TEXCOORD0;
	float2 pt1 : TEXCOORD1;
	float2 curPt : TEXCOORD2;
};

struct PsInput {
	float4 pos : SV_POSITION;
	float2 pt0 : TEXCOORD0;
	float2 pt1 : TEXCOORD1;
	float2 curPt : TEXCOORD2;
};

PsInput main(VsInput input) {
	PsInput output;

	output.pos = float4(input.pos, 1.0f, 1.0f);
	output.pos = mul(output.pos, MVP);

	output.pt0 = input.pt0;
	output.pt1 = input.pt1;
	output.curPt = input.curPt;

	return output;
}