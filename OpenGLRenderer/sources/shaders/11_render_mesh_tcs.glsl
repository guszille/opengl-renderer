#version 460 core

layout (vertices=4) out;

in vec2 oiVSTexCoords[]; // Varying input from vertex shader.

out vec2 oiTCSTexCoords[]; // Varying output to evaluation shader.

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    oiTCSTexCoords[gl_InvocationID] = oiVSTexCoords[gl_InvocationID];

    // Invocation zero controls tessellation levels for the entire patch.
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = 16;
        gl_TessLevelOuter[1] = 16;
        gl_TessLevelOuter[2] = 16;
        gl_TessLevelOuter[3] = 16;

        gl_TessLevelInner[0] = 16;
        gl_TessLevelInner[1] = 16;
    }
}
