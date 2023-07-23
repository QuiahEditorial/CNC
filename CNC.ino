// Por Roberto A. Zavala
// Quiah Editorial MR
// Breve : 
// Libro : https://www.amazon.com.mx/dp/B074TTGLL2
// 🙏🏼    : DNv7acPAeVBhTXbKv26itJecPG1SPy2o4F     MyDoge

// Se sustituye la solucion vía vectores por la solución vía polinomios
// Al finalizar el trazo el lapiz regresa a su pocición inicial
// Se agrega comando Dlta que mueve el lápiz a las coordenadas ingresadas

// Motor 3,5,6,9
// Motor 2,4,7,8
// MOSI 11, MISO 12, SCK 13, SC 10

#include<SPI.h>
#include<SD.h>

float RC[8]={0, 0, 0, 0, 0, 0, 0, 0 };
float D[3][2];
int   P = 30;
float t;
float T[3][2];
float B[31][2];
float d[2];
float tx = 0;
float ty = 0;
int   j = 0;
int   k = 0;
float m = 0;
int   cx = 0;
int   cy = 0;
float a = 0;
float X;
float Y;
int   nx = 50;
int   ny = 133.3;

int E[3][4] =
{
{ 0   , 0   , 0   , 0   } ,   // Motor eje x
{ 0   , 0   , 0   , 0   } ,   // Motor eje y
};

int Pas [8][4] = 
{
{ 255 , 0   , 0   , 0   } ,
{ 255 , 255 , 0   , 0   } ,
{ 0   , 255 , 0   , 0   } ,
{ 0   , 255 , 255 , 0   } ,
{ 0   , 0   , 255 , 0   } ,
{ 0   , 0   , 255 , 255 } ,
{ 0   , 0   , 0   , 255 } ,
{ 255 , 0   , 0   , 255 } ,
};

int PAS [36][4] = 
{
{ 255 , 0 , 0 , 0 } ,
{ 251 , 44  , 0 , 0 } ,
{ 240 , 87  , 0 , 0 } ,
{ 221 , 127 , 0 , 0 } ,
{ 195 , 164 , 0 , 0 } ,
{ 164 , 195 , 0 , 0 } ,
{ 128 , 221 , 0 , 0 } ,
{ 87  , 240 , 0 , 0 } ,
{ 44  , 251 , 0 , 0 } ,
{ 0 , 255 , 0 , 0 } ,
{ 0 , 251 , 44, 0 } ,
{ 0 , 240 , 87, 0 } ,
{ 0 , 221 , 127, 0 } ,
{ 0 , 195 , 164, 0 } ,
{ 0 , 164 , 195, 0 } ,
{ 0 , 128 , 221, 0 } ,
{ 0 , 87 , 240, 0 } ,
{ 0 , 44 , 251, 0 } ,
{ 0 , 0 , 255, 0 } ,
{ 0 , 0 , 251 , 44} ,
{ 0 , 0 , 240 , 87} ,
{ 0 , 0 , 221 , 127} ,
{ 0 , 0 , 195 , 164} ,
{ 0 , 0 , 164 , 195} ,
{ 0 , 0 , 128 , 221} ,
{ 0 , 0 , 87 , 240} ,
{ 0 , 0 , 44 , 251} ,
{ 0 , 0 , 0 , 255} ,
{ 44 , 0 , 251 , 0} ,
{ 87 , 0 , 240 , 0} ,
{ 127 , 0 , 221 , 0} ,
{ 164 , 0 , 195 , 0} ,
{ 195 , 0 , 164 , 0} ,
{ 221 , 0 , 128 , 0} ,
{ 240 , 0 , 87  , 0} ,
{ 251 , 0 , 44  , 0}
};

String comando;

String A;
char   z = ',' ;
char   c  ;
File   Bez;
int    Bzr = 1;    // la cantidad de bezieres en Bezier.txt
int    b;          // el contador de bezieres
int    pos = 0;    // la posición en bytes en Bezier.txt
int    s;          // el tamaño de Bezier.txt
int    S;
int    sa;

