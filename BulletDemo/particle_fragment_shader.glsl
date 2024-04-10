// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;
uniform int explosion;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);
    if (explosion == 0) color.rgb = vec3(0.01, 0.05, 0.9) * color_interp.r;
    if (explosion == 1) color.rgb = vec3(0.7, 0.4, 0.01) * color_interp.r;
    //vec4 color = vec4(0.8, 0.4, 0.01, 1.0);

    // Assign color to fragment
    gl_FragColor = vec4(color.r, color.g, color.b, color.a);

    // Check for transparency
    if(color.a < 1.0)
    {
         discard;
    }
}
