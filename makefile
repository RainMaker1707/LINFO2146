CONTIKI_PROJECT = project_LINFO2146
all: $(CONTIKI_PROJECT)
PROJECT_SOURCEFILES += light_sensor.c light_bulb.c sub_gateway.c
CONTIKI = ../
include $(CONTIKI)/Makefile.include