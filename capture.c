// DATE: Thu 15 Feb 01:37:03 2024 
// KEY: capture screen, get screen, opengl capture screen
// REF: https://pastebin.com/pMH2rDNH
// This C code captures the screen of Macs and shows it in an OpenGL window through the function glDrawPixels
// gcc -o opengl-capture capture.c -framework Carbon -framework OpenGL -framework GLUT
 
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
 
#include </System/Volumes/Data/Library/Developer/CommandLineTools/SDKs/MacOSX11.1.sdk/System/Library/Frameworks/CoreGraphics.framework/Versions/A/Headers/CoreGraphics.h>
#include <objc/objc-runtime.h>
 
unsigned int window_width = 1200, window_height = 1000;
 
int* getMainScreenSize() {
 CGRect r = CGDisplayBounds(kCGDirectMainDisplay);
 int *size=malloc(sizeof(int)*2);
 size[0]=r.size.width;
 size[1]=r.size.height;
 return size;
}
 
int* getAllScreensSize() {
 CGImageRef img = CGWindowListCreateImage(CGRectInfinite, kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageDefault);
 int *size=malloc(sizeof(int)*2);
 size[0]=CGImageGetWidth(img);
 size[1]=CGImageGetHeight(img);
 return size;
}
 
CGImageRef captureImage(CGRect r) {
 return CGWindowListCreateImage(r, kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageDefault);
}
 
unsigned char* macPixelsToRGB(CGImageRef image,unsigned char *pixelData) {
 int bytesPerPixel=CGImageGetBitsPerPixel(image)/8;
 int bytesPerRow=CGImageGetBytesPerRow(image);
 int imageWidth=CGImageGetWidth(image);
 int imageHeight=CGImageGetHeight(image);
 int x,y,m,p;
 unsigned char *rgb=(unsigned char*)malloc(imageWidth*imageHeight*3);
 for(y=0; y<imageHeight; y++) {
  for(x=0; x<imageWidth; x++) {
   m=y*bytesPerRow+x*bytesPerPixel;
   p=(y*imageWidth+x)*3;
   rgb[p+0]=pixelData[m+2]/2;
   rgb[p+1]=pixelData[m+1]/2;
   rgb[p+2]=pixelData[m+0]/2;
   //rgb[p+0]=(int)(127.0*(double)y/(double)imageHeight);
   //rgb[p+1]=0;
   //rgb[p+2]=0;
  }
 }
 return rgb;
}
 
unsigned char* getImageBuffer(CGImageRef image) {
 if(image == NULL) {
  fprintf(stderr,"CGWindowListCreateImage failed!\n");
  return NULL;
 }
 CGDataProviderRef provider = CGImageGetDataProvider(image);
 CFDataRef dataRef = CGDataProviderCopyData(provider);
 unsigned char *pixelData=(unsigned char*)CFDataGetBytePtr(dataRef);
 return macPixelsToRGB(image,pixelData);
}
 
unsigned char* captureScreenRegion(int x,int y,int width,int height) {
 CGRect r;
 r.origin.x = x;
 r.origin.y = y;
 r.size.width = width;
 r.size.height = height;
 return getImageBuffer(captureImage(r));
}
 
unsigned char* captureTheWholeScreen() {
 return getImageBuffer(captureImage(CGRectInfinite));
}
 
void display() {
    CGDirectDisplayID displayId = CGMainDisplayID();
    size_t pixelWidth  = CGDisplayPixelsWide(displayId) * 2;
    size_t pixelHeight = CGDisplayPixelsHigh(displayId) * 2;

    // unsigned char *pixels=captureScreenRegion(0,0,window_width,window_height);
    unsigned char *pixels=captureScreenRegion(0,0, pixelWidth * 0.5, pixelHeight * 0.5);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glRasterPos2f(-1,1);
    glPixelZoom( 0.5, -0.5 );
    glDrawPixels(pixelWidth, pixelHeight, GL_RGB, GL_BYTE, pixels);
    // glDrawPixels(window_width,window_height,GL_RGB,GL_BYTE,pixels);
    free(pixels);
    glutSwapBuffers();
}

void test(){
    CGDirectDisplayID displayId = CGMainDisplayID();
    size_t pixelWidth  = CGDisplayPixelsWide(displayId);
    size_t pixelHeight = CGDisplayPixelsHigh(displayId);
    printf("width=%d height=%d\n", pixelWidth, pixelHeight);
}
 
/*
int main(int argc, char** argv) {
    test();
}
*/

int main(int argc, char** argv) {
 usleep(3*1000000);

 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

 CGDirectDisplayID displayId = CGMainDisplayID();
 size_t pixelWidth  = CGDisplayPixelsWide(displayId) * 2;
 size_t pixelHeight = CGDisplayPixelsHigh(displayId) * 2;

 glutInitWindowSize(pixelWidth * 0.5, pixelHeight * 0.5);
 // glutInitWindowSize(window_width, window_height);
 glutCreateWindow("OpenGL glDrawPixels demo");
 glutDisplayFunc(display);
 glEnable(GL_DEPTH_TEST);
 glClearColor(0.0, 0.0, 0.0, 1.0);
 glutMainLoop();
}