void setup()
{
Serial.begin(9600);
for ( int i=2; i<=9; i++ ) { pinMode(i, OUTPUT); }    pinMode(14, OUTPUT);
if (!SD.begin(10)) {Serial.println("SD-");} else{ Serial.println("SD+"); }
Serial.println("CNC Inicio:");
sa = 10;  sp();  t=1/P;
}

void loop()
{
if(Serial.available() > 0 )
  {
  comando = Serial.readStringUntil('\n');
  comando.trim();
  if      ( comando.equals ("Test"   ) ) test();
  else if ( comando.equals ("Dlta"   ) ) delt();
  else if ( comando.equals ("Play"   ) ) play();
  }
delay(200);
}

void test()  // =============================================================================================
{
Bez = SD.open("Bez.txt", FILE_READ);
S = Bez.size(); Serial.print("Tamaño : ");  Serial.println(S);
while (Bez.available()) { A = Bez.readStringUntil('\n');  Serial.println(A); Bzr++; delay(10);} A = "";
Serial.print("Total Bezieres : ");  Serial.println(Bzr);
b=0;
pos=0;
Bez.close();
}

void delt()
{
for ( int i=4; i<=7; i++ ) { RC[i] = 0; }
Serial.print( "X = " );
while (Serial.available() == 0 ){} X = Serial.parseFloat(); Serial.println(X); RC[2] = X;
while (Serial.available() == 0 ){}Y = Serial.parseFloat();
Serial.print( "Y = " );
while (Serial.available() == 0 ){} Y = Serial.parseFloat(); Serial.println(Y); RC[3] = Y;
Serial.print( "Moviendo... " );
sol();
Serial.println( "Listo" );
for ( int i=2; i<=9; i++ ) { digitalWrite(i,0);}
}

void play()
{
for ( int q=0; q<=Bzr; q++ ) { lectura(); }
Serial.print("Listo");
fin();
}

void lectura()
{
Bez = SD.open("Bez.txt", FILE_READ);
if (Bez)
  { Bez.seek(pos);
    A = Bez.readStringUntil(',');
    if ( A.equals("+")||A.equals("\n+") ) { Serial.print(A); sa = 10; sp(); ler(); sa = 40; sp(); }
    else                                  { Serial.print(A); if ( A.equals("\n%") ) { return; }
                                                           else { ler();}
                                          }
  X += RC[2];
  Y += RC[3];
  } else {Serial.print("!SD");}
    Bez.close();
}

void ler() 
{ for ( int n=2; n<=7; n++ ) { A = Bez.readStringUntil(','); RC[n] = A.toFloat(); pos = Bez.position();}
                               Serial.print("Bezier "); sol(); Serial.print(b); b++; Serial.print(" +");}

void sp()
{
int mp = 13.87 * sa + 460;           // SG90: mínimo 459 us, Máximo 3513 um. LOW mínimo: 3us0
int low = 3516-mp;
for( int i=0; i<=60; i++ )             // Envía pulsos durante el tiempo que tarda desde 0 hasta 220°
  {
  digitalWrite(14, HIGH); delayMicroseconds (mp);
  digitalWrite(14, LOW);  delayMicroseconds(low);
  }
  delay(200);
}

void sol() // =============================================================================================
{
for ( int i=0; i<=P; i++)
{
t =   0.0333*i;
B[i][0] = RC[0]*(1-t)*(1-t)*(1-t) + RC[4]*3*t*(1-t)*(1-t)+RC[6]*3*t*t*(1-t)+RC[2]*t*t*t;
B[i][1] = RC[1]*(1-t)*(1-t)*(1-t) + RC[5]*3*t*(1-t)*(1-t)+RC[7]*3*t*t*(1-t)+RC[3]*t*t*t;
//Serial.print("("); Serial.print(B[i][0]); Serial.print(", "); Serial.print(B[i][1]);Serial.println(")");
}
//Serial.print("("); Serial.print(B[29][0]); Serial.print(", "); Serial.print(B[29][1]);Serial.println(")");
impres();
}

void impres()
{
//Serial.print("Escribiendo... ");
for ( int i=0; i<=P-1; i++ )
    { d[0] = B[i+1][0] - B[i][0];  d[1] = B[i+1][1] - B[i][1];
      tx = d[0] * nx ;            ty = d[1] * ny ;
      cnc();
    }
}

