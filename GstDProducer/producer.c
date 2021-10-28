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
#include <gstd.h>

#include "esUtil.h"

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

GstD *manager = NULL;


void initEGLStreamUtil () {
   eglGetStreamFileDescriptorKHR = (PFNEGLGETSTREAMFILEDESCRIPTORKHRPROC)eglGetProcAddress("eglGetStreamFileDescriptorKHR");
   eglStreamConsumerAcquireKHR = (PFNEGLSTREAMCONSUMERACQUIREKHRPROC)eglGetProcAddress("eglStreamConsumerAcquireKHR");
   eglStreamConsumerReleaseKHR = (PFNEGLSTREAMCONSUMERRELEASEKHRPROC)eglGetProcAddress("eglStreamConsumerReleaseKHR");
   eglStreamConsumerGLTextureExternalKHR = (PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC)eglGetProcAddress("eglStreamConsumerGLTextureExternalKHR");
   eglCreateStreamKHR = (PFNEGLCREATESTREAMKHRPROC)eglGetProcAddress("eglCreateStreamKHR");
   eglQueryStreamKHR = (PFNEGLQUERYSTREAMKHRPROC)eglGetProcAddress("eglQueryStreamKHR");
   eglStreamAttribKHR = (PFNEGLSTREAMATTRIBKHRPROC)eglGetProcAddress("eglStreamAttribKHR");
   eglCreateStreamFromFileDescriptorKHR = (PFNEGLCREATESTREAMFROMFILEDESCRIPTORKHRPROC)eglGetProcAddress("eglCreateStreamFromFileDescriptorKHR");
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


static gboolean
int_term_handler (gpointer user_data)
{
  GMainLoop *main_loop;

  main_loop = (GMainLoop *) user_data;
  g_main_loop_quit (main_loop);

  printf("\nBye!\n");

  return TRUE;
}

int main ( int argc, char *argv[] )
{
   ESContext esContext;
   UserData  userData;

   GMainLoop *main_loop;


   EGLNativeFileDescriptorKHR fd;
   char *socket_name = "gstd_eglstream_test";
   struct sockaddr_un address;
   int  socket_fd;
   int evfd;

   socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
   if (socket_fd < 0)  {
      fprintf(stderr,"socket() failed: %s\n", strerror(errno));
      return 1;
   }

   memset(&address, 0, sizeof(struct sockaddr_un));

   address.sun_family = AF_UNIX;
   snprintf(address.sun_path,sizeof(address.sun_path), "%s", socket_name);
   address.sun_path[0] = '\0';

   if (connect(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0) {
      fprintf(stderr,"connect() failed: %s\n", strerror(errno));
      return 1;
   }

   if (ancil_recv_fd(socket_fd, &evfd)) {
      perror("ancil_recv_fd");
      exit(1);   
   } else {
      printf("Received eventfd on: %d\n", evfd);
   }

   if (evfd < 0) {
      printf("bad event fd\n");
      exit(1);
   }

   close(socket_fd);


   int ret = GSTD_EOK;
   int time_sleep = 1;
   
   int fifoLength = 0;
   int latency = 0;

   GstdObject *resource = NULL;
   GstElement *element = NULL;


   esInitContext ( &esContext );
   esContext.userData = &userData;

   esCreateWindow ( &esContext, "Simple Texture 2D", WINDOW_WIDTH, WINDOW_HEIGHT, ES_WINDOW_RGB );

   initEGLStreamUtil ();

   stream = eglCreateStreamFromFileDescriptorKHR(esContext.eglDisplay, evfd);
   if (stream == EGL_NO_STREAM_KHR) {
      printf("Could not create EGL stream.\n");
      eglStatus = EGL_FALSE;
   } else {
      printf("Productor connected to stream.\n");
   }

    // Get stream attributes
    if (!eglQueryStreamKHR(esContext.eglDisplay, stream, EGL_STREAM_FIFO_LENGTH_KHR, &fifoLength)) {
        printf("Consumer: eglQueryStreamKHR EGL_STREAM_FIFO_LENGTH_KHR failed.\n");
    }
    if (!eglQueryStreamKHR(esContext.eglDisplay, stream, EGL_CONSUMER_LATENCY_USEC_KHR, &latency)) {
        printf("Consumer: eglQueryStreamKHR EGL_CONSUMER_LATENCY_USEC_KHR failed.\n");
    }

   printf("EGL Stream consumer - Mode: FIFO, Length: %d, latency %d.\n", fifoLength, latency);

   gstd_new (&manager, 0, NULL);

   g_print ("Starting...\n");
   if (!gstd_start (manager)) {
     return FALSE;
   }

   ret = gstd_create (manager, 
      "/pipelines", 
      "p",
      "videotestsrc pattern=0 ! nvvidconv ! nvvideosink name=nv_sink"); 
      // "filesrc location=/home/joseda8/Desktop/test3.mp4 ! qtdemux name=demux ! h264parse ! omxh264dec ! nvvidconv ! nvvideosink name=nv_sink");
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

   main_loop = g_main_loop_new (NULL, FALSE);

   g_unix_signal_add (SIGINT, int_term_handler, main_loop);
   g_unix_signal_add (SIGTERM, int_term_handler, main_loop);

   g_main_loop_run (main_loop);

   /* Stop any GstD array */
   gstd_update (manager, "/pipelines/p/state", "paused");
   gstd_delete (manager, "/pipelines", "p");

   gstd_stop (manager);
   gst_deinit ();
   gstd_free(manager);

}
