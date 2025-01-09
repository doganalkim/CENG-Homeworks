#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>

#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"
#include <iostream>

using namespace tinyxml2;
using namespace std;

#define MIN(x,y) ( x <= y ? x : y)
#define MAX(x,y) ( x >= y ? x : y)

int xRes, yRes;

double **zBuffer;

#define PI 3.141592653589793

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		this->vertices.push_back(vertex);
		this->colorsOfVertices.push_back(color);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);

		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = meshElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			mesh->type = WIREFRAME_MESH;
		}
		else
		{
			mesh->type = SOLID_MESH;
		}

		// read mesh transformations
		XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
		XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

		while (meshTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = meshTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *cloneStr;
		int v1, v2, v3;
		XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
		str = meshFacesElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

			if (result != EOF)
			{
				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
			}

			this->image.push_back(rowOfColors);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				this->image[i][j].r = this->backgroundColor.r;
				this->image[i][j].g = this->backgroundColor.g;
				this->image[i][j].b = this->backgroundColor.b;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}


/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "./magick " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ArrangeCameraVectors(Camera *camera)
{
	//std::cout << "Before" << std::endl;
	// printVec3(camera->u);
	// printVec3(camera->v);
	// printVec3(camera->w);

	//std::cout << "After" << std::endl;
	Vec3 w = inverseVec3(camera -> gaze);
	w = normalizeVec3(w);
	Vec3 u = crossProductVec3(camera -> v, w );
	u = normalizeVec3(u);
	Vec3 v = crossProductVec3(w, u);
	v = normalizeVec3(v);
	// printVec3(u);
	// printVec3(v);
	// printVec3(w);

	camera-> u = u ; camera -> v = v; camera -> w = w;
}

double ** initializeZBuffer( Camera * camera )
{
	double **res;
	res = new double*[camera->horRes];
	for(int i = 0; i < camera->horRes; i++)
	{
		res[i] = new double[camera->verRes];
		for(int j = 0; j < camera -> verRes; j++)
			res[i][j] = 1e6;
	}
	return  res;
}


Vec3 findV(Rotation *curRot)
{
	Vec3 res;
	if ( curRot->ux <= curRot -> uy && curRot->ux <= curRot -> uz)
	{
		res.x = 0;  res.y =  (curRot -> uz) * -1; res.z =  (curRot -> uy);
	}
	else if ( curRot->uy <= curRot -> ux && curRot->uy <= curRot -> uz)
	{
		res.y = 0;  res.x =  (curRot -> uz) * -1; res.z =  (curRot -> ux);
	}
	else if ( curRot->uz <= curRot -> ux && curRot->uz <= curRot -> uy)
	{
		res.z = 0;  res.x =  (curRot -> uy) * -1; res.y =  (curRot -> ux);
	}
	return res;
}

double getRadian(double angle)
{
	return angle * PI / 180;
}

Matrix4 rotationAroundX( double &angle )
{
	Matrix4 res;
 
	res.values[0][0] = 1; res.values[0][1] = 0; res.values[0][2] = 0; res.values[0][3] = 0;

	res.values[1][0] = 0; res.values[1][1] = cos(getRadian(angle)) ; res.values[1][2] = sin(getRadian(angle)) * -1; res.values[1][3] = 0;

	res.values[2][0] = 0; res.values[2][1] = sin(getRadian(angle)); res.values[2][2] = cos(getRadian(angle)); res.values[2][3] = 0;

	res.values[3][0] = 0; res.values[3][1] = 0; res.values[3][2] = 0; res.values[3][3] = 1;

	//double x = sin(90);
	//std::cout << "sin(90):" << x << "  sin(pi/2):" << sin(3.14/2)  << " sin(90):" << sin(getRadian(90)) << std::endl;
	return res;
}


