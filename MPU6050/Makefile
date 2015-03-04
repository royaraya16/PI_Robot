
all: Ejemplo

HDRS = helper_3dmath.h I2Cdev.h MPU6050_6Axis_MotionApps20.h MPU6050.h IMU.h
CMN_OBJS = I2Cdev.o MPU6050.o IMU.o
DMP_OBJS = Ejemplo.o


# Frecuencia (Hz) = 200/(DDMP_FIFO_RATE + 1)
#FIFO RATE en 1 para 100Hz

CXXFLAGS = -DDMP_FIFO_RATE=1 -Wall -g -O2

$(CMN_OBJS) $(DMP_OBJS) : $(HDRS)

Ejemplo: $(CMN_OBJS) $(DMP_OBJS)
	$(CXX) -o $@ $^ -lm

clean:
	rm -f $(CMN_OBJS) $(DMP_OBJS) Ejemplo


