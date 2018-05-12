#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 rainVertices;
layout(location = 1) in vec4 xyzs;
layout(location = 2) in vec4 color;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particlecolor;

// Values that stay constant for the whole mesh.
uniform vec3 RainCameraRight;
uniform vec3 RainCameraUp;
uniform mat4 RainVP;

void main()
{
	float particleSize = xyzs.w;
	vec3 particleCenter_worldspace = xyzs.xyz;
	
	vec3 vertexPosition_worldspace = 
		particleCenter_worldspace
		+ RainCameraRight * rainVertices.x * particleSize
		+ RainCameraUp * rainVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = RainVP * vec4(vertexPosition_worldspace, 1.0f);

	// UV of the vertex. No special space for this one.
	UV = rainVertices.xy + vec2(0.5, 0.5);
	particlecolor = color;
}

