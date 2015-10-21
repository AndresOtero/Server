################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Cola.cpp \
../Interprete.cpp \
../MySocket.cpp \
../NodoSimplementeEnlazado.cpp \
../User.cpp \
../server.cpp 

OBJS += \
./Cola.o \
./Interprete.o \
./MySocket.o \
./NodoSimplementeEnlazado.o \
./User.o \
./server.o 

CPP_DEPS += \
./Cola.d \
./Interprete.d \
./MySocket.d \
./NodoSimplementeEnlazado.d \
./User.d \
./server.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__GXX_EXPERIMENTAL_CXX0X__ -I"/home/federico/git/AgeOfEmpiresTaller/AgeOfEmpiresTaller" -I"/home/federico/git/AgeOfEmpiresTaller/AgeOfEmpiresTaller/libs/plog-master/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