void cnc () // ============================================================================================
{
if ( tx == 0 ) { if      ( ty > 0 ) { for ( int i=1; i<=ty; i++ ) { Pasy(); Mover(); }    cy = 0; cx = 0; }
                 else if ( ty < 0 ) { for ( int i=1; i<=ty; i++ ) { paSy(); Mover(); }    cy = 0; cx = 0; }
               }
if ( tx >  0 ) { if      ( ty > 0 ) { for ( int k=1; k <= 10 * tx; k++ )
                                          { m = ty / tx ; Pasx(); Mover(); a = (m / 10 * cx) - cy ;
                                            for ( int i=1; i<=a; i++ ) { Pasy(); Mover(); }
                                          } while ( cy < abs(ty) ) { Pasy(); }            cy = 0; cx = 0;
                                    }
               if        ( ty < 0 ) { for ( int k=1; k <= 10 * tx; k++ )
                                          { m = -ty / tx ; Pasx(); Mover(); a = (m / 10 * cx) - cy ;
                                            for ( int i=1; i<=a; i++ ) { paSy(); Mover(); }
                                          } while ( cy < abs(ty) ) { paSy(); }            cy = 0; cx = 0;
                                    }
               if        ( ty ==0 ) { for ( int i=0; i<=tx; i++ ) { Pasx(); }  Mover(); } cy = 0; cx = 0;
               }
if ( tx <  0 ) { if      ( ty > 0 ) { for ( int k=1; k <= 10 * -tx; k++ )
                                          { m = ty / -tx ; paSx(); Mover(); a = (m / 10 * cx) - cy ;
                                            for ( int i=1; i<=a; i++ ) { Pasy(); Mover(); }
                                          } while ( cy < abs(ty) ) { Pasy(); }            cy = 0; cx = 0;
                                    }
                 if      ( ty < 0 ) { for ( int k=1; k <= 10 * -tx; k++ )
                                          { m = ty / tx ; paSx(); Mover(); a = (m / 10 * cx) - cy ;
                                            for ( int i=1; i<=a; i++ ) { paSy(); Mover(); }
                                          } while ( cy < abs(ty) ) { paSy(); }            cy = 0; cx = 0;
                                    }
                 if ( ty == 0 ) { for ( int i=0; i<=tx; i++ ) { Pasx(); }  Mover(); }
               }
}           // ============================================================================================

void Mover()
{
// Ejecuta los pasos lógicos, después juntarlos en una sola matriz
digitalWrite(3, E[0][0]);
digitalWrite(5, E[0][1]);
digitalWrite(6, E[0][2]);
digitalWrite(9, E[0][3]);
digitalWrite(2, E[1][0]);
digitalWrite(4, E[1][1]);
digitalWrite(7, E[1][2]);
digitalWrite(8, E[1][3]);
// Serial.print(cx); Serial.print("  ");  Serial.println(cy);
delay(1);
}

void fin()
{
b=0;  pos=0;
sa = 10; sp();
for ( int i=4; i<=7; i++ ) { RC[i] = 0; }
RC[2]= -X; RC[3] = -Y; sol();
for ( int i=2; i<=9; i++ ) { digitalWrite(i,0);}
return;
}

void Pasx()
{
if ( j > 35 ) { j = 0; }
E[0][0] = PAS[j][0];
E[0][1] = PAS[j][1];
E[0][2] = PAS[j][2];
E[0][3] = PAS[j][3];
cx++;
j++;
}
void paSx()
{
if ( j < 0 ) { j = 35; }
E[0][0] = PAS[j][0];
E[0][1] = PAS[j][1];
E[0][2] = PAS[j][2];
E[0][3] = PAS[j][3];
cx++;
j--;
}

void Pasy()
{
if ( k > 7 ) { k = 0; }
E[1][0] = Pas[k][0];
E[1][1] = Pas[k][1];
E[1][2] = Pas[k][2];
E[1][3] = Pas[k][3];
cy++;
k++;
}
void paSy()
{
if ( k < 0 ) { k = 7; }
E[1][0] = Pas[k][0];
E[1][1] = Pas[k][1];
E[1][2] = Pas[k][2];
E[1][3] = Pas[k][3];
cy++;
k--;
}
