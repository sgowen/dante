struct VOut
{
	float4 position : SV_POSITION;
	float2 texcoord: TEXCOORD;        // texture coordinates
};

// Vertex Shader
VOut main(float2 position : a_Position)
{
	// create a VOut struct
	VOut output;

	// set the output values
	output.position = float4(position, 0, 1); // pass through (assuming position is in screen coordinates of -1 to 1)
	output.texcoord.x = (position.x + 1.0) / 2.0;
	output.texcoord.y = (position.y + 1.0) / 2.0;
	output.texcoord.y = 1 - output.texcoord.y;

	// return the output values
	return output;
}