Matrix4 getRotationMatrix(Rotation *curRot)
{
	// to do
	Vec3 u ; u.x = curRot -> ux; u.y = curRot -> uy; u.z = curRot -> uz;
	Vec3 v = findV(curRot);
	Vec3 w = crossProductVec3( u, v );
	u = normalizeVec3(u); v = normalizeVec3(v); w = normalizeVec3(w);

	Matrix4 I = getIdentityMatrix();
	Matrix4 m(u,v,w);

	I = multiplyMatrixWithMatrix(m, I);

	Matrix4 r = rotationAroundX( curRot->angle );
	I =  multiplyMatrixWithMatrix(r, I);

	I =  multiplyMatrixWithMatrix( getTranspose(m), I);
	return I;
}

Matrix4 getSpecificTranslation(double tx, double ty, double tz)
{
	Matrix4 res;

	for(int i = 0; i < 4 ; i++)
	{
		for(int j = 0; j < 4 ; j++)
		{
			res.values[i][j] = 0;
			if( i == j) res.values[i][j] = 1;
		}
	}
	res.values[0][3] = tx ; res.values[1][3] = ty ; res.values[2][3] = tz ;
	return res;
}

Matrix4 getTranslationMatrix(Translation* curTran)
{
	// to do
	return getSpecificTranslation(curTran->tx,curTran->ty,curTran->tz );
}

Matrix4 getSpecificScaling(double sx, double sy, double sz)
{
	Matrix4 res;
		for(int i = 0; i < 4 ; i++)
	{
		for(int j = 0; j < 4 ; j++)
		{
			res.values[i][j] = 0;
		}
	}
	res.values[0][0] = sx; res.values[1][1] = sy; res.values[2][2] = sz; res.values[3][3] = 1;
	return res;
}

Matrix4 getScalingMatrix(Scaling *sca)
{
	return getSpecificScaling( sca->sx, sca->sy, sca->sz );
}

Matrix4 getCameraTransformationMatrix(Camera *camera)
{
	Matrix4 res = getIdentityMatrix();

	res = multiplyMatrixWithMatrix( getSpecificTranslation( -camera->position.x, -camera->position.y, -camera->position.z) ,res);

	Matrix4 m(camera->u, camera ->v , camera -> w);

	res = multiplyMatrixWithMatrix(m, res);

	return res;
}

Matrix4 getOrthMatrix(Camera *camera)
{
	Matrix4 res;
	for(int i = 0; i < 4 ; i++)
		for(int j = 0; j < 4 ; j++) 
			res.values[i][j] = 0;
	
	res.values[0][0] = 2 / (camera->right - camera->left); res.values[0][3] = - (camera->right + camera->left) / (camera->right - camera->left) ;

	res.values[1][1] = 2 / (camera->top - camera->bottom); res.values[1][3] = - (camera->top + camera->bottom) / (camera->top - camera->bottom) ;

	res.values[2][2] = -2 / (camera->far - camera->near); res.values[2][3] = - (camera->far + camera->near) / (camera->far - camera->near) ;

	res.values[3][3] = 1;

	return res;
}

Matrix4 getP20Matrix(Camera *camera)
{
	Matrix4 res;
	for(int i = 0; i < 4 ; i++)
		for(int j = 0; j < 4 ; j++) 
			res.values[i][j] = 0;

	res.values[0][0] = res.values[1][1] = camera->near;
	res.values[2][2] = (camera->near + camera->far);
	res.values[2][3] = (camera->near * camera->far);
	res.values[3][2] = -1;

	return res;
}


Matrix4 getVPMatrix(Camera *camera)
{
	Matrix4 res;
	for(int i = 0; i < 4 ; i++)
		for(int j = 0; j < 4 ; j++) 
			res.values[i][j] = 0;

	res.values[0][0] = (camera->horRes / 2); res.values[0][3] = (camera -> horRes - 1) / 2;
	res.values[1][1] = (camera->verRes / 2); res.values[1][3] = (camera -> verRes - 1) / 2;
	res.values[2][2] = res.values[2][3] = (double)1 / 2;
	res.values[3][3] = 1;
	return res;
}


double f(int x, int y, int x0, int y0, int x1, int y1 )
{
	return x * ( y0 - y1) + y * (x1 - x0) + x0*y1 - y0*x1;
}

