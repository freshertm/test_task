# Hi
# Prerequisites
## Dockerfile
Built with docker using Dockerfile.
```
  from ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y \
  && apt-get install -y \
     build-essential \
     cmake \
     python3 \
     python3-pip \
     git git-lfs \
     ninja-build \
  && ln -fs /usr/share/zoneinfo/Asia/Tomsk /etc/localtime \
  && dpkg-reconfigure --frontend noninteractive tzdata \
  && rm -rf /var/run/cache/* \
  && pip3 install conan \
  && conan config set compiler.libcxx=libstdc++11 \
  && conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
```

## Prepare
### When use Dockerfile 
- conan install . --build missing
### Otherwise
- conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
- conan install . -s compiler.libcxx=libstdc++11 --build=missing
### Build
- mkdir build
- cd build
- cmake -G "Ninja" ..
- cmake --build . -j $(nproc)  


## Run 
- bin/test_client
- bin/test_server 127.0.0.1 
