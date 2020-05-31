#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define TIMER_ID 0
#define TIMER_INTERVAL 50

static void init(int, char**);

static void on_display(void);

static void on_keyboard(unsigned char, int, int);

static void on_reshape(int, int);

static void on_timer(int);

static int timer_active;                // Fleg koji odredjuje stanje tajmera
static float x_basket = 10;             // Trenutne koordinate kosa
static float x_curr = 0, y_curr = 1;    // Trenutne koordinate lopte
static float radius = 0.37;             // Poluprecnik lopte
static float v_b = 0.15;                // Vektorska brzina kretanja obruca
static int animation = 1;               // Fleg koji odredjuje status animacije
static float v_y = 0.1;                 // Vektorska brzina kretanja lopte
static float jump = 1;                  // Odskok lopte na kraju
static int flag = 0;                    // Fleg koji sluzi da se broj poena poveca samo jednom ako lopta prodje kroz obruc
static int bucket = 0;                  // Poeni
static unsigned kraj = 0;               // Fleg koji sluzi za Ispisivanje kraja na konzoli
static int period;                      // Promenljiva za merenje vremena
static unsigned k = 0;                  // Promenljiva koja sluzi za proveru u slucaju prolaska lopte kroz obruc u trenutnom ciklusu

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

        glutKeyboardFunc(on_keyboard);
        glutReshapeFunc(on_reshape);
        glutDisplayFunc(on_display);
}

static void on_keyboard(unsigned char key, int x, int y)
{
        switch(key) 
        {
                case 27:
//                         Zavrsava se program
                        if(kraj == 0)
                                printf("Game over!\n");
                        exit(0);
                        break;
                case 'g':
                case 'G':
//                         Pokrece se animacija
                        if(!timer_active) 
                        {
                                timer_active = 1;
                                printf("New game!\n");
                                glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
                        }
                        break;
                case 's':
                case 'S':
//                         Zaustavlja se animacija
                        printf("Stop!\n");
                        timer_active = 0;
                        break;
                case 'r':
                case 'R':
                        printf("Restart!\n");
                        x_curr = 0;
                        y_curr = 1;
                        x_basket = 10;
                        timer_active = 0;
                        animation = 1;
                        bucket = 0;
                        jump = 1;
                        period = 0;
                        kraj = 0;
                        glClearColor(1, 1, 1, 0);
                        break;
                case 'm':
                case 'M':
                        if(animation){
                                v_y += 0.7;
                                if(v_y >= 0.6)
                                        v_y = 0.6;
                        }
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
        gluPerspective(45, (float) width / height, 1, 1500);
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
        if(x_basket <= -10)
                x_basket = 10;
        
//      Uslov da lopta ne ode ispod poda
        if(y_curr <= -(3-radius))
                y_curr = -(3-radius);
        
//      Uslov da je kraj igre ako udari u plafon
        if(y_curr >= (3.75-radius)){
                y_curr = 3.75-radius;
                v_y = 0;
                glClearColor(0.7, 0.8, 0.8, 0);
                kraj++;
                animation = 0;
        }

//      Uslov da je kraj igre ako udari u pod
        if(y_curr <= -(3-radius)){
                v_y += jump;
                glClearColor(0.7, 0.8, 0.8, 0);
                kraj++;
                animation = 0;
                jump -= 0.2;
        }

//      Uslov da se lopta ne pojavljuje ako je kraj i lopta se ne vidi na ekranu
        if(animation == 0 && x_basket >= 7){
                x_basket = 10;
                kraj++;
        }
        
//      Uslov da se lopta ne pojavljuje ako je kraj i lopta se ne vidi na ekranu
        if(animation == 0 && x_basket <= -10){
                timer_active = 0;
                kraj++;
        }
        
//      Povecava se broj poena ako lopta prodje kroz obruc
        if((x_basket < 0.5 && x_basket > -0.5) && (y_curr < 0.4 && y_curr > -0.4) && animation != 0){
                if(flag == 0)
                        bucket += 1;
                b += 1;
                k++;
        }
        
//      Ako lopta padne na levu ili desnu stranu obruca odbija se
        if(((x_basket > 0.5 && x_basket < 1) || (x_basket > -1 && x_basket < -0.5)) && (y_curr < 0.6 && y_curr > 0.2) && v_y < 0 && animation != 0){
                v_y += 0.8;
        }
        
//      Lopta se odbija od obruca u slucaju da se nalazi ispod njega
        if((x_basket < 1 && x_basket > -1) && (y_curr < -0.4 && y_curr > -0.8) && animation != 0){
                v_y = -0.2;
        }
        
//      Ispisivanje trenutnog rezultata na terminal u slucaju pogotka
        if(k > 0 && x_basket < -2){
                printf("Score! +1\n");
                printf("Total score %d\n", bucket);
                k = 0;
        }
        
//      Ako je obruc prosao, a lopta nije prosla kroz njega - game over
        if(flag == 0 && x_basket <= -3){
                glClearColor(0.7, 0.8, 0.8, 0);
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
        
//      Ispisivanje na terminal u slucaju kraja
        if(kraj == 1){
                        printf("Game over!\n");
                        printf("Total score: %d\n", bucket);
        }
        
        glutPostRedisplay();
        
        if(timer_active)
                glutTimerFunc(50, on_timer, 0);
}

static void on_display(void)
{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//      Postavlja se kamera
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
        
//      Implementacija plafona i poda
        glPushMatrix();
                glColor3f(0, 0.3, 0.3);
                glTranslatef(0, 73, 0);
                glutSolidCube(100);
        glPopMatrix();
        
        glPushMatrix();
                glColor3f(0, 0.3, 0.3);
                glTranslatef(0, -68, 0);
                glutSolidCube(100);
        glPopMatrix();
                
//      Implementacija lopte
        glPushMatrix();
                glTranslatef(x_curr, y_curr, 0);
                glColor3f(0.8, 0.3, 0);
                glutSolidSphere(radius, 100, 100);
        glPopMatrix();
        
//      Implementacija obruca
        glPushMatrix();
                glRotatef(90, 1, 0, 0);
                glTranslatef(x_basket, 0, 0);
                glColor3f(0.7, 0.1, 0);
                glutSolidTorus(0.06, 0.85, 100, 100);
        glPopMatrix();
        
//      Bitmapa za broj poena
        glPushMatrix();
        glScalef(1.3, 1.3, 0);
        glColor3f(0.7, 0.3, 0.7);
        glRasterPos3f(2.88, 3, 0);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (bucket/100%10) +'0');
        glRasterPos3f(2.95, 3, 0);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (bucket/10%10) +'0');
        glRasterPos3f(3.02, 3, 0);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (bucket%10) +'0');
        glPopMatrix();
        
//      Bitmapa za vreme
        glPushMatrix();
        glScalef(1.3, 1.3, 0);
        glColor3f(1, 0, 0);
        glRasterPos3f(-3.11, 3, 0);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (period/100000%10)+'0');
        glRasterPos3f(-3.04, 3, 0);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (period/10000%10)+'0');
        glRasterPos3f(-2.97, 3, 0);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (period/1000%10)+'0');
        glRasterPos3f(-2.90, 3, 0);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (period/100%10)+'0');
        glRasterPos3f(-2.83, 3, 0);
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (period/10%10)+'0');
        glPopMatrix();
        
        glutSwapBuffers();
}