void clampColor(Color &c)
{
	if(c.r < 0) c.r = 0;
	else if(c.r > 255) c.r = 255;

	if(c.g < 0) c.g = 0;
	else if(c.g > 255) c.g = 255;

	if(c.b < 0) c.b = 0;
	else if(c.b > 255) c.b = 255;
}

void drawPixel(std::vector<std::vector<Color> > &image, int x,int y, double z, Color c)
{
	//std::cout << "Z:" << z << std::endl;
	if( x < 0 || y < 0  || x >= xRes || y >= yRes ) return;
	if( zBuffer[x][y] <= z  || z < 0) {

		//std::cout << "DEPTH TEST FAILED: zBuffer[x][y] = " << zBuffer[x][y] << std::endl;
		return;
	}
	zBuffer[x][y] = z;

	clampColor(c);
	image[x][y] = c; 
}

void firstMidPoint(std::vector<std::vector<Color> > &image,int x0, int y0, int x1, int y1, double depth0 , double depth1, Color *c0, Color *c1)
{	
	if (  x1 < x0 && y1 < y0 ){
		firstMidPoint(image, x1, y1, x0, y0, depth1, depth0, c1, c0 ); return;
	}
	int dx = (x1-x0);
	Color c ; c.r = c0->r; c.g = c0 -> g; c.b = c0 -> b;
	Color dc; dc.r = (c1 -> r - c0 -> r) / dx;  dc.g = (c1 -> g - c0 -> g) / dx;  dc.b = (c1 -> b - c0 -> b) / dx;
	double z = depth0, zd = (depth1 - depth0) / (double)(dx);
	int y = y0;
	int d = 2 * (y0 - y1) + dx; 
	for(int x = x0; x <= x1 ;  x++ )
	{
		drawPixel(image,x,y,z,c);
		if (d < 0)
		{
			y++;
			d += 2 * ( (y0 - y1) + dx ); 
		}
		else{
			d += 2 * ( y0 - y1);
		}
		z += zd;
		c.r += dc.r; c.g += dc.g; c.b += dc.b;
	}

}

void secondMidPoint(std::vector<std::vector<Color> > &image,int x0, int y0, int x1, int y1, double depth0 , double depth1,Color *c0, Color *c1)
{	
	if (  x1 <= x0 && y1 <= y0 ){
		secondMidPoint(image, x1, y1, x0, y0, depth1, depth0, c1, c0 ); return;
	}
	int dy = (y1 - y0);
	Color c ; c.r = c0->r; c.g = c0 -> g; c.b = c0 -> b;
	Color dc; dc.r = (c1 -> r - c0 -> r) / dy;  dc.g = (c1 -> g - c0 -> g) / dy;  dc.b = (c1 -> b - c0 -> b) / dy;
	double z = depth0, zd = (depth1 - depth0) / (double)(dy);
	int x = x0;
	int d = (y0 - y1) + 2 * (x1 - x0);
	for(int y = y0; y <= y1 ;  y++ )
	{
		drawPixel(image,x,y,z,c);
		if (d > 0)
		{
			x++;
			d += 2 * ( (y0 - y1) + (x1 - x0) ); 
		}
		else{
			d += 2 * ( x1 - x0);
		}
		z += zd;
		c.r += dc.r; c.g += dc.g; c.b += dc.b;
	}
}

void thirdMidPoint(std::vector<std::vector<Color> > &image,int x0, int y0, int x1, int y1, double depth0 , double depth1,Color *c0, Color *c1)
{	
	if (  x0 < x1 && y0 > y1 ){
		thirdMidPoint(image, x1, y1, x0, y0, depth1, depth0, c1, c0 ); return;
	}
	int dx = (x0 - x1);
	Color c ; c.r = c0->r; c.g = c0 -> g; c.b = c0 -> b;
	Color dc; dc.r = (c1 -> r - c0 -> r) / dx;  dc.g = (c1 -> g - c0 -> g) / dx;  dc.b = (c1 -> b - c0 -> b) / dx;
	double z = depth0, zd = (depth1 - depth0) / ( dx);
	int y = y0;
	int d = -2 * (y0 - y1) +  (x1 - x0);
	for(int x = x0; x >= x1 ;  x-- )
	{
		drawPixel(image,x,y,z,c);
		if (d > 0)
		{
			y++;
			d += 2 * (  -(y0 - y1) + (x1 - x0) ); 
		}
		else{
			d += -2 * ( y0 - y1);
		}
		z += zd;
		c.r += dc.r; c.g += dc.g; c.b += dc.b;
	}
}


