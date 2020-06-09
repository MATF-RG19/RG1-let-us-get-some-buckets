#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h> 
#include <string.h>
#include "image.h"

#define FILENAME0 "floor.bmp"
#define FILENAME1 "logo.bmp"
#define FILENAME2 "vice.bmp"
#define FILENAME3 "fire.bmp"

#define TIMER_ID 0
#define TIMER_INTERVAL 50

static GLuint names[4];

static void init(int, char**);

static void on_display(void);

static void on_keyboard(unsigned char, int, int);

static void on_reshape(int, int);

static void on_timer(int);

static int timer_active;                // Fleg koji odredjuje stanje tajmera
static float x_basket = 10,y_basket = 0;// Trenutne koordinate kosa
static float x_curr = 0, y_curr = 1;    // Trenutne koordinate lopte
static float radius = 0.38;             // Poluprecnik lopte
static float v_b = 0.15;                // Vektorska brzina kretanja obruca
static int animation = 1;               // Fleg koji odredjuje status animacije
static float v_y = 0.1;                 // Vektorska brzina kretanja lopte
static float jump = 1;                  // Odskok lopte na kraju
static int flag = 0;                    // Fleg koji sluzi da se broj poena poveca samo jednom ako lopta prodje kroz obruc
static int bucket = 0;                  // Poeni
static unsigned kraj = 0;               // Fleg koji sluzi za Ispisivanje kraja na konzoli
static int period;                      // Promenljiva za merenje vremena
static unsigned k = 0;                  // Promenljiva koja sluzi za proveru u slucaju prolaska lopte kroz obruc u trenutnom ciklusu
static float v_a = 0;                   // Promenljiva koja sluzi za fokusiranje logoa
static float v_z = 0;                   // Promenljiva koja sluzi za fokusiranje logoa
static int background = 0;              // Promenljiva koja sluzi za ukljucivanje teksture pozadine
static float fade = 0;                  // Promenljiva koja sluzi za pomeranje ispisa nakon pogotka
static char score[10] = "SCORE!+1";     // Rezultat za ispis nakon pogotka
static char total_score[50] = "Total score: "; // Rezultat za ispis na kraju
static char game_over[50] = "Game over!"; // Ispis u slucaju kraja

int main(int argc, char* argv[])
{
        init(argc, argv);
        
//      Postavlja se boja
        glClearColor(1, 1, 1, 0);
        glEnable(GL_DEPTH_TEST);
        
        glutMainLoop();

        return 0;
}

static void init(int argc, char* argv[]) {
//      Inicijalizacija aplikacije, kreiranje prozora i pozivanje funkcija
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

        glutInitWindowSize(700, 700);
        glutInitWindowPosition(300, 100);
        glutCreateWindow(argv[0]);
        
        srand(time(0));

        glutKeyboardFunc(on_keyboard);
        glutReshapeFunc(on_reshape);
        glutDisplayFunc(on_display);
        
        /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
        GLfloat light_position[] = { 1, 1, 1, 0 };

        /* Ambijentalna boja svetla. */
        GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1 };

        /* Difuzna boja svetla. */
        GLfloat light_diffuse[] = { 0.1, 0.1, 0.1, 1 };

        /* Spekularna boja svetla. */
        GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
        
        /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        
        glEnable(GL_TEXTURE_2D);

        glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);
        
        Image * image;
        
        image = image_init(0, 0);

        /* Generisu se identifikatori tekstura. */
        glGenTextures(4, names);

        /* Kreira se prva tekstura. */
        image_read(image, FILENAME0);

        glBindTexture(GL_TEXTURE_2D, names[0]);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
        
        /* Kreira se druga tekstura. */
        image_read(image, FILENAME1);

        glBindTexture(GL_TEXTURE_2D, names[1]);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
        
        /* Kreira se treca tekstura. */
        image_read(image, FILENAME2);

        glBindTexture(GL_TEXTURE_2D, names[2]);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
        
        /* Kreira se cetvrta tekstura. */
        image_read(image, FILENAME3);

        glBindTexture(GL_TEXTURE_2D, names[3]);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
        
        /* Iskljucujemo aktivnu teksturu */
        glBindTexture(GL_TEXTURE_2D, 0);

        /* Unistava se objekat za citanje tekstura iz fajla. */
        image_done(image);
}

