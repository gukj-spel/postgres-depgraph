MODULE_big = postgres-depgraph 
OBJS = postgres-depgraph.o

CC = gcc
CXX = g++
CXXFLAGS = -std=c++17 -O0 -g -DDEBUG

ONNXRUNTIME_SRC = /home/baum/postgres-test/onnxruntime-main
ONNXRUNTIME_BUILD = $(ONNXRUNTIME_SRC)/build/Linux/RelWithDebInfo

INCLUDES = -I$(ONNXRUNTIME_SRC)/include/onnxruntime/core/session \
           -I$(ONNXRUNTIME_BUILD)/include \
           -Idepgraph -Ionnx -Idb

# 头文件路径
PG_CPPFLAGS = -I$(libpq_srcdir)

# 共享库链接选项
SHLIB_LINK = -L$(libpq_builddir) -L$(ONNXRUNTIME_BUILD) -lpq -lonnxruntime



# 模块安装路径
EXTENSION = age_onnx_depgraph
DATA = age_onnx_depgraph--1.0.sql

# 包含 PostgreSQL 编译环境
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

# 安装规则
all: $(MODULE_big).so

# 自定义规则：编译根目录下的mian.c
postgres-depgraph.o: main.c
	$(CXX) $(CXXFLAGS) $(PG_CPPFLAGS) ${INCLUDES} -c $< -o $@
# 自定义规则：编译 depgraph 目录下的文件
depgraph/%.o: depgraph/%.c depgraph/%.h
	$(CXX) $(CXXFLAGS) $(PG_CPPFLAGS) ${INCLUDES} -c $< -o $@

# 自定义规则：编译 onnx 目录下的文件
onnx/%.o: onnx/%.c onnx/%.h
	$(CXX) $(CPPFLAGS) $(PG_CPPFLAGS) ${INCLUDES} -c $< -o $@

# 自定义规则：编译 db 目录下的文件
db/%.o: db/%.c db/%.h
	$(CXX) $(CPPFLAGS) $(PG_CPPFLAGS) ${INCLUDES} -c $< -o $@

# 清理规则
clean:
	rm -f $(OBJS) $(MODULE_big).so $(MODULE_big).o