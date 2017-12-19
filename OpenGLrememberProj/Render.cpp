#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"



bool textureMode = true;
bool lightMode = true; 

bool alpha = false;

int ugol = 0;
GLuint texId[2];

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;


	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


			 //Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}


	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2f, 0.2f, 0.2f, 0 };
		GLfloat dif[] = { 1.0f, 1.0f, 1.0f, 0 };
		GLfloat spec[] = { 0.7f, 0.7f, 0.7f, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1.0f };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




		  //старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}


	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}


}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'A' && !alpha)
	{
		lightMode = !lightMode;
		textureMode = !textureMode;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		alpha = true;
	}

	else if (key == 'A' && alpha)
	{
		lightMode = !lightMode;
		textureMode = !textureMode;

		glDisable(GL_BLEND);
		alpha = false;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}




//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);


	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;
	RGBTRIPLE *texarray1; //я

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);





	glGenTextures(2, texId);

	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId[0]);



	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	
	//отчистка памяти
	free(texCharArray);
	free(texarray);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	char *texCharArray1; //я

	OpenGL::LoadBMP("texture1.bmp", &texW, &texH, &texarray1); //я
	OpenGL::RGBtoChar(texarray1, texW, texH, &texCharArray1); //я

	glBindTexture(GL_TEXTURE_2D, texId[1]); //я

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray1); //я

	free(texCharArray1); //я
	free(texarray1); //я
	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH);


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
}




void normal(double a1[], double b1[], double c1[], double vn[])
{
	double a[] = { a1[0] - b1[0],a1[1] - b1[1],a1[2] - b1[2] };
	double b[] = { c1[0] - b1[0],c1[1] - b1[1],c1[2] - b1[2] };

	vn[0] = a[1] * b[2] - b[1] * a[2];
	vn[1] = -a[0] * b[2] + b[0] * a[2];
	vn[2] = a[0] * b[1] - b[0] * a[1];

	double length = sqrt(pow(vn[0], 2) + pow(vn[1], 2) + pow(vn[2], 2));

	vn[0] /= length;
	vn[1] /= length;
	vn[2] /= length;
}

