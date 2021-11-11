/*
 * Author: José Daniel Montoya Salazar <jose.montoya@ridgerun.com>
 * Using: libancillary - black magic on Unix domain sockets by Nicolas George
 *
 * About libancillary
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../Common/esUtil.h"

#define SAVE_FRAME TRUE
#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240

PFNEGLGETSTREAMFILEDESCRIPTORKHRPROC            eglGetStreamFileDescriptorKHR;
PFNEGLSTREAMCONSUMERACQUIREKHRPROC              eglStreamConsumerAcquireKHR;
PFNEGLSTREAMCONSUMERRELEASEKHRPROC              eglStreamConsumerReleaseKHR;
PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC    eglStreamConsumerGLTextureExternalKHR;
PFNEGLCREATESTREAMKHRPROC                       eglCreateStreamKHR;
PFNEGLQUERYSTREAMKHRPROC                        eglQueryStreamKHR;
PFNEGLSTREAMATTRIBKHRPROC                       eglStreamAttribKHR;
PFNEGLCREATESTREAMFROMFILEDESCRIPTORKHRPROC     eglCreateStreamFromFileDescriptorKHR;
PFNEGLDESTROYSTREAMKHRPROC                      eglDestroyStreamKHR;

EGLStreamKHR stream;
EGLBoolean eglStatus = EGL_TRUE;
GLuint textureId2D;
GLuint FramebufferName;

void initEGLStreamUtil () {
   eglGetStreamFileDescriptorKHR = (PFNEGLGETSTREAMFILEDESCRIPTORKHRPROC)eglGetProcAddress("eglGetStreamFileDescriptorKHR");
   eglStreamConsumerAcquireKHR = (PFNEGLSTREAMCONSUMERACQUIREKHRPROC)eglGetProcAddress("eglStreamConsumerAcquireKHR");
   eglStreamConsumerReleaseKHR = (PFNEGLSTREAMCONSUMERRELEASEKHRPROC)eglGetProcAddress("eglStreamConsumerReleaseKHR");
   eglStreamConsumerGLTextureExternalKHR = (PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC)eglGetProcAddress("eglStreamConsumerGLTextureExternalKHR");
   eglCreateStreamKHR = (PFNEGLCREATESTREAMKHRPROC)eglGetProcAddress("eglCreateStreamKHR");
   eglQueryStreamKHR = (PFNEGLQUERYSTREAMKHRPROC)eglGetProcAddress("eglQueryStreamKHR");
   eglStreamAttribKHR = (PFNEGLSTREAMATTRIBKHRPROC)eglGetProcAddress("eglStreamAttribKHR");
   eglDestroyStreamKHR = (PFNEGLDESTROYSTREAMKHRPROC)eglGetProcAddress("eglDestroyStreamKHR");
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

void save_frame () {
   FILE *output_image = NULL;

   output_image = fopen("/home/joseda8/Desktop/output.ppm", "wt");
   fprintf(output_image,"P3\n");
   fprintf(output_image,"# Created by Jose Montoya\n");
   fprintf(output_image,"%d %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
   fprintf(output_image,"255\n");

   unsigned char *pixels = (unsigned char*) malloc(WINDOW_WIDTH*WINDOW_HEIGHT*3);
   int i, j, k;
   int sum = 0;

   glReadBuffer (GL_COLOR_ATTACHMENT0);
   glReadPixels (0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);

   glBindTexture (GL_TEXTURE_2D, textureId2D);
   glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

   k = 0;
   for(i=0; i<WINDOW_WIDTH; i++)
   {
      for(j=0; j<WINDOW_HEIGHT; j++)
      {
         // sum = pixels[k] + pixels[k+1] + pixels[k+2] + pixels[k+3];
         // if (sum > 255) {
         //    // printf ("%u %u %u %u\n", (unsigned int)pixels[k],(unsigned int)pixels[k+1], (unsigned int)pixels[k+2], (unsigned int)pixels[k+3]);
         // }
         fprintf(output_image,"%u %u %u ",(unsigned int)pixels[k],(unsigned int)pixels[k+1], (unsigned int)pixels[k+2]);
         k = k+3;
      }
      fprintf(output_image,"\n");
   }

   free(pixels);
}

///
// Create a simple 2x2 texture image with four different colors
//
GLuint CreateSimpleTexture2D()
{
   // Texture object handle
   GLuint textureId;

   // Generate a texture object
   glGenTextures ( 1, &textureId );
   if (glGetError () == GL_NO_ERROR) {
    printf ("%s\n", "Texture generated.");
   }

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_EXTERNAL_OES, textureId );

   glTexParameterf (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
   glTexParameterf (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
   glTexParameterf (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameterf (GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
   glGenTextures ( 1, &textureId2D );

   glActiveTexture(GL_TEXTURE0);
   glBindTexture ( GL_TEXTURE_2D, textureId2D );

   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   glGenFramebuffers (1, &FramebufferName);
   glBindFramebuffer (GL_FRAMEBUFFER, 0);

   glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId2D, 0);
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
   {
      printf("%s %d\n", "Error: Could not setup frame buffer.", glCheckFramebufferStatus(GL_FRAMEBUFFER));
   } 
   else 
   {
      printf("%s\n", "Setup frame buffer.");
   }

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
   userData->samplerLoc = glGetUniformLocation ( userData->programObject, "texunit" );
   if (userData->samplerLoc == -1) {
      printf ("%s\n", "No active uniform.");
   }

   // Load the texture
   userData->textureId = CreateSimpleTexture2D ();

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
   GLfloat vVertices[] = { 
                           // -1.0f,  1.0f, 0.0f,  // Position 0
                           //  0.0f,  0.0f,        // TexCoord 0 
                           // -1.0f, -1.0f, 0.0f,  // Position 1
                           //  0.0f,  1.0f,        // TexCoord 1
                           //  1.0f, -1.0f, 0.0f,  // Position 2
                           //  1.0f,  1.0f,        // TexCoord 2
                           //  1.0f,  1.0f, 0.0f,  // Position 3
                           //  1.0f,  0.0f         // TexCoord 3

                           -0.5f,  0.5f, 0.0f,  // Position 0
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
         exit (0);
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
   
   if (SAVE_FRAME) {
      save_frame ();
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

int connection_handler(int connection_fd, EGLNativeFileDescriptorKHR fd)
{
    int i;

    if (ancil_send_fd(connection_fd, fd)) {
        perror("ancil_send_fd");
        exit(1);
    } else {
       printf("Sent evfd %d\n", fd);
    }
    
    return 0;
}


int main ( int argc, char *argv[] )
{
   ESContext esContext;
   UserData  userData;

   static const EGLint streamAttrFIFOMode[] = { EGL_STREAM_FIFO_LENGTH_KHR, 5, EGL_SUPPORT_RESET_NV, EGL_TRUE, EGL_NONE };
   
   int fifoLength = 0;
   int latency = 0;

   EGLNativeFileDescriptorKHR fd;
   char *socket_name = "gstd_eglstream_test";

   struct sockaddr_un address;
   int socket_fd, connection_fd;
   socklen_t address_length  = sizeof(address);
   int enable = 1;

   socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
   if(socket_fd < 0) {
      printf("socket() failed\n");
      return 1;
   }

   setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

   memset(&address, 0, sizeof(struct sockaddr_un));
   address.sun_family = AF_UNIX;

   snprintf(address.sun_path,sizeof(address.sun_path), "%s", socket_name);
   address.sun_path[0] = '\0';

   if (bind(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0) {
      fprintf(stderr,"bind() failed: %s\n", strerror(errno));
      return 1;
   }

   if (listen(socket_fd, 5) != 0) {
      fprintf(stderr,"listen() failed: %s\n", strerror(errno));
      return 1;
   }

   esInitContext ( &esContext );
   esContext.userData = &userData;

   esCreateWindow ( &esContext, "Simple Texture 2D", WINDOW_WIDTH, WINDOW_HEIGHT, ES_WINDOW_RGB, TRUE );

   initEGLStreamUtil ();
   if ( !Init ( &esContext ) ) {
      return FALSE;
   }

   stream = eglCreateStreamKHR(esContext.eglDisplay, streamAttrFIFOMode);
   if (stream == EGL_NO_STREAM_KHR) {
     printf("Could not create EGL stream.\n");
     eglStatus = EGL_FALSE;
   }

   fd = eglGetStreamFileDescriptorKHR(esContext.eglDisplay, stream);
   if (fd == EGL_NO_FILE_DESCRIPTOR_KHR) {
     printf("Could not get file descriptor.\n");
     eglStatus = EGL_FALSE;
   }
   printf("File descriptor: %d\n.", fd);


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

   printf ("Waiting for client...\n");
   if ((connection_fd = accept(socket_fd, (struct sockaddr *) &address, &address_length)) > -1) {
      connection_handler(connection_fd, fd);
      close(connection_fd);
   } else {
      fprintf(stderr,"accept() failed: %s\n", strerror(errno));
      close(socket_fd);
   }

   esRegisterDrawFunc ( &esContext, Draw );

   esMainLoop ( &esContext );

   ShutDown ( &esContext );

   if ( eglDestroyStreamKHR (esContext.eglDisplay, stream) ) {
      printf ("%s\n", "Could not destroy the stream");
   } else {
      stream = EGL_NO_STREAM_KHR;
   }

}
