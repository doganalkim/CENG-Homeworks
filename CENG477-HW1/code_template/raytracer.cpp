#include <iostream>
#include "parser.h"
#include "ppm.h"
#include "math.h"

using namespace std;

#define EPS (1e-7)
#define LIMIT 1e6

parser::Scene scene;

typedef unsigned char RGB[3];

typedef struct{
    parser::Vec3f d,e;
    int depth;
} Ray;

typedef struct{
    float a,b,c,d,e,f,g,h,i; 
} Matrix;

void intersects(Ray &ray, float &curt, int &matId, parser::Vec3f &n);
parser::Vec3f computeColor(Ray &ray);
parser::Vec3f  normalize(parser::Vec3f &vec);

float computeMagnitute(parser::Vec3f &vec)
{
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}


parser::Vec3f vectorSub(parser::Vec3f &a, parser::Vec3f &b)
{
    parser::Vec3f res;

    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;

    return res;
}

parser::Vec3f computeCorner(parser::Camera &c, int &width, int &height, float &distance, parser::Vec3f &u)
{
    parser::Vec3f corner;

    // m = e + -wdistance
    corner.x = c.position.x  + c.gaze.x * distance;
    corner.y = c.position.y  + c.gaze.y * distance;
    corner.z = c.position.z  + c.gaze.z * distance;


    // after the step above corner becomes m, that is the middle point

    // the order is left, right, bottom, top for vec4f in camera 
    //               x     y        z     w 
    //               w     x        y     z 

    // q = m + lu + tv
    corner.x += c.near_plane.x * u.x + c.near_plane.w * c.up.x;
    corner.y += c.near_plane.x * u.y + c.near_plane.w * c.up.y;
    corner.z += c.near_plane.x * u.z + c.near_plane.w * c.up.z;

    return corner;
}

parser::Vec3f crossProduct( parser:: Vec3f &a, parser::Vec3f  &b)
{
    parser::Vec3f res;

    res.x = a.y * b.z - a.z * b.y ;
    res.y = -( a.x * b.z - a.z * b.x) ;
    res.z = a.x * b.y - a.y * b.x ;

    return res;
}

Ray computeRay(parser::Vec3f &e, parser::Vec3f &s)
{
    Ray ray;
    ray.e = e;
    ray.d = vectorSub(s,e);
    ray.d = normalize(ray.d);
    return ray;
}

