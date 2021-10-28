INCDIR=-I./Common
LIBS=-lGLESv2 -lEGL -lm -lX11 -lancillary -lgstd-1.0 `pkg-config --cflags --libs gstreamer-1.0`

COMMONSRC=./Common/esShader.c    \
          ./Common/esTransform.c \
          ./Common/esShapes.c

COMMONHRD=esUtil.h

UTILBASE=./Base/esUtil.c
UTILCONSUMER=./Consumer/esUtil.c
UTILGSTD=./GstDBase/esUtil.c
UTILGSTDPRODUCER=./GstDProducer/esUtil.c
UTILPRODUCER=./Producer/esUtil.c

SRC_B=./Base/base.c
SRC_C=./Consumer/consumer.c
SRC_G=./GstDBase/gstd.c
SRC_PC=./GstDProducer/gstd.c
SRC_P=./Producer/producer.c

default: all

all:                                      \
     ./Base/base                          \
     ./Consumer/consumer                  \
     ./GstDBase/gstd                      \
     ./GstDProducer/gstd                  \
     ./Producer/producer

clean:
	rm ./Base/base ./Consumer/consumer ./GstDBase/gstd ./Producer/producer ./GstDProducer/gstd

./Base/base: ${UTILBASE} ${COMMONHDR} ${SRC_B}
	gcc ${COMMONSRC} ${UTILBASE} ${SRC_B} -o ./$@ ${INCDIR} ${LIBS}

./GstDBase/gstd: ${UTILGSTD} ${COMMONHDR} ${SRC_G}
	gcc ${COMMONSRC} ${UTILGSTD} ${SRC_G} -o ./$@ ${INCDIR} ${LIBS}

./Consumer/consumer: ${UTILCONSUMER} ${COMMONHDR} ${SRC_C}
	gcc ${COMMONSRC} ${UTILCONSUMER} ${SRC_C} -o ./$@ ${INCDIR} ${LIBS}

./GstDProducer/gstd: ${UTILGSTDPRODUCER} ${COMMONHDR} ${SRC_PC}
	gcc ${COMMONSRC} ${UTILGSTDPRODUCER} ${SRC_PC} -o ./$@ ${INCDIR} ${LIBS}

./Producer/producer: ${UTILPRODUCER} ${COMMONHDR} ${SRC_P}
	gcc ${COMMONSRC} ${UTILPRODUCER} ${SRC_P} -o ./$@ ${INCDIR} ${LIBS}

