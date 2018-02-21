#version 150                                          // Keeping you on the bleeding edge!
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack: enable


// This is passed on from the vertex shader
in vec3 FragmentPosition;
in vec3 FragmentNormal;
in float time;
// This is no longer a built-in variable
layout (location=0) out vec4 FragColor;

float unionOperation(in float d1, in float d2 ) { return min(d1,d2); }
float intersectionOperation(in float d1, in float d2 ) { return max(d1,d2); }

float offset1()
{
   float offset = pow(sin(5), 2.0)/3.0;
    return offset;
}

float offset2()
{
    float offset = pow(sin(time*1.5), 2.0)/10.0;
        return offset;
}


float signedDistanceLine(in vec2 p, in vec2 s1, in vec2 s2)
{
    float distance = (s2[1]-s1[1])*p.x - (s2[1]-s1[1])*p.y + (s2[0]*s1[1]) - (s2[1]*s1[0])/sqrt(((s2[1]-s1[1])*(s2[1]-s1[1])) + ((s2[0]-s1[0])*(s2[0]-s1[0])));
    distance = abs(distance);

    return distance;
}

float distanceToSegment(vec2 a, vec2 b, vec2 UV)
{
    vec2 p = UV;
    vec2 pa = p - a;
        vec2 ba = b - a;
    if(dot(a-b,a-b) == 0.0) return 1.0;
    float h = clamp( dot(pa,ba) / dot(ba,ba), 0.0, 1.0 );
        return length( pa - ba*h);
}

float distanceToLine1(vec2 UV)
{
    vec2 uv = UV;
    float distance = min(distanceToSegment(vec2(-1.0, 0.3), vec2(-0.5, 0.0), uv),
                         distanceToSegment(vec2(-0.5, 0.0), vec2(-0.1, 0.5),uv));


    distance = min(distance,distanceToSegment(vec2(-0.1, 0.5), vec2(0.2, 0.2),uv));

    return distance;

}

float distanceToLine2(vec2 UV)
{
    vec2 uv = UV;
    float distance = min(distanceToSegment(vec2(0.0, 0.1), vec2(0.6, 0.2), uv),
                         distanceToSegment(vec2(0.6, 0.2), vec2(0.8, 0.5),uv));


    distance = min(distance,distanceToSegment(vec2(0.8, 0.5), vec2(1, 0.5),uv));

    return distance;

}

float line2_ub(in vec2 position)
{
    float line_distance = distanceToLine2(position);
    return line_distance-offset1();
}

float line1_ub(in vec2 position)
{
        float line_distance = distanceToLine1(position);
    return line_distance-offset1();

}


float line2(in vec2 position)
{
    float line2_distance = distanceToLine2(position);

    float line1_distance = distanceToLine1(position);

    float bound = max(min(line2_ub(position)-line1_ub(position), line2_distance), -line1_distance);
    float r = 0.;
    if (bound < 0.)
    {
        float fa = abs(bound/0.1);
        r = fa/(fa+1.0);
    }

        return line2_distance-offset1()*r;

}

float line1(in vec2 position)
{
    float line2_distance = distanceToLine2(position);
        float line1_distance = distanceToLine1(position);


    float bound = max(min(line1_ub(position)-line2_ub(position), line1_distance), -line2_distance);
    float r = 0.;
    if (bound < 0.)
    {
        float fa = abs(bound/0.1);
        r = fa/(fa+1.0);
    }

    return line1_distance-offset1()*r;

}

vec3 isolines(in vec3 position, float offset) {
    float l = mod(offset * 08.0, 1.0);
    vec3 color = vec3(1.0, 1.0, 1.0);
    vec3 border_color = vec3(0.0, 0.0, 1.);
    float bands = clamp(-100.0 * l * (l - 0.2), 0.0, 1.0);
    color *= bands;
    color = mix(color * 0.05, border_color, 1.0-smoothstep(0.00, 0.015, abs(offset)));
    return color;
}

float scene(in vec2 position)
{
    float line2_distance = distanceToLine2(position);
        float line1_distance = distanceToLine1(position);

    return max(min(line1_ub(position)-line2_ub(position), line1_distance), -line2_distance);;
}

float unionScene(in vec2 position ) {
    float obj1 = line1(position);
    float obj2 = line2(position);

    float result = unionOperation(obj1, obj2);
    return result;
}

float intersectionScene(in vec2 position)
{
    float obj1 = line1(position);
    float obj2 = line2(position);

    float result = intersectionOperation(obj1, obj2);
    return result;

}

//------------------------------------------------------------------------------------------------


/* light copied from vert */
struct LightInfo
{
    vec4 Position; // Light position in eye coords.
    vec3 La; // Ambient light intensity
    vec3 Ld; // Diffuse light intensity
    vec3 Ls; // Specular light intensity
};

// We'll have a single light in the scene with some default values
uniform LightInfo Light = LightInfo(
            vec4(2.0, 20.0, 10.0, 1.0),   // position
            vec3(0.2, 0.2, 0.2),        // La
            vec3(1.0, 1.0, 1.0),        // Ld
            vec3(1.0, 1.0, 1.0)         // Ls
            );

uniform LightInfo Light_2 = LightInfo(
            vec4(-20.0, 20.0, 10.0, 1.0),   // position
            vec3(0.2, 0.2, 0.2),        // La
            vec3(1.0, 1.0, 1.0),        // Ld
            vec3(1.0, 1.0, 1.0)         // Ls
            );


/* material copied from vert */
// The material properties of our object
struct MaterialInfo
{
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
};

// The object has a material
uniform MaterialInfo Material = MaterialInfo(
            vec3(0.1, 0.1, 0.1),    // Ka
            vec3(1.0, 1.0, 1.0),    // Kd
            vec3(1.0, 1.0, 1.0),    // Ks
            10.0                    // Shininess
            );



void main()
{
    vec3 n = normalize( FragmentNormal );

    // Calculate the light vector
    vec3 s = normalize( vec3(Light.Position) + vec3(Light_2.Position) - FragmentPosition);

    // Calculate the vertex position
    vec3 v = normalize(-vec3(FragmentPosition));

    // Reflect the light about the surface normal
    vec3 r = reflect( -s, n );

    // Compute the light from the ambient, diffuse and specular components
   vec3  LightIntensity = (
            Light.La * Material.Ka +
            Light.Ld * Material.Kd * max( dot(s, n), 0.0 ) +
            Light.Ls * Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess));


    // Set the output color of our current pixel
   //FragColor = vec4(color,1.0);
        FragColor = vec4(LightIntensity * vec3(1,0,0),1.0);
}