static void on_keyboard(unsigned char key, int x, int y)
{
        switch(key) 
        {
                case 27:
//                      Zavrsava se program
                        if(kraj == 0)
                                printf("Game over! Total score: %d\n", bucket);
                        glDeleteTextures(3, names);
                        exit(0);
                        break;
                case 'g':
                case 'G':
//                      Pokrece se animacija
                        if(!timer_active) 
                        {
                                timer_active = 1;
                                printf("New game!\n");
                                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                        }
                        break;
                case 's':
                case 'S':
//                      Zaustavlja se animacija
                        printf("Stop!\n");
                        timer_active = 0;
                        break;
                case 'r':
                case 'R':
//                      Restartovanje programa
                        printf("Restart!\n");
                        x_curr = 0;
                        y_curr = 1;
                        x_basket = 10;
                        y_basket = 0;
                        timer_active = 0;
                        animation = 1;
                        bucket = 0;
                        jump = 1;
                        period = 0;
                        kraj = 0;
                        v_a = 0;
                        v_z = 0;
                        glutPostRedisplay();
                        break;
                case 'm':
                case 'M':
//                      Skok
                        if(animation){
                                v_y += 0.7;
                                if(v_y >= 0.6)
                                        v_y = 0.6;
                        }
                        break;
                case 'b':
                case 'B':
//                      Postavljanje pozadine
                        background = 1;
                        glutPostRedisplay();
                        break;
                case 'w':
                case 'W':
//                      Gasenje pozadine
                        background = 0;
                        glutPostRedisplay();
                        break;
        }
}

static void on_reshape(int width, int height)
{
//      Postavlja se viewport
        glViewport(0, 0, width, height);

//      Postavljaju se parametri projekcije
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45, (float) width / height, 1, 100);
}


static void on_timer(int value)
{
//      Proverava se da li callback dolazi od odgovarajuceg tajmera
        if(value != TIMER_ID)
                return;
        
//      Vreme
        if(animation)
                period += 1;
        
//      Pomocna promenljiva za povecavanje broja poena
        int b = 0;
        
        
//      Brzina kojom lopta pada
        v_y -= .1;
        if(v_y <= -0.3)
                v_y = -0.3;
        
        y_curr += v_y;
        
//      Brzina kojom se kos krece i simulacija njegovog kretanja
        x_basket -= v_b;
        if(x_basket <= -10){
                x_basket = 10;
//                 if(bucket >= 0) {
//                         y_basket = (float)rand()/(float)(RAND_MAX)*4 - 1.2;
//                 }
//                 printf("%f", y_basket);
        }
        
//      Uslov da lopta ne ode ispod poda
        if(y_curr <= -(3-radius))
                y_curr = -(3-radius);
        
//      Uslov da je kraj igre ako udari u plafon
        if(y_curr >= (3.75-radius)){
                y_curr = 3.75-radius;
                v_y = 0;
                kraj++;
        }

//      Uslov da je kraj igre ako udari u pod
        if(y_curr <= -(3-radius)){
                v_y += jump;
                kraj++;
                jump -= 0.2;
        }

//      Uslov da se obruc ne pojavljuje ako je kraj i ako se ne vidi na ekranu
        if(animation == 0 && x_basket >= 7){
                x_basket = 10;
                kraj++;
        }
        
//      Uslov da se obruc ne pojavljuje ako je kraj i ako se ne vidi na ekranu
        if(animation == 0 && x_basket <= -10){
                timer_active = 0;
                kraj++;
        }
        
//      Povecava se broj poena ako lopta prodje kroz obruc
        if((x_basket < 0.5 && x_basket > -0.5) && (y_curr < y_basket + 0.4 && y_curr > y_basket -0.4) && animation != 0){
                if(flag == 0){
                        bucket += 1;
                }
                b += 1;
                k++;
        }
        
//      Ako lopta padne na levu ili desnu stranu obruca odbija se
        if(((x_basket > 0.5 && x_basket < 1.1) || (x_basket > -1.1 && x_basket < -0.5)) && (y_curr < y_basket + 0.6 && y_curr > y_basket + 0.2) && v_y < 0 && animation != 0){
                v_y += 0.8;
        }
        
//      Lopta se odbija od obruca u slucaju da se nalazi ispod njega
        if((x_basket < 1 && x_basket > -1) && (y_curr < y_basket && y_curr > y_basket - 0.5) && animation != 0){
                v_y = -0.2;
        }
        
//      Ispisivanje trenutnog rezultata na terminal u slucaju pogotka
        if(k > 0 && x_basket < -2){
                printf("Score! +1\n");
                k = 0;
        }
        
//      Ako je obruc prosao, a lopta nije prosla kroz njega - game over
        if(flag == 0 && x_basket <= -3){
                kraj++;
                animation = 0;
        }
        
//      Provera koja sluzi da se broj poena uveca samo jednom ako prodje lopta kroz obruc
        if(b > 0)
                flag = 1;
        
//      Resetovanje flega za novi ciklus
        if(x_basket > 7){
                flag = 0;
        }
        
        if(flag == 1) {
                fade += 0.01;
        }
        else
                fade = 0;
        
//      Ispisivanje na terminal u slucaju kraja
        if(kraj == 1){
                printf("--------------------------\n");
                printf("Game over! Total score: %d\n", bucket);
                printf("Total time: %d\n", period/10);
                if(bucket/100)
                        total_score[strlen(total_score)] = bucket/100%10 + '0';
                if(bucket/10)
                        total_score[strlen(total_score)] = bucket/10%10 + '0';
                total_score[strlen(total_score)] = bucket%10 + '0';
                kraj++;
                animation = 0;
        }
        
        if(kraj > 0) {
                v_a -= .1;
                if(v_a <= -6)
                        v_a = -6;
                v_z -= .05;
                if(v_z <= -1)
                        v_z = -1;
        }
        
        glutPostRedisplay();
        
        if(timer_active)
                glutTimerFunc(50, on_timer, 0);
}