void fourthMidPoint(std::vector<std::vector<Color> > &image,int x0, int y0, int x1, int y1, double depth0 , double depth1,Color *c0, Color *c1)
{	
	if (  x0 < x1 && y0 > y1 ){
		fourthMidPoint(image, x1, y1, x0, y0, depth1, depth0, c1, c0 ); return;
	}
	int dy = ( y1 - y0 );
	Color c ; c.r = c0->r; c.g = c0 -> g; c.b = c0 -> b;
	Color dc; dc.r = (c1 -> r - c0 -> r) / dy;  dc.g = (c1 -> g - c0 -> g) / dy;  dc.b = (c1 -> b - c0 -> b) / dy;
	double z = depth0, zd = (depth1 - depth0) / (dy);
	int x = x0;
	int d =  -(y0 - y1) +  2*(x1 - x0);
	for(int y = y0; y <= y1 ;  y++ )
	{
		drawPixel(image,x,y,z,c);
		if (d < 0)
		{
			x--;
			d += 2 * (  -(y0 - y1) + (x1 - x0) ); 
		}
		else{
			d +=  2* ( x1 - x0);
		}
		z += zd;
		c.r += dc.r; c.g += dc.g; c.b += dc.b;
	}
}

void drawLine(std::vector<std::vector<Color> > &image, Vec4 &v0, Vec4 &v1, Color *c0, Color *c1)
{
	int x0 = v0.x, y0 = v0.y, x1 = v1.x, y1 = v1.y; 
	double m = (double)(y1-y0) / (x1 - x0);
	if( m >= 0 &&  m <= 1)
	{
		firstMidPoint(image, x0, y0, x1, y1, v0.z, v1.z, c0, c1);
	}
	else if( m > 1)
	{
		secondMidPoint(image, x0, y0, x1, y1, v0.z, v1.z, c0, c1);
	}
	else if( m < 0 &&  m >= -1)
	{
		thirdMidPoint(image, x0, y0, x1, y1, v0.z, v1.z,  c0, c1);
	}
	else if( m < -1)
	{
		fourthMidPoint(image, x0, y0, x1, y1, v0.z, v1.z,  c0, c1);
	}

}

void paintTriangle(std::vector<std::vector<Color> > &image, Vec4 &v0,Vec4 & v1,Vec4 & v2,Color * c0,Color *  c1, Color * c2)
{
	int xmin = MIN(v0.x, MIN(v1.x, v2.x)), ymin = MIN(v0.y, MIN(v1.y, v2.y));
	int xmax = MAX(v0.x, MAX(v1.x, v2.x)), ymax = MAX(v0.y, MAX(v1.y, v2.y));

	double alfadenum = f(v0.x, v0.y, v1.x, v1.y , v2.x , v2.y);
	double betadenum = f(v1.x, v1.y, v2.x, v2.y , v0.x , v0.y);
	double gamadenum = f(v2.x, v2.y, v0.x, v0.y , v1.x , v1.y);

	for(int x = xmin ; x < xmax ; x++)
	{
		for(int y = ymin ; y < ymax ; y++)
		{
			double alfa = f(x,y,v1.x, v1.y , v2.x , v2.y) / alfadenum;
			double beta =  f(x, y, v2.x, v2.y , v0.x , v0.y) / betadenum;
			double gama = f(x, y, v0.x, v0.y , v1.x , v1.y) / gamadenum;
			
			if( alfa >= 0 && gama >= 0 && beta >= 0)
			{
				Color c; 
				c.r = alfa * c0->r + beta*c1->r + gama * c2->r	;
				c.g = alfa * c0->g + beta*c1->g + gama * c2->g	;
				c.b = alfa * c0->b + beta*c1->b + gama * c2->b	;

				double depth = alfa * v0.z + beta*v1.z + gama * v2.z;

				drawPixel(image, x,y, depth, c);
			}
		}
	}
}

