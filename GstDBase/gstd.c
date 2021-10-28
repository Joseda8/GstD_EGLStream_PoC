//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// Simple_Texture2D.c
//
//    This is a simple example that draws a quad with a 2D
//    texture image. The purpose of this example is to demonstrate 
//    the basics of 2D texturing
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gstd.h>

#include "esUtil.h"

PFNEGLGETSTREAMFILEDESCRIPTORKHRPROC eglGetStreamFileDescriptorKHR;
PFNEGLSTREAMCONSUMERACQUIREKHRPROC eglStreamConsumerAcquireKHR;
PFNEGLSTREAMCONSUMERRELEASEKHRPROC eglStreamConsumerReleaseKHR;
PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC eglStreamConsumerGLTextureExternalKHR;
PFNEGLCREATESTREAMKHRPROC eglCreateStreamKHR;
PFNEGLQUERYSTREAMKHRPROC eglQueryStreamKHR;
PFNEGLSTREAMATTRIBKHRPROC eglStreamAttribKHR;


EGLStreamKHR stream;
EGLBoolean eglStatus = EGL_TRUE;

GstD *manager = NULL;


void initEGLStreamUtil () {
   eglGetStreamFileDescriptorKHR = (PFNEGLGETSTREAMFILEDESCRIPTORKHRPROC)eglGetProcAddress("eglGetStreamFileDescriptorKHR");
   eglStreamConsumerAcquireKHR = (PFNEGLSTREAMCONSUMERACQUIREKHRPROC)eglGetProcAddress("eglStreamConsumerAcquireKHR");
   eglStreamConsumerReleaseKHR = (PFNEGLSTREAMCONSUMERRELEASEKHRPROC)eglGetProcAddress("eglStreamConsumerReleaseKHR");
   eglStreamConsumerGLTextureExternalKHR = (PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC)eglGetProcAddress("eglStreamConsumerGLTextureExternalKHR");
   eglCreateStreamKHR = (PFNEGLCREATESTREAMKHRPROC)eglGetProcAddress("eglCreateStreamKHR");
   eglQueryStreamKHR = (PFNEGLQUERYSTREAMKHRPROC)eglGetProcAddress("eglQueryStreamKHR");
   eglStreamAttribKHR = (PFNEGLSTREAMATTRIBKHRPROC)eglGetProcAddress("eglStreamAttribKHR");
}

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Attribute locations
   GLint  positionLoc;
   GLint  texCoordLoc;

   // Sampler location
   GLint samplerLoc;

   // Texture handle
   GLuint textureId;

} UserData;

