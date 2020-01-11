#!/bin/bash
set -e -u -E

########################################
# download & build depend software
########################################

WORK_DIR=$(cd $(dirname $0); pwd)
DEPS_SOURCE=$WORK_DIR/third_src
DEPS_PREFIX=$WORK_DIR/third_party
DEPS_CONFIG="--prefix=${DEPS_PREFIX} --disable-shared --with-pic"
FLAG_DIR=$WORK_DIR/.build
GIT_REPO="https://github.com/taotaowill/thirdsrc"

export PATH=${DEPS_PREFIX}/bin:$PATH
mkdir -p ${DEPS_SOURCE} ${DEPS_PREFIX} ${FLAG_DIR}

cd ${DEPS_SOURCE}

# boost
if [ ! -f "${FLAG_DIR}/boost_1_57_0" ] \
    || [ ! -d "${DEPS_PREFIX}/boost_1_57_0/boost" ]; then
    wget --no-check-certificate -O boost_1_57_0.tar.gz  $GIT_REPO/raw/master/boost_1_57_0.tar.gz
    tar zxf boost_1_57_0.tar.gz
    rm -rf ${DEPS_PREFIX}/boost_1_57_0
    mv boost_1_57_0 ${DEPS_PREFIX}
    cd ${DEPS_PREFIX}/boost_1_57_0 && ./bootstrap.sh && ./b2 --with-filesystem link=static
    cd -
    touch "${FLAG_DIR}/boost_1_57_0"
fi

# protobuf
if [ ! -f "${FLAG_DIR}/protobuf_2_6_1" ] \
    || [ ! -f "${DEPS_PREFIX}/lib/libprotobuf.a" ] \
    || [ ! -d "${DEPS_PREFIX}/include/google/protobuf" ]; then
    wget --no-check-certificate -O protobuf-2.6.1.tar.gz $GIT_REPO/raw/master/protobuf-2.6.1.tar.gz
    tar zxf protobuf-2.6.1.tar.gz
    cd protobuf-2.6.1
    ./configure ${DEPS_CONFIG}
    make -j4
    make install
    cd -
    touch "${FLAG_DIR}/protobuf_2_6_1"
fi

# cmake
if ! which cmake ; then
    wget --no-check-certificate -O CMake-3.2.1.tar.gz $GIT_REPO/raw/master/CMake-3.2.1.tar.gz
    tar zxf CMake-3.2.1.tar.gz
    cd CMake-3.2.1
    ./configure --prefix=${DEPS_PREFIX}
    make -j4
    make install
    cd -
fi

# gtest
if [ ! -f "${FLAG_DIR}/gtest_1_7_0" ] \
    || [ ! -f "${DEPS_PREFIX}/lib/libgtest.a" ] \
    || [ ! -d "${DEPS_PREFIX}/include/gtest" ]; then
    rm -rf ./googletest-release-1.7.0
    wget --no-check-certificate -O gtest_1_7_0.tar.gz $GIT_REPO/raw/master/gtest_1_7_0.tar.gz
    tar -xzf gtest_1_7_0.tar.gz
    cd googletest-release-1.7.0
    sed -i 's/-Wno-missing-field-initializers//g' cmake/internal_utils.cmake
    cmake .
    make
    cp -af lib*.a ${DEPS_PREFIX}/lib
    cp -af include/gtest ${DEPS_PREFIX}/include
    cd -
    touch "${FLAG_DIR}/gtest_1_7_0"
fi

#muduo
if [ ! -f "${FLAG_DIR}/muduo" ] \
    || [ ! -f "${DEPS_PREFIX}/lib/libmuduo_base.a" ]; then
    rm -rf muduo*
    wget https://github.com/chenshuo/muduo/archive/v1.0.9.tar.gz
    tar -xzvf v1.0.9.tar.gz
    cd muduo-1.0.9
    sed -i 's/-Wold-style-cast//g' CMakeLists.txt
    sed -i 's/-Werror//g' CMakeLists.txt
    sed -i 's/find_package(Protobuf)//g' CMakeLists.txt
    sed -i 's/add_subdirectory(examples)//g' CMakeLists.txt
    sed -i 's/find_path(TCMALLOC_INCLUDE_DIR gperftools\/heap-profiler\.h)//g' CMakeLists.txt
    sed -i "/find_package(Boost REQUIRED)/iset(BOOST_ROOT $DEPS_PREFIX/boost_1_57_0)" CMakeLists.txt
    echo "set(TCMALLOC_INCLUDE_DIR 0)" >> CMakeLists.txt
    bash build.sh install
    cp -r ../build/release-install/lib/* $DEPS_PREFIX/lib
    cp -r ../build/release-install/include/* $DEPS_PREFIX/include
    touch "${FLAG_DIR}/muduo"
    cd -
fi

cd ${WORK_DIR}
echo "build deps done!"