void drawTriangle(std::vector<std::vector<Color> > &image, int horRes, int verRes, Vec4 &v0, Vec4 &v1, Vec4 &v2, Color *c0, Color *c1, Color *c2, int type)
{
	//std::cout << "(int)v0.x : " << (int)v0.x <<  " (int)v0.y:" << (int)v0.y <<   "v0.z:" << v0.z  << std::endl;
	//std::cout << "(int)v1.x : " << (int)v1.x <<  " (int)v1.y:" << (int)v1.y <<  " v1.z:" << v1.z  << std::endl;
	//std::cout << "(int)v2.x : " << (int)v2.x <<  " (int)v2.y:" << (int)v2.y <<  " v2.z:" << v2.z  << std::endl;

	drawLine(image, v0, v1, c0, c1);
	drawLine(image, v1, v2, c1, c2);
	drawLine(image, v0, v2, c0, c2);

	if(type == 0) return;

	paintTriangle(image, v0, v1, v2, c0, c1, c2);

}

void perspectiveDivide( Vec4 &vec)
{
	vec.x /= vec.t;
	vec.y /= vec.t;
	vec.z /= vec.t;
	vec.t  = 1;
}

bool visible(double den, double num, double &tE, double &tL)
{
	if( den > (double)0)
	{
		double  t = num / den;

		if ( t > tL ) return false;

		if (t > tE) tE = t;
	}
	else if( den < (double)0)
	{	
		double t = num / den;

		if ( t < tE) return false;

		if ( t < tL) tL = t;
 	}
	return true;
}

void makeEqual(Vec4 &v0, Color *c0, Vec4 &newV0, Color *newC0)
{
	newV0.x = v0.x; newC0->r = c0 ->r;
	newV0.y = v0.y; newC0->g = c0 ->g;
	newV0.z = v0.z; newC0->b = c0 ->b;
	newV0.t = v0.t; 
}

bool liang_barsky(double w, Vec4 &v0, Vec4 &v1, Color *c0, Color *c1, Vec4 &newV0, Vec4 &newV1, Color *newC0, Color *newC1)
{
	double tE = 0.0, tL = 1.0;

	makeEqual(v0,c0, newV0, newC0);
	makeEqual(v1,c1, newV1, newC1);

	double dx = v1.x - (double)v0.x;  
	double dy = v1.y - (double)v0.y; 
	double dz = v1.z - (double)v0.z; 

	if ( dx == 0 &&  v0.x < -w  ) 
		return false;
	else if ( dx == 0 && v0.x > w ) 
		return  false;
	else if (dy == 0 &&  v0.y < -w) 
		return  false;
	else if ( dy == 0 &&  v0.y > w ) 
		return  false;
	else if (dz == 0 &&  v0.z < -w) 
		return  false;
	else if ( dz == 0 &&  v0.z > w ) 
		return  false;

	bool isvisible = false;


	if (  visible( dx , (double)-w - v0.x, tE, tL) &&
		  visible( -dx , (double)-w + v0.x, tE, tL) &&
		  visible( dy , (double)-w - v0.y, tE, tL) &&
		  visible( -dy , (double)-w + v0.y, tE, tL) &&
		  visible( dz , (double)-w - v0.z, tE, tL) &&
		  visible( -dz , (double)-w + v0.z, tE, tL) ){
			  
		isvisible = true;

		if(tL == 1.0 && tE == 0.0) return true;
		//std::cout << "tL: " << tL << " tE: " << tE << std::endl;

		if(  tL < 1.0 )
		{
			newV1.x = v0.x + tL*dx; 
			newV1.y = v0.y + tL*dy; 
			newV1.z = v0.z + tL*dz; 
			newV1.t = v0.t;

			newC1->r = c0->r + ( c1->r - c0->r) * tL;
			newC1->g = c0->g + ( c1->g - c0->g) * tL;
			newC1->b = c0->b + ( c1->b - c0->b) * tL;

		}
		if( tE > 0.0 )
		{
			newV0.x = v0.x + tE*dx; 
			newV0.y = v0.y + tE*dy; 
			newV0.z = v0.z + tE*dz; 
			newV0.t = v0.t;

			newC0->r = c0->r + ( c1->r - c0->r) * tE;
			newC0->g = c0->g + ( c1->g - c0->g) * tE;
			newC0->b = c0->b + ( c1->b - c0->b) * tE;
		}
	}

	return isvisible;
}

