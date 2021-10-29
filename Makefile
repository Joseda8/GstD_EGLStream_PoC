INCDIR=-I./Common
LIBS=-lGLESv2 -lEGL -lm -lX11 -lancillary -lgstd-1.0 `pkg-config --cflags --libs gstreamer-1.0`

COMMONSRC=./Common/esShader.c    \
          ./Common/esUtil.c

COMMONHRD=./Common/esUtil.h

SRC_B=./Base/base.c
SRC_C=./Consumer/consumer.c
SRC_G=./GstDBase/gstd.c
SRC_PC=./GstDProducer/producer.c
SRC_P=./Producer/producer.c

default: all

all:                                      \
     ./Base/base                          \
     ./Consumer/consumer                  \
     ./GstDBase/gstd                      \
     ./GstDProducer/producer              \
     ./Producer/producer

clean:
	rm ./Base/base ./Consumer/consumer ./GstDBase/gstd ./Producer/producer ./GstDProducer/producer

./Base/base: ${COMMONHDR} ${SRC_B}
	gcc ${COMMONSRC} ${SRC_B} -o ./$@ ${INCDIR} ${LIBS}

./GstDBase/gstd: ${COMMONHDR} ${SRC_G}
	gcc ${COMMONSRC} ${SRC_G} -o ./$@ ${INCDIR} ${LIBS}

./Consumer/consumer: ${COMMONHDR} ${SRC_C}
	gcc ${COMMONSRC} ${SRC_C} -o ./$@ ${INCDIR} ${LIBS}

./GstDProducer/producer: ${COMMONHDR} ${SRC_PC}
	gcc ${COMMONSRC} ${SRC_PC} -o ./$@ ${INCDIR} ${LIBS}

./Producer/producer: ${COMMONHDR} ${SRC_P}
	gcc ${COMMONSRC} ${SRC_P} -o ./$@ ${INCDIR} ${LIBS}

