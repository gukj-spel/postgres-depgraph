MODULE_big = postgres-depgraph 

#目标文件
# 收集所有需要编译的源文件
DEPGRAPH_SRCS := $(wildcard depgraph/*.cc)
ONNX_SRCS := $(wildcard onnx/*.cc)
DB_SRCS := $(wildcard db/*.cc)

# 生成对应的目标文件列表
DEPGRAPH_OBJS := $(patsubst depgraph/%.cc, depgraph/%.o, $(DEPGRAPH_SRCS))
ONNX_OBJS := $(patsubst onnx/%.cc, onnx/%.o, $(ONNX_SRCS))
DB_OBJS := $(patsubst db/%.cc, db/%.o, $(DB_SRCS))

# 最终的目标文件列表
OBJS = postgres-depgraph.o $(DEPGRAPH_OBJS) $(ONNX_OBJS) $(DB_OBJS)

CC = gcc
CXX = g++
CXXFLAGS = -std=c++20 -O0 -g -DDEBUG

ONNXRUNTIME_DIR = /usr/local/orttraining
ONNXRUNTIME_SRC = /home/baum/postgres-test/onnxruntime-main
ONNXRUNTIME_BUILD = $(ONNXRUNTIME_DIR)/build/Linux/RelWithDebInfo

INCLUDES = -I$(ONNXRUNTIME_DIR)/include \
           -Idepgraph -Ionnx -Idb

# 头文件路径
PG_CPPFLAGS = -I$(libpq_srcdir)

# 共享库链接选项
SHLIB_LINK = -L$(libpq_builddir) -L$(ONNXRUNTIME_DIR)/lib -lpq -lonnxruntime



# 模块安装路径
EXTENSION = postgres-depgraph
DATA = postgres-depgraph--1.0.sql

# 包含 PostgreSQL 编译环境
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)



# 自定义规则：编译根目录下的mian.c
postgres-depgraph.o: main.cc
	$(CXX) $(CXXFLAGS) $(PG_CPPFLAGS) ${INCLUDES} -c $< -o $@

# 自定义规则：编译 depgraph 目录下的文件
depgraph/%.o: depgraph/%.cc depgraph/%.h
	$(CXX) $(CXXFLAGS) $(PG_CPPFLAGS) ${INCLUDES} -c $< -o $@

# 自定义规则：编译 onnx 目录下的文件
onnx/%.o: onnx/%.cc onnx/%.h
	$(CXX) $(CPPFLAGS) $(PG_CPPFLAGS) ${INCLUDES} -c $< -o $@

# 自定义规则：编译 db 目录下的文件
db/%.o: db/%.cc db/%.h
	$(CXX) $(CPPFLAGS) $(PG_CPPFLAGS) ${INCLUDES} -c $< -o $@