void make_last_component_equal(Vec4 &v0, Vec4 &v1, Vec4 &v2)
{
	double t0, t1, t2;

	t0 = v1.t * v2.t;
	t1 = v0.t * v2.t;
	t2 = v0.t * v1.t;

	v0.x *= t0; v0.y *= t0; v0.z *= t0; v0.t *= t0;
	v1.x *= t1; v1.y *= t1; v1.z *= t1; v1.t *= t1;
	v2.x *= t2; v2.y *= t2; v2.z *= t2; v2.t *= t2;
}

/*
	Transformations, clipping, culling, rasterization are done here.
*/
void Scene::forwardRenderingPipeline(Camera *camera)
{
	// TODO: Implement this function
	ArrangeCameraVectors(camera);

	xRes = camera->horRes; yRes = camera->verRes;
		
	zBuffer = initializeZBuffer(camera);

	Matrix4 cameraTransformMatrix =  getCameraTransformationMatrix(camera);
	Matrix4 Morth = getOrthMatrix(camera);
	Matrix4 Mp2o = getP20Matrix(camera);
	Matrix4 vp = getVPMatrix(camera);


	for(int i = 0; i < camera->horRes ; i++){
		Color c; c.r = 0; c.g = 0; c.b = 0;
		for(int j = 0; j < camera->verRes ; j++){
			image[i][j] = backgroundColor;
			//image[i][j] = c;
		}
	}
	int i = 0;
	// start with modeling transformation
	for(Mesh *m: meshes)
	{
		Matrix4 I = getIdentityMatrix();
		int transformationsize = m->numberOfTransformations; 

		for(int i = 0; i < transformationsize ; i++)
		{
			int id =  m->transformationIds[i];
			Rotation *curRot;
			Translation *curTra;
			Scaling *curSca;
			switch(  m->transformationTypes[i] )
			{
				case 'r':
					for( Rotation *r: rotations ){
						if(r ->rotationId == id){
							curRot = r; break;
						}
					}
					I = multiplyMatrixWithMatrix( getRotationMatrix( curRot ), I);
					break;
				case 't':
					for( Translation *t: translations ){
						if(t ->translationId == id){
							curTra = t; break;
						}
					}
					I = multiplyMatrixWithMatrix( getTranslationMatrix( curTra ), I);
					break;
				case 's':
					for( Scaling *s: scalings ){
						if(s -> scalingId == id){
							curSca = s; break;
						}
					}
					I = multiplyMatrixWithMatrix( getScalingMatrix( curSca ), I);
					break;
				default:
					break;
			}	
		}	

		// Now we have composite modeling transformation for a specific mesh

		for( Triangle t: m->triangles  )
		{
			Vec3 *v0 = vertices[t.vertexIds[0]-1], *v1 = vertices[t.vertexIds[1]-1], *v2 = vertices[t.vertexIds[2]-1];
			Vec4 pointv0( v0->x, v0->y, v0 ->z, 1, v0 ->colorId ), pointv1( v1->x, v1->y, v1 ->z, 1, v1 ->colorId  ), pointv2( v2->x, v2->y, v2 ->z, 1, v2 ->colorId  );
 
			pointv0 = multiplyMatrixWithVec4( I,pointv0);  pointv1 = multiplyMatrixWithVec4( I,pointv1); pointv2 = multiplyMatrixWithVec4( I,pointv2); 

			pointv0 = multiplyMatrixWithVec4(cameraTransformMatrix, pointv0);
			pointv1 = multiplyMatrixWithVec4(cameraTransformMatrix, pointv1);
			pointv2 = multiplyMatrixWithVec4(cameraTransformMatrix, pointv2);

			if( cullingEnabled )
			{
				Vec3 n;
				Vec3 v10 ( pointv1.x - pointv0.x , pointv1.y - pointv0.y  ,pointv1.z - pointv0.z );
				Vec3 v20 ( pointv2.x - pointv0.x , pointv2.y - pointv0.y  ,pointv2.z - pointv0.z );

				Vec3 ray( pointv0.x,  pointv0.y,  pointv0.z );

				n = crossProductVec3(v10, v20);

				if( dotProductVec3(n,ray) > 0) continue;
				
			}

			if(camera -> projectionType == 1)
			{
				pointv0 = multiplyMatrixWithVec4( Mp2o , pointv0);
				pointv1 = multiplyMatrixWithVec4( Mp2o , pointv1);
				pointv2 = multiplyMatrixWithVec4( Mp2o , pointv2);
			}

			pointv0 = multiplyMatrixWithVec4( Morth , pointv0);
			pointv1 = multiplyMatrixWithVec4( Morth , pointv1);
			pointv2 = multiplyMatrixWithVec4( Morth , pointv2);

			//std:: cout << "v0 ->" << pointv0.z <<  " v1 ->" << pointv1.z <<  " v2 ->" << pointv2.z << std::endl;

			//CLIPPING
			if( m->type == 0 )
			{
				make_last_component_equal(pointv0, pointv1, pointv2);
				//perspectiveDivide( pointv0); perspectiveDivide( pointv1); perspectiveDivide( pointv2);
				Color *c0 = colorsOfVertices[pointv0.colorId - 1], *c1 = colorsOfVertices[pointv1.colorId - 1], *c2 = colorsOfVertices[pointv2.colorId - 1];
				
				Color *newColorFirst = new Color, *newColorSecond =  new Color;
				Vec4 firstV, secondV;
				double w = pointv0.t;
				bool isVis = false;
				
				isVis = liang_barsky(w, pointv0, pointv1, c0, c1, firstV, secondV, newColorFirst, newColorSecond );
				if(isVis)
				{	
					perspectiveDivide(firstV); perspectiveDivide(secondV);
					firstV = multiplyMatrixWithVec4( vp , firstV);
					secondV = multiplyMatrixWithVec4( vp , secondV);

					drawLine(image, firstV, secondV, newColorFirst, newColorSecond);
				}

				isVis = liang_barsky(w, pointv1, pointv2, c1, c2, firstV, secondV, newColorFirst, newColorSecond );
				if(isVis)
				{
					perspectiveDivide(firstV); perspectiveDivide(secondV);
					firstV = multiplyMatrixWithVec4( vp , firstV);
					secondV = multiplyMatrixWithVec4( vp , secondV);

					drawLine(image, firstV, secondV, newColorFirst, newColorSecond);	
				}

				isVis = liang_barsky(w, pointv2, pointv0, c2, c0, firstV, secondV, newColorFirst, newColorSecond );
				if(isVis)
				{
					perspectiveDivide(firstV); perspectiveDivide(secondV);
					firstV = multiplyMatrixWithVec4( vp , firstV);
					secondV = multiplyMatrixWithVec4( vp , secondV);

					drawLine(image, firstV, secondV, newColorFirst, newColorSecond);
				}
			}
			else
			{
				perspectiveDivide( pointv0); perspectiveDivide( pointv1); perspectiveDivide( pointv2);

				//std:: cout << "v0 ->" << pointv0.z <<  " v1 ->" << pointv1.z <<  " v2 ->" << pointv2.z << std::endl;

				pointv0 = multiplyMatrixWithVec4( vp , pointv0);
				pointv1 = multiplyMatrixWithVec4( vp , pointv1);
				pointv2 = multiplyMatrixWithVec4( vp , pointv2);

				Color *c0 = colorsOfVertices[pointv0.colorId - 1], *c1 = colorsOfVertices[pointv1.colorId - 1], *c2 = colorsOfVertices[pointv2.colorId - 1];
				drawTriangle( image, camera->horRes, camera -> verRes ,pointv0, pointv1, pointv2, c0, c1, c2,  m->type );
			}
		}	

	}
}
