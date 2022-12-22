#version 330 core  
layout (location = 0) in vec3 aPos;    // pos variable has location 0

out vec3 ourColor;  
uniform vec3 color;
uniform float offset;
uniform float scale;
out vec3 ourPosition;

void main()  
{  
	gl_Position = vec4(scale * aPos.x, scale * aPos.y + scale * offset, scale * aPos.z, 1.0);  
	ourColor = color;
	ourPosition = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
};