///
// Create a simple 2x2 texture image with four different colors
//
GLuint CreateSimpleTexture2D( int red, int green, int blue )
{
   // Texture object handle
   GLuint textureId;
   
   // 2x2 Image, 3 bytes per pixel (R, G, B)
   GLubyte pixels[4 * 3] =
   {  
      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      red, green, blue  // Yellow
   };

   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, &textureId );
   if (glGetError () == GL_NO_ERROR) {
    printf ("%s\n", "Texture generated.");
   }

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_EXTERNAL_OES, textureId );
   // glBindTexture ( GL_TEXTURE_2D, textureId );

   glTexParameterf (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
   glTexParameterf (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
   glTexParameterf (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   return textureId;

}

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   esContext->userData = malloc(sizeof(UserData));	
   UserData *userData = esContext->userData;
   GLbyte vShaderStr[] =  
      "attribute vec4 a_position;   \n"
      "attribute vec2 a_texCoord;   \n"
      "varying vec2 v_texCoord;     \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = a_position; \n"
      "   v_texCoord = a_texCoord;  \n"
      "}                            \n";
   
   GLbyte fShaderStr[] =  
      "#extension GL_OES_EGL_image_external : enable       \n"
      "precision mediump float;                            \n"
      "uniform samplerExternalOES texunit;                 \n"
      "varying vec2 v_texCoord;                            \n"
      "uniform sampler2D s_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  gl_FragColor = texture2D( texunit, v_texCoord );  \n"
      "}                                                   \n";     

   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( vShaderStr, fShaderStr );
   glUseProgram(userData->programObject);

   // Get the attribute locations
   userData->positionLoc = glGetAttribLocation ( userData->programObject, "a_position" );
   userData->texCoordLoc = glGetAttribLocation ( userData->programObject, "a_texCoord" );
   
   // Get the sampler location
   userData->samplerLoc = glGetUniformLocation ( userData->programObject, "s_texture" );
   if (userData->samplerLoc == -1) {
      printf ("%s\n", "No active uniform.");
   }

   // Load the texture
   userData->textureId = CreateSimpleTexture2D (255, 50, 255);

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

   // Set up texture to be used for the clients
   glUniform1i(userData->samplerLoc, 0);

   return GL_TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;
   GLfloat vVertices[] = { -0.5f,  0.5f, 0.0f,  // Position 0
                            0.0f,  0.0f,        // TexCoord 0 
                           -0.5f, -0.5f, 0.0f,  // Position 1
                            0.0f,  1.0f,        // TexCoord 1
                            0.5f, -0.5f, 0.0f,  // Position 2
                            1.0f,  1.0f,        // TexCoord 2
                            0.5f,  0.5f, 0.0f,  // Position 3
                            1.0f,  0.0f         // TexCoord 3
                         };
   GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

   eglStatus = 0;
   eglQueryStreamKHR(esContext->eglDisplay, stream, EGL_STREAM_STATE_KHR, &eglStatus);
   if (eglStatus == EGL_STREAM_STATE_NEW_FRAME_AVAILABLE_KHR) {
    printf ("%s\n", "New frame.");
   }

   if (!eglStreamConsumerAcquireKHR(esContext->eglDisplay, stream)) {

      eglStatus = 0;
      eglQueryStreamKHR(esContext->eglDisplay, stream, EGL_STREAM_STATE_KHR, &eglStatus);

      switch (eglStatus)
      {
      case EGL_STREAM_STATE_DISCONNECTED_KHR:
         printf("Lost connection.\n");
         break;
      case EGL_BAD_STATE_KHR:
         printf("Bad state.\n");
         break;
      case EGL_STREAM_STATE_EMPTY_KHR:
         printf("Empty.\n");
         break;
      case EGL_STREAM_STATE_CONNECTING_KHR:
         printf("Connecting.\n");
         break;
      case EGL_STREAM_STATE_NEW_FRAME_AVAILABLE_KHR:
         // printf("New frame.\n");
         break;
      case EGL_STREAM_STATE_OLD_FRAME_AVAILABLE_KHR:
         printf("Old frame.\n");
         break;
      default:
         printf("Unexpected stream state: %04x.\n", eglStatus);
      }

   } else { 
      printf("Valid.\n");
      // printf ("%d\n", EGL_CONSUMER_FRAME_KHR);
   }

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );
   
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->programObject );

   // Load the vertex position
   glVertexAttribPointer ( userData->positionLoc, 3, GL_FLOAT, 
                           GL_FALSE, 5 * sizeof(GLfloat), vVertices );
   // Load the texture coordinate
   glVertexAttribPointer ( userData->texCoordLoc, 2, GL_FLOAT,
                           GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3] );

   glEnableVertexAttribArray ( userData->positionLoc );
   glEnableVertexAttribArray ( userData->texCoordLoc );

   // Bind the texture
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_EXTERNAL_OES, userData->textureId );

   // Set the sampler texture unit to 0
   glUniform1i ( userData->samplerLoc, 0 );

   glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );


   eglStatus = eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
   if (!eglStatus) {
      printf ("%s\n", "Bad swapping.\n");
   }

   eglStatus = glGetError ();

   switch (eglStatus) {
    case EGL_BAD_DISPLAY:
       printf ("%s\n", "Bad display.");
       break;
    case EGL_NOT_INITIALIZED:
       printf ("%s\n", "Not initialized.");
       break;
    case EGL_BAD_SURFACE:
       printf ("%s\n", "Bad surface.");
       break;
    case EGL_CONTEXT_LOST:
       printf ("%s\n", "Context lost.");
       break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
       printf ("%s\n", "Invalid buffer operation.");
       break;
    case GL_INVALID_OPERATION:
       printf ("%s\n", "Invalid operation.");
       break;
    case GL_NO_ERROR:
       printf ("%s\n", "Swap done");
       break;
    default:
       printf("Unexpected state for swap: %04x.\n", eglStatus);
   }

   if (!eglStreamConsumerReleaseKHR(esContext->eglDisplay, stream)) {
    printf ("Release frame failed.\n\n");
   } else {
    printf ("Frame released.\n\n");
   }

}

///
// Cleanup
//
void ShutDown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // Delete texture object
   glDeleteTextures ( 1, &userData->textureId );

   // Delete program object
   glDeleteProgram ( userData->programObject );
	
   free(esContext->userData);
}