void fun_low_vip() //выпуклость на нижнем основании
{
	double a[] = { -5,-2,-5 };
	double h[] = { 7,-2,-5 };
	double m[] = { 1,-4,-5 };
	double o[] = { 1, 6,-5 }; //центр окружности
	int r = 10; //радиус окружности
	double point[] = { 0,0,-5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(0.8) * 180 / 3.14;
	int betta = 360 - asin(0.8) * 180 / 3.14;
	glBegin(GL_POLYGON);

	double vn[] = { 0,0,0 };//вектор нормали
	normal(a, m, h, vn);
	glNormal3dv(vn);

	glTexCoord2d(420.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(a);
	for (int i = alpha; i <= betta; i++)
	{
		point[0] = o[0] + r*cos(i*3.14 / 180);
		point[1] = o[1] + r*sin(i*3.14 / 180);
		glTexCoord2d((360.0 - 100.0 * cos(i*3.14 / 180.0)) / 512.0, (512.0 - 80.0 + 100.0*sin(i*3.14 / 180)) / 512.0);
		glVertex3dv(point);
	}
	glTexCoord2d(300.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(h);
	glEnd();
}

void fun_low() //нижнее основание
{
	double a[] = { -5,-2,-5 };
	double b[] = { -5, 1,-5 };
	double c[] = { -1, 1,-5 };
	double d[] = { -2, 4,-5 };
	double e[] = { 7, 4,-5 };
	double f[] = { 5, 1,-5 };
	double g[] = { 7, 1,-5 };
	double h[] = { 7,-2,-5 };
	//для вогнутости
	double n[] = { 2,2,-5 };
	double o[] = { 2.5, 8,-5 }; //центр окружности
	float r = 6.02; //радиус окружности
	double point[] = { 0,0,-5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(4 / 6.02) * 180 / 3.14;
	int betta = 360 - asin(4 / 6.02) * 180 / 3.14;

	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый

	double vn[] = { 0,0,0 };//вектор нормали
	normal(g, b, a, vn);
	glNormal3dv(vn);


	glBegin(GL_QUADS);
	glTexCoord2d(420.0/512.0, (512.0-160.0)/512.0);
	glVertex3dv(a);
	glTexCoord2d(420.0 / 512.0, (512.0 - 130.0) / 512.0);
	glVertex3dv(b);
	glTexCoord2d(300.0 / 512.0, (512.0 - 130.0) / 512.0);
	glVertex3dv(g);
	glTexCoord2d(300.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(h);
	glEnd();

	glBegin(GL_QUAD_STRIP);
		glTexCoord2d(390.0 / 512.0, 1.0 - 100.0 / 512.0);
		glVertex3dv(d);
		glTexCoord2d(380.0 / 512.0, 1.0 - 130.0 / 512.0);
		glVertex3dv(c);
		double osn[] = { -1,1,-5 };
		int alala = 0;
		for (int i = alpha; i < betta; i++)
		{
			alala++;
			point[0] = o[0] + r*cos(i*3.14 / 180);
			point[1] = o[1] + r*sin(i*3.14 / 180);
			glTexCoord2d((345.0 - 60.2*cos(i*3.14 / 180.0)) / 512.0, (512.0 - 60.0 + 60.2*sin(i*3.14 / 180.0)) / 512.0);
			glVertex3dv(point);
			double osn[] = { -1 + 0.06186*alala,1,-5 };
			glTexCoord2d((380.0 - (60.0 / 97.0)*alala) / 512.0, 1.0 - 130.0 / 512.0); //исправь
			glVertex3dv(osn);
		}
		glTexCoord2d(300.0 / 512.0, 1.0 - 100.0 / 512.0);
		glVertex3dv(e);
		glTexCoord2d(320.0 / 512.0, 1.0 - 130.0 / 512.0);
		glVertex3dv(f);
	glEnd();

	fun_low_vip(); //выпуклость на нижнем основании
}




void fun_up_vip() //выпуклость на верхнем основании
{
	double a1[] = { -5,-2,5 };
	double h1[] = { 7,-2, 5 };
	double m1[] = { 1,-4, 5 };
	double o1[] = { 1, 6, 5 }; //центр окружности
	int r = 10; //радиус окружности
	double point1[] = { 0,0,5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(0.8) * 180 / 3.14;
	int betta = 360 - asin(0.8) * 180 / 3.14;
	glBegin(GL_POLYGON);

	double vn[] = { 0,0,0 };//вектор нормали
	normal(h1, m1, a1, vn);
	glNormal3dv(vn);

	glTexCoord2d(100.0 /512.0, (512.0 -160.0)/512.0);
	glVertex3dv(a1);
	for (int i = alpha; i <= betta; i++)
	{
		point1[0] = o1[0] + r*cos(i*3.14 / 180);
		point1[1] = o1[1] + r*sin(i*3.14 / 180);
		glTexCoord2d((160.0 + 100.0 * cos(i*3.14 / 180.0))/512.0,(512.0 - 80.0 + 100.0*sin(i*3.14 / 180))/512.0);
		glVertex3dv(point1);
	}
	glTexCoord2d(220.0 / 512.0, (512.0 - 160.0) / 512.0);
	glVertex3dv(h1);
	glEnd();
}

void fun_up() //верхнее основание
{
	double a1[] = { -5,-2, 5 };
	double b1[] = { -5, 1, 5 };
	double c1[] = { -1, 1, 5 };
	double d1[] = { -2, 4, 5 };
	double e1[] = { 7, 4, 5 };
	double f1[] = { 5, 1, 5 };
	double g1[] = { 7, 1, 5 };
	double h1[] = { 7,-2, 5 };//для вогнутости
	double n[] = { 2,2,5 };
	double o[] = { 2.5, 8,5 }; //центр окружности
	float r = 6.02; //радиус окружности
	double point[] = { 0,0,5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(4 / 6.02) * 180 / 3.14;
	int betta = 360 - asin(4 / 6.02) * 180 / 3.14;

	//glColor3f(1.0f, 1.0f, 0.0f); //желтый
	
	//glColor4f(1.0f, 1.0f, 0.0f, 0.0f);

	double vn[] = { 0,0,0 };//вектор нормали
	normal(a1, b1, g1, vn);
	glNormal3dv(vn);
	//glNormal3f(0,0,1);

	glBegin(GL_QUADS); //A1B1G1H1
	//переделать, вместо z1 и z2 посчитать самому
	GLdouble z1 = 100.0 / 512.0;
	GLdouble z2 = (512.0 - 160.0) / 512.0;
		glTexCoord2d(z1, z2);
		glVertex3dv(a1);
		z1 = 100.0 / 512.0;
		z2 = (512.0 - 130.0) / 512.0;
		glTexCoord2d(z1, z2);
		glVertex3dv(b1);
		z1 = 220.0 / 512.0;
		z2 = (512.0 - 130.0) / 512.0;
		glTexCoord2d(z1, z2);
		glVertex3dv(g1);
		z1 = 220.0 / 512.0;
		z2 = (512.0 - 160.0) / 512.0;
		glTexCoord2d(z1, z2);
		glVertex3dv(h1);
	glEnd();

	glBegin(GL_QUAD_STRIP); //C1D1E1F1
		glTexCoord2d(130.0 / 512.0, 1.0 - 100.0 / 512.0);
		glVertex3dv(d1);
		glTexCoord2d(140.0/512.0, 1.0 - 130.0/512.0);
		glVertex3dv(c1);
		double osn[] = { -1,1,5 };
		int alala = 0;
		for (int i = alpha; i < betta; i++)
		{
			alala++;
			point[0] = o[0] + r*cos(i*3.14 / 180);
			point[1] = o[1] + r*sin(i*3.14 / 180);
			glTexCoord2d((175.0 + 60.2*cos(i*3.14 / 180.0))/512.0 , (512.0 - 60.0 + 60.2*sin(i*3.14 / 180.0)) / 512.0);
			glVertex3dv(point);
			double osn[] = { -1 + 0.06186*alala,1,5 };
			glTexCoord2d((140.0 + (60.0/97.0)*alala)/512.0, 1.0 - 130.0 / 512.0); //исправь
			glVertex3dv(osn);
		}
		glTexCoord2d(220.0 / 512.0, 1.0 - 100.0 / 512.0);
		glVertex3dv(e1);
		glTexCoord2d(200.0 / 512.0, 1.0 - 130.0 / 512.0);
		glVertex3dv(f1);
	glEnd();

	fun_up_vip(); //выпуклость на верхнем основании
}

void fun_side() //боковая сторона
{
	//проверить все нормали, правильно ли точки закидываю?
	
	//точки нижнего основания
	double a[] = { -5,-2,-5 };
	double b[] = { -5, 1,-5 };
	double c[] = { -1, 1,-5 };
	double d[] = { -2, 4,-5 };
	double e[] = { 7, 4,-5 };
	double f[] = { 5, 1,-5 };
	double g[] = { 7, 1,-5 };
	double h[] = { 7,-2,-5 };
	//точки верхнего основания
	double a1[] = { -5,-2, 5 };
	double b1[] = { -5, 1, 5 };
	double c1[] = { -1, 1, 5 };
	double d1[] = { -2, 4, 5 };
	double e1[] = { 7, 4, 5 };
	double f1[] = { 5, 1, 5 };
	double g1[] = { 7, 1, 5 };
	double h1[] = { 7,-2, 5 };
	//точки для дуги выпуклости
	double o11[] = { 1, 6,-5 }; //центр окружности
	double o12[] = { 1, 6, 5 }; //центр окружности
	int r1 = 10; //радиус окружности
	double point11[] = { 0,0,-5 }; //точка дуги, x и y выбираем в цикле
	double point12[] = { 0,0,5 }; //точка дуги, x и y выбираем в цикле
	int alpha1 = 180 + asin(0.8) * 180 / 3.14;
	int betta1 = 360 - asin(0.8) * 180 / 3.14;
	//соединяем точки верхнего и нижнего основания

	double vn[] = { 0,0,0 };//вектор нормали

	
	glBegin(GL_QUAD_STRIP);

	normal(c1, b1, b, vn);
	glNormal3dv(vn);

	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(467.0/512.0, (512.0 - 300.0)/512.0);
	glVertex3dv(b);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(467.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(b1);
	glColor3f(1.0f, 1.0f, 0.0f); //желтый

	normal(d1, c1, c, vn);
	glNormal3dv(vn);
	glTexCoord2d(427.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(c);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(427.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(c1);
	glColor3f(1.0f, 1.0f, 0.0f); //желтый

	glTexCoord2d(395.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(d);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(395.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(d1);

	//	glBegin(GL_QUAD_STRIP);
	//glColor3f(0.0f, 1.0f, 0.0f); //green
	//glVertex3dv(c1);
	//glVertex3dv(d1);
	double o22[] = { 2.5, 8,5 }; //центр окружности
	double o21[] = { 2.5, 8,-5 }; //центр окружности
	float r = 6.02; //радиус окружности
	double point[] = { 0,0,5 }; //точка дуги, x и y выбираем в цикле

	double point1[] = { 0,0,-5 }; //точка дуги, x и y выбираем в цикле
	int alpha = 180 + asin(4 / 6.02) * 180 / 3.14;
	int betta = 360 - asin(4 / 6.02) * 180 / 3.14;
	int alala = 0;
	
	double pred[] = { -2, 4, 5 };//сначала тут d1
	double pred1[] = { -2, 4, -5 };//сначала тут d

	for (int i = alpha; i < betta; i++) //вогнутость
	{
		alala++;
		point[0] = o21[0] + r*cos(i*3.14 / 180);
		point[1] = o21[1] + r*sin(i*3.14 / 180);
		point1[0] = o22[0] + r*cos(i*3.14 / 180);
		point1[1] = o22[1] + r*sin(i*3.14 / 180);

		normal(point, pred, pred1, vn);
		glNormal3dv(vn);

		glColor3f(1.0f, 1.0f, 0.0f); //желтый
		glTexCoord2d((395.0 -(102/97)*alala)/ 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(point1);
		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d((395.0 - (102 / 97)*alala) / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(point);

		pred[0] = point[0];
		pred[1] = point[1];
		pred1[0] = point1[0];
		pred1[1] = point1[1];
	}


	normal(f1, e1, e, vn);
	glNormal3dv(vn);

	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(293.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(e);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(293.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(e1);

	normal(g1, f1, f, vn);
	glNormal3dv(vn);

	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(257.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(f);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(257.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(f1);

	normal(h1, g1, g, vn);
	glNormal3dv(vn);

	
	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(237.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(g);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(237.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(g1);

	glColor3f(1.0f, 1.0f, 0.0f); //желтый
	glTexCoord2d(207.0 / 512.0, (512.0 - 300.0) / 512.0);
	glVertex3dv(h);
	glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	glTexCoord2d(207.0 / 512.0, (512.0 - 200.0) / 512.0);
	glVertex3dv(h1);
	glColor3f(1.0f, 1.0f, 0.0f); //желтый

	
	pred[0] = 7;//сначала тут h1
	pred[1] = -2;
	pred[2] = 5;
	pred1[0] = 7;//сначала тут h
	pred1[1] = -2;
	pred1[2] = -5;

	for (int i = betta1; i >= alpha1; i--) //выпуклость
	{
		point11[0] = o11[0] + r1*cos(i*3.14 / 180);
		point11[1] = o11[1] + r1*sin(i*3.14 / 180);
		point12[0] = o12[0] + r1*cos(i*3.14 / 180);
		point12[1] = o12[1] + r1*sin(i*3.14 / 180);

		normal(point12, pred, pred1, vn);
		glNormal3dv(vn);

		glTexCoord2d((207.0 - (127 / 73)*alala) / 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(point11);
		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d((207.0 - (127 / 73)*alala) / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(point12);
		glColor3f(1.0f, 1.0f, 0.0f); //желтый

		pred[0] = point12[0];
		pred[1] = point12[1];
		pred1[0] = point11[0];
		pred1[1] = point11[1];
	}

	//glColor3f(1.0f, 1.0f, 0.0f); //желтый
	//glVertex3dv(a);
	//glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
	//glVertex3dv(a1);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texId[1]);
	glBegin(GL_TRIANGLES);

		normal(b1, a1, a, vn);
		glNormal3dv(vn);

		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d(80.0 / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(a1);
		glColor3f(1.0f, 1.0f, 0.0f); //желтый
		glTexCoord2d(80.0 / 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(a);
		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d(50.0 / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(b1);

		normal(a, b, b1, vn);
		glNormal3dv(vn);

		glColor3f(1.0f, 1.0f, 0.0f); //желтый
		glTexCoord2d(80.0 / 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(a);
		glTexCoord2d(50.0 / 512.0, (512.0 - 300.0) / 512.0);
		glVertex3dv(b);
		glColor3f(1.0f, 0.5f, 0.0f); //оранжевый
		glTexCoord2d(50.0 / 512.0, (512.0 - 200.0) / 512.0);
		glVertex3dv(b1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId[0]);
}

void fun_circle(void) //portal icon
{
	double center[] = {2,0,0};
	GLfloat theta;
	GLfloat pi = acos(-1.0);
	GLfloat radius = 1.0f; 

	glBegin(GL_TRIANGLE_FAN);
		glColor3f(1.0f, 0.0f, 0.0f); //red
		glNormal3f(0,0,1);

		glTexCoord2d(0.0625, 0.9375);
		glVertex3f(center[0], center[1], 0.0f);

		for (GLfloat a = 0.0f; a <= 360.0f; a ++) {
			theta = pi * a / 180.0f;

			GLdouble x = 0.0625 + 0.0625 * cos(theta);
			GLdouble y = 0.9375 + 0.0625 * sin(theta);

			glTexCoord2d(x, y);
			glVertex3f(radius * cos(theta) + center[0], radius * sin(theta) + center[1], 0.0f);
		}
	glEnd();
}

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); 
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//Сглаживание
	glShadeModel(GL_SMOOTH);



	glRotatef(ugol++, 0, 0, 1);
	glTranslatef(-2.0f, 0.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texId[0]);
	fun_low(); //нижнее основание
	fun_side(); //боковая сторона

	fun_circle(); //круг внутри призмы

	glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
	fun_up(); //верхнее основание
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	


	char c[350];  //MaxiMuM len of message
	sprintf_s(c, "(T)Текстуры - %d\n(L)Свет - %d\n\nУправление светом:\n"
		"G - перемещение в горизонтальной плоскости,\nG+ЛКМ+перемещение по вертикальной линии\n"
		"R - установить камеру и свет в начальное положение\n"
		"F - переместить свет в точку камеры\n"
		"A - Включить/выключить альфа наложение", textureMode, lightMode);
	ogl->message = std::string(c);




}   
