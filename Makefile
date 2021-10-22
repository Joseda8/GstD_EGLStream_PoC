INCDIR=-I./Common
LIBS=-lGLESv2 -lEGL -lm -lX11 -lancillary -lgstd-1.0 `pkg-config --cflags --libs gstreamer-1.0`

COMMONSRC=./Common/esShader.c    \
          ./Common/esTransform.c \
          ./Common/esShapes.c

COMMONHRD=esUtil.h

UTILBASE=./Base/esUtil.c
UTILGSTD=./GstDBase/esUtil.c
UTILGSTDCONSUMER=./GstDConsumer/esUtil.c
UTILGSTDPRODUCER=./GstDProducer/esUtil.c
UTILCONSUMER=./Consumer/esUtil.c
UTILPRODUCER=./Producer/esUtil.c

SRC_B=./Base/base.c
SRC_G=./GstDBase/gstd.c
SRC_GC=./GstDConsumer/gstd.c
SRC_PC=./GstDProducer/gstd.c
SRC_C=./Consumer/consumer.c
SRC_P=./Producer/producer.c

default: all

all:                                      \
     ./Base/base                          \
     ./GstDBase/gstd                      \
     ./GstDConsumer/gstd                  \
     ./GstDProducer/gstd                  \
     ./Consumer/consumer                  \
     ./Producer/producer

clean:
	rm ./Consumer/consumer ./Producer/producer ./Base/base ./GstDBase/gstd ./GstDConsumer/gstd ./GstDProducer/gstd

./Base/base: ${UTILBASE} ${COMMONHDR} ${SRC_B}
	gcc ${COMMONSRC} ${UTILBASE} ${SRC_B} -o ./$@ ${INCDIR} ${LIBS}

./GstDBase/gstd: ${UTILGSTD} ${COMMONHDR} ${SRC_G}
	gcc ${COMMONSRC} ${UTILGSTD} ${SRC_G} -o ./$@ ${INCDIR} ${LIBS}

./GstDConsumer/gstd: ${UTILGSTDCONSUMER} ${COMMONHDR} ${SRC_GC}
	gcc ${COMMONSRC} ${UTILGSTDCONSUMER} ${SRC_GC} -o ./$@ ${INCDIR} ${LIBS}

./GstDProducer/gstd: ${UTILGSTDPRODUCER} ${COMMONHDR} ${SRC_PC}
	gcc ${COMMONSRC} ${UTILGSTDPRODUCER} ${SRC_PC} -o ./$@ ${INCDIR} ${LIBS}

./Consumer/consumer: ${UTILCONSUMER} ${COMMONHDR} ${SRC_C}
	gcc ${COMMONSRC} ${UTILCONSUMER} ${SRC_C} -o ./$@ ${INCDIR} ${LIBS}

./Producer/producer: ${UTILPRODUCER} ${COMMONHDR} ${SRC_P}
	gcc ${COMMONSRC} ${UTILPRODUCER} ${SRC_P} -o ./$@ ${INCDIR} ${LIBS}