float dotProduct(parser::Vec3f &a, parser::Vec3f &b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

float calculateDeterminant(Matrix &m)
{
    return m.a * ( m.e * m.i - m.f * m.h) - m.b * ( m.d * m.i - m.f * m.g ) + m.c * ( m.d * m.h - m.e * m.g ); 
}

float findIntersectionWithFace(parser::Face &face, Ray &ray)
{
    float dp = dotProduct(ray.d, face.n);
    if( dp >= (float)0){
        return -1;
    }

    // 0 -> a, 1 -> b , 2 -> c
    parser::Vec3f firstCol = vectorSub( scene.vertex_data[face.v0_id - 1], scene.vertex_data[face.v1_id - 1] );
    parser::Vec3f secondCol = vectorSub( scene.vertex_data[face.v0_id - 1], scene.vertex_data[face.v2_id - 1] );
    parser::Vec3f resCol = vectorSub( scene.vertex_data[face.v0_id - 1], ray.e );


    Matrix A;

    A.a = firstCol.x;
    A.d = firstCol.y;
    A.g = firstCol.z;

    A.b = secondCol.x;
    A.e = secondCol.y;
    A.h = secondCol.z;

    A.c = ray.d.x;
    A.f = ray.d.y;
    A.i = ray.d.z;

    float  detA = calculateDeterminant(A);

    if(detA == 0) return -1;

    Matrix betaMatrix = A;
    betaMatrix.a = resCol.x;
    betaMatrix.d = resCol.y;
    betaMatrix.g = resCol.z;

    float beta = calculateDeterminant(betaMatrix) / detA;

    if( beta < -EPS || beta > 1) return -1;


    Matrix gamaMatrix = A;
    gamaMatrix.b = resCol.x;
    gamaMatrix.e = resCol.y;
    gamaMatrix.h = resCol.z;


    float gama = calculateDeterminant(gamaMatrix) / detA;

    if( gama <  -EPS  || gama > (1+EPS) ) return -1;


    A.b = secondCol.x;
    A.e = secondCol.y;
    A.h = secondCol.z;

    Matrix tMatrix = A;
    tMatrix.c = resCol.x;
    tMatrix.f = resCol.y;
    tMatrix.i = resCol.z;

    float t = calculateDeterminant(tMatrix) / detA;

    if( beta + gama <= (float)(1 + EPS) && t > -EPS ) return t;

    return -1;
}

float findIntersectionWithSphere(parser::Sphere &s, Ray &ray, parser::Vec3f &center)
{
    parser::Vec3f oMinusc = vectorSub( ray.e , center);

    float dDotProductD = dotProduct(ray.d, ray.d);
    float dDotProductOminusC = dotProduct(ray.d, oMinusc);

    float oMinuscDotProductoMinusc = dotProduct(oMinusc,oMinusc);

    float determinant = dDotProductOminusC * dDotProductOminusC  - dDotProductD * ( oMinuscDotProductoMinusc - s.radius * s.radius ) ;
    
    if(determinant < 0) return -1;

    if(!determinant) return -dDotProductOminusC / dDotProductD;

    float t1 = (-dDotProductOminusC - sqrt(determinant ))/ dDotProductD;
    float t2 = (-dDotProductOminusC + sqrt(determinant ))/ dDotProductD;

    if(t1 < 0 && t2 < 0) return -1;
    if( t1 <= t2 && t1 > 0) return t1;
    else if (t2 > 0) return t2;
    
    return -1;
    
}

parser::Vec3f computeDiffuseColor(parser::Vec3f &diffuseCoefficient, Ray &wi, parser::Vec3f &I, parser::Vec3f &n)
{
    parser::Vec3f color;

    color.x = color.y = color.z = 0;

    float cosTheta = dotProduct(wi.d,n);
    cosTheta /= computeMagnitute(n);

    float magn = computeMagnitute(wi.d);

    if(cosTheta <= 0) return color;

    cosTheta /= magn;

    // distance is the magnitute square
    magn *= magn;

    color.x = (diffuseCoefficient.x * I.x * cosTheta) / magn;
    color.y = (diffuseCoefficient.y * I.y * cosTheta) / magn;
    color.z = (diffuseCoefficient.z * I.z * cosTheta) / magn;

    return color;
}

parser::Vec3f vectorAdd(parser::Vec3f &a, parser::Vec3f &b)
{
    parser::Vec3f res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}

parser::Vec3f  normalize(parser::Vec3f &vec)
{
    parser::Vec3f res;
    float magn = computeMagnitute(vec);
    res.x = vec.x / magn; res.y = vec.y / magn; res.z = vec.z / magn;
    return res;
}

parser::Vec3f computeSpecularColor(Ray &wi, Ray &ray, parser::Material &material,parser::Vec3f &I , parser::Vec3f &n)
{
    parser::Vec3f color;

    color.x = color.y = color.z = 0;

    parser::Vec3f reverseRay; 
    reverseRay.x = -ray.d.x; 
    reverseRay.y = -ray.d.y; 
    reverseRay.z = -ray.d.z;

    parser::Vec3f unitwo = normalize(reverseRay), unitwi = normalize(wi.d), unitn = normalize(n);

    parser::Vec3f h = vectorAdd(unitwi, unitwo);

    float magn = computeMagnitute(h);
    h = normalize(h);

    float cosTheta = dotProduct(h,unitn);
    
    if(cosTheta <= 0) return color;

    float resultExp = pow(cosTheta,  material.phong_exponent );

    float wiMagn = computeMagnitute(wi.d); wiMagn *= wiMagn;

    color.x = ( material.specular.x * resultExp * I.x ) / wiMagn;
    color.y = ( material.specular.y * resultExp * I.y ) / wiMagn;
    color.z = ( material.specular.z * resultExp * I.z ) / wiMagn;

    return color;
}

parser::Vec3f computeReflectionRay(parser::Vec3f &n, Ray &ray)
{
    Ray reverseRay;
    reverseRay.d.x = -ray.d.x;
    reverseRay.d.y = -ray.d.y;
    reverseRay.d.z = -ray.d.z;
    reverseRay.d = normalize(reverseRay.d);

    parser::Vec3f unitn = normalize(n);

    float cosTheta = dotProduct(unitn,reverseRay.d);

    parser::Vec3f rigthHand;

    rigthHand.x = 2 * unitn.x * cosTheta;
    rigthHand.y = 2 * unitn.y * cosTheta;
    rigthHand.z = 2 * unitn.z * cosTheta;

    parser::Vec3f res = vectorSub(rigthHand, reverseRay.d);

    res = normalize(res);

    return res;
}

// For V, we need ray. For reflect vector, we need wi ( x to light ) and normal 
parser::Vec3f  computeSpecularColorWithoutApproximation(Ray &ray, Ray &wi, parser::Vec3f &n,  parser::Material &mat, parser::Vec3f &I) 
{
    parser::Vec3f color;
    color.x = color.y = color.z = 0;

    parser::Vec3f v;
    v.x = -ray.d.x; v.y = -ray.d.y; v.z = -ray.d.z;

    Ray minusWi;
    minusWi.d.x = -wi.d.x; minusWi.d.y =-wi.d.y;  minusWi.d.z =-wi.d.z; 
    minusWi.e = wi.e;
    parser::Vec3f r = computeReflectionRay(n, minusWi );

    float cosTheta = dotProduct(v,r);

    if(cosTheta <= 0) return color;

    float magn = computeMagnitute(v) * computeMagnitute(r);

    cosTheta /= magn;

    cosTheta = pow(cosTheta, mat.phong_exponent);

    color.x = I.x * cosTheta * mat.specular.x;
    color.y = I.y * cosTheta * mat.specular.y;
    color.z = I.z * cosTheta * mat.specular.z;

    return color;
}

parser::Vec3f applyShading(parser::Vec3f &normal, Ray &ray, float &t, parser::Material &material)
{
    parser::Vec3f color = scene.ambient_light;

    color.x *=  material.ambient.x;
    color.y *=  material.ambient.y;
    color.z *=  material.ambient.z;

    // this is the ray from intersection point to the light
    Ray wi;
    // start point is the intersection point x 
    wi.e.x = ray.e.x + ray.d.x * t;
    wi.e.y = ray.e.y + ray.d.y * t;
    wi.e.z = ray.e.z + ray.d.z * t;

    float normalMagn = computeMagnitute(normal);

    if( material.is_mirror && !(material.mirror.x == 0 && material.mirror.y == 0 && material.mirror.z == 0  ))
    {   
        Ray  reflectionRay;
        reflectionRay.d  =  computeReflectionRay(normal, ray);
        reflectionRay.e = wi.e;
        reflectionRay.e.x += normal.x * scene.shadow_ray_epsilon / normalMagn ;
        reflectionRay.e.y += normal.y * scene.shadow_ray_epsilon / normalMagn ;
        reflectionRay.e.z += normal.z * scene.shadow_ray_epsilon / normalMagn ;
        reflectionRay.depth =  ray.depth + 1;

        parser::Vec3f refColor = computeColor(reflectionRay);

        color.x += refColor.x * material.mirror.x;
        color.y += refColor.y * material.mirror.y;
        color.z += refColor.z * material.mirror.z;
    }   


    for(parser::PointLight &pl: scene.point_lights)
    {   
        wi.d = vectorSub(pl.position, wi.e);
 
        float lightDistance = computeMagnitute(wi.d);

        parser::Vec3f n;
        Ray tempRay;
        tempRay.d = wi.d; 
        tempRay.e.x = wi.e.x + scene.shadow_ray_epsilon * normal.x / normalMagn; 
        tempRay.e.y = wi.e.y + scene.shadow_ray_epsilon * normal.y / normalMagn;
        tempRay.e.z = wi.e.z + scene.shadow_ray_epsilon * normal.z / normalMagn;
        float curT = LIMIT;
        int matID;
        intersects(tempRay,curT, matID,n);

        parser::Vec3f dt; 
        dt.x = wi.d.x * curT; dt.y = wi.d.y *  curT;  dt.z = wi.d.z * curT;

        if( curT != LIMIT && curT > scene.shadow_ray_epsilon  &&  lightDistance > computeMagnitute(dt)){
            continue;
        }

        parser::Vec3f diffuseTerm = computeDiffuseColor(material.diffuse, wi, pl.intensity, normal );

        color.x += diffuseTerm.x;
        color.y += diffuseTerm.y;
        color.z += diffuseTerm.z;

        parser::Vec3f specularTerm = computeSpecularColor(wi,ray, material, pl.intensity, normal);

        color.x += specularTerm.x;
        color.y += specularTerm.y;
        color.z += specularTerm.z;
    }

    return color;
}

void intersects(Ray &ray, float &curt, int &matId, parser::Vec3f &n)
{
    for(parser::Triangle &tri : scene.triangles)
    {
        float t = findIntersectionWithFace( tri.indices, ray);
        if(t > 0 && t < curt)
        {   
            curt = t;
            matId = tri.material_id;
            n = tri.indices.n;
        }
            
    }

    int cnt = 0;
    for(parser::Mesh &m : scene.meshes)
    {
        for(parser::Face &f: m.faces)
        {
            float t = findIntersectionWithFace(f, ray);
            if(t > 0 && t < curt)
            {
                curt = t;
                matId =  m.material_id;
                n = f.n;
            }
        }
        cnt++;
    }
    cnt = 0;
    for(parser::Sphere &s: scene.spheres)
    {
        parser::Vec3f center = scene.vertex_data[s.center_vertex_id - 1];

        float t = findIntersectionWithSphere(s, ray, center );
        if(t > 0 && t < curt)
        {
            //cout << "SPHERE" << endl;
            curt = t;
            matId = s.material_id;

            parser::Vec3f vec, normal;
            vec.x = ray.e.x + ray.d.x * t;
            vec.y = ray.e.y + ray.d.y * t;
            vec.z = ray.e.z + ray.d.z * t;
            
            normal = vectorSub(vec, center); 

            float magn = computeMagnitute(normal);
            normal.x /= magn;
            normal.y /= magn;
            normal.z /= magn;

            n = normal;
        }
        cnt++;
    }

}

parser::Vec3f computeColor(Ray &ray)
{
    // this is the result color
    parser::Vec3f color;

    float curt = LIMIT;   // initialize with INTMAX to compare
    int matId;              // material ID
    parser::Vec3f n;        // normal

    color.x = color.y = color.z = 0; // initialize as black

    if( ray.depth  > scene.max_recursion_depth)  return color;

    intersects(ray, curt, matId, n);

    if(curt != LIMIT)
    {
        parser::Material material = scene.materials[matId - 1];
        return applyShading(n, ray, curt, material);
    }

    if( !ray.depth  && curt == LIMIT)
    {
        color.x = scene.background_color.x;
        color.y = scene.background_color.y;
        color.z = scene.background_color.z;
        return color;
    }

    return color;
}


void computeN( parser::Face &face)
{
    parser::Vec3f ilkKenar, ikinciKenar; 

    ilkKenar = vectorSub(scene.vertex_data[face.v1_id - 1],scene.vertex_data[face.v0_id - 1] );
    ikinciKenar = vectorSub(scene.vertex_data[face.v2_id - 1], scene.vertex_data[face.v0_id -1 ]);
       
    face.n =  crossProduct(ilkKenar, ikinciKenar);
    face.n = normalize(face.n);

}

void clampColor(parser::Vec3f &color)
{
    if(color.x < 0) color.x = 0;
    else if(color.x > 255) color.x = 255;

    if(color.y < 0) color.y = 0;
    else if(color.y > 255) color.y = 255;

    if(color.z < 0) color.z = 0;
    else if(color.z > 255) color.z = 255;
}

int main(int argc, char* argv[])
{

    scene.loadFromXml(argv[1]);

    // HERE COMPUTE THE NORMAL OF EACH FACE, DO NOT FORGET, IMPLEMENT IT BEFORE IMPLEMENTING SHADING

    for(parser::Triangle &tri : scene.triangles)
        computeN( tri.indices);

    for( parser::Mesh &m : scene.meshes)
        for( parser::Face &f:  m.faces)
            computeN(f);

    for(parser::Camera &c: scene.cameras)
    {
        int width = c.image_width, height = c.image_height;
        float distance = c.near_distance;

        c.gaze = normalize(c.gaze); 
        c.up = normalize(c.up);

        parser::Vec3f w;
        w.x = -c.gaze.x;
        w.y = -c.gaze.y;
        w.z = -c.gaze.z;

        parser::Vec3f newu = crossProduct( c.up,  w), u;

        u = normalize(newu);

        parser::Vec3f corner = computeCorner(c, width, height, distance, u);

        double su = (  c.near_plane.y - c.near_plane.x  ) / (float) width;
        double sv = (  c.near_plane.w - c.near_plane.z  ) / (float) height;


        corner.x += (su * 0.5 * u.x - sv * 0.5 * c.up.x);
        corner.y += (su * 0.5 * u.y - sv * 0.5 * c.up.y);
        corner.z += (su * 0.5 * u.z - sv * 0.5 * c.up.z);

        parser::Vec3f currentPixel;

        unsigned char* image = new unsigned char [width * height * 3];

        int i = 0;

        for(int y = 0; y < height ; y++)
        {
            currentPixel = corner;

            for(int x = 0 ; x < width ; x ++)
            {
                // Here we need to compute the ray
                // We have computed the ray
                Ray ray = computeRay(c.position, currentPixel);

                ray.depth = 0;

                // do not forget the clamp
                parser::Vec3f color = computeColor( ray);

                clampColor(color);

                image[i++] = (int)color.x;
                image[i++] = (int)color.y;
                image[i++] = (int)color.z;

                currentPixel.x += su * u.x;
                currentPixel.y += su * u.y;
                currentPixel.z += su * u.z;
            }
            corner.x -= sv * c.up.x;
            corner.y -= sv * c.up.y;
            corner.z -= sv * c.up.z;

        }
        write_ppm(c.image_name.c_str(), image, width, height);
    }
}
