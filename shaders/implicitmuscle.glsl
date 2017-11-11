// This is no longer a built-in variable
layout (location=0) out vec4 fragColor;

// This is passed on from the vertex shader
in vec3 FragmentPosition;
in vec3 FragmentNormal;
in vec2 texCoord;
in float time;

// The MIT License
// Copyright © 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


// A list of useful distance function to simple primitives, and an example on how to
// do some interesting boolean operations, repetition and displacement.
//
// More info here: http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm



float iTime = time;
vec3 iResolution = FragmentPosition;

float unionOperation(in float d1, in float d2 ) { return min(d1,d2); }
float intersectionOperation(in float d1, in float d2 ) { return max(d1,d2); }

float offset1()
{
   float offset = pow(sin(iTime), 2.0)/3.0;
    return offset;
}

float offset2()
{
    float offset = pow(sin(iTime*1.5), 2.0)/10.0;
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

    float offset = pow(sin(iTime), 2.0)/3.0;

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



///float object1(in vec2 position)
///{
        ///float box_distance = distanceToLine(position);
    ///float offset = pow(sin(iTime), 2.0)/3.0;
    ///return box_distance-offset;
///}

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



void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
        vec2 uv = fragCoord.xy / iResolution.xy;
    //vec4 m  = abs(iMouse) / iResolution.xyxy;
    vec2 c = 2.0 * uv - 1.0;
    //m = 2.0 * m - 1.0;

    float aspect_ratio = iResolution.x/iResolution.y;
    c.x *= aspect_ratio;
    //m.xz *= aspect_ratio;

    float distance = scene(c);

    vec3 color = isolines(vec3(c, 0.0), distance);
    if (unionScene(c) < 0.) color = vec3(0.0, 1.0, 1.);

    if (intersectionScene(c) < 0.) color = vec3(1.0, 0.0, 0.0);

    color = pow(color, vec3(0.4545));

        fragColor = vec4(color,1.0);
}
