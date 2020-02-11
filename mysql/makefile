INC_DIR= -I..
SRC_DIR = ./mysql ./ndn ./blockchain ./common
OBJ_DIR = ./objs

#SRC=$(wildcard $(SRC_DIR)/*.cpp)   

SRC=  test.cpp \
	  ../mysql/sqlexecutor.cpp \
	  ../mysql/sqlfunc.cpp \
	  ../mysql/user.cpp \
	  ../mysql/whitelist.cpp \
	  ../mysql/sqlutil.cpp \
	  ../mysql/realTimeMsg.cpp \
	  ../mysql/approval.cpp \
	  ../ndn/useractHelper.cpp \
	  ../ndn/ndnserver.cpp \
	  ../ndn/ndnKey.cpp \
	  ../ndn/loginTable.cpp \
	  ../ndn/register.cpp \
	  ../ndn/fileTransfer.cpp \
	  ../ndn/approval.cpp \
	  ../ndn/rtmsg.cpp \
	  ../blockchain/dns_client.cpp \
	  ../common/util.cpp

OBJS := $(patsubst %.cpp, %.o,$(SRC))

CC := g++ 
CFLAGS :=-Wall -g -std=c++14 ${INC_DIR}
LFLAGS := -lpthread -ljsoncpp -lpcap -lndn-cxx -lboost_system -lmysqlclient \
		-lsqlite3 -lcryptopp -luuid
TARGET := test 

all: ${TARGET}


$(TARGET): ${OBJS}
	$(CC) ${OBJS} -o ${TARGET}  $(LFLAGS)

${OBJS} : %.o: %.cpp
	${CC} ${CFLAGS} -c $< -o $@
	@#echo $^


#$@  表示目标文件
#$^  表示所有依赖文件
#$<  表示第一个依赖文件
#$?  表示比目标新的依赖文件列表

clean:  
	rm -f $(TARGET) *.o ${OBJS} 

