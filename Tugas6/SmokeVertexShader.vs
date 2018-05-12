#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 smokeVertices;
layout(location = 1) in vec4 xyzs;
layout(location = 2) in vec4 color;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particlecolor;

// Values that stay constant for the whole mesh.
uniform vec3 SmokeCameraRight;
uniform vec3 SmokeCameraUp;
uniform mat4 SmokeVP;

void main()
{
	float particleSize = xyzs.w;
	vec3 particleCenter_worldspace = xyzs.xyz;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_worldspace
		+ SmokeCameraRight * smokeVertices.x * particleSize
		+ SmokeCameraUp * smokeVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = SmokeVP * vec4(vertexPosition_worldspace, 1.0f);

	// UV of the vertex. No special space for this one.
	UV = smokeVertices.xy + vec2(0.5, 0.5);
	particlecolor = color;
}