static void on_display(void)
{
//      Postavljanje koeficijenata materijala
        static GLfloat ambient_coeffs[] = { 0.3, 0.3, 0.3, 1 };

        static GLfloat ambient_coeffs1[] = { 3, 0, 0, 1 };
        
        static GLfloat diffuse_coeffs[] = { 0.0, 0.0, 0.8, 1 };

        static GLfloat specular_coeffs[] = { 1, 1, 1, 1 };

        static GLfloat shininess = 20;

//      Brise se prethodni sadrzaj prozora.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//      Podesava se vidna tacka. 
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if(kraj == 0)
                gluLookAt(0, 1, 10, 0, 0, 0, 0, 1, 0);
        else
                gluLookAt(0, 1+v_z, 10+v_a, 0, 0, 0, 0, 1, 0);

        if(flag){
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);
                for(int i = 0; i < strlen(score); i++){
                        glRasterPos3f(-0.9 + i*0.2, 2.3 + fade, 0);
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score[i]);
                }
                
                glPushMatrix();
                        glTranslatef(2.4, 1.2 + fade, 0);
                        glRotatef(90, 0, 0, 1);
                        glBindTexture(GL_TEXTURE_2D, names[3]);
                        glBegin(GL_QUADS);
                                glNormal3f(0, 0, 1);

                                glTexCoord2f(0, 0);
                                glVertex3f(1, 1, 0);

                                glTexCoord2f(1, 0);
                                glVertex3f(1, 1.5, 0);

                                glTexCoord2f(1, 1);
                                glVertex3f(1.5, 1.5, 0);

                                glTexCoord2f(0, 1);
                                glVertex3f(1.5, 1, 0);
                        glEnd();
                        glBindTexture(GL_TEXTURE_2D, 0);
                glPopMatrix();
        }
        
//         Implementacija pozadine
        if(background){
                glPushMatrix();
                        glBindTexture(GL_TEXTURE_2D, names[2]);
                        glBegin(GL_QUADS);
                                glNormal3f(0, 0, 1);

                                glTexCoord2f(0, 0);
                                glVertex3f(-10, -3.1, 0);

                                glTexCoord2f(1, 0);
                                glVertex3f(8.6, -3.1, 0);

                                glTexCoord2f(1, 1);
                                glVertex3f(8.6, 3.75, 0);

                                glTexCoord2f(0, 1);
                                glVertex3f(-10, 3.75, 0);
                        glEnd();
                        glBindTexture(GL_TEXTURE_2D, 0);
                glPopMatrix();
        }
        
//      Implementacija plafona i poda
        glPushMatrix();
                glTranslatef(0, 3, 0);
                glBindTexture(GL_TEXTURE_2D, names[0]);
                glBegin(GL_QUADS);
                        glNormal3f(0, 0, 1);

                        glTexCoord2f(0, 0);
                        glVertex3f(-10, 0.7, 0);

                        glTexCoord2f(1, 0);
                        glVertex3f(10, 0.7, 0);

                        glTexCoord2f(1, 1);
                        glVertex3f(10, 3, 0);

                        glTexCoord2f(0, 1);
                        glVertex3f(-10, 3, 0);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
        
        glPushMatrix();
                glTranslatef(0, 3, 0);
                glBindTexture(GL_TEXTURE_2D, names[0]);
                glBegin(GL_QUADS);
                        glNormal3f(0, 0, 1);

                        glTexCoord2f(0, 0);
                        glVertex3f(-10, -9, 0);

                        glTexCoord2f(1, 0);
                        glVertex3f(10, -9, 0);

                        glTexCoord2f(1, 1);
                        glVertex3f(10, -6.06, 0);

                        glTexCoord2f(0, 1);
                        glVertex3f(-10, -6.06, 0);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
                
