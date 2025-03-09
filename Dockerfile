FROM debian:buster-20210311-slim 

RUN dpkg --add-architecture i386 && apt-get update

RUN apt-get install -y --no-install-recommends libstdc++5:i386=1:3.3.6-30
RUN apt-get install -y --no-install-recommends g++-multilib=4:8.3.0-1
RUN apt-get install -y --no-install-recommends default-libmysqlclient-dev:i386=1.0.5
RUN apt-get install -y --no-install-recommends libsqlite3-0:i386=3.27.2-3+deb10u1 libsqlite3-dev:i386=3.27.2-3+deb10u1

WORKDIR /app

COPY . .

RUN chmod +x doit.sh

RUN ./doit.sh cod2_1_0
RUN ./doit.sh cod2_1_2
RUN ./doit.sh cod2_1_3

CMD ["bash"]