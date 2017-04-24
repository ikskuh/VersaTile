#version 330

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vUV;

uniform mat4 matTransform;

out vec2 uv;

void main()
{
	  uv = vUV;
		gl_Position = matTransform * vec4(vPosition.xyz, 1.0);
}