//      Implementacija obruca
        glPushMatrix();
                glTranslatef(x_basket, y_basket, 0);
                
                if(flag == 0)
                        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
                else
                        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs1);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);
                
                glLineWidth(20);
                glBegin(GL_LINE_STRIP);
                        glVertex3f(-2, -0.3, 0);
                        glVertex3f(-2, 2.2, 0);
                        glVertex3f(2, 2.2, 0);
                        glVertex3f(2, -0.3, 0);
                        glVertex3f(-2, -0.3, 0);
                glEnd();
                
                glBegin(GL_LINE_STRIP);
                        glVertex3f(-0.8, 0.15, 0);
                        glVertex3f(-0.8, 1, 0);
                        glVertex3f(0.8, 1, 0);
                        glVertex3f(0.8, 0.15, 0);
                        glVertex3f(-0.8, 0.15, 0);
                glEnd();
                
                glRotatef(90, 1, 0, 0);
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);
                glutSolidTorus(0.06, 0.85, 100, 100);
        glPopMatrix();
        
//      Implementacija lopte
        glPushMatrix();
                glTranslatef(x_curr, y_curr, 0);
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);
                glutSolidSphere(radius, 100, 100);
        glPopMatrix();
        
//      Implementacija mrezice
        glPushMatrix();
                glTranslatef(x_basket, y_basket, 0);
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);
                glBegin(GL_LINES);
                        glVertex3f(-0.8, 0, 0);
                        glVertex3f(-0.6, -0.7, 0);
                        
                        glVertex3f(-0.6, -0.7, 0);
                        glVertex3f(0.6, -0.7, 0);
                        
                        glVertex3f(0.6, -0.7, 0);
                        glVertex3f(0.8, 0, 0);
                        
                        glVertex3f(-0.7, -0.35, 0);
                        glVertex3f(0.7, -0.35, 0);
                        
                        glVertex3f(0, -0.1, 0);
                        glVertex3f(0, -0.7, 0);
                        
                        glVertex3f(-0.6, 0, 0);
                        glVertex3f(-0.4, -0.7, 0);
                        
                        glVertex3f(0.6, 0, 0);
                        glVertex3f(0.4, -0.7, 0);
                glEnd();
        glPopMatrix();
        
//         NBA logo
        if(kraj > 0){
        glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, names[1]);
                glBegin(GL_QUADS);
                        glNormal3f(0, 0, 1);

                        glTexCoord2f(0, 0);
                        glVertex3f(-1, -1.85, 0);

                        glTexCoord2f(1, 0);
                        glVertex3f(1, -1.85, 0);

                        glTexCoord2f(1, 1);
                        glVertex3f(1, 2, 0);

                        glTexCoord2f(0, 1);
                        glVertex3f(-1, 2, 0);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
        }
                
        if(kraj > 0) {
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);
                for(int i = 0; i < strlen(game_over); i++){
                        glRasterPos3f(-0.85 + i*0.2, 1.55, 0);
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, game_over[i]);
                }
                
                for(int i = 0; i < strlen(total_score); i++){
                        glRasterPos3f(-0.55 + i*0.09, 1.4, 0);
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, total_score[i]);
                }
        }
        
//      Bitmapa za broj poena
        glPushMatrix();
                glScalef(1.28, 1.28, 0);
                
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);
                
                if(bucket/100){
                        glRasterPos3f(2.82, 3, 0);
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (bucket/100%10) +'0');
                }
                
                if(bucket/10){
                        glRasterPos3f(2.92, 3, 0);
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (bucket/10%10) +'0');
                }
                
                glRasterPos3f(3.02, 3, 0);
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (bucket%10) +'0');
        glPopMatrix();
        
//      Bitmapa za vreme
        glPushMatrix();
                glScalef(1.28, 1.28, 0);
                
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);
                
                if(period/100000){
                        glRasterPos3f(-3.11, 3, 0);
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (period/100000%10)+'0');
                }
                
                if(period/10000){
                        glRasterPos3f(-3.01, 3, 0);
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (period/10000%10)+'0');
                }
                
                if(period/1000){
                        glRasterPos3f(-2.91, 3, 0);
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (period/1000%10)+'0');
                }
                
                if(period/100){
                        glRasterPos3f(-2.81, 3, 0);
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (period/100%10)+'0');
                }
                
                glRasterPos3f(-2.71, 3, 0);
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (period/10%10)+'0');
        glPopMatrix();
        
        glutSwapBuffers();
}