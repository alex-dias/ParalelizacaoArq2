/*
Modified from: http://rosettacode.org/wiki/Mandelbrot_set#PPM_non_interactive

c program:
--------------------------------
1. draws Mandelbrot set for Fc(z)=z*z +c
using Mandelbrot algorithm ( boolean escape time )
-------------------------------
2. technique of creating ppm file is  based on the code of Claudio Rocchini
http://en.wikipedia.org/wiki/Image:Color_complex_plot.jpg
create 24 bit color graphic file ,  portable pixmap file = PPM
see http://en.wikipedia.org/wiki/Portable_pixmap
to see the file use external application ( graphic viewer)
*/

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <math.h>

int main()
{
		double dois = 2;

        /* screen ( integer) coordinate */
        int iX,iY;
        const int iXmax = 16384;
        const int iYmax = 16384;

        /* world ( double) coordinate = parameter plane*/
        double Cx,Cy;
        const double CxMin=-2.5;
        const double CxMax=1.5;
        const double CyMin=-2.0;
        const double CyMax=2.0;

        /* */
        double PixelWidth=(CxMax-CxMin)/iXmax;
        double PixelHeight=(CyMax-CyMin)/iYmax;

        /* color component ( R or G or B) is coded from 0 to 255 */
        /* it is 24 bit color RGB file */
        const int MaxColorComponentValue=255;
        FILE * fp;
        char *filename="mandelbrot.ppm";
        static unsigned char color[3];

        /* Z=Zx+Zy*i  ;   Z0 = 0 */
        double Zx, Zy;
        double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */

        /*  */
        int Iteration;
        const int IterationMax=256;

        /* bail-out value , radius of circle ;  */
        const double EscapeRadius=2;
        double ER2=EscapeRadius*EscapeRadius;

        /*create new file,give it a name and open it in binary mode  */
        fp= fopen(filename,"wb"); /* b -  binary mode */
        /*write ASCII header to the file*/
        fprintf(fp,"P6\n %d\n %d\n %d\n",iXmax,iYmax,MaxColorComponentValue);
        /* compute and write image data bytes to the file*/
        for(iY=0;iY<iYmax;iY++)
        {
             Cy=CyMin + iY*PixelHeight;

             /*__asm{
				 vmovups ymm0, iY
				 vmovups ymm1, PixelHeight
				 vmulps ymm2, ymm1, ymm0
				 vmovups ymm3, CyMin
				 vaddps ymm4, ymm3, ymm2
				 vmovups Cy, ymm4

				vzeroall

             }*/

             if (fabs(Cy)< PixelHeight/2) Cy=0.0; /* Main antenna */
             for(iX=0;iX<iXmax;iX++)
             {

                        Cx=CxMin + iX*PixelWidth;
                        /* initial value of orbit = critical point Z= 0 */
                        Zx=0.0;
                        Zy=0.0;
                        Zx2=Zx*Zx;

						/*_asm{
							vmovups ymm0, Zx
							vmulps ymm0, ymm0, ymm0
							vmovups Zx2, ymm0
							vzeroall
						}*/

                        Zy2=Zy*Zy;
                        /* */
                        for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++)
                        {
                            Zy=2*Zx*Zy + Cy;

							/*__asm{
								movups xmm0, Zx
								movups xmm1, Zy
								vmulps xmm0, xmm1, xmm0
								movups xmm3, dois
								vmulps xmm0, xmm0, xmm3
								movups xmm1, Cy
								vaddps xmm0, xmm1, xmm0
								movups Zy, xmm0

							}*/

                            Zx=Zx2-Zy2 +Cx;
                            Zx2=Zx*Zx;
                            Zy2=Zy*Zy;
                        };
                        /* compute  pixel color (24 bit = 3 bytes) */
                        if (Iteration==IterationMax)
                        { /*  interior of Mandelbrot set = black */
                           color[0]=0;
                           color[1]=0;
                           color[2]=0;
                        }
                        else
                        { /* exterior of Mandelbrot set = white */
                             color[0]=((IterationMax-Iteration) % 8) *  63;  /* Red */
                             color[1]=((IterationMax-Iteration) % 4) * 127;  /* Green */
                             color[2]=((IterationMax-Iteration) % 2) * 255;  /* Blue */
                        };
                        /*write color to the file*/
                        fwrite(color,1,3,fp);
                }
        }
        fclose(fp);
        return 0;
}