int main ( int argc, char *argv[] )
{
   ESContext esContext;
   UserData  userData;

   static const EGLint streamAttrFIFOMode[] = { EGL_STREAM_FIFO_LENGTH_KHR, 5, EGL_SUPPORT_RESET_NV, EGL_TRUE, EGL_NONE };


   int ret = GSTD_EOK;
   int time_sleep = 1;
   
   int fifoLength = 0;
   int latency = 0;

   GstdObject *resource = NULL;
   GstElement *element = NULL;


   esInitContext ( &esContext );
   esContext.userData = &userData;

   esCreateWindow ( &esContext, "Simple Texture 2D", 320, 240, ES_WINDOW_RGB );

   initEGLStreamUtil ();
   if ( !Init ( &esContext ) ) {
      return FALSE;
   }

   stream = eglCreateStreamKHR(esContext.eglDisplay, streamAttrFIFOMode);
   if (stream == EGL_NO_STREAM_KHR) {
     printf("Could not create EGL stream.\n");
     eglStatus = EGL_FALSE;
   }

   if (!eglStreamConsumerGLTextureExternalKHR(esContext.eglDisplay, stream)) {
     printf("Could not bind texture.\n");
     eglStatus = EGL_FALSE;
   }

    if (!eglStreamAttribKHR(esContext.eglDisplay, stream, EGL_CONSUMER_LATENCY_USEC_KHR, 0)) {
        printf("Consumer: streamAttribKHR EGL_CONSUMER_LATENCY_USEC_KHR failed.\n");
    }
    if (!eglStreamAttribKHR(esContext.eglDisplay, stream, EGL_CONSUMER_ACQUIRE_TIMEOUT_USEC_KHR, 0)) {
        printf("Consumer: streamAttribKHR EGL_CONSUMER_ACQUIRE_TIMEOUT_USEC_KHR failed.\n");
    }

    // Get stream attributes
    if (!eglQueryStreamKHR(esContext.eglDisplay, stream, EGL_STREAM_FIFO_LENGTH_KHR, &fifoLength)) {
        printf("Consumer: eglQueryStreamKHR EGL_STREAM_FIFO_LENGTH_KHR failed.\n");
    }
    if (!eglQueryStreamKHR(esContext.eglDisplay, stream, EGL_CONSUMER_LATENCY_USEC_KHR, &latency)) {
        printf("Consumer: eglQueryStreamKHR EGL_CONSUMER_LATENCY_USEC_KHR failed.\n");
    }

   printf("EGL Stream consumer - Mode: FIFO, Length: %d, latency %d.\n", fifoLength, latency);



   esRegisterDrawFunc ( &esContext, Draw );



   gstd_new (&manager, 0, NULL);

   g_print ("Starting...\n");
   if (!gstd_start (manager)) {
     return FALSE;
   }

   // ret = gstd_create (manager, "/pipelines", "p", "videotestsrc name=vts ! autovideosink");
   ret = gstd_create (manager, 
      "/pipelines", 
      "p", 
      "filesrc location=/home/joseda8/Desktop/test2.mp4 ! qtdemux name=demux ! h264parse ! omxh264dec ! nvvidconv ! nvvideosink name=nv_sink");
   if (GSTD_EOK != ret) {
   g_print ("Failed \n");
   return 0;
   }

   gstd_read (manager, "pipelines/p/elements/nv_sink", &resource);

   /* Get the gstreamer element from the resource */
   g_object_get (resource, "gstelement", &element, NULL);
   if (element == NULL) {
      printf ("%s\n", "No element");
      return FALSE;
   }

   g_object_set (element, "display", esContext.eglDisplay, NULL);
   g_object_set (element, "stream", stream, NULL);
   g_object_set (element, "fifo", TRUE, NULL);
   g_object_set (element, "fifo-size", fifoLength, NULL);

   gstd_update (manager, "/pipelines/p/state", "playing");

   /* Change the property from the gstreamer element */
   // g_object_set (element, "pattern", 1, NULL);

   esMainLoop ( &esContext );

   /* Stop any GstD array */

   gstd_update (manager, "/pipelines/p/state", "paused");
   gstd_delete (manager, "/pipelines", "p");

   gstd_stop (manager);
   gst_deinit ();
   gstd_free(manager);

   ShutDown ( &esContext );
}
